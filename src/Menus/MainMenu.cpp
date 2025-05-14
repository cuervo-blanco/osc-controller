#include "Globals.h"
#include "Menus/MainMenu.h"
#include "Settings/AppState.h"
#include "Settings/LanguageManager.h"

namespace osc_controller::menus {

using namespace osc_controller;

void resetMenuState() {
  encoder.setPosition(0);
  selectedIndex = 0;
  forceRedraw = true;
}

void drawMenu(const char* const items[], int itemCount) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">");
  lcd.print(String(items[selectedIndex]).substring(0, 15));
  if (itemCount > 1) {
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.print(String(items[(selectedIndex + 1) % itemCount]).substring(0, 15));
  }
}

void handleMainMenu() {
  const char* items[] = {
    settings::t("menu_cues"), 
    settings::t("menu_run_show"), 
    settings::t("menu_settings")
  };
  int itemCount = 3;

  static int lastDrawnIndex = -1;
  long newPos = encoder.getPosition();
  selectedIndex = (newPos % itemCount + itemCount) % itemCount;

  if (forceRedraw || selectedIndex != lastDrawnIndex) {
    drawMenu(items, itemCount);
    lastDrawnIndex = selectedIndex;
    forceRedraw = false;
  }

  if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    switch (selectedIndex) {
      case 0: currentState = settings::CUES_MENU; break;
      case 1: currentState = settings::RUN_SHOW_MENU; break;
      case 2: currentState = settings::SETTINGS_MENU; break;
    }
    resetMenuState();
    lastDrawnIndex = -1;
  }
}

} // namespace osc_controller::menus
