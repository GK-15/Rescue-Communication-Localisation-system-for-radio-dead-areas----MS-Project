// Header library files

#include <SoftwareSerial.h>
#include <TinyGPS++.h> // GPS module header
#include <ESP8266WiFi.h> // WiFi header
#include <LoRa.h> // LoRa header

// object initialization
TinyGPSPlus gps;  // creating a TinyGPS++ object

SoftwareSerial com(3, 4); // Serial communication

#define NSS 15
#define RST 0
#define DIO0 2

//  network credentials
const char* ssid     = "Ganesh";
const char* password = "";
float longitudess , longitude;
String  lat_coord , lng_coord;

// web server port number = 80
WiFiServer server(80);

// HTTP request variable
String header;

// current output state storage variable
String output5State = "off";
String output4State = "off";

// Current_time
unsigned long curr_time = millis();
// Previous_time
unsigned long prev_time = 0;
// Timeout_time 
const long tt_time = 2000;

void setup() {
  Serial.begin(115200);
    com.begin(9600);
  // Initialize output variables
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);

  // Connecting to the Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Printing the local IP address 
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  while (!Serial);
  Serial.println("Sender Host");

  // Lora initialization
  LoRa.setPins(NSS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
    while (1);
  }
}

void loop(){
  WiFiClient client = server.available();   // waiting for the incoming clients

  if (client) {                             // check if new client is connecting
    Serial.println("New Client is connecting.");        
    String curr_line = "";               
    curr_time = millis();
    prev_time = curr_time;
    
    while (client.connected() && curr_time - prev_time <= tt_time) {       // while the client is connected
      curr_time = millis();        
      if (client.available()) {            
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
   
          if (curr_line.length() == 0) {
           
            client.println("HTTP/1.1 200 is OK");
            client.println("Content-type:text/html");
            client.println("Connection is close");
            client.println();
           
    
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              Serial.print("Sending Data: ");
              while (com.available() > 0)
                     if (gps.encode(com.read())) 
                       {
                        if (gps.location.isValid()) //checking if the gps location is valid
                              {
                          
                              lat_coord = String(gps.location.lat() , 6); // lat and long location is stored in a string
                              Serial.println("lat_coord");
                              Serial.println(lat_coord);
                             lng_coord = String(gps.location.lng() , 6);
                              Serial.println("lng_coord");
                              Serial.println(lng_coord);
                        
                       }
                       }
  
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
             
            }
           
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // CSS is used to style the on and off buttons 
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
           
            // Heading for web page
            client.println("<body><h1>QMULRES Rescue Communication Support System</h1>");
           
            // to display the current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>Rescue - Location " + output5State + "</p>");
            // If the output5State is off then it displays the ON button in the screen     
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">SOS ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">SOS OFF</button></a></p>");
            }
               
           
            client.println("</body></html>");
           
            
            client.println();
          
            break;
          } else {
            curr_line = "";
          }
        } else if (c != '\r') {  
          curr_line += c;      
        }
      }
    }
    // to clear the header variable before the new operation is executed
    header = "";
    // Close the connection between the client
    client.stop();
    Serial.println("Client is disconnected.");
    Serial.println(" ");
  }
}
