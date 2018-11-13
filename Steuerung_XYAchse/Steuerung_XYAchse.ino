/*
 * Andreas Wehner
 * 30.10.2018
 * Steuerung der XY Achse und des Lasers.
 * - Anfahren von Endschaltern und festlegen des Arbeitsbereichs
 * - Bewegung relativ zur aktuellen Position
 * - Bewegung auf spezifische Position
 * - Erzeugen von Kreis mit unterschiedlichem Radius
 * - Erzeugen von Kreuz mit unterschiedlicher Kantenlänge
 * - Laser On / Off
 * - Serielle Kommunikation:
 *    + M,XXXX,YYYY     Bewegung zur Position X,Y
 *    + R,XXXX,YYYY     Relative Bewegung um X und Y
 *    + C               Bewegung zur Ausgangsstellung (7500, 7500)
 *    + K               Markiere Kreis und X-Target       (ACHTUNG, VERWENDET LASER!)
 *    + P               Konzentrische Kreise              (ACHTUNG, VERWENDET LASER!)
 *    + X               X-Target                          (ACHTUNG, VERWENDET LASER!)
 *    + V               Konzentrische Kreise und X-Target (ACHTUNG, VERWENDET LASER!)
 *    + L               Lock Motor
 *    + U               Unlock Motor
 *    + T               TriggerPunkte
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


const int numChars = 256;
char receivedChars[numChars];

bool newData = false;

char commandIndex[32] = {0};
long numValue;

char commandIn;
long valueOneIn;
long valueTwoIn; 
 
/*
 * /////////////////////////////////////////////////////////////////////////////
 *                             SETUP
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
  
}

/*
 * /////////////////////////////////////////////////////////////////////////////
 *                             MAIN LOOP
 * /////////////////////////////////////////////////////////////////////////////
 */
 
void loop() {
  recvWithEndMarker();
  delay(100);
  if(newData == true){
    parseData();
    // showParsedData();
    newData = false;

    switch(commandIndex[0]){
      case 'M':
      // Bewegung zur X / Y Koordinate
      Serial.println("Case M");
      Serial.print("Bewege auf Position: ");
      Serial.print(valueOneIn);
      Serial.print("/");
      Serial.println(valueTwoIn);
      moveXY(valueOneIn, valueTwoIn);
      break;
      
      case 'K':
      Serial.println("Case K");
      Serial.println("Markiere...");
      makeCircle(1000);
      makeXTarget(1000);

      case 'R':
      // Relative Bewegung zum letzten Standpunkt
      Serial.println("Case R");
      Serial.print("Bewege relativ um: ");
      Serial.print(valueOneIn);
      Serial.print("/");
      Serial.println(valueTwoIn);
      moveXYrelative(valueOneIn, valueTwoIn);
      break;

      case 'C':
      // Bewegung zur Mittelposition
      Serial.println("Case C");
      Serial.print("Bewege auf Mittelposition... ");
      moveXY(7500,7500);
      break;

      case 'P':
      // Konzentrische Kreise
      Serial.println("Case P");
      Serial.println("Markiere...");
      makeCircle(500);
      makeCircle(250);
      makeCircle(50);
      break;

      case 'X':
      // Ein X Target
      Serial.println("Case X");
      Serial.println("Markiere...");
      makeXTarget(500);
      break;

      case 'V':
      // Konzentrische Kreise mit einem X
      Serial.println("Case V");
      Serial.println("Markiere...");
      makeCircle(500);
      makeCircle(250);
      makeCircle(50);
      makeXTarget(500);
      break;

      case 'L':
      // Lock Motors (Motoren an)
      Serial.println("Case L");
      Serial.println("Motoren an...");
      motorsOn();
      break;

      case 'U':
      // Unlock Motors (Motoren aus)
      Serial.println("Case U");
      Serial.println("Motoren aus...");
      motorsOff();
      break;


      case 'T':
      // Trigger Positionen setzen
      Serial.println("Case T");
      Serial.println("Fahre Endpunkte an.");
      moveXToTrigger();
      stepperX.setCurrentPosition(200);
      delay(2000);
      moveXTo(7500);      // Mitte X Achse

      moveYToTrigger();
      stepperY.setCurrentPosition(-1500);
      moveYTo(7500);
      break;



    }
    
  }
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
  while(i <= 6.8){
    long dX = long(cos(i)*radius);
    long dY = long(sin(i)*radius);
    positions[0] = xPos+ dX;
    positions[1] = yPos + dY;
    steppers.moveTo(positions);
    steppers.runSpeedToPosition();
    if(i == 0){
      laserOn();
    }
    i =  i + 0.005;
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
  delay(100);
  moveXY(posX, posY + diameter);
  laserOff();
  delay(20);
  moveXY(posX - diameter, posY);
  laserOn();
  delay(100);
  moveXY(posX + diameter, posY);
  laserOff();
  delay(20);
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


void recvWithEndMarker(){
  static int ndx = 0;
  char endMarker = '\n';
  char rc;

  while(Serial.available() > 0 && newData == false){
    rc = Serial.read();

    if(rc != endMarker){
      receivedChars[ndx] = rc; 
      ndx++;
      if(ndx >= numChars){
        ndx = numChars - 1;
      }
    }else{
      receivedChars[ndx] = '\0';  // Terminate String
      ndx = 0;
      newData = true;
    }
  }
}

void parseData(){
  // Daten splitten bei Trennzeichen ","
  char * strtokIndx; // commandIndex für strtok()

  strtokIndx = strtok(receivedChars,","); // Ersten Teil
  strcpy(commandIndex, strtokIndx);

  strtokIndx = strtok(NULL, ",");
  valueOneIn = atol(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  valueTwoIn = atol(strtokIndx);
    
}

void showParsedData(){
  Serial.print("Befehl: ");
  Serial.println(commandIndex);
  Serial.print("Value1: ");
  Serial.println(valueOneIn);
  Serial.print("Value2: ");
  Serial.println(valueTwoIn);
}

