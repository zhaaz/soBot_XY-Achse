#include <AccelStepper.h>

AccelStepper Zaxis(1,6,5);  // (Modus, StepPin, DirPin)

void setup(){
  Zaxis.setMaxSpeed(2000);   // Steps pro Sekunde
  Zaxis.setAcceleration(2000);
  Zaxis.setSpeed(500);
  
}

void loop(){
  Zaxis.runSpeed();
}
