#include <WiFiUdp.h>
#include <Preferences.h>
#include <OSCMessage.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include "Globals.h"
#include "Menus/SettingsMenu.h"
#include "Menus/MainMenu.h"
#include "Menus/WorkspaceManagerMenu.h"
#include "Settings/NetworkSettings.h"
#include "Settings/WifiConnector.h"
#include "Settings/LanguageManager.h"
#include "Settings/WorkspaceManager.h"
#include "Utilities/RotaryTextInput.h"
#include "CueStorage.h"

namespace osc_controller::menus {

using namespace osc_controller;

enum ThirdButtonAction {
  ACTION_NONE,
  ACTION_PANIC_ALL,
  ACTION_FIRE_CUSTOM,
  ACTION_GOTO_CUE,
  ACTION_STOP_ALL,
  ACTION_RESUME_ALL,
  ACTION_GO,
  ACTION_PANIC,
  ACTION_SAVE,
};

ThirdButtonAction thirdButtonAction = ACTION_NONE;
String customOscPath = "";
String workspaceID = "";
String gotoCueID = "";

Preferences settingsPrefs;
bool scrollLockEnabled = false;

void loadSettings() {
  settingsPrefs.begin("settings", true);
  thirdButtonAction = (ThirdButtonAction)settingsPrefs.getUChar("thirdAction", ACTION_NONE);
  customOscPath = settingsPrefs.getString("customPath", "");
  gotoCueID = settingsPrefs.getString("gotoCue", "");
  scrollLockEnabled = settingsPrefs.getBool("scrollLock", false);
  workspaceID = settingsPrefs.getString("workspaceID", "");
  settingsPrefs.end();
  
  settings::loadWorkspaces();
  workspaceID = settings::getPrimaryWorkspace().name;
}

void saveSettings() {
  settingsPrefs.begin("settings", false);
  settingsPrefs.putUChar("thirdAction", thirdButtonAction);
  settingsPrefs.putString("customPath", customOscPath);
  settingsPrefs.putString("gotoCue", gotoCueID);
  settingsPrefs.putBool("scrollLock", scrollLockEnabled);
  settingsPrefs.end();
}

bool isScrollLockEnabled() {
  settingsPrefs.begin("settings", true);
  bool value = settingsPrefs.getBool("scrollLock", false);
  settingsPrefs.end();
  return value;
}

void performFactoryReset() {
  lcd.clear();
  lcd.print(settings::t("reset_notify"));
  delay(1000);

  resetCues();
  settings::resetNetworkSettings(); 
  settingsPrefs.begin("settings", false);
  settingsPrefs.clear();
  settingsPrefs.end();

  settingsPrefs.begin("workspaces", false);
  settingsPrefs.clear();
  settingsPrefs.end();

  workspaceID = "";
  scrollLockEnabled = false;
  thirdButtonAction = ACTION_NONE;
  customOscPath = "";
  gotoCueID = "";
  settings::port = "53000";

  lcd.clear();
  lcd.print(settings::t("reset_done_notify"));
  delay(1500);
  forceRedraw = true;
}

void setupThirdButton() {
  const char* actions[] = {
    settings::t("action_none"),
    settings::t("action_panic_all"),
    settings::t("action_custom_osc"),
    settings::t("action_goto_cue"),
    settings::t("action_stop_all"),
    settings::t("action_resume_all"),
    settings::t("action_go"),
    settings::t("action_panic"),
    settings::t("action_save")
  };

  int actionCount = sizeof(actions) / sizeof(actions[0]);

  int selected = thirdButtonAction;
  long lastPos = -999;

  while (true) {
    encoder.tick();
    long newPos = encoder.getPosition();
    selected = (newPos % actionCount + actionCount) % actionCount;

    if (newPos != lastPos) {
      lastPos = newPos;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings::t("select_action"));
      lcd.setCursor(0, 1);
      lcd.print(actions[selected]);
    }

    if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      thirdButtonAction = (ThirdButtonAction)selected;

      if (thirdButtonAction == ACTION_FIRE_CUSTOM) {
        lcd.clear(); lcd.print(settings::t("enter_osc_path"));
        delay(500);
        utilities::initTextInput();
        while (!utilities::updateTextInput()) encoder.tick();
        if (!utilities::didUserCancel()) customOscPath = utilities::getFinalInput();
      }

      if (thirdButtonAction == ACTION_GOTO_CUE) {
        lcd.clear(); lcd.print(settings::t("enter_cue_id"));
        delay(500);
        utilities::initTextInput();
        while (!utilities::updateTextInput()) encoder.tick();
        if (!utilities::didUserCancel()) gotoCueID = utilities::getFinalInput();
      }

      saveSettings();
      lcd.clear();
      lcd.print(settings::t("saved_notify"));
      delay(800);
      break;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      break;
    }
  }
}

void performOTAUpdate() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(settings::t("checking_update"));

  delay(1000);

  String firmwareURL = "https://github.com/cuervo-blanco/osc-controller/releases/latest/download/firmware.bin"; 

  WiFiClient client;
  t_httpUpdate_return ret = httpUpdate.update(client, firmwareURL);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings::t("update_failed"));
      lcd.setCursor(0, 1);
      lcd.print(String(httpUpdate.getLastError()));
      break;
    case HTTP_UPDATE_NO_UPDATES:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings::t("no_updates"));
      break;
    case HTTP_UPDATE_OK:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings::t("update_done"));
      break;
  }

  delay(3000);
}


void handleSettingsMenu() {
  const char* items[] = {
    settings::t("scroll_lock_item"),
    settings::t("third_button_item"),
    settings::t("workspace_manager_item"),
    settings::t("network_item"),
    settings::t("factory_reset_item"),
    settings::t("device_info_item"),
    settings::t("language_item"),
    settings::t("update_item")
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
    lcd.clear();
    switch (selectedIndex) {
      case 0:
        scrollLockEnabled = !scrollLockEnabled;
        saveSettings();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(scrollLockEnabled ? settings::t("enabled") : settings::t("disabled"));
        delay(1000);
        break;
      case 1:
        setupThirdButton();
        break;
      case 2:
        currentState = settings::WORKSPACE_MANAGER_MENU;
        break;
      case 3:
        currentState = settings::NETWORK_MENU;
        break;
      case 4:
        lcd.clear();
        lcd.print(settings::t("confirm_reset"));
        lcd.setCursor(0, 1);
        lcd.print(settings::t("click_yes"));

        while (true) {
          encoder.tick();
          if (digitalRead(settings::ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            performFactoryReset();
            break;
          }
          if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            lcd.clear(); lcd.print(settings::t("cancelled_label"));
            delay(1000);
            break;
          }
        }
        break;
      case 5:
        handleDeviceInfo();
        break;
      case 6:
        lcd.clear();
        lcd.print(settings::t("language_label"));
        delay(1000);
        setLanguage(settings::getLanguage() == settings::LANG_EN ? settings::LANG_ES : settings::LANG_EN);
        lcd.clear();
        lcd.print(settings::getLanguage() == settings::LANG_EN ? "English" : "Espanol");
        delay(1000);
        break;
      case 7:
        if (WiFi.status() != WL_CONNECTED) {
          lcd.clear();
          lcd.print(settings::t("no_wifi"));
          delay(1000);
        } else {
          performOTAUpdate();
        }
        break;
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

void handleThirdButton() {
  if (digitalRead(settings::THIRD_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Not connected to Wi-Fi. Cannot send OSC.");
      return;
    }

    IPAddress targetIP;
    if (!targetIP.fromString(settings::ipAddress)) {
      Serial.println("Invalid IP address: " + settings::ipAddress);
      return;
    }

    String path;

    settings::MenuState returnState = currentState;

    switch (thirdButtonAction) {
      case ACTION_NONE:
        return; 
      case ACTION_PANIC_ALL:
        if (workspaceID == "") {
          Serial.println("Workspace ID not set.");
          lcd.clear(); 
          lcd.print(settings::t("no_workspace_id"));
          delay(800);
          lcd.clear();
          currentState = returnState;
          resetMenuState();
          forceRedraw = true;
          return;
        }
        path = "/workspace/" + workspaceID + "/panic";
        break;
      case ACTION_FIRE_CUSTOM:
        path = customOscPath.c_str();
        break;
      case ACTION_GOTO_CUE:
        path = "/cue/" + gotoCueID + "/start";
        break;
      case ACTION_STOP_ALL:
        if (workspaceID == "") {
          Serial.println("Workspace ID not set.");
          lcd.clear(); 
          lcd.print(settings::t("no_workspace_id"));
          delay(800);
          currentState = returnState;
          resetMenuState();
          forceRedraw = true;
          return;
        }
        path = "/workspace/" + workspaceID + "/stop";
        break;
      case ACTION_RESUME_ALL:
        if (workspaceID == "") {
          Serial.println("Workspace ID not set.");
          lcd.clear(); 
          lcd.print(settings::t("no_workspace_id"));
          delay(800);
          currentState = returnState;
          resetMenuState();
          forceRedraw = true;
          return;
        }
        path = "/workspace/" + workspaceID + "/resume";
        break;
      case ACTION_GO:
        path = "/go";
        break;
      case ACTION_PANIC:
        path = "/panic";
        break;

      case ACTION_SAVE:
        path = "/save";
        break;
    }

    if (path) {
      OSCMessage msg(path.c_str());
      Serial.print("Sending OSC: ");
      Serial.println(path);
      settings::udp.beginPacket(targetIP, settings::port.toInt());
      msg.send(settings::udp);
      settings::udp.endPacket();
      msg.empty();
    } else {
      Serial.println("OSC address is empty.");
    }
  }
}

void handleDeviceInfo() {
  lcd.clear();

  const int totalPages = 5;
  const unsigned long pageDuration = 2500; 
  unsigned long lastChange = millis();
  int page = 0;

  const char* version = "v1.0.0";
  String name = "Cuervo Blanco";
  String wsID = workspaceID.length() ? workspaceID : settings::t("none");
  String ip = settings::ipAddress.length() ? settings::ipAddress : settings::t("unset");
  String portStr = settings::port;
  String scroll = scrollLockEnabled ? settings::t("ON") : settings::t("OFF");

  const char* actionStrs[] = {
    settings::t("action_none"), settings::t("action_panic_all"), settings::t("action_custom_osc"),
    settings::t("action_goto_cue"), settings::t("action_stop_all"), settings::t("action_resume_all")
  };
  String action = actionStrs[thirdButtonAction];

  lcd.setCursor(0, 0); 
  lcd.print(settings::t("version_label")); 
  lcd.print(version);
  lcd.setCursor(0, 1); 
  lcd.print(name);
  page++;
  lastChange = millis();

  while (page < totalPages) {
    unsigned long now = millis();

    if (now - lastChange >= pageDuration) {
      lcd.clear();

      switch (page) {
        case 1:
          lcd.setCursor(0, 0); lcd.print(settings::t("workspace_id_label"));
          lcd.setCursor(0, 1); lcd.print(wsID.substring(0, 16));
          break;
        case 2:
          lcd.setCursor(0, 0); lcd.print(settings::t("qlab_ip_label"));
          lcd.setCursor(0, 1); lcd.print(ip.substring(0, 16));
          break;
        case 3:
          lcd.setCursor(0, 0); lcd.print(settings::t("port_label")); lcd.print(portStr);
          lcd.setCursor(0, 1); lcd.print(settings::t("scroll_label")); lcd.print(scroll);
          break;
        case 4:
          lcd.setCursor(0, 0); lcd.print(settings::t("third_btn_action_label"));
          lcd.setCursor(0, 1); lcd.print(action.substring(0, 16));
          break;
      }

      page++;
      lastChange = now;
    }

    if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      break;
    }

    delay(10);
  }

  currentState = settings::SETTINGS_MENU;
  resetMenuState();
  lcd.clear();
}

} // namespace osc_controller::menus
