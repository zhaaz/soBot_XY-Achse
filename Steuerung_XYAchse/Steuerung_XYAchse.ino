/*
 * Andreas Wehner
 * 30.10.2018
 * Steuerung der XY Achse und des Lasers.
 * - Anfahren von Endschaltern und festlegen des Arbeitsbereichs
 * - Bewegung relativ zur aktuellen Position
 * - Bewegung auf spezifische Position
 * - Erzeugen von Kreis mit unterschiedlichem Radius
 * - Erzeugen von Kreuz mit unterschiedlicher Kantenlänge
 * - laser On / Off
 */
 
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

const int laserPin = 10;

AccelStepper stepperX(1, stepPinX, dirPinX);
AccelStepper stepperY(1, stepPinY, dirPinY);

MultiStepper steppers;

const int triggerPinY = 9;
int triggerValY = 0;

const int triggerPinX = 8;
int triggerValX = 0;

long SCHRANKE_X = 15000;
long SCHRANKE_Y = 15000;
 
/*
 * /////////////////////////////////////////////////////////////////////////////
 *                             MAIN LOOP
 * /////////////////////////////////////////////////////////////////////////////
 */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  // Motoroen langsam bewegen (1/16 Schritt)
  pinMode(sixtheenStepX, OUTPUT);
  pinMode(sixtheenStepY, OUTPUT);
  digitalWrite(sixtheenStepX, HIGH);
  digitalWrite(sixtheenStepY, HIGH);

  // X Achse
  stepperX.setMaxSpeed(2400);
  stepperX.setAcceleration(4800);
  stepperX.setSpeed(400);
  stepperX.setEnablePin(sleepPinX);
  stepperX.disableOutputs();

  stepperY.setMaxSpeed(2400);
  stepperY.setAcceleration(4800);
  stepperY.setSpeed(400);
  stepperY.setEnablePin(sleepPinY);
  stepperY.disableOutputs();


  
  pinMode(triggerPinX, INPUT);
  pinMode(triggerPinY, INPUT);

  pinMode(laserPin, OUTPUT);
  laserOff();

  steppers.addStepper(stepperX);
  steppers.addStepper(stepperY);

  moveXToTrigger();
  stepperX.setCurrentPosition(200);
  delay(2000);
  moveXTo(7500);      // Mitte X Achse
//
//  moveYToTrigger();
//  stepperY.setCurrentPosition(-1500);
//  moveYTo(7500);
//
//  delay(2000);
//  makeCircle(2000);


//  delay(2000);
//  makeXTarget(500);
//  makeCircle(500);

  
}

/*
 * /////////////////////////////////////////////////////////////////////////////
 *                             MAIN LOOP
 * /////////////////////////////////////////////////////////////////////////////
 */
 
void loop() {
  
}

/*
 * /////////////////////////////////////////////////////////////////////////////
 *                             METHODEN
 * /////////////////////////////////////////////////////////////////////////////
 */
 
void moveXToTrigger(){
  stepperX.setSpeed(2400);
  stepperX.enableOutputs();
  while(digitalRead(triggerPinX) == 1){
    stepperX.runSpeed();
  }
  stepperX.disableOutputs();
}

void moveXTo(long posX){
  if(posX >= 0 && posX <= SCHRANKE_X){ 
    posX = -posX;    // für Arbeitsbereich
    stepperX.setSpeed(2400);
    stepperX.enableOutputs();
    stepperX.runToNewPosition(posX);
    stepperX.run();
    stepperX.disableOutputs();
  }
}

void moveYToTrigger(){
  stepperY.setSpeed(-2400);
  stepperY.enableOutputs();
  while(digitalRead(triggerPinY) == 1){
    stepperY.runSpeed();
  }
  stepperY.disableOutputs();
}

void moveYTo(long posY){
  if(posY >= 0 && posY <= SCHRANKE_Y){    
    stepperY.setSpeed(2400);
    stepperY.enableOutputs();
    stepperY.runToNewPosition(posY);
    stepperY.run();
    stepperY.disableOutputs();
  }
}


void moveXY(long posX, long posY){
  motorsOn();
  if((posX >= 0 && posX <= SCHRANKE_X)&&(posY >= 0 && posY <= SCHRANKE_Y)){
    long positions[2];
    positions[0] = -posX;
    positions[1] = posY;
    steppers.moveTo(positions);
    steppers.runSpeedToPosition();
  }
  motorsOff();
}

void moveXYrelative(long xToGo, long yToGo){
  long actualXpos = stepperX.currentPosition();
  long actualYpos = stepperY.currentPosition();
  long posToGoX = -actualXpos + xToGo;
  long posToGoY = actualYpos + yToGo;
  moveXY(posToGoX, posToGoY);
}

void makeCircle(long radius){
   
  long xPos = stepperX.currentPosition();
  long yPos = stepperY.currentPosition();
  float i = 0;
  motorsOn();
  long positions[2];
  while(i <= 6.4){
    long dX = long(cos(i)*radius);
    long dY = long(sin(i)*radius);
    positions[0] = xPos + dX;
    positions[1] = yPos + dY;
    steppers.moveTo(positions);
    steppers.runSpeedToPosition();
    if(i == 0){
      laserOn();
    }
    i =  i + 0.01;
  }
  laserOff();
  positions[0] = xPos;  // Wieder auf Ausgangsposition
  positions[1] = yPos;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition();
  motorsOff();
}

void makeXTarget(long diameter){
  // Diameter am besten so wie bei Kreis  
  stepperX.setSpeed(800);
  stepperY.setSpeed(800);
  
  long posX = stepperX.currentPosition()*(-1);
  long posY = stepperY.currentPosition();
 
  moveXY(posX, posY - diameter);
  laserOn();
  delay(20);
  moveXY(posX, posY + diameter);
  laserOff();
  moveXY(posX - diameter, posY);
  laserOn();
  delay(20);
  moveXY(posX + diameter, posY);
  laserOff();
  moveXY(posX, posY);
}

void motorsOn(){
  stepperX.enableOutputs();
  stepperY.enableOutputs();
}

void motorsOff(){
  stepperX.disableOutputs();
  stepperY.disableOutputs();
}

void laserOn(){
  digitalWrite(laserPin, HIGH);
}

void laserOff(){
  digitalWrite(laserPin, LOW);
}
