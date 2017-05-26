//
// Copyright 2017, Jason S. McMullan <jason.mcmullan@gmail.com>
//

#include <Arduino.h>
#include <TimerOne.h>

#include "Stepper.h"
#include "Feeder.h"
#include "UserInterfaceLCD.h"

#define PIN_STEP    6
#define PIN_DIR     5
#define PIN_ENABLE  A0
#define PIN_BUTTON  3

#define STEP_DELAY_MS 10

Stepper motor(PIN_ENABLE, PIN_DIR, PIN_STEP);

StepQueue queue = StepQueue(&motor, STEP_DELAY_MS);

Button button(PIN_BUTTON);

Feeder feeder = Feeder(&queue,-15,50 + 15);

UserInterfaceLCD ui = UserInterfaceLCD(&feeder, &button, 30000 /* menu timeout in ms */);

void update_isr()
{
    queue.update_isr();
}

void setup()
{
    pinMode(PIN_ENABLE, OUTPUT);
    pinMode(PIN_DIR, OUTPUT);
    pinMode(PIN_STEP, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    motor.begin();
    feeder.begin();

    Serial.begin(115200);
    delay(2000);
    Serial.println("And so it begins...");

    // Update stepper every 1ms
    Timer1.initialize(1000);
    Timer1.attachInterrupt(update_isr);

    ui.begin();
}

bool last_button;
void loop()
{
    // Update every 250ms
    delay(250);

    // Input
    button.update();

    // Processing
    ui.update();

    // Outputs
    feeder.update();
}
