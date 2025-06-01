//
// Created by Jude Pereira on 31/05/2025.
//

#include <Arduino.h>
#include "FastAccelStepper.h"
#include "PixiAxis.h"

#ifndef PIXI_PLACER_PIXIPLACER_H
#define PIXI_PLACER_PIXIPLACER_H


class PixiPlacer {
private:
    FastAccelStepperEngine engine = FastAccelStepperEngine();
    FastAccelStepper *stepperX = nullptr;
    FastAccelStepper *stepperY = nullptr;
    FastAccelStepper *stepperHead = nullptr;
    PixiAxis xAxis, yAxis;
    PixiHomingHelper xHomingHelper, yHomingHelper;

    bool homing = false;

    void loopHoming();

    void log(const char *fmt...);

public:
    void init();

    void loop();

    [[noreturn]] void die(const char *reason);
};


#endif //PIXI_PLACER_PIXIPLACER_H
