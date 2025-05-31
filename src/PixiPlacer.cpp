//
// Created by Jude Pereira on 31/05/2025.
//

#include "PixiPlacer.h"

#define PIXI_LIMIT_X_START_PIN 16
#define PIXI_LIMIT_X_END_PIN 17

// Two sets of limit for the Y axis. Perhaps StallGuard would've been a better solution...
#define PIXI_LIMIT_Y1_START_PIN 18
#define PIXI_LIMIT_Y1_END_PIN 19
#define PIXI_LIMIT_Y2_START_PIN 20
#define PIXI_LIMIT_Y2_END_PIN 21

#define PIXI_X_AXIS_DIR_PIN 5
#define PIXI_X_AXIS_ENABLE_PIN 4
#define PIXI_X_AXIS_STEP_PIN 3

#define PIXI_Y_AXIS_DIR_PIN 6
#define PIXI_Y_AXIS_ENABLE_PIN 7
#define PIXI_Y_AXIS_STEP_PIN 8

#define PIXI_HEAD_DIR_PIN 10
#define PIXI_HEAD_ENABLE_PIN 12
#define PIXI_HEAD_STEP_PIN 11

#define PIXI_STEPPER_SPEED_BEFORE_HOMING    1000

void PixiPlacer::init() {
    engine.init();

    stepperX = engine.stepperConnectToPin(PIXI_X_AXIS_STEP_PIN);
    stepperY = engine.stepperConnectToPin(PIXI_Y_AXIS_STEP_PIN);
    stepperHead = engine.stepperConnectToPin(PIXI_HEAD_STEP_PIN);

    if (stepperX && stepperY && stepperHead) {
        stepperX->setDirectionPin(PIXI_X_AXIS_DIR_PIN);
        stepperX->setEnablePin(PIXI_X_AXIS_ENABLE_PIN);

        stepperY->setDirectionPin(PIXI_Y_AXIS_DIR_PIN);
        stepperY->setEnablePin(PIXI_Y_AXIS_ENABLE_PIN);

        stepperHead->setDirectionPin(PIXI_HEAD_DIR_PIN);
        stepperHead->setEnablePin(PIXI_HEAD_ENABLE_PIN);

        for (FastAccelStepper *stepper: {stepperX, stepperY, stepperHead}) {
            stepper->setAutoEnable(true);
            stepper->setSpeedInHz(PIXI_STEPPER_SPEED_BEFORE_HOMING);
            stepper->setAcceleration(PIXI_STEPPER_SPEED_BEFORE_HOMING);
        }
    } else {
        die("One or more stepper motors failed to initialise!");
    }

    for (uint8_t pin: {PIXI_LIMIT_X_START_PIN, PIXI_LIMIT_X_END_PIN,
                       PIXI_LIMIT_Y1_START_PIN, PIXI_LIMIT_Y1_END_PIN,
                       PIXI_LIMIT_Y2_START_PIN, PIXI_LIMIT_Y2_END_PIN}) {
        pinMode(pin, PinMode::INPUT_PULLUP);
    }
}

void PixiPlacer::loop() {


}

void PixiPlacer::die(String reason) {
    while (true) {
        Serial.println(reason);
        delay(5000);
    }
}
