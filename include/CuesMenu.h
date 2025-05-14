#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

#include "AppState.h"
#include "CueStorage.h"

void handleCuesMenu();

extern LiquidCrystal_I2C lcd;
extern RotaryEncoder encoder;
extern MenuState currentState;
extern int selectedIndex;
extern bool forceRedraw;
extern unsigned long lastPressTime;
extern const unsigned long debounceDelay;

void handleAddCueMenu();
void handleEditCueMenu();
void handleDeleteCueMenu();
void handleCueControlMenu();
void handleReorderCueMenu();
void handleEnterQLabIP();
