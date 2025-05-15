#include <WiFiUdp.h>
#include "Globals.h"
#include "Settings/AppState.h"
#include "Settings/NetworkSettings.h"
#include "Utilities/RotaryTextInput.h"
#include "Menus/MainMenu.h"

namespace osc_controller {

LiquidCrystal_I2C lcd(0x27, 16, 2);
RotaryEncoder encoder(settings::ROTARY_DT, settings::ROTARY_CLK);
settings::MenuState currentState = settings::MAIN_MENU;
int selectedIndex = 0;
bool forceRedraw = true;
unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 300;

void handleEnterText(String& targetField, settings::MenuState returnState) {
  if (utilities::updateTextInput()) {
    if (!utilities::didUserCancel()) {
      targetField = utilities::getFinalInput();
      if (returnState == settings::NETWORK_MENU && &targetField == &settings::port) {
        settings::savePort(settings::port.toInt());
      }
    }

    currentState = returnState;
    menus::resetMenuState();
  }
}

} // namespace osc_controller

