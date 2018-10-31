/*
 * Andreas Wehner
 * 31.10.2018
 * 
 * Seriel Communication für die XY Steuerung
 * Sendet Kennbuchstaben (commandIndex, nur erster Buchstabe) und zwei Zahlen
 */

const int numChars = 256;
char receivedChars[numChars];

bool newData = false;

char commandIndex[32] = {0};
long numValue;

char commandIn;
long valueOneIn;
long valueTwoIn; 




void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("<Arduino is ready>");
}

void loop() {
  // put your main code here, to run repeatedly:
  recvWithEndMarker();
  delay(100);
  if(newData == true){
    parseData();
    showParsedData();
    newData = false;

    switch(commandIndex[0]){
      case 'M':
      Serial.println("Case M");
      break;
      
      case 'B':
      Serial.println("Case B");
      break;
    }
    
  }
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

