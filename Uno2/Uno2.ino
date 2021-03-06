#include <Bounce2.h> //debounce buttons

#include <SoftwareSerial.h> // if need to use software serial instead of hardware serial
SoftwareSerial mySerial(A1, A0); //RX, TX, These pins will be used to send the data to another Arduino

//LCD SCREEN
#include <Wire.h> //for i2c
#include <LiquidCrystal_I2C.h> //lcd library

LiquidCrystal_I2C lcd(0x3F, 20, 4);  //MUNA AÐ TENGJA SDA Í A4 OG SCL Í A5
//

//
int lcdTimer = 80; //how often to update LCD
int lastPrint = 0;
bool needToUpdate = true;
//

//
int transmitTimer = 50;
int lastTransmit = 0;

// Rotary Encoder Inputs og dot
int CLK[3] = {4, 7, 10};
int DT[3] = {3, 6, 9};

int counter[3] = {0, 0, 0};
int currentStateCLK[3];
int lastStateCLK[3];
//

const char* effectName[3] = {"Chorus", "Delay", "Reverb",}; // const char* array for loop names on sd card
const char* param0[3] = {"Voices", "DTime", "RoomSize",}; // const char* array for loop names on sd card
const char* param1[3] = {"", "Feedback", "Damping",}; // const char* array for loop names on sd card
const char* param2[3] = {"DRY/WET", "DRY/WET", "DRY/WET",}; // const char* array for loop names on sd card


const char startOfTransmissionDelimiter = '<';
const char endOfTransmissionDelimiter   = '>';
const char nextSensorDelimiter = 'n';

unsigned int values[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int tempArray[8];
//0 ROTARY1, 1 ROTARY2, 2 ROTARY3, 3 MODE, 4 joyX, 5 joyY, 6 DIST, 7 takki

int iterator = 0;

//DISTANCE SENSOR variables
const int trigPin = 11;
const int echoPin = 12;
long duration;
int distanceCm = 0;
//

//mode stilling
int mode = 0;
//

//pinnar fyrir alla takka
int takkiPin = 13;
//

//pinnar fyrir joystick
int joyStickXPin = A2;
int joyStickYPin = A3;
//


//takkar instantiate
Bounce takki = Bounce();
//

void setup () {
  mySerial.begin (9600);
  Serial.begin(9600);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  //DISTANCE SENSOR PINS
  pinMode(trigPin, OUTPUT); //distance sensor
  pinMode(echoPin, INPUT); //distance sensor
  //

  //takkar
  takki.attach(takkiPin, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  takki.interval(25); // Use a debounce interval of 25 milliseconds
  //


} // end of setup

void loop () {
  while (Serial.available ()) {
    processInput ();
  }


  readButtons(); //read takki, rotary buttons
  if (values[7]) { //ef takki er on
    values[6] = pollDistanceSensor(); //updeita distance sensor í
  }
  readJoysticks();

  //    if (millis() - lastPrint >= lcdTimer) {
  if (needToUpdate == true) {
    lcdPrint();
    needToUpdate = false;
    //    Serial.println("lcd");
  }
  if (millis() - lastTransmit >= transmitTimer) {
    sendValues();
    lastTransmit = millis();
  }
}  // end of loop


void lcdPrint() {
  //
  //  const char* effectName[3] = {"Chorus", "Delay", "Reverb",}; // const char* array for loop names on sd card
  //  const char* param0[3] = {"Voices", "Time", "RSize",}; // const char* array for loop names on sd card
  //  const char* param1[3] = {"", "Feedbk", "Damping",}; // const char* array for loop names on sd card
  //  const char* param2[3] = {"Wet", "Wet", "Wet",}; // const char* array for loop names on sd card
  //setCursor(x,y);

  lcd.clear();
  lcd.setCursor(12, 0);
  lcd.print(effectName[mode]);

  lcd.setCursor(0, 1);
  lcd.print(param0[mode]);
  lcd.setCursor(0, 2);
  lcd.print(param1[mode]);
  lcd.setCursor(0, 3);
  lcd.print(param2[mode]);

  lcd.setCursor(15, 1);
  lcd.print(values[0]); //param0
  lcd.setCursor(15, 2);
  lcd.print(values[1]); //param1
  lcd.setCursor(15, 3);
  lcd.print(values[2]); //param2


}


void readJoysticks() {
  values[4] = analogRead(joyStickXPin);
  values[5] = analogRead(joyStickYPin);
}

void readButtons() {
  takki.update();
  if ( takki.changed() ) {
    int debouncedValue = takki.read();
    values[7] = debouncedValue;
    //    needToUpdate = true;
  }
}


int pollDistanceSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  int distCm = duration * 0.034 / 2;
  needToUpdate = true;

  return distCm;
}

//
//
//FOR SENDING INFO TO TEENSY

void sendValues() {
  //  Serial.println ("Start of transmission");

  Serial.print (startOfTransmissionDelimiter);

  for (int i = 0; i < 8; i++) {
    Serial.print(values[i]);
    Serial.print(nextSensorDelimiter);
  }
  //    mySerial.print (la);    // send the number
  Serial.print(endOfTransmissionDelimiter);
  Serial.println ();

  //  Serial.println("End of transmission");
}


//
//
//FOR RECEIVING DATA FROM PRO MICRO

void processNumber (const long n) {
  tempArray[iterator++] = n;
  //Serial.println (n);
}  // end of processNumber

void processArrays() {
  for (int i = 0; i < 4; i++) {
    if (values[i] != tempArray[i]) {
      values[i] = tempArray[i];
      needToUpdate = true;
    }
    mode = values[3];
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
