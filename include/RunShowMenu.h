#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "AppState.h"

void handleRunShowMenu();

extern LiquidCrystal_I2C lcd;
extern RotaryEncoder encoder;
extern MenuState currentState;
extern int selectedIndex;
extern bool forceRedraw;
extern unsigned long lastPressTime;
extern const unsigned long debounceDelay;
