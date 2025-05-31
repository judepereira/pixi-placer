//
// Created by Jude Pereira on 31/05/2025.
//

#include <Arduino.h>
#include "FastAccelStepper.h"

#ifndef PIXI_PLACER_PIXIPLACER_H
#define PIXI_PLACER_PIXIPLACER_H


class PixiPlacer {
private:
    FastAccelStepperEngine engine = FastAccelStepperEngine();
    FastAccelStepper *stepperX = nullptr;
    FastAccelStepper *stepperY = nullptr;
    FastAccelStepper *stepperHead = nullptr;
public:
    void init();
    void loop();

    [[noreturn]] void die(String reason);
};


#endif //PIXI_PLACER_PIXIPLACER_H
