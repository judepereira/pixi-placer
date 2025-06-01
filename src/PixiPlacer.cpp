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
#define PIXI_STEPPER_SPEED_AFTER_HOMING    6000

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

    xAxis.name = "X";
    yAxis.name = "Y";

    homing = true;
}

void PixiPlacer::loop() {
    if (homing) {
        loopHoming();
        return;
    }
}

void PixiPlacer::loopHoming() {
    if (millis() > 30000) {
        // Homing too long, disable steppers and bail.
        for (FastAccelStepper *stepper: {stepperX, stepperY, stepperHead}) {
            stepper->forceStop();
        }
        die("Unable to home!");
    }

    if (xHomingHelper.stepper == nullptr) {
        xHomingHelper.stepper = stepperX;
        xHomingHelper.axis = &xAxis;

        yHomingHelper.stepper = stepperY;
        yHomingHelper.axis = &yAxis;
    }

    bool yStartHit = digitalReadFast(PIXI_LIMIT_Y1_START_PIN)
                     || digitalReadFast(PIXI_LIMIT_Y1_START_PIN);

    bool yEndHit = digitalReadFast(PIXI_LIMIT_Y1_END_PIN)
                   || digitalReadFast(PIXI_LIMIT_Y1_END_PIN);

    bool xStartHit = digitalReadFast(PIXI_LIMIT_X_START_PIN);
    bool xEndHit = digitalReadFast(PIXI_LIMIT_X_END_PIN);

    xHomingHelper.startHit = xStartHit;
    xHomingHelper.endHit = xEndHit;
    yHomingHelper.startHit = yStartHit;
    yHomingHelper.endHit = yEndHit;

    for (PixiHomingHelper h: {xHomingHelper, yHomingHelper}) {
        if (h.startHit && h.endHit) {
            die("Invalid state - multiple ends hit!");
        } else if (h.startHit) {
            h.stepper->forceStopAndNewPosition(0);
            h.stepper->moveTo(INT32_MAX);
        } else if (h.endHit) {
            h.axis->maxSteps = h.stepper->getCurrentPosition();
            h.stepper->forceStopAndNewPosition(h.axis->maxSteps);
            h.stepper->setSpeedInHz(PIXI_STEPPER_SPEED_AFTER_HOMING);
            h.stepper->setAcceleration(PIXI_STEPPER_SPEED_AFTER_HOMING);
            h.stepper->moveTo(0);
            log(h.axis->name, " end hit, max steps=", h.axis->maxSteps);
        } else if (!(h.stepper->isRunning())) {
            if (h.axis->maxSteps == 0) { // Not discovered yet.
                h.stepper->moveTo(INT32_MIN);
            }
        }
    }

    if (xAxis.maxSteps > 0 && yAxis.maxSteps > 0) {
        homing = false;
        log("Homing complete!");
    }
}

void PixiPlacer::log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Serial.printf(fmt, args);
    Serial.println();
    va_end(args);
}

void PixiPlacer::die(const char *reason) {
    while (true) {
        log(reason);
        log(xAxis.toString().c_str());
        log(yAxis.toString().c_str());
        delay(5000);
    }
}
