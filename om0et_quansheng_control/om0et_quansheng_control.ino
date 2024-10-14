// -----
// om0et_quansheng_control.ino - Rotary encoder based control of OM0ET Quansheng desktop transceiver
// -----
// 13.10.2014 created by Pavol OM0WT
// Created: "2024-10-13 21:25:38"
// Last-Modified: "2024-10-14 12:44:15"
// -----

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <ezButton.h>  // The library to use for SW pin
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


// Wemos MINI ESP32 D1
#define PIN_ENC_CLK  35   // Encoder CLK
#define PIN_ENC_DT 33     // Encoder DT
#define PIN_ENC_SW  23    // Encode SWN
#define PIN_OPT_UP   27   // IO27 - OPTO UP
#define PIN_OPT_DOWN 25   // IO25 - OPTO DOWN
#define PIN_OPT_BTN  32   // IO32 - OPTO BUTTON 
#define PIN_OPT_BTN_LONG 4 //IO04 - OPTO LONG PRESS BUTTON
#define VERSION 1.0

const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME  = 1000; // 1000 milliseconds

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
bool isShortDetected = false;
bool switchIt = false;
int controlPIN = 0;
int optoDelay = 100;   // OTPO delay between ON and OFF
int dir = 0;

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DT, RotaryEncoder::LatchMode::TWO03);
ezButton button(PIN_ENC_SW); 

void setup()
{
  button.setDebounceTime(70);
  Serial.begin(115200);
  while (! Serial);
  Serial.println("OM0ET Quansheng control");
  pinMode(PIN_OPT_BTN, OUTPUT);
  pinMode(PIN_OPT_UP, OUTPUT);
  pinMode(PIN_OPT_DOWN, OUTPUT);

} // setup()


// Read the current position of the encoder and print out when changed.
void loop()
{
  static int pos = 0;
  encoder.tick();
  button.loop();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    if ( (pos % 2) == 0 ) {
        // Whole turn
        dir = (int)(encoder.getDirection());
        if (dir == 1) {
            Serial.println("Pressed UP");
            optoDelay = 50;
            controlPIN = PIN_OPT_UP;
            switchIt = true;
            digitalWrite(controlPIN, LOW);
            digitalWrite(controlPIN, HIGH);
            delay(optoDelay);
            digitalWrite(controlPIN, LOW);
        }  
          
        if (dir == -1) {
            Serial.println("Pressed DOWN");
            optoDelay = 50;
            controlPIN = PIN_OPT_DOWN;
            switchIt = true;
            digitalWrite(controlPIN, LOW);
            digitalWrite(controlPIN, HIGH);
            delay(optoDelay);
            digitalWrite(controlPIN, LOW);
        }
    }
    pos = newPos; 
  } // if

  if(button.isPressed()){
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  }

  if(button.isReleased()) {
    isPressing = false;
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_PRESS_TIME ) {
       Serial.println("A short press is detected");
       controlPIN = PIN_OPT_BTN;
       optoDelay = 50;
       switchIt = true;
       digitalWrite(controlPIN, LOW);
       digitalWrite(controlPIN, HIGH);
       delay(optoDelay);
       digitalWrite(controlPIN, LOW);
    }

  }

  if(isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;

    if( pressDuration > LONG_PRESS_TIME ) {
      Serial.println("A long press is detected");
      isLongDetected = true;
      controlPIN = PIN_OPT_BTN;
      optoDelay = 200;
      digitalWrite(controlPIN, LOW);
      digitalWrite(controlPIN, HIGH);
      delay(optoDelay);
      digitalWrite(controlPIN, LOW);
    }
  }
  
  switchIt = false;
} // loop ()
// The End
