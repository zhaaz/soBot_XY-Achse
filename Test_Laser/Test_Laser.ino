/*
 * Andreas Wehner
 * 30.10.2018
 * Einfacher Test des Lasers auf dem XY Board
 * 
 */
const int laserPin = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(laserPin, OUTPUT);
  delay(1000);
  
  digitalWrite(laserPin, HIGH);
  delay(1000);
  digitalWrite(laserPin, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
 
  
}
