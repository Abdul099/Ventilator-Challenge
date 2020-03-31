/*
   Ventilator Challenge: MEGA UI Script
   Author: Abdullatif Hassan
   Date Started: March 23, 2020
   Last Modified: March 28, 2020

   Descrpition: The main aim behind this sketch is to control the LCD screen and the UI controls. SPI communication is used between an Arduino MEGA
                and digital pins are used to communicate with a rotary encoder and a button both responsible for the user controls. The LCD screen follows
                a state machine design where each state is a screen menu or notification.

*/

#define sclk 52  // Don't change
#define mosi 51  // Don't change
#define cs   9
#define dc   8
#define rst  7  // you can also connect this to the Arduino reset
#define ANSWERSIZE 5
#define BatteryPin A0

#include <Adafruit_GFX_AS.h>    // Core graphics library
#include <Adafruit_ST7735_AS.h> // Hardware-specific library
#include <SPI.h>
#include <Encoder.h>
#include <String.h>
#include <Wire.h>

Adafruit_ST7735_AS tft = Adafruit_ST7735_AS(cs, dc, rst);       // Invoke custom library
Encoder myEnc(5, 6);

byte state = 0;
byte substate = 0;
bool encoderButton = 1;
bool pendingMessage;
bool flip = 0; //boolean used to change states
int oldPosition  = -999;
uint8_t defaultO2 = 60;
byte EP;
byte IP;
byte bpm;
byte battery;

void checkExitState() {
  if (encoderButton == 0) {
    state = 0;
    delay(100);
    flip = 1;
  }
}

int checkBattery() { 
  float val = analogRead(BatteryPin);
  //val = 1000; //mock value
  if (val > (3.7 / 5) * 1023) return 3;
  if (((3.3 / 5) * 1023) < val && val < (3.7 / 5) * 1023) return 2;
  if (((3 / 5) * 1023) < val && val < (3.3 / 5) * 1023) return 1;
  return 0;
}

void readFromUno() {
  // while (0 < Wire.available()) {
  byte x = Wire.read();
  Serial.print("This is x:");
  Serial.println(x);
  byte y = Wire.read();
  Serial.print("This is y:");
  Serial.println(y);
  byte z = Wire.read();
  Serial.print("This is z:");
  Serial.println(z);
  byte a = Wire.read();
  Serial.print("This is a:");
  Serial.println(a);
  if (x != 100) {
    state = x;
    flip = 1;
  }
  if (x == 100 && state == 1) {
    state = 1;
    flip = 1;
  }
  // values to update on the monitor
  IP = y;
  EP = z;
  bpm = a;

  //}
}

void writeToUno(String answer) {
  // Setup byte variable in the correct size
  byte response[ANSWERSIZE];

  // Format answer as array
  for (byte i = 0; i < ANSWERSIZE; i++) {
    response[i] = (byte)answer.charAt(i);
  }
  Wire.write(response, sizeof(response));
}

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  Serial.begin(9600);
  Wire.begin(9);
  

  // Function to run when data requested from master
  Wire.onRequest(writeToUno);

  // Function to run when data received from master
  Wire.onReceive(readFromUno);

  pinMode(12, INPUT);
  pinMode(30, INPUT);
}

void loop() {
  int newPosition = myEnc.read();
  encoderButton = digitalRead(12);
  if (newPosition - oldPosition > 4 || oldPosition - newPosition > 4 || !encoderButton || flip == 1) {
    flip = 0;
    if (newPosition - oldPosition > 4) substate = (substate + 3 - 1) % 3;
    if (oldPosition - newPosition > 4) substate = (substate + 1) % 3;
    oldPosition = newPosition;
    tft.fillRect (0, 0, 200, 200, ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE, ST7735_WHITE); // Note these fonts do not plot the background colour
    tft.setCursor (80, 5);
    battery = checkBattery();
    if (battery == 0) state = 10;
    if (battery == 1) tft.print("Battery |..");
    if (battery == 2) tft.print("Battery ||.");
    if (battery == 3) tft.print("Battery |||");

    switch (state) {

      case 0: //default menu
        tft.setTextColor(ST7735_WHITE, ST7735_WHITE);
        if (substate == 0) {
          tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
          if (encoderButton == 0) {
            state = 3;
            // delay(100);
            flip = 1;
            break;
          }
        }
        tft.drawCentreString("CALIBRATE", 75, 30, 4);
        tft.setTextColor(ST7735_WHITE, ST7735_WHITE);
        if (substate == 1) {
          tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
          if (encoderButton == 0) {
            state = 2;
            //   delay(100);
            flip = 1;
            break;
          }
        }
        tft.drawCentreString("SET FiO2", 60, 60, 4);
        tft.setTextColor(ST7735_WHITE, ST7735_WHITE);
        if (substate == 2) {
          tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
          if (encoderButton == 0) {
            state = 1;
            //   delay(100);
            flip = 1;
            break;
          }
        }
        tft.drawCentreString("Monitor", 50, 90, 4);
        break;

      case 1:
        if (encoderButton == 0) {
          state = 0;
          delay(100);
          flip = 1;
          break;
        }
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawNumber(bpm, 10, 18, 4);
        tft.drawCentreString("bpm", 80, 18, 4);
        tft.drawNumber(defaultO2, 10, 45, 4);
        tft.drawCentreString("%", 55, 45, 4);
        tft.drawNumber(IP, 10, 70, 4);
        tft.drawCentreString("mmH20", 90, 70, 4);
        tft.drawNumber(EP, 10, 95, 4);
        tft.drawCentreString("mmH20", 90, 95, 4);
        break;

      case 2:
        if (encoderButton == 0) {
          state = 0;
          delay(100);
          flip = 1;
          break;
        }
        tft.setTextColor(ST7735_RED, ST7735_RED);
        tft.drawCentreString("FiO2", 60, 30, 4);
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("60 %", 60, 70, 4);

        while (encoderButton == 1) {
          newPosition = myEnc.read();
          if (oldPosition - newPosition > 3 || newPosition - oldPosition > 3) {
            if (oldPosition - newPosition > 3) defaultO2 += 10;
            if (newPosition - oldPosition > 3) defaultO2 -= 10;
            if (defaultO2 >= 100) defaultO2 = 100;
            if (defaultO2 <= 20) defaultO2 = 20;
            oldPosition = newPosition;
            tft.fillRect (0, 0, 200, 200, ST7735_BLACK);
            tft.setTextColor(ST7735_RED, ST7735_RED);
            tft.drawCentreString("FiO2", 60, 30, 4);
            tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
            tft.drawNumber(defaultO2, 40, 70, 4);
            tft.drawCentreString("%", 90, 70, 4);
          }
          encoderButton = digitalRead(12);
        }
        break;

      case 3: //calibration
        if (encoderButton == 0) {
          state = 0;
          delay(100);
          flip = 1;
          break;
        }
        tft.setTextColor(ST7735_BLUE, ST7735_BLUE);
        tft.drawCentreString("Starting Calibration...", 60, 30, 2);
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("will fix that later", 50, 60, 2);
        break;

      case 4://Low O2 warning
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Low Oxygen", 35, 60, 4);
        break;

      case 5://Low Respiration Rate Warning
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Bradipnea", 50, 70, 4);
        break;

      case 6://High Respiration Rate Warning
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Tachypnea", 50, 70, 4);
        break;

      case 7://Low expiratory pressure warning
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Low Expiratory Pressure", 50, 70, 2);
        break;

      case 8://Valve Malfunction
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Valve Malfunction", 50, 70, 2);
        break;

      case 9://Disconnection warning
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Disconnection", 50, 70, 2);
        break;

      case 10://Low Battery warning
        checkExitState();
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("LOW BATTERY", 50, 70, 2);
        break;
    }
  }
}
