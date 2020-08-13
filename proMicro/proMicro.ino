#include <Bounce2.h>
Bounce button0 = Bounce();
Bounce button1 = Bounce();
Bounce button2 = Bounce();

#include <SoftwareSerial.h>
SoftwareSerial mySerial(14, 9); //RX, TX, These pins will be used to send the data to another Arduino

const char startOfTransmissionDelimiter = '<';
const char endOfTransmissionDelimiter   = '>';
const char nextSensorDelimiter = 'n';

unsigned int values[4] = {0, 1, 2, 17};
//0 ROTARY1, 1 ROTARY2, 2 ROTARY3, 3 MODE

//mode stilling
int mode = 0;


//long oldPot0[3] = { -999, -999, -999};
//long oldPot1[3] = { -999, -999, -999};
//long oldPot2[3] = { -999, -999, -999};

long oldPot0[3] = {0, 0, 0};
long oldPot1[3] = {0, 0, 0};
long oldPot2[3] = {0, 0, 0};

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc0(2, 7);
Encoder myEnc1(0, 1);
Encoder myEnc2(3, 4);
//   avoid using pins with LEDs attached

void setup () {

  Serial.begin(9600);
  button0.attach(5, INPUT_PULLUP);
  button0.interval(5);
  
    button1.attach(6, INPUT_PULLUP);
    button1.interval(5);

  button2.attach(8, INPUT_PULLUP);
  button2.interval(5);

  mySerial.begin(9600);


} // end of setup

void loop () {

  button0.update();
  button1.update();
  button2.update();

  if (button0.fell()) {
    if (mode != 0) {
      values[3] = 0;

      mode = 0;
      //      setMode(mode);
    }
    values[0] = oldPot0[mode];
    values[1] = oldPot1[mode];
    values[2] = oldPot2[mode];
    myEnc0.write(values[0]);
    myEnc1.write(values[1]);
    myEnc2.write(values[2]);
    sendValues();
  }

  if (button1.fell()) {
    if (mode != 1) {
      mode = 1;
      values[3] = 1;
    }

    values[0] = oldPot0[mode];
    values[1] = oldPot1[mode];
    values[2] = oldPot2[mode];
    myEnc0.write(values[0]);
    myEnc1.write(values[1]);
    myEnc2.write(values[2]);
    sendValues();
  }

  if (button2.fell()) {
    if (mode != 2) {
      values[3] = 2;
      mode = 2;

      //      setMode(mode);
    }
    values[0] = oldPot0[mode];
    values[1] = oldPot1[mode];
    values[2] = oldPot2[mode];
    myEnc0.write(values[0]);
    myEnc1.write(values[1]);
    myEnc2.write(values[2]);
    sendValues();
  }



  long newPosition0 = myEnc0.read();
  if (newPosition0 > 200) {
    myEnc0.write(200);
    newPosition0 = 200;
  }
  if (newPosition0 < 0) {
    myEnc0.write(0);
    newPosition0 = 0;
  }

  long newPosition1 = myEnc1.read();
  if (newPosition1 > 200) {
    myEnc1.write(200);
    newPosition1 = 200;
  }
  if (newPosition1 < 0) {
    myEnc1.write(0);
    newPosition1 = 0;
  }

  long newPosition2 = myEnc2.read();
  if (newPosition2 > 200) {
    myEnc2.write(200);
    newPosition2 = 200;
  }
  if (newPosition2 < 0) {
    myEnc2.write(0);
    newPosition2 = 0;
  }

  if (newPosition0 != oldPot0[mode]) {
    oldPot0[mode] = newPosition0;
    values[0] = newPosition0;
    sendValues();
  }

  if (newPosition1 != oldPot1[mode]) {
    oldPot1[mode] = newPosition1;
    values[1] = newPosition1;
    sendValues();
  }

  if (newPosition2 != oldPot2[mode]) {
    oldPot2[mode] = newPosition2;
    values[2] = newPosition2;
    sendValues();
  }
}  // end of loop



//void setMode(int newMode) {
//  mode = newMode;
//  values[4] = mode;
//  myEnc0.write(oldPot0[mode]);
//  myEnc1.write(oldPot1[mode]);
//  myEnc2.write(oldPot2[mode]);
//  sendValues();
//}


void sendValues() {
  //  Serial.println ("Start of transmission");

  mySerial.print (startOfTransmissionDelimiter);

  for (int i = 0; i < 4; i++) {
    mySerial.print(values[i]);
    mySerial.print(nextSensorDelimiter);
  }
  //    mySerial.print (la);    // send the number
  mySerial.print(endOfTransmissionDelimiter);
  mySerial.println ();

  //  Serial.println("End of transmission");
}