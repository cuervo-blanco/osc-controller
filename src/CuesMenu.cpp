#include <WiFiUdp.h>
#include <OSCMessage.h>

#include "CuesMenu.h"
#include "AppState.h"
#include "MainMenu.h" 
#include "CueStorage.h"
#include "RotaryTextInput.h"
#include "NetworkSettings.h"
#include "LanguageManager.h"

#define BACK_BUTTON 13

static bool fireButtonPreviouslyDown = false;

void handleCuesMenu() {
  const char* items[] = {
    t("add_cue_item"),
    t("edit_cue_item"),
    t("delete_cue_item"),
    t("preview_item"),
    t("reorder_cues_item"),
  };
  int itemCount = sizeof(items) / sizeof(items[0]);

  static int lastDrawnIndex = -1;
  long newPos = encoder.getPosition();
  selectedIndex = (newPos % itemCount + itemCount) % itemCount;

  if (forceRedraw || selectedIndex != lastDrawnIndex) {
    drawMenu(items, itemCount);
    lastDrawnIndex = selectedIndex;
    forceRedraw = false;
  }

  if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    switch (selectedIndex) {
      case 0: currentState = ADD_CUE_MENU; break;
      case 1: currentState = EDIT_CUE_MENU; break;
      case 2: currentState = DELETE_CUE_MENU; break;
      case 3: currentState = CUE_CONTROL_MENU; break;
      case 4: currentState = REORDER_CUE_MENU; break;
    }

    resetMenuState();
    lastDrawnIndex = -1;
  }

  if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = MAIN_MENU;
    resetMenuState();
    lastDrawnIndex = -1;
    lcd.clear();
  }
}

void handleAddCueMenu() {
  const char* types[] = {
    t("start_cue_item"),
    t("stop_cue_item"),
    t("pause_cue_item"),
    t("load_cue_item"),
    t("custom_path_item"),
  };
  const int typeCount = sizeof(types) / sizeof(types[0]);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(t("select_type"));

  int selectedType = 0;
  long lastPos = -999;

  while (true) {
    encoder.tick();
    long newPos = encoder.getPosition();
    selectedType = (newPos % typeCount + typeCount) % typeCount;

    if (newPos != lastPos) {
      lastPos = newPos;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String(t("type")) + ":");
      lcd.setCursor(0, 1);
      lcd.print(types[selectedType]);
    }

    if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      break;
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = CUES_MENU;
      lcd.clear();
      return;
    }
  }

  String osc;
  String cueID = "";

  if (selectedType == 4) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("enter_osc_path"));
    delay(500);

    initTextInput();
    while (!updateTextInput()) {
      encoder.tick();
    }

    if (didUserCancel()) {
      lcd.clear();
      lcd.print(t("cancelled_label"));
      delay(800);
      currentState = CUES_MENU;
      return;
    }

    osc = getFinalInput();
    cueID = "custom";
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("enter_cue_id"));
    delay(500);

    initTextInput();
    while (!updateTextInput()) {
      encoder.tick();
    }

    if (didUserCancel()) {
      lcd.clear();
      lcd.print(t("cancelled_label"));
      delay(800);
      currentState = CUES_MENU;
      return;
    }

    cueID = getFinalInput();
    const char* method = "";

    switch (selectedType) {
      case 0: method = "start"; break;
      case 1: method = "stop"; break;
      case 2: method = "pause"; break;
      case 3: method = "load"; break;
    }

    osc = "/cue/" + cueID + "/" + method;
  }

  Cue newCue;
  newCue.index = getCueCount();
  newCue.type = CUE_TARGET;
  newCue.oscCommand = osc;
  newCue.cueID = cueID;
  newCue.enabled = true;

  if (addCue(newCue)) {
    lcd.clear();
    lcd.print(t("cue_added"));
  } else {
    lcd.clear();
    lcd.print(t("add_failed"));
  }

  delay(1000);
  currentState = CUES_MENU;
}

void handleEditCueMenu() {
  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("no_cues_edit"));
    delay(1000);
    currentState = CUES_MENU;
    return;
  }

  static int selected = 0;
  long lastPos = -999;
  bool editing = true;

  while (editing) {
    encoder.tick();
    long newPos = encoder.getPosition();
    selected = (newPos % count + count) % count;

    if (newPos != lastPos) {
      lastPos = newPos;
      Cue* cue = getCue(selected);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("edit_cue_label"));
      lcd.print(cue->index);
      lcd.setCursor(0, 1);
      lcd.print(cue->oscCommand.substring(0, 16));
    }

    if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();

      Cue* cue = getCue(selected);
      initPrefilledInput(cue->oscCommand);
      while (!updateTextInput()) {
        encoder.tick();
      }

      if (!didUserCancel()) {
        cue->oscCommand = getFinalInput();
        lcd.clear();
        lcd.print(t("updated_label"));
        delay(800);
      } else {
        lcd.clear();
        lcd.print(t("cancelled_label"));
        delay(800);
      }

      lastPos = -999; 
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = CUES_MENU;
      lcd.clear();
      break;
    }
  }
}

void handleDeleteCueMenu() {
  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.print(t("no_cues_del"));
    delay(1000);
    currentState = CUES_MENU;
    return;
  }

  static int selected = 0;
  static int lastDrawn = -1;
  long newPos = encoder.getPosition();
  int itemCount = count + 1;
  selected = (newPos % itemCount + itemCount) % itemCount;

  encoder.tick();

  if (selected != lastDrawn || forceRedraw) {
    lcd.clear();

    if (selected == count) {
      lcd.setCursor(0, 0);
      lcd.print(t("delete_all"));
      lcd.setCursor(0, 1);
      lcd.print(t("press_confirm"));
    } else {
      Cue* cue = getCue(selected);
      lcd.setCursor(0, 0);
      lcd.print(t("del_cue_label"));
      lcd.print(cue->index);
      lcd.setCursor(0, 1);
      lcd.print(cue->oscCommand.substring(0, 16));
    }

    lastDrawn = selected;
    forceRedraw = false;
  }

  if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    if (selected == count) {
      resetCues();
      lcd.clear();
      lcd.print(t("all_cues_del_label"));
      delay(1000);
      currentState = CUES_MENU;
      encoder.setPosition(0);
      selected = 0;
      lastDrawn = -1;
      return;
    } else {
      Cue* cue = getCue(selected);
      if (deleteCue(cue->index)) {
        lcd.clear();
        lcd.print(t("deleted_label"));
        lcd.print(cue->index);
      } else {
        lcd.clear();
        lcd.print(t("delete_failed"));
      }

      delay(1000);
      selected = 0;
      encoder.setPosition(0);
      lastDrawn = -1;
      forceRedraw = true;

      if (getCueCount() == 0) {
        currentState = CUES_MENU;
        lcd.clear();
      }
    }
  }

  if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = CUES_MENU;
    lcd.clear();
    selected = 0;
    encoder.setPosition(0);
    lastDrawn = -1;
  }
}

void handleCueControlMenu() {
  static unsigned long menuEntryTime = 0;
  static bool justEntered = true;

  if (justEntered) {
    menuEntryTime = millis();
    justEntered = false;
    fireButtonPreviouslyDown = (digitalRead(FIRE_BUTTON) == LOW);
  }

  encoder.tick();

  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.print(t("no_cues_label"));
    delay(1000);
    currentState = CUES_MENU;
    return;
  }

  static int lastDrawn = -1;
  static int selected = 0;
  long newPos = encoder.getPosition();
  selected = (newPos % count + count) % count;

  Cue* cue = getCue(selected);
  
  if (selected != lastDrawn || forceRedraw) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cue #");
    lcd.print(cue->index);
    lcd.setCursor(0, 1);
    lcd.print(cue->oscCommand.substring(0, 15));
    lastDrawn = selected;
    forceRedraw = false;
  }

  bool fireButtonDown = (digitalRead(FIRE_BUTTON) == LOW);
  if (!fireButtonPreviouslyDown && fireButtonDown && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    sendCueOSC(*cue);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("fired_label"));
    lcd.setCursor(0, 1);
    lcd.print(cue->oscCommand.substring(0, 15));
    delay(800); 

    forceRedraw = true;
    lastDrawn = -1;
  }
  fireButtonPreviouslyDown = fireButtonDown;

  if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = CUES_MENU;
    lcd.clear();
    lastDrawn = -1;
    justEntered = true;
  }
}

void handleReorderCueMenu() {
  int count = getCueCount();
  if (count < 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("not_enough_cues"));
    delay(1000);
    currentState = CUES_MENU;
    return;
  }

  static int selected = 0;
  long lastPos = -999;
  bool selectingCue = true;

  while (selectingCue) {
    encoder.tick();
    long newPos = encoder.getPosition();
    selected = (newPos % count + count) % count;

    if (newPos != lastPos) {
      lastPos = newPos;
      Cue* cue = getCue(selected);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("move_cue_label"));
      lcd.print(cue->index);
      lcd.setCursor(0, 1);
      lcd.print(cue->oscCommand.substring(0, 16));
    }

    if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      selectingCue = false;
      break;
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = CUES_MENU;
      lcd.clear();
      return;
    }
  }

  int targetPos = 0;
  lastPos = -999;

  while (true) {
    encoder.tick();
    long newPos = encoder.getPosition();
    targetPos = (newPos % count + count) % count;

    if (newPos != lastPos) {
      lastPos = newPos;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("to_position"));
      lcd.print(targetPos);
      lcd.setCursor(0, 1);
      lcd.print(t("press_confirm"));
    }

    if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();

      reorderCues(selected, targetPos);  // ← use your safe, tested function

      lcd.clear();
      lcd.print(t("reordered_label"));
      delay(1000);
      currentState = CUES_MENU;
      return;
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = CUES_MENU;
      lcd.clear();
      return;
    }
  }
}

