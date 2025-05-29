//
// Created by Jude Pereira on 25/05/2025.
//
#include <Arduino.h>
#include "FastAccelStepper.h"

#define X_AXIS_DIR_PIN 5
#define X_AXIS_ENABLE_PIN 4
#define X_AXIS_STEP_PIN 3

#define Y_AXIS_DIR_PIN 6
#define Y_AXIS_ENABLE_PIN 7
#define Y_AXIS_STEP_PIN 8

#define HEAD_DIR_PIN 10
#define HEAD_ENABLE_PIN 12
#define HEAD_STEP_PIN 11

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

uint64_t lastStatusPrint = millis();

void setup() {
    Serial.begin(115200);
    while (!Serial);
    engine.init();
    stepper = engine.stepperConnectToPin(stepPinStepper);
    if (stepper) {
        stepper->setDirectionPin(dirPinStepper);
        stepper->setEnablePin(enablePinStepper);
        stepper->setAutoEnable(true);

        stepper->setSpeedInHz(2400);
        stepper->setAcceleration(2400);
    }

    pinMode(5, PinMode::INPUT_PULLUP)
}

void loop() {
    if (millis() - lastStatusPrint > 1000) {
//        Serial.println("Alive!");
        lastStatusPrint = millis();
    }

    while (Serial.available()) {
        delay(2);  //delay to allow byte to arrive in input buffer
        uint8_t c = Serial.read();
        Serial.print("Read ");
        Serial.println(c);
        if (c == 65) { // Move back
            stepper->move(800);
        } else if (c == 66) { // Move forward
//            Serial.println(stepper->getCurrentPosition() - 200);
            stepper->move(-800);
        }
        break;
    }
}
