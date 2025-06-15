//
// Created by Jude Pereira on 25/05/2025.
//
#include <Arduino.h>
#include "PixiPlacer.h"

PixiPlacer *pixiPlacer = nullptr;

void setup() {
    Serial.begin(115200);
    pixiPlacer = new PixiPlacer();
    pixiPlacer->init();
}

void loop() {
    pixiPlacer->loop();
}
