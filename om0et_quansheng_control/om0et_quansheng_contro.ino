// -----
// SimplePollRotator.ino - Example for the RotaryEncoder library.
// This class is implemented for use with the Arduino environment.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD 3-Clause License. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 04.02.2021 conditions and settings added for ESP8266
// -----

// This example checks the state of the rotary encoder in the loop() function.
// The current position and direction is printed on output when changed.

// Hardware setup:
// Attach a rotary encoder with output pins to
// * A2 and A3 on Arduino UNO.
// * D5 and D6 on ESP8266 board (e.g. NodeMCU).
// Swap the pins when direction is detected wrong.
// The common contact should be attached to ground.

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <ezButton.h>  // The library to use for SW pin
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input signals on pin 2 and 3
#define PIN_IN1 D11
#define PIN_IN2 D12

#elif defined(ESP8266)
// Example for ESP8266 NodeMCU with input signals on pin D5 and D6
#define PIN_IN1 D5
#define PIN_IN2 D6

#endif
// Arduino NANO
//#define PIN_IN1 11
//#define PIN_IN2 12
//#define SW_PIN 10
//#define PIN_UP 9
//#define PIN_DOWN 8

// Wemos MINI ESP32 D1
#define PIN_ENC_CLK  35   // Encoder CLK
#define PIN_ENC_DT 33     // Encoder DT
#define PIN_ENC_SW  23    // Encode SWN
#define PIN_OPT_UP   27   // IO27 - OPTO UP
#define PIN_OPT_DOWN 25   // IO25 - OPTO DOWN
#define PIN_OPT_BTN  32   // IO32 - OPTO BUTTON 
#define PIN_OPT_BTN_LONG 4 //IO04 - OPTO LONG PRESS BUTTON

const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME  = 1000; // 1000 milliseconds

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
bool isShortDetected = false;
bool isControlling = false; // OPTO is controlling....
bool switchIt = false;
int controlPIN = 0;
int optoDelay = 500;   // OTPO delay between ON and OFF
int dir = 0;

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_ENC_CLK, PIN_ENC_DT, RotaryEncoder::LatchMode::TWO03);
ezButton button(PIN_ENC_SW); 

void setup()
{
  button.setDebounceTime(70);
  Serial.begin(115200);
  while (! Serial);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");
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
  //    Serial.print("pos:");
  //    Serial.print(newPos);
  //    Serial.print(" dir:");
  //    Serial.println((int)(encoder.getDirection()));
      
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
