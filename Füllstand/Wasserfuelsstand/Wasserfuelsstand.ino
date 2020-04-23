/*
 * (C) 2017 Thorsten Gurzan - beelogger.de
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
 
// beelogger.de - Arduino Datenlogger für Imker
// Erläuterungen dieses Programmcodes unter http://beelogger.de
 
#include <HX711.h> 
HX711 scale(4, 5);
 
long TaragewichtA = 1; // Hier ist der Wert aus der Kalibrierung für Kanal A einzutragen 
float SkalierungA = 1; // Hier ist der Wert aus der Kalibrierung für Kanal A einzutragen 
 
long TaragewichtB = 1; // Hier ist der Wert aus der Kalibrierung für Kanal B einzutragen 
float SkalierungB = 1; // Hier ist der Wert aus der Kalibrierung für Kanal B einzutragen
 
long GewichtA = 999999;
long GewichtB = 999999;                     
 
 
void setup() { 
  Serial.begin(9600); 
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
 
  delay(3000);
}
