#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "Settings/AppState.h"

namespace osc_controller::menus {
void handleSettingsMenu();
void loadSettings();
void saveSettings();
bool isScrollLockEnabled();
void handleThirdButton();
void handleDeviceInfo();
} // namespace Menus
