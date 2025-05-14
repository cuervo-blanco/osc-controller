#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "AppState.h"

extern MenuState currentState;
extern int selectedIndex;
extern bool forceRedraw;
extern unsigned long lastPressTime;
extern const unsigned long debounceDelay;
extern LiquidCrystal_I2C lcd;
extern RotaryEncoder encoder;

void handleMainMenu();
void resetMenuState();
void drawMenu(const char* const items[], int itemCount);

