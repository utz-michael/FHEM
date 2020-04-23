
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HX711.h> 
 HX711 scale(4,5);

const char* SSID = "Connection1";
const char* PSK = ".Sonja00";
const char* MQTT_BROKER = "192.168.192.2";
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

float Abwasser_float=0;
float Frischwasser_float=0;
static char Abwasser[15];
static char Frischwasser[15];

long TaragewichtA = -231091; // Hier ist der Wert aus der Kalibrierung für Kanal A einzutragen 
float SkalierungA = -11.3; // Hier ist der Wert aus der Kalibrierung für Kanal A einzutragen 
 
long TaragewichtB = -37520; // Hier ist der Wert aus der Kalibrierung für Kanal B einzutragen 
float SkalierungB = -4.79; // Hier ist der Wert aus der Kalibrierung für Kanal B einzutragen
 
long GewichtA = 999999;
long GewichtB = 999999;     

int FrischwasserRAW = 0;
int AbwasserRAW = 0;

long KalibrierungA = 15800;// 
long KalibrierungB = 28400;// 
//long KalibrierungA = 17300;//  -3%
//long KalibrierungB = 30400;//  -4%
//float FuellmengeA = 0.5; // angabe in 1 %  bei 50 l also 0.5
//float FuellmengeB = 0.5;
float FuellmengeA = 0.75; // angabe in 1 %  bei 50 l also 0.5
float FuellmengeB = 0.25;

#define filterSamples   9              // filterSamples should  be an odd number, no smaller than 3
int sensSmoothArray1 [filterSamples];   // array for holding raw sensor values for sensor1 
int sensSmoothArray2 [filterSamples];   // array for holding raw sensor values for sensor2 

void setup() { 
  Serial.begin(9600);
   
  setup_wifi();
  client.setServer(MQTT_BROKER, 1883);
  
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
      // if (!client.connect("Sensor2")) {
      
      if (!client.connect("Sensor2","/Boot/Tank/Frischwasser",0, true,"offline")) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}
 
void loop() { 

  scale.set_gain(128); 
  scale.set_offset(TaragewichtA);  
  scale.set_scale(SkalierungA); 
  GewichtA= scale.get_units(10); 
 
  scale.set_gain(32); 
  scale.set_offset(TaragewichtB);  
  scale.set_scale(SkalierungB); 
  GewichtB= scale.get_units(10); 
 
  Serial.print("Gewicht Kanal A: ");
  Serial.print(GewichtA);
  Serial.println(" g");
 
  Serial.print("Gewicht Kanal B: ");
  Serial.print(GewichtB);
  Serial.println(" g");
  
  ///////////////////////////////////umrechnung gewicht in füllstand %/////////////////////
  
  // FrischwasserRAW=((GewichtA-KalibrierungA)/1000)/ FuellmengeA;
  // AbwasserRAW=((GewichtB-KalibrierungB)/1000)/ FuellmengeB;
   FrischwasserRAW=GewichtA;
   AbwasserRAW=GewichtB;
  Serial.print("% Kanal A: ");
  Serial.print(FrischwasserRAW);
  Serial.println(" %");
 
  Serial.print("% Kanal B: ");
  Serial.print(AbwasserRAW);
  Serial.println(" %");



  
  
 Frischwasser_float = digitalSmooth(FrischwasserRAW, sensSmoothArray1) ; 
 Abwasser_float = digitalSmooth(AbwasserRAW, sensSmoothArray2) ;
 
  



  

if (!client.connected()) {
        reconnect();
    }
    client.loop();
 
   
   
    static unsigned long prev_millis;
    if (millis() - prev_millis > 10000) {
  
           snprintf (msg, 50, "Alive since %ld milliseconds", millis());
           Serial.print("Publish message: ");
           Serial.println(msg);
      
          Serial.print ("Frischwasser: ");
          Serial.println(Frischwasser_float);
          Serial.print ("Abwasser: ");
          Serial.println(Abwasser_float);
  
 
          dtostrf(Frischwasser_float, 6, 0, Frischwasser);
         dtostrf(Abwasser_float, 6, 0, Abwasser);
      
       
       client.publish("/Boot/Tank/Frischwasser", Frischwasser);
        client.publish("/Boot/Tank/Abwasser", Abwasser);
        prev_millis = millis();
    }

 
    
  
} 


int digitalSmooth(int rawIn, int *sensSmoothArray){     // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
 // static int raw[filterSamples];
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j=0; j<filterSamples; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

/*
  for (j = 0; j < (filterSamples); j++){    // print the array to debug
    Serial.print(sorted[j]); 
    Serial.print("   "); 
  }
  Serial.println();
*/

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1); 
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 
    // Serial.print(sorted[j]); 
    // Serial.print("   "); 
  }

//  Serial.println();
//  Serial.print("average = ");
//  Serial.println(total/k);
  return total / k;    // divide by number of samples
}
