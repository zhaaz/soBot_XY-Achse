/*
 * Endschalter Beispiel für XY Achse (Schalter Y Achse, Telefonkabel)
 * Andreas Wehner
 * 29.11.2018
 * 
 */// define pins numbers
  const int triggerPin = 8;
  int triggerVal = 0;

void setup() {
  pinMode(triggerPin, INPUT);
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:

  triggerVal = digitalRead(triggerPin);
  Serial.println(triggerVal);
  delay(10);
// triggerPin = 1 -> Trigger nicht ausgelöst
// triggerPin = 0 -> Trigger ausgelöst
}
