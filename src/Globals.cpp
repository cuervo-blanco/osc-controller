#include "Globals.h"

namespace osc_controller {
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  RotaryEncoder encoder(settings::ROTARY_DT, settings::ROTARY_CLK);
  settings::MenuState currentState = settings::MAIN_MENU;
  int selectedIndex = 0;
  bool forceRedraw = true;
  unsigned long lastPressTime = 0;
  const unsigned long debounceDelay = 300;
}

