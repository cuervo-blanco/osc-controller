#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "AppState.h"

void handleSettingsMenu();
void loadSettings();
void saveSettings();
bool isScrollLockEnabled();
void handleThirdButton();
void handleDeviceInfo();

extern LiquidCrystal_I2C lcd;
extern RotaryEncoder encoder;
extern MenuState currentState;
extern int selectedIndex;
extern bool forceRedraw;
extern unsigned long lastPressTime;
extern const unsigned long debounceDelay;
extern String ipAddress;
extern String port;
