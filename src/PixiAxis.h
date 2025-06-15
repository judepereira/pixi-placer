//
// Created by Jude Pereira on 01/06/2025.
//

#include "Arduino.h"
#include "AccelStepper.h"
#include <format>
#include <iostream>


#ifndef PIXI_PLACER_PIXIAXIS_H
#define PIXI_PLACER_PIXIAXIS_H

struct PixiAxis {
    const char *name = nullptr;
    int32_t maxSteps = 0;

    String toString() {
        String x = "axis=";
        x += name;
        x += ", maxSteps=";
        x += maxSteps;
        return x;
    }
};

struct PixiHomingHelper {
    bool startHit = false;
    bool endHit = false;
    AccelStepper *stepper = nullptr;
    PixiAxis *axis = nullptr;
};

#endif //PIXI_PLACER_PIXIAXIS_H
