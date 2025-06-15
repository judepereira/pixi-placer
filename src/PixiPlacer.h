//
// Created by Jude Pereira on 31/05/2025.
//

#include <Arduino.h>
#include <AccelStepper.h>
#include "PixiAxis.h"

#ifndef PIXI_PLACER_PIXIPLACER_H
#define PIXI_PLACER_PIXIPLACER_H


class PixiPlacer {
private:
    AccelStepper *stepperX = nullptr;
    AccelStepper *stepperY = nullptr;
    AccelStepper *stepperHead = nullptr;
    PixiAxis xAxis, yAxis;
    PixiHomingHelper xHomingHelper, yHomingHelper;

    bool homing = false;

    void loopHoming();

    void log(String a);
    void log(String a, String b);
    void log(String a, String b, String c);
    void log(String a, String b, String c, String d);
    void log(String a, String b, String c, String d, String e);

public:
    void init();

    void loop();

    [[noreturn]] void die(const char *reason);
};


#endif //PIXI_PLACER_PIXIPLACER_H
