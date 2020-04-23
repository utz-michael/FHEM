/*
    Victron.Arduino-ESP8266
    A:Pim Rutgers
    E:pim@physee.eu

    Code to grab data from the VE.Direct-Protocol on Arduino / ESP8266.
    Tested on NodeMCU v1.0
,
    The fields of the serial commands are configured in "config.h"

*/

#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* SSID = "Connection1";
const char* PSK = ".Sonja00";
const char* MQTT_BROKER = "192.168.192.2";
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
//int value = 0;
float CE_float=0;
float SOC_float=0;
float Strom_float=0;
float TTG_float=0;
float Volt_float=0;
float VoltStarter_float=0;
float Temperatur_Luft_float=0;
float Temperatur_Kajuete_float=0;
static char Temperatur_Luft[15];
static char Temperatur_Kajuete[15];

#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);










// Serial variables
 int rxPin=4;
 int txPin=5;                                    // TX Not used
//SoftwareSerial victronSerial(rxPin, txPin);         // RX, TX Using Software Serial so we can use the hardware serial to check the ouput
                                                    // via the USB serial provided by the NodeMCU.
char receivedChars[buffsize];                       // an array to store the received data
char tempChars[buffsize];                           // an array to manipulate the received data
char recv_label[num_keywords][label_bytes]  = {0};  // {0} tells the compiler to initalize it with 0. 
char recv_value[num_keywords][value_bytes]  = {0};  // That does not mean it is filled with 0's
char value_mqtt[num_keywords][value_bytes]       = {0};  // The array that holds the verified data
static byte blockindex = 0;
bool new_data = false;
bool blockend = false;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(19200);
   // victronSerial.begin(19200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
   sensors.begin();
}

void setup_wifi() {
    delay(10);
   // Serial.println();
   // Serial.print("Connecting to ");
   //Serial.println(SSID);
 
    WiFi.begin(SSID, PSK);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
   //    Serial.print(".");
    }
 
   // Serial.println("");
  //  Serial.println("WiFi connected");
   // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
}
 
void reconnect() {
    while (!client.connected()) {
       // Serial.print("Reconnecting...");
 if (!client.connect("ESP8266Client","/Boot/Batterie/Volt",0, true,"offline")) {
       // if (!client.connect("ESP8266Client")) {
           // Serial.print("failed, rc=");
           // Serial.print(client.state());
           // Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}









void loop() {
 
  
    // Receive information on Serial from MPPT
    RecvWithEndMarker();
    HandleNewData();

    // Just print the values every second,
    // Add your own code here to use the data. 
    // Make sure to not used delay(X)s of bigger than 50ms,
    // so make use of the same principle used in PrintEverySecond() 
    // or use some sort of Alarm/Timer Library
    PrintEverySecond();
}

// Serial Handling
// ---
// This block handles the serial reception of the data in a 
// non blocking way. It checks the Serial line for characters and 
// parses them in fields. If a block of data is send, which always ends
// with "Checksum" field, the whole block is checked and if deemed correct
// copied to the 'value' array. 

void RecvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial.available() > 0 && new_data == false) {
        rc = Serial.read();
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= buffsize) {
                ndx = buffsize - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            new_data = true;
        }
        yield();
    }
}

void HandleNewData() {
    // We have gotten a field of data 
    if (new_data == true) {
        //Copy it to the temp array because parseData will alter it.
        strcpy(tempChars, receivedChars);
        ParseData();
        new_data = false;
    }
}

void ParseData() {
    char * strtokIndx; // this is used by strtok() as an index
    strtokIndx = strtok(tempChars,"\t");      // get the first part - the label
    // The last field of a block is always the Checksum
    if (strcmp(strtokIndx, "Checksum") == 0) {
        blockend = true;
    }
    strcpy(recv_label[blockindex], strtokIndx); // copy it to label
    
    // Now get the value
    strtokIndx = strtok(NULL, "\r");    // This continues where the previous call left off until '/r'.
    if (strtokIndx != NULL) {           // We need to check here if we don't receive NULL.
        strcpy(recv_value[blockindex], strtokIndx);
    }
    blockindex++;

    if (blockend) {
        // We got a whole block into the received data.
        // Check if the data received is not corrupted.
        // Sum off all received bytes should be 0;
        byte checksum = 0;
        for (int xx = 0; xx < blockindex; xx++) {
            // Loop over the labels and value gotten and add them.
            // Using a byte so the the % 256 is integrated. 
            char *vv = recv_value[xx];
            char *ll = recv_label[xx];
            while (*vv) {
                checksum += *vv;
                vv++;
            }
            while (*ll) {
                checksum+= *ll;
                ll++;
            }
            // Because we strip the new line(10), the carriage return(13) and 
            // the horizontal tab(9) we add them here again.  
            checksum += 32;
        }
        // Checksum should be 0, so if !0 we have correct data.
        if (!checksum) {
            // Since we are getting blocks that are part of a 
            // keyword chain, but are not certain where it starts
            // we look for the corresponding label. This loop has a trick
            // that will start searching for the next label at the start of the last
            // hit, which should optimize it. 
            int start = 0;
            for (int ii = 0; ii < blockindex; ii++) {
              for (int jj = start; (jj - start) < num_keywords; jj++) {
                if (strcmp(recv_label[ii], keywords[jj % num_keywords]) == 0) {
                  // found the label, copy it to the value array
                  strcpy(value_mqtt[jj], recv_value[ii]);
                  start = (jj + 1) % num_keywords; // start searching the next one at this hit +1
                  break;
                }
              }
            }
        }
        // Reset the block index, and make sure we clear blockend.
        blockindex = 0;
        blockend = false;
    }
}

void PrintEverySecond() {
    static unsigned long prev_millis;
    if (millis() - prev_millis > 10000) {
        PrintValues();
        prev_millis = millis();
    }
}


void PrintValues() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  Temperatur_Luft_float=sensors.getTempCByIndex(0);
  Temperatur_Kajuete_float=sensors.getTempCByIndex(1);
  //Serial.print(Temperatur_Luft_float);
  //Serial.print(Temperatur_Kajuete_float);
   // for (int i = 0; i < num_keywords; i++){
   //     Serial.print(keywords[i]);
   //     Serial.print(",");
   //     Serial.println(value_mqtt[i]);
   CE_float=atof(value_mqtt[3])/1000.0;
   SOC_float=atof(value_mqtt[4])/10.0;
   Strom_float=atof(value_mqtt[2])/1000.0;
   TTG_float=atof(value_mqtt[5])/60.0;
   Volt_float=atof(value_mqtt[0])/1000.0;
   VoltStarter_float=atof(value_mqtt[1])/1000.0;
   dtostrf(CE_float, 6, 1, value_mqtt[3]);
   dtostrf(SOC_float, 6, 1, value_mqtt[4]);
   dtostrf(Strom_float, 6, 2, value_mqtt[2]);
   dtostrf(TTG_float, 6, 1, value_mqtt[5]);
   dtostrf(Volt_float, 6, 2, value_mqtt[0]);
   dtostrf(VoltStarter_float, 6, 2, value_mqtt[1]);
   dtostrf(Temperatur_Luft_float, 6, 2, Temperatur_Luft);
   dtostrf(Temperatur_Kajuete_float, 6, 2, Temperatur_Kajuete);
   
   
    client.publish("/Boot/Batterie/CE", value_mqtt[3]);
    client.publish("/Boot/Batterie/SOC", value_mqtt[4]);
    client.publish("/Boot/Batterie/Strom", value_mqtt[2]);
    client.publish("/Boot/Batterie/TTG", value_mqtt[5]);
    client.publish("/Boot/Batterie/Volt", value_mqtt[0]);
    client.publish("/Boot/Batterie/VoltStarter", value_mqtt[1]);
    client.publish("/Boot/Batterie/Temperatur_Luft", Temperatur_Luft);
    client.publish("/Boot/Batterie/Temperatur_Kajuete", Temperatur_Kajuete);


        
        if (!client.connected()) {
        reconnect();
    }
    client.loop();
    //}
}
