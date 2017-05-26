//
// Copyright 2017, Jason S. McMullan <jason.mcmullan@gmail.com>
//

#include <Arduino.h>
#include <TimerOne.h>

#include "Stepper.h"
#include "Feeder.h"

#define PIN_STEP    6
#define PIN_DIR     5
#define PIN_ENABLE  3

#define STEP_DELAY_MS   5

Stepper motor(PIN_ENABLE, PIN_DIR, PIN_STEP);

StepQueue queue = StepQueue(&motor, STEP_DELAY_MS);

Feeder feeder = Feeder(&queue,-5,55);

//Adafruit_Joystick joy = Adafruit_Joystick(JOY_PIN);

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//UserInterface ui = UserInterface(&feeder, &tft, ST7735_TFTWIDTH, ST7735_TFTHEIGHT_18, 0, 0);

void update_isr()
{
    queue.update_isr();
}

void setup()
{
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(PIN_STEP, OUTPUT);
    motor.begin();

    Serial.begin(115200);
    delay(2000);
    Serial.println("And so it begins...");

#if 0
    digitalWrite(PIN_ENABLE, false);
    digitalWrite(PIN_DIR, false);
    for (;;)
    {
        delay(1);
        digitalWrite(PIN_STEP, true);
        delay(1);
        digitalWrite(PIN_STEP, false);
        delay(5);
    }
#endif

    for (;;)
    {
        queue.append(100);
        for (int i = 0; i < 1000; i++)
        {
            delay(1);
            queue.update_isr();
        }

        delay(5000);
    }

    feeder.interval(5000);
    feeder.count(2);

//    Timer1.initialize(1000);
//    Timer1.attachInterrupt(update_isr);
}

void loop()
{
    // Update feeder every 10ms
    feeder.update();
    delay(1);
    queue.update_isr();
}
