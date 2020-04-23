#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "TNLS";
const char* password = "56925622491421384628";

ESP8266WebServer server(80);

String webPage = "";

int gpio13Led = 13;
int gpio12Relay = 12;

void setup(void){
  webPage += "<h1>Remote Start</h1><p><a href=\"on\"><button>START</button></a>&nbsp;</p>";  
  // preparing GPIOs
  pinMode(gpio13Led, OUTPUT);
  digitalWrite(gpio13Led, LOW);
  
  pinMode(gpio12Relay, OUTPUT);
  digitalWrite(gpio12Relay, LOW);
 
  Serial.begin(115200); 
  delay(5000);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/on", [](){
    server.send(200, "text/html", webPage);
    digitalWrite(gpio13Led, HIGH);
    digitalWrite(gpio12Relay, HIGH);
    delay(750);
    digitalWrite(gpio13Led, LOW);
    digitalWrite(gpio12Relay, LOW);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
} 
