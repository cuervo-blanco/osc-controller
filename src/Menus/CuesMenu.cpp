#include <WiFiUdp.h>
#include <OSCMessage.h>

#include "Globals.h"
#include "CueStorage.h"
#include "Menus/CuesMenu.h"
#include "Menus/MainMenu.h" 
#include "Menus/CuePathType.h"
#include "Settings/WorkspaceManager.h"
#include "Settings/AppState.h"
#include "Settings/NetworkSettings.h"
#include "Settings/LanguageManager.h"
#include "Utilities/RotaryTextInput.h"

namespace osc_controller::menus {
using namespace osc_controller;

static bool fireButtonPreviouslyDown = false;

void handleCuesMenu() {
  const char* items[] = {
    settings::t("add_cue_item"),
    settings::t("edit_cue_item"),
    settings::t("delete_cue_item"),
    settings::t("preview_item"),
    settings::t("reorder_cues_item"),
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

  if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    switch (selectedIndex) {
      case 0: currentState = settings::ADD_CUE_MENU; break;
      case 1: currentState = settings::EDIT_CUE_MENU; break;
      case 2: currentState = settings::DELETE_CUE_MENU; break;
      case 3: currentState = settings::CUE_CONTROL_MENU; break;
      case 4: currentState = settings::REORDER_CUE_MENU; break;
    }

    resetMenuState();
    lastDrawnIndex = -1;
  }

  if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = settings::MAIN_MENU;
    resetMenuState();
    lastDrawnIndex = -1;
    lcd.clear();
  }
}

void handleAddCueMenu() {
  const char* types[] = {
    settings::t("start_cue_item"),
    settings::t("stop_cue_item"),
    settings::t("pause_cue_item"),
    settings::t("load_cue_item"),
    settings::t("custom_path_item"),
  };
  const int typeCount = sizeof(types) / sizeof(types[0]);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(settings::t("select_type"));

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
      lcd.print(String(settings::t("type")) + ":");
      lcd.setCursor(0, 1);
      lcd.print(types[selectedType]);
    }

    if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      break;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = settings::CUES_MENU;
      lcd.clear();
      return;
    }
  }

  String osc;
  String cueID = "";

  if (selectedType == 4) {
    const char* pathTypes[] = { "Cue", "Workspace", "Overrides", settings::t("custom_item") };
    int pathTypeCount = sizeof(pathTypes) / sizeof(pathTypes[0]);
    int selectedPathType = 0;
    long lastPos = -999;

    while (true) {
      encoder.tick();
      long newPos = encoder.getPosition();
      selectedPathType = (newPos % pathTypeCount + pathTypeCount) % pathTypeCount;

      if (newPos != lastPos) {
        lastPos = newPos;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(settings::t("select_path_type"));
        lcd.setCursor(0, 1);
        lcd.print(pathTypes[selectedPathType]);
      }

      if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
        lastPressTime = millis();
        break;
      }

      if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
        lastPressTime = millis();
        currentState = settings::CUES_MENU;
        lcd.clear();
        return;
      }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(settings::t("enter_osc_path"));
    delay(500);

    String prefix = "";
    if (selectedPathType == 1) {
      int wsCount = settings::getWorkspaceCount();
      if (wsCount == 0) {
        lcd.clear(); lcd.print(settings::t("no_workspaces"));
        delay(1000);
        prefix = "/workspace/";
      } else {
        const char* customOption = settings::t("custom_item");
        long lastPos = -999;
        int selected = 0;
        bool done = false;

        while (!done) {
          encoder.tick();
          long newPos = encoder.getPosition();
          int totalItems = wsCount + 1;
          selected = (newPos % totalItems + totalItems) % totalItems;

          if (newPos != lastPos) {
            lastPos = newPos;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(settings::t("select_workspace"));
            lcd.setCursor(0, 1);
            if (selected < wsCount) {
              lcd.print(settings::getWorkspace(selected).name.substring(0, 16));
            } else {
              lcd.print(customOption);
            }
          }

          if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            if (selected < wsCount) {
              prefix = "/workspace/" + settings::getWorkspace(selected).name + "/";
            } else {
              prefix = "/workspace/";
            }
            done = true;
          }

          if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            currentState = settings::CUES_MENU;
            lcd.clear();
            return;
          }
        }
      }
    } else {
      switch (selectedPathType) {
        case 0: prefix = "/cue/"; break;
        case 2: prefix = "/overrides/"; break;
        case 3: prefix = ""; break;
      }
    }

  utilities::initPrefilledInput(prefix, false, false, 64);

    while (!utilities::updateTextInput()) {
      encoder.tick();
    }

    if (utilities::didUserCancel()) {
      lcd.clear();
      lcd.print(settings::t("cancelled_label"));
      delay(800);
      currentState = settings::CUES_MENU;
      return;
    }

    osc = utilities::getFinalInput();
    cueID = osc;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(settings::t("enter_cue_id"));
    delay(500);

    utilities::initTextInput();
    while (!utilities::updateTextInput()) {
      encoder.tick();
    }

    if (utilities::didUserCancel()) {
      lcd.clear();
      lcd.print(settings::t("cancelled_label"));
      delay(800);
      currentState = settings::CUES_MENU;
      return;
    }

    cueID = utilities::getFinalInput();
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
    lcd.print(settings::t("cue_added"));
  } else {
    lcd.clear();
    lcd.print(settings::t("add_failed"));
  }

  delay(1000);
  currentState = settings::CUES_MENU;
}

void handleEditCueMenu() {
  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(settings::t("no_cues_edit"));
    delay(1000);
    currentState = settings::CUES_MENU;
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
      lcd.print(settings::t("edit_cue_label"));
      lcd.print(cue->index);
      lcd.setCursor(0, 1);
      lcd.print(cue->oscCommand.substring(0, 16));
    }

    if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();

      Cue* cue = getCue(selected);
      utilities::initPrefilledInput(cue->oscCommand, false, false, 99);
      while (!utilities::updateTextInput()) {
        encoder.tick();
      }

      if (!utilities::didUserCancel()) {
        cue->oscCommand = utilities::getFinalInput();
        lcd.clear();
        lcd.print(settings::t("updated_label"));
        delay(800);
      } else {
        lcd.clear();
        lcd.print(settings::t("cancelled_label"));
        delay(800);
      }

      lastPos = -999; 
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = settings::CUES_MENU;
      lcd.clear();
      break;
    }
  }
}

void handleDeleteCueMenu() {
  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.print(settings::t("no_cues_del"));
    delay(1000);
    currentState = settings::CUES_MENU;
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
      lcd.print(settings::t("delete_all"));
      lcd.setCursor(0, 1);
      lcd.print(settings::t("press_confirm"));
    } else {
      Cue* cue = getCue(selected);
      lcd.setCursor(0, 0);
      lcd.print(settings::t("del_cue_label"));
      lcd.print(cue->index);
      lcd.setCursor(0, 1);
      lcd.print(cue->oscCommand.substring(0, 16));
    }

    lastDrawn = selected;
    forceRedraw = false;
  }

  if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    if (selected == count) {
      resetCues();
      lcd.clear();
      lcd.print(settings::t("all_cues_del_label"));
      delay(1000);
      currentState = settings::CUES_MENU;
      encoder.setPosition(0);
      selected = 0;
      lastDrawn = -1;
      return;
    } else {
      Cue* cue = getCue(selected);
      if (deleteCue(cue->index)) {
        lcd.clear();
        lcd.print(settings::t("deleted_label"));
        lcd.print(cue->index);
      } else {
        lcd.clear();
        lcd.print(settings::t("delete_failed"));
      }

      delay(1000);
      selected = 0;
      encoder.setPosition(0);
      lastDrawn = -1;
      forceRedraw = true;

      if (getCueCount() == 0) {
        currentState = settings::CUES_MENU;
        lcd.clear();
      }
    }
  }

  if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = settings::CUES_MENU;
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
    fireButtonPreviouslyDown = (digitalRead(settings::FIRE_BUTTON) == LOW);
  }

  encoder.tick();

  int count = getCueCount();
  if (count == 0) {
    lcd.clear();
    lcd.print(settings::t("no_cues_label"));
    delay(1000);
    currentState = settings::CUES_MENU;
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

  bool fireButtonDown = (digitalRead(settings::FIRE_BUTTON) == LOW);
  if (!fireButtonPreviouslyDown && fireButtonDown && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();

    settings::sendCueOSC(*cue);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(settings::t("fired_label"));
    lcd.setCursor(0, 1);
    lcd.print(cue->oscCommand.substring(0, 15));
    delay(800); 

    forceRedraw = true;
    lastDrawn = -1;
  }
  fireButtonPreviouslyDown = fireButtonDown;

  if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = settings::CUES_MENU;
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
    lcd.print(settings::t("not_enough_cues"));
    delay(1000);
    currentState = settings::CUES_MENU;
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
      lcd.print(settings::t("move_cue_label"));
      lcd.print(cue->index + 1);
      lcd.setCursor(0, 1);
      lcd.print(cue->oscCommand.substring(0, 16));
    }

    if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      selectingCue = false;
      break;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = settings::CUES_MENU;
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
      lcd.print(settings::t("to_position"));
      lcd.print(targetPos);
      lcd.setCursor(0, 1);
      lcd.print(settings::t("press_confirm"));
    }

    if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();

      reorderCues(selected, targetPos); 

      lcd.clear();
      lcd.print(settings::t("reordered_label"));
      delay(1000);
      currentState = settings::CUES_MENU;
      return;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      currentState = settings::CUES_MENU;
      lcd.clear();
      return;
    }
  }
}
} // namespace osc_controller::menus
