#include "MQ135.h" 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
           
//#define DHTPIN 14          
//#define DHTTYPE DHT22 
DHT dht;
int val; 
//DHT dht(DHTPIN, DHTTYPE);

MQ135 gasSensor = MQ135(A0); 
float h = 20;
float t = 18;


const char* SSID = "Connection1";
const char* PSK = ".Sonja00";
const char* MQTT_BROKER = "192.168.192.2";
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

float Temperatur_Motor_float=0;
float Temperatur_Wasser_float=0;
static char Temperatur_Motor[15];
static char Temperatur_Wasser[15];
static char Luft_ppm[15];
static char BilgeWasser[15];

#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void setup() { 
  Serial.begin(9600);
   pinMode(A0, INPUT); //Analog eingang
   pinMode(13, INPUT); //Digital eingang
  setup_wifi();
  client.setServer(MQTT_BROKER, 1883);
  sensors.begin();
  dht.setup(14); 
  //dht.begin();


  
} 

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);
 
    WiFi.begin(SSID, PSK);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
 
void reconnect() {
    while (!client.connected()) {
     
        Serial.print("Reconnecting...");
       // if (!client.connect("Sensor1")) {
      if (!client.connect("Sensor1","/Boot/AlarmSensor/MotorLuftQualitaet",0, true,"offline")) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}
 
void loop() { 
  // Temperaturen
   delay(dht.getMinimumSamplingPeriod());
  //float h = dht.readHumidity();    // Lesen der Luftfeuchtigkeit und speichern in die Variable h
  //float t = dht.readTemperature();
  float h = dht.getHumidity();  
float t = dht.getTemperature();

  
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Temperatur_Motor_float=sensors.getTempCByIndex(1);
  Temperatur_Wasser_float=sensors.getTempCByIndex(0);
 
  
  //Luftqualitätauslesen
  val = analogRead(A0); 
  float rzero = gasSensor.getRZero();
  float rzeroc = gasSensor.getCorrectedRZero( t, h);
  float ppm = gasSensor.getPPM();
  float ppmc = gasSensor.getCorrectedPPM(t, h);
  Serial.print ("raw = "); 
  Serial.println (val); 
  Serial.print ("TemperaturMotor: ");
  Serial.println(t);
  Serial.print ("LuftfeuchteMotor: ");
  Serial.println(h);
  Serial.print ("TemperaturWasser: ");
  Serial.println(Temperatur_Wasser_float);
  Serial.print ("rzero: "); 
  Serial.println (rzero); 
  Serial.print ("rzeroC: "); 
  Serial.println (rzeroc); 
  Serial.print ("ppm: "); 
  Serial.println (ppm);
  Serial.print ("ppmC: "); 
  Serial.println (ppmc);  
Serial.print ("Bilge: "); 
  Serial.println (digitalRead(13));  
  
  dtostrf(ppmc , 6, 2, Luft_ppm);
Temperatur_Motor_float=t;
 
  dtostrf(Temperatur_Wasser_float, 6, 2, Temperatur_Wasser);
  dtostrf(Temperatur_Motor_float, 6, 2, Temperatur_Motor);

if(digitalRead(13)==0) {client.publish("/Boot/AlarmSensor/BilgeWasser", "nass");}
else { client.publish("/Boot/AlarmSensor/BilgeWasser", "trocken");}


  

if (!client.connected()) {
        reconnect();
    }
    client.loop();
 
    snprintf (msg, 50, "Alive since %ld milliseconds", millis());
    Serial.print("Publish message: ");
    Serial.println(msg);
    
    client.publish("/Boot/AlarmSensor/MotorLuftQualitaet", Luft_ppm);
     client.publish("/Boot/AlarmSensor/MotorRaumTemperatur", Temperatur_Motor);
    client.publish("/Boot/AlarmSensor/WasserTemperatur", Temperatur_Wasser);
  
  delay(2000); 
} 
