#include <AccelStepper.h>
#include <MultiStepper.h>


const int dirPinX = 2;
const int stepPinX = 3;
const int sleepPinX = 4;
const int sixtheenStepX = 12;

const int dirPinY = 5;
const int stepPinY = 6;
const int sleepPinY = 7;
const int sixtheenStepY = 11;

AccelStepper stepperX(1, stepPinX, dirPinX);
AccelStepper stepperY(1, stepPinY, dirPinY);

MultiStepper steppers;

void setup() {
  Serial.begin(19200);

  pinMode(sixtheenStepX, OUTPUT);
  pinMode(sixtheenStepY, OUTPUT);

  digitalWrite(sixtheenStepX, HIGH);
  digitalWrite(sixtheenStepY, HIGH);
  
  stepperX.setMaxSpeed(2000);
  stepperY.setMaxSpeed(2000);

  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);

}

void loop() {
  // put your main code here, to run repeatedly:
  long positions[2];

  positions[0] = 500;
  positions[1] = 1000;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
  delay(1000);

  positions[0] = -200;
  positions[1] = -200;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
  delay(1000);
  

}
