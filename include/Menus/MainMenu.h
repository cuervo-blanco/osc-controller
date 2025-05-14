#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "Settings/AppState.h"

namespace osc_controller::menus {
void handleMainMenu();
void resetMenuState();
void drawMenu(const char* const items[], int itemCount);
} // namespace Menus
