/*
   (C) 2018 Thorsten Gurzan - beelogger.de
 
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program. If not, see http://www.gnu.org/licenses/.
*/
 
// beelogger.de - Arduino Datenlogger für Imker
// Erläuterungen dieses Programmcodes unter http://beelogger.de
 
 
#include "HX711.h"

HX711 scale(4, 5);
 
char c;
long Gewicht = 999999;                      
long LetztesGewicht = 0; 
const long Max_Abweichung = 500;
 
 
long Taragewicht = 1;
float Skalierung = 1.0;
 
// hier kann das Kalibriergewicht in Gramm eingetragen werden
long KalibrierGewicht = 1000;  // 1000 = 1kG Referenzgewicht
 
 
// Für beelogger-Solar: Aktivierung der Stromversorgung für Sensoren, Module und DS3231
#define Power_Pin 2
 
void setup() {
  Serial.begin(9600);
  Serial.println("Waage Kalibrierung");
  Serial.println();
  digitalWrite(Power_Pin, HIGH);
  pinMode(Power_Pin, OUTPUT);
 
  delay(5000);
  scale.set_gain(32); 
  Serial.println("Zur Kalibrierung der Stockwaage bitte den Anweisungen folgen!");
  Serial.println();
  Serial.println("Waage ohne Gewicht - Kalibrierung mit '1' und 'Enter' starten!");
  Serial.println();
  while (c != '1') {
    c = Serial.read();
    yield();

  };
  c = 'x';
  Serial.println();
  Serial.print("Kalibriere ... ");
  scale.set_scale();
  Serial.print("  ...  ");
  delay(100);
  scale.read_average(20);
  Serial.println("  ...");
  Taragewicht = scale.read_average(20);
  Serial.print("Waage mit genau ");
  Serial.print(KalibrierGewicht);
  Serial.println(" Gramm beschweren - Kalibrierung mit '2' und 'Enter' starten!");
  Serial.println();
  while (c != '2') {
    c = Serial.read();
    yield();
  };
  Serial.println();
  Serial.print("Kalibriere ... ");
  scale.set_offset(Taragewicht);
  Serial.print("  ...  ");
  delay(100);
  scale.get_units(20);
  Serial.println("  ...");
  Skalierung = ((scale.get_units(20)) / KalibrierGewicht);
  scale.set_scale(Skalierung);
  Serial.print("Pruefe Gewicht: ");
  Serial.println(scale.get_units(20), 1);
  Serial.println();
  Serial.print("Taragewicht: ");
  Serial.println(Taragewicht);
  Serial.println();
  Serial.print("Skalierung: ");
  Serial.println(Skalierung);
  Serial.println();
}
 
void loop() {
  for (byte j = 0 ; j < 3; j++) { // Anzahl der Wiederholungen, wenn Abweichung zum letzten Gewicht zu hoch
    Serial.println("Messung wird vorgenommen ...");
    Gewicht = (long) scale.get_units(10);
    if ( ((Gewicht - LetztesGewicht) < Max_Abweichung) and ((LetztesGewicht - Gewicht) < Max_Abweichung) ) break; // Abweichung für Fehlererkennung
    Serial.println("Warte auf stabilen Messwert ...");
    if (j < 3) delay(3000);  // Wartezeit zwischen Wiederholungen
  }
  LetztesGewicht = Gewicht;
  Serial.print("Gewicht: ");
  Serial.print(Gewicht);
  Serial.println(" g");
  delay(5000);
  Serial.print("Skalierung: ");
  Serial.println(Skalierung);
  Serial.print("Taragewicht: ");
  Serial.println(Taragewicht);
  delay(5000);
}
