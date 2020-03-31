/*
   Ventilator Challenge: UNO Script
   Author: Abdullatif Hassan
   Date Started: March 25, 2020
   Last Modified: March 31, 2020

   Descrpition: This is the sketch that controls the Arduino UNO board uses in the Ventilator System. The aim behind it is to implement the control system that
                will be used to control the operation of the vebtilator. Due to the lack of sensor data at the time being, mock data is used throughout the implementation. I2C communication
                is used to interface between the Arduino UNO, and an Arduino Mega controlling the User Interface.


*/

#define SLAVE_ADDR 9
#define ServoPin 9
#define MP5050_1PIN A0
#define MP5050_2PIN A1
#define MP5100PIN A2

#include <Wire.h>
#include<Servo.h>

bool start = 1;//1 for started 0 for OFF
int bpm;
int Targetbpm;
float IP;//might use ints for pressure instead
float targetIP;
float EP;
bool expValve;//1 for open 0 for closed
long targetTime;
long lastBreathStart;
long criticalIP;
long neededEP;
int motorPosition;//mock variable for motor control 0--> resting position
String response = "";
byte battery = 3;
byte maxMotor = 180;
Servo motor;
const int offset = 41; // zero pressure calibrate
const int fullScale = 962; // max pressure calibrate
float pressure1; // MPX5050
float pressure2; // MPX5050
float pressure3; // MPX5100

void calibrate() { // calibration function --> will work on later
}

void printToMega(int state) {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(state);
  Wire.write((byte)IP);
  Wire.write((byte)EP);
  Wire.write((byte)bpm);
  Wire.endTransmission();
}

void readFromMega() {
  Wire.requestFrom(SLAVE_ADDR, 5);
  response = "";
  while (Wire.available()) {
    char b = Wire.read();
    response += b;
  }
  Serial.println(response);
}


void setup() {
  Wire.begin();
  Serial.begin(9600);
  motor.attach(ServoPin, 1000, 2000);
  bpm = 20;
  targetIP = 40;
  expValve = 1;
  lastBreathStart = -(60 / Targetbpm) * 1000;
  motorPosition = 0;
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  calibrate();
}

void loop() {
  //step 1
  if (start) {
    //step 2: need to read inputs from sensors, now using mock data
    
    Targetbpm = 23;
    IP = 3;
    EP = 5;

    //step 3: the actual cycle
    //3a

    if (millis() - lastBreathStart > (60 / Targetbpm) * 1000 || IP < -5) { //if enough time has passed from last breath or if patient attempts to breath inspiratory pressure below -5 cmH20
      expValve = 0; //3b close
      if (IP < targetIP) {
        bpm = 60/(millis() - lastBreathStart);
        lastBreathStart = millis(); //might change the location of this line of code
      }
      while (IP < targetIP) {//3c: deliver breath as long as IP is still low

        if (IP >= criticalIP) {
          Serial.println("Warning High Inspiratory Pressure"); //this will be sent to the lcd screen in later versions
          printToMega(9); // add extra error menu
          break;
        }
        else {
          int difference = targetIP - IP;
          int deliver = (difference/targetIP)*maxMotor;
          motor.write(deliver); // make p controller 
        }

      }
      motor.write(0); //reset motor to resting position
      expValve = 1; //open expiratory valve
      delay(1000);//will adjust the value later
      if (EP < neededEP) {
        Serial.println("Warning Low  Pressure");
        printToMega(7);
      }

      IP = IP + 1; //mock IP reading
      bpm = 18; //mock data --> might calculate bpm somewhere else
      EP = 20;//mock data
      Serial.println("Printing to lcd"); // communicate with other arduino --> will work on it later
    }
    //end of step 3 (cycle)--> go back to step 1

  }
  if (bpm < 5) printToMega(5); //low breathing rate Warning
  else if (bpm > 40) printToMega(6); //high breathing rate warning
  printToMega(100);
  delay(1000); // will remove
}
