// Demonstrate the use of a function to generate the NMEA 8-bit CRC
// - A message is built and placed into a buffer string
// - The checksum is computed, and formatted to two chars
// - The full string is printed to the IDE monitor
//   EX1:  $test*   crc = 16
//   EX2:  $GPRMC,023405.00,A,1827.23072,N,06958.07877,W,1.631,33.83,230613,,,A*  crc = 42

// 2017-10-19 streamlined this to be a clearer example, posted to github

const byte buff_len = 90;
char CRCbuffer[buff_len];

// create pre-defined strings to control flexible output formatting
String sp = " ";
String delim = ",";
String splat = "*";
String msg = "";

int position = 0 ;
int position_old = 0;
int x=0;
int xx=0;


unsigned long previousMillis = 0; // speichert den Zeitpunkt an dem zuletzt geschalten wurde
const long interval = 500; // Länge der Pause in ms


// -----------------------------------------------------------------------
void setup() {
  Serial.begin(4800); // Init and set rate for serial output
  // wait for serial port to connect. Needed for Leonardo and Teensyduino 3
  while (!Serial && (millis() < 5000)) {} // include timeout if print console isn't opened
pinMode (2, INPUT_PULLUP);
pinMode (3, INPUT_PULLUP);
pinMode (4, INPUT_PULLUP);
pinMode (5, INPUT_PULLUP);
pinMode (6, INPUT_PULLUP);
pinMode (7, INPUT_PULLUP);
pinMode (8, INPUT_PULLUP);
pinMode (9, INPUT_PULLUP);
  
}

// -----------------------------------------------------------------------
void loop() {
  // build msg 

/*

//Magnet oben 
if (digitalRead(2)==LOW){position = 0;}
if (digitalRead(3)==LOW){position = 45;}
if (digitalRead(4)==LOW){position = 90;}
if (digitalRead(5)==LOW){position = 135;}
if (digitalRead(6)==LOW){position = 180;}
if (digitalRead(7)==LOW){position = 225;}
if (digitalRead(8)==LOW){position = 270;}
if (digitalRead(9)==LOW){position = 315;}
*/
if (digitalRead(2)==LOW){position = 180;}
if (digitalRead(3)==LOW){position = 225;}
if (digitalRead(4)==LOW){position = 270;}
if (digitalRead(5)==LOW){position = 315;}
if (digitalRead(6)==LOW){position = 0;}
if (digitalRead(7)==LOW){position = 45;}
if (digitalRead(8)==LOW){position = 90;}
if (digitalRead(9)==LOW){position = 135;}




if (position == 0 && x <= 270 && x >= - 270 ) { x = 0;} 
 
if (  position_old != position && position_old == 315 && position == 0 ) { 
  position_old = position;
  x = x + 45;
  }

if (  position_old != position && position_old == 0 && position == 315 ) { 
  position_old = position;
  x = x - 45;
  } 

if ( position_old != position && position_old ==135 && position == 225 ) { 
  position_old = position;
  x = x + 90;
  }
else  if ( position_old != position && position_old < position) { 
  position_old = position;
  x = x + 45;
  }

if ( position_old != position && position_old == 225 && position == 135 ) { 
  position_old = position;
  x = x - 90;
  }
else  if ( position_old != position && position_old > position) { 
  position_old = position;
  x = x - 45;
  }  
  /*
Serial.println(position);
Serial.println(position_old);
Serial.println(x);
*/
unsigned long currentMillis = millis(); // Aktuelle Zeit wird in currentMillis gespeichert
  if (currentMillis - previousMillis >= interval) { // Falls mehr als 1000 ms vergangen sind
     previousMillis = currentMillis; // Zeitpunkt der letzten Schaltung wird festgehalten 


xx = map(x, -450, 450, -40, 40);
  /*Serial.println(digitalRead(2));
  Serial.println(digitalRead(3));
  Serial.println(digitalRead(4));
  Serial.println(digitalRead(5));
  Serial.println(digitalRead(6));
  Serial.println(digitalRead(7));
  Serial.println(digitalRead(8));
  Serial.println(digitalRead(9));
  */
  char strX[8];
  char strY[8];
   
  float y = 20;
  String cmd = "$IIRSA";    // a command name
  String cmd1 = "A";    // a command name
  dtostrf(xx, 4, 1, strX);   // format float value to string XX.X
  dtostrf(y, 4, 1, strY);
  msg = cmd + delim + strX + delim + cmd1 + delim + strY + delim + cmd1 + splat;
  outputMsg(msg); // print the entire message string, and append the CRC
  }
}

// -----------------------------------------------------------------------
byte convertToCRC(char *buff) {
  // NMEA CRC: XOR each byte with previous for all chars between '$' and '*'
  char c;
  byte i;
  byte start_with = 0;    // index of starting char in msg
  byte end_with = 0;      // index of starting char in msg
  byte crc = 0;

  for (i = 0; i < buff_len; i++) {
    c = buff[i];
    if (c == '$') {
      start_with = i;
    }
    if (c == '*') {
      end_with = i;
    }
  }
  if (end_with > start_with) {
    for (i = start_with + 1; i < end_with; i++) { // XOR every character between '$' and '*'
      crc = crc ^ buff[i] ;  // xor the next char
    }
  }
  else { // else if error, print a msg  
    Serial.println("CRC ERROR");
  }
  return crc;
  
  // based on code by Elimeléc López - July-19th-2013
}

// -----------------------------------------------------------------------
void outputMsg(String msg) {
  msg.toCharArray(CRCbuffer, sizeof(CRCbuffer)); // put complete string into CRCbuffer
  byte crc = convertToCRC(CRCbuffer); // call function to compute the crc value

  Serial.print(msg);
  if (crc < 16) Serial.print("0"); // add leading 0 if needed
  Serial.println(crc, HEX);
 
}
