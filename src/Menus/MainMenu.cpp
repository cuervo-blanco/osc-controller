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
  if (selectedIndex < 0 || selectedIndex >= itemCount) {
    Serial.printf("Invalid selectedIndex: %d\n", selectedIndex);
    selectedIndex = 0;
  }
  lcd.clear();
  const char* currentItem = items[selectedIndex];
  if (!currentItem || strlen(currentItem) == 0) {
    currentItem = "[?]";
  }

  lcd.setCursor(0, 0);
  lcd.print(">");
  lcd.print(String(currentItem).substring(0, 15));
  if (itemCount > 1) {
    int nextIndex = (selectedIndex + 1) % itemCount;
    const char* nextItem = items[nextIndex];
    if (!nextItem || strlen(nextItem) == 0) {
      nextItem = "[?]";
    }
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.print(String(nextItem).substring(0, 15));
  }
}

void handleMainMenu() {
  const char* items[] = {
    settings::t("menu_cues"), 
    settings::t("menu_run_show"), 
    settings::t("menu_settings")
  };
  const int itemCount = sizeof(items) / sizeof(items[0]);

  static int lastDrawnIndex = -1;
  long newPos = encoder.getPosition();
  if (itemCount <= 0) {
    lcd.clear();
    lcd.print("No menu items");
    return;
  }

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
      default:
        Serial.printf("Invalid main menu selection: %d\n", selectedIndex);
        currentState = settings::MAIN_MENU;
        break;
    }
    resetMenuState();
    lastDrawnIndex = -1;
  }
}

} // namespace osc_controller::menus
