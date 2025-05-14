#pragma once
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>
#include "Settings/AppState.h"

namespace osc_controller {
  extern LiquidCrystal_I2C lcd;
  extern RotaryEncoder encoder;
  extern settings::MenuState currentState;
  extern int selectedIndex;
  extern bool forceRedraw;
  extern unsigned long lastPressTime;
  extern const unsigned long debounceDelay;
}

