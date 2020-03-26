#define sclk 52  // Don't change
#define mosi 51  // Don't change
#define cs   9
#define dc   8
#define rst  7  // you can also connect this to the Arduino reset
#include <Adafruit_GFX_AS.h>    // Core graphics library
#include <Adafruit_ST7735_AS.h> // Hardware-specific library
#include <SPI.h>
#include <Encoder.h>
#include <String.h>

Adafruit_ST7735_AS tft = Adafruit_ST7735_AS(cs, dc, rst);       // Invoke custom library
Encoder myEnc(5, 6);

byte state = 0;
byte substate = 0;
bool encoderButton = 1;
bool flip = 0;
int oldPosition  = -999;
uint8_t defaultO2 = 60;
char fromUNO[10];
char toUNO[10];

void readFromUno() {
  Serial.readBytes(fromUNO, 10); //Read the serial data and store in var
}

void setup(void) {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  Serial.begin(9600);
  pinMode(12, INPUT);
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
    tft.print("Battery ||.");
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
        tft.drawCentreString("18 bpm", 50, 18, 4);
        tft.drawCentreString("60%", 35, 45, 4);
        tft.drawCentreString("40 mmH20", 70, 70, 4);
        tft.drawCentreString("20 mmH20", 70, 95, 4);
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
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Low Oxygen", 35, 60, 4);
        break;

      case 5://Low Respiration Rate Warning
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Bradipnea", 50, 70, 4);
        break;

      case 6://High Respiration Rate Warning
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Tachypnea", 50, 70, 4);
        break;

      case 7://Low expiratory pressure warning
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Low Expiratory Pressure", 50, 70, 2);
        break;

      case 8://Valve Malfunction
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Valve Malfunction", 50, 70, 2);
        break;


      case 9://Disconnection warning
        tft.setTextColor(ST7735_YELLOW, ST7735_YELLOW);
        tft.drawCentreString("WARNING!", 60, 30, 4);
        tft.drawCentreString("Disconnection", 50, 70, 2);
        break;
    }


  }
}
