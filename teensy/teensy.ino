// Example of receiving numbers by Serial
// Author: Nick Gammon
//// Date: 31 March 2012
//
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(4, 5); //RX, TX, These pins will be used to send the data to another Arduino


const char startOfTransmissionDelimiter = '<';
const char endOfTransmissionDelimiter   = '>';
const char nextSensorDelimiter = 'n';

unsigned int tempArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int values[8] = {1, 2, 3, 4, 5, 6, 7, 8};
//0 ROTARY1, 1 ROTARY2, 2 ROTARY3, 3 MODE, 4 joyX, 5 joyY, 6 DIST, 7 dist on/off
unsigned int oldValues[] = {
                            {1, 2, 3},
                            {1, 2, 3},
                            {1, 2, 3},
                            };

//oldValues[mode][param];

//if(values[0] != oldValues[values[mode][0]){
pottur 0 á mode breyttist
//}

int iterator = 0;

unsigned long lastPrint = 0;
int interVal = 50;

void setup () {

  processArrays();

  Serial.begin(9600);
  Serial1.begin(9600);
  //Serial.println ("Starting ...");
} // end of setup

void processNumber (const long n) {
  tempArray[iterator++] = n;
  //Serial.println (n);
}  // end of processNumber

void processArrays() {
  for (int i = 0; i < 8; i++) {
    if (values[i] != tempArray[i]) {
      values[i] = tempArray[i];
    }
  }

  for (int i = 0; i < 8; i++) {
    //    Serial.print("Array at spot: ");
    //    Serial.print(i);
    //    Serial.print(" is: ");
    //    Serial.println(values[i]);
  }
}

void processInput () {
  static long receivedNumber = 0;
  byte c = Serial1.read ();

  switch (c) {

    case endOfTransmissionDelimiter:
      //      Serial.println("end of transmission");
      iterator = 0;
      processArrays();
      break;

    case nextSensorDelimiter:
      processNumber (receivedNumber);

    // fall through to start a new number
    case startOfTransmissionDelimiter:
      receivedNumber = 0;
      break;

    case '0' ... '9':
      receivedNumber *= 10;
      receivedNumber += c - '0';
      break;

  } // end of switch
}  // end of processInput

void loop () {

  while (Serial1.available ()) {
    processInput ();
  }
  if (millis() - lastPrint > interVal) {
    printTheStuff();
    lastPrint = millis();
  }

  // do other stuff here
} // end of loop

void printTheStuff() {
  Serial.print("0: ");
  Serial.println(values[0]);

  Serial.print("1: ");
  Serial.println(values[1]);

  Serial.print("2: ");
  Serial.println(values[2]);

  Serial.print("3: ");
  Serial.println(values[3]);

  Serial.print("4: ");
  Serial.println(values[4]);

  Serial.print("5: ");
  Serial.println(values[5]);

  Serial.print("6: ");
  Serial.println(values[6]);

  Serial.print("7: ");
  Serial.println(values[7]);

}
