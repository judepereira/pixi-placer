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
#define PIXI_STEPPER_SPEED_AFTER_HOMING    10000

void PixiPlacer::init() {
    stepperX = new AccelStepper(AccelStepper::DRIVER, PIXI_X_AXIS_STEP_PIN, PIXI_X_AXIS_DIR_PIN);
    stepperY = new AccelStepper(AccelStepper::DRIVER, PIXI_Y_AXIS_STEP_PIN, PIXI_Y_AXIS_DIR_PIN);
    stepperHead = new AccelStepper(AccelStepper::DRIVER, PIXI_HEAD_STEP_PIN, PIXI_HEAD_DIR_PIN);

    for (int pin: {PIXI_X_AXIS_ENABLE_PIN, PIXI_Y_AXIS_ENABLE_PIN, PIXI_HEAD_ENABLE_PIN}) {
        pinMode(pin, PinMode::OUTPUT);
        digitalWrite(pin, LOW);
    }

    for (AccelStepper *stepper: {stepperX, stepperY, stepperHead}) {
        stepper->setMaxSpeed(PIXI_STEPPER_SPEED_BEFORE_HOMING);
        stepper->setAcceleration(100000);
    }

    for (uint8_t pin: {PIXI_LIMIT_X_START_PIN, PIXI_LIMIT_X_END_PIN,
                       PIXI_LIMIT_Y1_START_PIN, PIXI_LIMIT_Y1_END_PIN,
                       PIXI_LIMIT_Y2_START_PIN, PIXI_LIMIT_Y2_END_PIN}) {
        pinMode(pin, PinMode::INPUT_PULLUP);
    }

    xAxis.name = "X";
    yAxis.name = "Y";

    homing = true;
    log("Init complete!");
}

void PixiPlacer::loop() {
    stepperX->run();
    stepperY->run();
    stepperHead->run();
    if (homing) {
        loopHoming();
        return;
    }
}

void PixiPlacer::loopHoming() {
    if (millis() > 30000) {
        // Homing too long, disable steppers and bail.
        for (AccelStepper *stepper: {stepperX, stepperY, stepperHead}) {
            stepper->stop();
        }
        die("Unable to home!");
    }

    if (xHomingHelper.stepper == nullptr) {
        xHomingHelper.stepper = stepperX;
        xHomingHelper.axis = &xAxis;

        yHomingHelper.stepper = stepperY;
        yHomingHelper.axis = &yAxis;
        log("Homing helpers initialised");
    }

    bool yStartHit = !digitalReadFast(PIXI_LIMIT_Y1_START_PIN)
                     || !digitalReadFast(PIXI_LIMIT_Y2_START_PIN);

    bool yEndHit = !digitalReadFast(PIXI_LIMIT_Y1_END_PIN)
                   || !digitalReadFast(PIXI_LIMIT_Y2_END_PIN);

    bool xStartHit = !digitalReadFast(PIXI_LIMIT_X_START_PIN);
    bool xEndHit = !digitalReadFast(PIXI_LIMIT_X_END_PIN);

    xHomingHelper.startHit = xStartHit;
    xHomingHelper.endHit = xEndHit;
    yHomingHelper.startHit = yStartHit;
    yHomingHelper.endHit = yEndHit;

    for (PixiHomingHelper h: {xHomingHelper, yHomingHelper}) {
        if (h.axis->maxSteps != 0) {
            continue;
        } else if (h.startHit && h.endHit) {
            die("Invalid state - multiple ends hit!");
        } else if (h.startHit) {
            h.stepper->stop();
            h.stepper->runToPosition();
            h.stepper->setCurrentPosition(0);
            h.stepper->moveTo(INT32_MAX);
        } else if (h.endHit) {
            h.axis->maxSteps = h.stepper->currentPosition();
            h.stepper->stop();
            h.stepper->runToPosition();
            h.stepper->setCurrentPosition(h.axis->maxSteps);
            h.stepper->setMaxSpeed(PIXI_STEPPER_SPEED_AFTER_HOMING);
            h.stepper->setAcceleration(PIXI_STEPPER_SPEED_AFTER_HOMING * 2);
            h.stepper->moveTo(0);
            log(h.axis->name, " end hit, max steps=", String(h.axis->maxSteps));
        } else if (!(h.stepper->isRunning())) {
            if (h.axis->maxSteps == 0) { // Not discovered yet.
                log(h.stepper->isRunning() ? "running" : "not running");
                h.stepper->moveTo(INT32_MIN);
                log("Moving", h.axis->name);
            }
        }
    }

    if (xAxis.maxSteps > 0 && yAxis.maxSteps > 0) {
        homing = false;
        log("Homing complete!");
    }
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
#pragma ide diagnostic ignored "ConstantParameter"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void PixiPlacer::log(String a) {
    Serial.printf(a.c_str());
    Serial.println();
}

void PixiPlacer::log(String a, String b) {
    Serial.printf(a.c_str());
    Serial.printf(b.c_str());
    Serial.println();
}

void PixiPlacer::log(String a, String b, String c) {
    Serial.printf(a.c_str());
    Serial.printf(b.c_str());
    Serial.printf(c.c_str());
    Serial.println();
}

void PixiPlacer::log(String a, String b, String c, String d) {
    Serial.printf(a.c_str());
    Serial.printf(b.c_str());
    Serial.printf(c.c_str());
    Serial.printf(d.c_str());
    Serial.println();
}

void PixiPlacer::log(String a, String b, String c, String d, String e) {
    Serial.printf(a.c_str());
    Serial.printf(b.c_str());
    Serial.printf(c.c_str());
    Serial.printf(d.c_str());
    Serial.printf(e.c_str());
    Serial.println();
}

#pragma clang diagnostic pop

void PixiPlacer::die(const char *reason) {
    while (true) {
        log(reason);
        log(xAxis.toString().c_str());
        log(yAxis.toString().c_str());
        delay(5000);
    }
}
