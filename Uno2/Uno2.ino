#include <Bounce2.h> //debounce buttons

#include <SoftwareSerial.h> // if need to use software serial instead of hardware serial
SoftwareSerial mySerial(A1, A0); //RX, TX, These pins will be used to send the data to another Arduino

//LCD SCREEN
#include <Wire.h> //for i2c
#include <LiquidCrystal_I2C.h> //lcd library

LiquidCrystal_I2C lcd(0x3F, 20, 4);  //MUNA AÐ TENGJA SDA Í A4 OG SCL Í A5
//

//
int lcdTimer = 100; //how often to update LCD
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

const char startOfTransmissionDelimiter = '<';
const char endOfTransmissionDelimiter   = '>';
const char nextSensorDelimiter = 'n';

unsigned int values[8] = {0, 1, 2, 3, 4, 5, 6, 7};
//0 MODE, 1 ROTARY1, 2 ROTARY2, 3 ROTARY3, 4 joyX, 5 joyY, 6 DIST, 7 takki

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
int buttonR1 = 2;
int buttonR2 = 5;
int buttonR3 = 8;
//

//pinnar fyrir joystick
int joyStickXPin = A2;
int joyStickYPin = A3;
//


//takkar instantiate
Bounce takki = Bounce();
Bounce r1button = Bounce();
Bounce r2button = Bounce();
Bounce r3button = Bounce();
//

void setup () {
  mySerial.begin (9600);
  Serial.begin(9600);

  lcd.init();
  lcd.clear();
  lcd.backlight();

  // Set encoder pins as inputs
  for (int i = 0; i < 3; i++) {
    pinMode(CLK[i], INPUT);
    pinMode(DT[i], INPUT);
    lastStateCLK[i] = digitalRead(CLK[i]);
  }
  //

  //DISTANCE SENSOR PINS
  pinMode(trigPin, OUTPUT); //distance sensor
  pinMode(echoPin, INPUT); //distance sensor
  //

  //takkar
  takki.attach(takkiPin, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  takki.interval(25); // Use a debounce interval of 25 milliseconds

  r1button.attach(buttonR1, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  r1button.interval(25); // Use a debounce interval of 25 milliseconds

  r2button.attach(buttonR2, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  r2button.interval(25); // Use a debounce interval of 25 milliseconds

  r3button.attach(buttonR3, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  r3button.interval(25); // Use a debounce interval of 25 milliseconds
  //


} // end of setup

void loop () {
  readButtons(); //read takki, rotary buttons
  if (values[7]) { //ef takki er on
    values[6] = pollDistanceSensor(); //updeita distance sensor í
  }
  readJoysticks();
  readEncoders();


  //  Serial.print("mode = ");
  //  Serial.println(values[0]);

  //  Serial.print("r1 = ");
  //  Serial.println(values[1]);
  //
  //  Serial.print("r2 = ");
  //  Serial.println(values[2]);
  //
  //  Serial.print("r3 = ");
  //  Serial.println(values[3]);
  //
  //  Serial.print("joyx = ");
  //  Serial.println(values[4]);
  //
  //  Serial.print("joyy = ");
  //  Serial.println(values[5]);
  //
  //  Serial.print("dist = ");
  //  Serial.println(values[6]);
  //
  //  Serial.print("takki = ");
  //  Serial.println(values[7]);

//  if (needToUpdate == true) {
//    lcdPrint();
//    Serial.println("lcd");
//  }
  if (millis() - lastTransmit >= transmitTimer) {
    sendValues();
    lastTransmit = millis();
  }
}  // end of loop


void lcdPrint() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(values[6]);
  lcd.print(" cm");

  lcd.setCursor(0, 1);
  lcd.print("Mode: ");
  lcd.print(values[0]);

  needToUpdate = false;
}



void readEncoders() {
  for (int i = 0; i < 3; i++) {
    // Read the current state of CLK
    currentStateCLK[i] = digitalRead(CLK[i]);

    // If last and current state of CLK are different, then pulse occurred
    // React to only 1 state change to avoid double count
    if (currentStateCLK[i] != lastStateCLK[i]  && currentStateCLK[i] == 1) {

      // If the DT state is different than the CLK state then
      // the encoder is rotating CCW so decrement
      if (digitalRead(DT[i]) != currentStateCLK[i]) {
        values[i + 1] ++;
        Serial.print("rotary ");
        Serial.println(i + 1);
        Serial.println(values[i + 1]);
        needToUpdate = true;
        //        currentDir = "CCW";
      } else {
        // Encoder is rotating CW so increment
        values[i + 1] --;
        Serial.print("rotary ");
        Serial.println(i + 1);
        Serial.println(values[i + 1]);
        needToUpdate = true;

        //        currentDir = "CW";
      }

      //      Serial.print("Direction: ");
      //      Serial.print(currentDir);
      //      Serial.print(" | Counter: ");
      //      Serial.println(values[i+1]);
    }

    // Remember last CLK state
    lastStateCLK[i] = currentStateCLK[i];
  }

}



void readJoysticks() {
  values[4] = analogRead(joyStickXPin);
  values[5] = analogRead(joyStickYPin);
}

void readButtons() {
  takki.update();
  r1button.update();
  r2button.update();
  r3button.update();

  if (r1button.fell()) {
    if (values[0] != 0) { //values[0] == mode
      values[0] = 0;
      Serial.print("Mode = ");
      Serial.println(mode);
      needToUpdate = true;

    }
  }

  if (r2button.fell()) {
    if (values[0] != 1) {
      values[0] = 1;
      Serial.print("Mode = ");
      Serial.println(mode);
      needToUpdate = true;

    }
  }

  if (r3button.fell()) {
    if (values[0] != 2) {
      values[0] = 2;
      Serial.print("Mode = ");
      Serial.println(mode);
      needToUpdate = true;

    }
  }

  if ( takki.changed() ) {
    int debouncedValue = takki.read();
    values[7] = debouncedValue;
    needToUpdate = true;

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


void sendValues() {
  //  Serial.println ("Start of transmission");

  mySerial.print (startOfTransmissionDelimiter);

  for (int i = 0; i < 8; i++) {
    mySerial.print(values[i]);
    mySerial.print(nextSensorDelimiter);
  }
  //    mySerial.print (la);    // send the number
  mySerial.print(endOfTransmissionDelimiter);
  mySerial.println ();

  //  Serial.println("End of transmission");
}
