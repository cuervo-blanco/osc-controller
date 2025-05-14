#pragma once

#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

#include "AppState.h"

void handleNetworkMenu();

extern LiquidCrystal_I2C lcd;
extern RotaryEncoder encoder;
extern MenuState currentState;
extern String port, password, connectedSSID, ipAddress, passcode;
extern int selectedIndex;
extern bool forceRedraw;
extern unsigned long lastPressTime;
extern const unsigned long debounceDelay;

void handleShowInfo();

