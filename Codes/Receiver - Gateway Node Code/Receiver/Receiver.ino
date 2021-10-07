#include <ESP8266WiFi.h>
#include <LoRa.h>
#define SS 15
#define RST 0
#define DIO0 2
const char* ssid     = "xxx"; // Your ssid
const char* password = ""; // Your Password
String data;




WiFiServer server(80);
void setup() {
 Serial.begin(115200);
 // pinMode(A0, INPUT);  
 
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi is connected");
server.begin();
Serial.println("Server started");
Serial.println(WiFi.localIP());
while (!Serial);
  Serial.println("Sender Host");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa Error");
    delay(100);
    while (1);
  }
}
void loop() {
 
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Receiving Data: ");
    while (LoRa.available()) {
       data = LoRa.readString();
      Serial.println(data);
    }}
   
 


WiFiClient client = server.available();
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println("Connection: close");  // the connection will be closed after completion of the response
client.println("Refresh: 2");

client.println();
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>Rescue Team</strong></span></p>");
client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>User Location= ");
client.println(data);


client.print("</p>");
client.println("</html>");
//delay(5000);
 
}
