#include <WiFiUdp.h>
#include <OSCMessage.h>

#include "RunShowMenu.h"
#include "MainMenu.h"
#include "CueStorage.h"
#include "AppState.h"
#include "NetworkSettings.h"
#include "SettingsMenu.h"
#include "LanguageManager.h"

#define BACK_BUTTON 13

static int currentCueIndex = 0;
static long lastEncoderPos = -999;
static bool fireButtonPreviouslyDown = false;
static bool showComplete = false;

void handleRunShowMenu() {
  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.print(t("no_cues_to_run"));
    delay(1000);
    currentState = MAIN_MENU;
    return;
  }

  encoder.tick();
  long newPos = encoder.getPosition();
  int scrollIndex = (newPos % count + count) % count;

  if (showComplete && currentState != RUN_SHOW_MENU) {
    currentCueIndex = 0;
    encoder.setPosition(0);
    lastEncoderPos = -999;
    showComplete = false;
    fireButtonPreviouslyDown = false;
  }

  if (isScrollLockEnabled() && newPos != lastEncoderPos && !showComplete) {
    lastEncoderPos = newPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("scroll_locked"));
    delay(600);
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
      lcd.print(t("show_complete"));
      lcd.setCursor(0, 1);
      lcd.print(t("press_back"));
      shown = true;
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = MAIN_MENU;
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
    lcd.print(t("invalid_cue"));
    delay(1000);
    currentState = MAIN_MENU;
    return;
  }

  static int lastDrawnIndex = -1;
  if (lastDrawnIndex != currentCueIndex || forceRedraw) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cue ");
    lcd.print(currentCueIndex);
    lcd.print(":");

    lcd.setCursor(0, 1);
    lcd.print(cue->oscCommand.substring(0, 16));

    lastDrawnIndex = currentCueIndex;
    forceRedraw = false;
  }

  bool fireButtonDown = (digitalRead(FIRE_BUTTON) == LOW);
  if (!fireButtonPreviouslyDown && fireButtonDown && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    sendCueOSC(*cue);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(("fired_cue_label"));
    lcd.print(currentCueIndex);
    lcd.setCursor(0, 1);
    lcd.print(cue->oscCommand.substring(0, 16));
    delay(800);

    currentCueIndex++;
    lastEncoderPos = currentCueIndex;
    encoder.setPosition(currentCueIndex);

    if (currentCueIndex >= count) {
      showComplete = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("show_complete"));
      lcd.setCursor(0, 1);
      lcd.print(t("press_back"));
    }

    lastDrawnIndex = -1;
    forceRedraw = true;
  }
  fireButtonPreviouslyDown = fireButtonDown;

  if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = MAIN_MENU;
    currentCueIndex = 0;
    lastEncoderPos = -999;
    showComplete = false;
    fireButtonPreviouslyDown = false;
    forceRedraw = true;
    lcd.clear();
  }
}

