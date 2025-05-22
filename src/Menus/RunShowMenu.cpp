#include <WiFiUdp.h>
#include <OSCMessage.h>

#include "Globals.h"
#include "Menus/RunShowMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/SettingsMenu.h"
#include "Settings/AppState.h"
#include "Settings/NetworkSettings.h"
#include "Settings/LanguageManager.h"
#include "CueStorage.h"

namespace osc_controller::menus {

using namespace osc_controller;

static int currentCueIndex = 0;
static long lastEncoderPos = -999;
static bool fireButtonPreviouslyDown = false;
static bool showComplete = false;

String safeTrim(const String& s, int len = 16) {
  return s.length() > len ? s.substring(0, len) : s;
}

int safeIndex(long pos, int max) {
  return (max == 0) ? 0 : ((pos % max + max) % max);
}

void handleRunShowMenu() {
  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.print(settings::t("no_cues_to_run"));
    delay(1000);
    currentState = settings::MAIN_MENU;
    return;
  }

  encoder.tick();
  long newPos = encoder.getPosition();
  int scrollIndex = safeIndex(newPos, count);


  if (showComplete && currentState != settings::RUN_SHOW_MENU) {
    currentCueIndex = 0;
    encoder.setPosition(0);
    lastEncoderPos = -999;
    showComplete = false;
    fireButtonPreviouslyDown = false;
  }

  if (menus::isScrollLockEnabled() && newPos != lastEncoderPos && !showComplete) {
    lastEncoderPos = newPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(settings::t("scroll_locked"));
    delay(600);
    forceRedraw = true;
  }

  if (!menus::isScrollLockEnabled() && !showComplete && newPos != lastEncoderPos) {
    currentCueIndex = scrollIndex;
    lastEncoderPos = newPos;
    forceRedraw = true;
  }

  if (currentCueIndex >= count) {
    showComplete = true;
  }

  if (showComplete) {
    static bool shown = false;
    if (!shown) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings::t("show_complete"));
      lcd.setCursor(0, 1);
      lcd.print(settings::t("press_back"));
      shown = true;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = settings::MAIN_MENU;
      currentCueIndex = 0;
      lastEncoderPos = -999;
      showComplete = false;
      fireButtonPreviouslyDown = false;
      forceRedraw = true;
      shown = false;
      lcd.clear();
    }

    return;
  }

  Cue* cue = getCue(currentCueIndex);
  if (!cue) {
    lcd.clear();
    lcd.print(settings::t("invalid_cue"));
    delay(1000);
    currentState = settings::MAIN_MENU;
    return;
  }

  static int lastDrawnIndex = -1;
  if (lastDrawnIndex != currentCueIndex || forceRedraw) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cue ");
    lcd.print(currentCueIndex + 1);
    lcd.print(":");

    lcd.setCursor(0, 1);
    lcd.print(safeTrim(cue->oscCommand));

    lastDrawnIndex = currentCueIndex;
    forceRedraw = false;
  }

  bool fireButtonDown = (digitalRead(settings::FIRE_BUTTON) == LOW);
  if (!fireButtonPreviouslyDown && fireButtonDown && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    bool success = settings::sendCueOSC(*cue);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(success ? settings::t("fired_cue_label") : settings::t("send_failed"));
    lcd.print(currentCueIndex + 1);
    lcd.setCursor(0, 1);
    lcd.print(safeTrim(cue->oscCommand));
    delay(800);

    currentCueIndex++;
    lastEncoderPos = currentCueIndex;
    encoder.setPosition(currentCueIndex);

    if (currentCueIndex >= count) {
      showComplete = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings::t("show_complete"));
      lcd.setCursor(0, 1);
      lcd.print(settings::t("press_back"));
    }

    lastDrawnIndex = -1;
    forceRedraw = true;
  }
  fireButtonPreviouslyDown = fireButtonDown;

  if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = settings::MAIN_MENU;
    currentCueIndex = 0;
    lastEncoderPos = -999;
    showComplete = false;
    fireButtonPreviouslyDown = false;
    forceRedraw = true;
    lcd.clear();
  }
}

} // namespace osc_controller::menus
