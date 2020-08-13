// Example of receiving numbers by Serial
// Author: Nick Gammon
//// Date: 31 March 2012
//
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(4, 5); //RX, TX, These pins will be used to send the data to another Arduino

//LCD SCREEN
#include <Wire.h> //for i2c
#include <LiquidCrystal_I2C.h> //lcd library
LiquidCrystal_I2C lcd(0x3F, 20, 4);  //MUNA AÐ TENGJA SDA Í A4 OG SCL Í A5


const char startOfTransmissionDelimiter = '<';
const char endOfTransmissionDelimiter   = '>';
const char nextSensorDelimiter = 'n';

unsigned int tempArray[8] = {0,0,0,0,0,0,0,0};
unsigned int values[8] = {1,2,3,4,5,6,7,8};

int iterator = 0;

unsigned long lastPrint = 0;
int interVal = 50;

void setup () {
  
  lcd.init();
  lcd.clear();
  lcd.backlight();

  processArrays();
  
  Serial.begin(9600);
  //Serial.println ("Starting ...");
} // end of setup

void processNumber (const long n) {
  tempArray[iterator++] = n;
  //Serial.println (n);
}  // end of processNumber

void processArrays(){
  for(int i = 0; i < 8; i++){
    if(values[i] != tempArray[i]){
      values[i] = tempArray[i];
    }
  }
  
  for(int i = 0; i < 8; i++){
//    Serial.print("Array at spot: ");
//    Serial.print(i);
//    Serial.print(" is: ");
//    Serial.println(values[i]);
  }
}

void processInput () {
  static long receivedNumber = 0;
  byte c = Serial.read ();

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

  while (Serial.available ()){
    processInput ();
  }
  if(millis() - lastPrint > interVal){
  lcdPrint();
  lastPrint = millis();
  }

  // do other stuff here
} // end of loop


void lcdPrint() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("0: ");
  lcd.print(values[0]);

  lcd.setCursor(0, 1);
  lcd.print("1: ");
  lcd.print(values[1]);

  lcd.setCursor(0,2);
  lcd.print("2: ");
  lcd.print(values[2]);

  lcd.setCursor(0,3);
  lcd.print("Mode: ");
  lcd.print(values[3]);

}
