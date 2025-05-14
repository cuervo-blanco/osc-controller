#include <WiFiUdp.h>
#include <Preferences.h>
#include <OSCMessage.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include "SettingsMenu.h"
#include "MainMenu.h"
#include "CueStorage.h"
#include "NetworkSettings.h"
#include "RotaryTextInput.h"
#include "WifiConnector.h"
#include "LanguageManager.h"

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
}

void saveSettings() {
  settingsPrefs.begin("settings", false);
  settingsPrefs.putUChar("thirdAction", thirdButtonAction);
  settingsPrefs.putString("customPath", customOscPath);
  settingsPrefs.putString("gotoCue", gotoCueID);
  settingsPrefs.putBool("scrollLock", scrollLockEnabled);
  settingsPrefs.putString("workspaceID", workspaceID);
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
  lcd.print(t("reset_notify"));
  delay(1000);

  resetCues();
  resetNetworkSettings(); 
  settingsPrefs.begin("settings", false);
  settingsPrefs.clear();
  settingsPrefs.end();

  workspaceID = "";
  scrollLockEnabled = false;
  thirdButtonAction = ACTION_NONE;
  customOscPath = "";
  gotoCueID = "";
  port = "53000";

  lcd.clear();
  lcd.print(t("reset_done_notify"));
  delay(1500);
  forceRedraw = true;
}

void setupThirdButton() {
  const char* actions[] = {
    t("action_none"),
    t("action_panic_all"),
    t("action_custom_osc"),
    t("action_goto_cue"),
    t("action_stop_all"),
    t("action_resume_all"),
    t("action_go"),
    t("action_panic"),
    t("action_save")
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
      lcd.print(t("select_action"));
      lcd.setCursor(0, 1);
      lcd.print(actions[selected]);
    }

    if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      thirdButtonAction = (ThirdButtonAction)selected;

      if (thirdButtonAction == ACTION_FIRE_CUSTOM) {
        lcd.clear(); lcd.print(t("enter_osc_path"));
        delay(500);
        initTextInput();
        while (!updateTextInput()) encoder.tick();
        if (!didUserCancel()) customOscPath = getFinalInput();
      }

      if (thirdButtonAction == ACTION_GOTO_CUE) {
        lcd.clear(); lcd.print(t("enter_cue_id"));
        delay(500);
        initTextInput();
        while (!updateTextInput()) encoder.tick();
        if (!didUserCancel()) gotoCueID = getFinalInput();
      }

      saveSettings();
      lcd.clear();
      lcd.print(t("saved_notify"));
      delay(800);
      break;
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      break;
    }
  }
}

void performOTAUpdate() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(t("checking_update"));

  delay(1000);

  String firmwareURL = "https://github.com/cuervo-blanco/osc-controller/releases/latest/download/firmware.bin"; 

  WiFiClient client;
  t_httpUpdate_return ret = httpUpdate.update(client, firmwareURL);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("update_failed"));
      lcd.setCursor(0, 1);
      lcd.print(String(httpUpdate.getLastError()));
      break;
    case HTTP_UPDATE_NO_UPDATES:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("no_updates"));
      break;
    case HTTP_UPDATE_OK:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("update_done"));
      break;
  }

  delay(3000);
}


void handleSettingsMenu() {
  const char* items[] = {
    t("scroll_lock_item"),
    t("third_button_item"),
    t("workspace_id_item"),
    t("network_item"),
    t("factory_reset_item"),
    t("device_info_item"),
    t("language_item"),
    t("update_item")
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
    lcd.clear();
    switch (selectedIndex) {
      case 0:
        scrollLockEnabled = !scrollLockEnabled;
        saveSettings();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(scrollLockEnabled ? t("enabled") : t("disabled"));
        delay(1000);
        break;
      case 1:
        setupThirdButton();
        break;
      case 2:
        lcd.clear(); 
        initPrefilledInput(workspaceID);
        while (!updateTextInput()) encoder.tick();
        if (!didUserCancel()) {
          workspaceID = getFinalInput();
          saveSettings();
          lcd.clear(); 
          lcd.print(t("saved_notify"));
          delay(800);
        }
        break;
      case 3:
        currentState = NETWORK_MENU;
        break;
      case 4:
        lcd.clear();
        lcd.print(t("confirm_reset"));
        lcd.setCursor(0, 1);
        lcd.print(t("click_yes"));

        while (true) {
          encoder.tick();
          if (digitalRead(ENCODER_SW) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            performFactoryReset();
            break;
          }
          if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
            lastPressTime = millis();
            lcd.clear(); lcd.print(t("cancelled_label"));
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
        lcd.print(t("language_label"));
        delay(1000);
        setLanguage(getLanguage() == LANG_EN ? LANG_ES : LANG_EN);
        lcd.clear();
        lcd.print(getLanguage() == LANG_EN ? "English" : "Espanol");
        delay(1000);
        break;
      case 7:
        if (WiFi.status() != WL_CONNECTED) {
          lcd.clear();
          lcd.print(t("no_wifi"));
          delay(1000);
        } else {
          performOTAUpdate();
        }
        break;
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

void handleThirdButton() {
  if (digitalRead(THIRD_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Not connected to Wi-Fi. Cannot send OSC.");
      return;
    }

    IPAddress targetIP;
    if (!targetIP.fromString(ipAddress)) {
      Serial.println("Invalid IP address: " + ipAddress);
      return;
    }

    String path;

    MenuState returnState = currentState;

    switch (thirdButtonAction) {
      case ACTION_NONE:
        return; 
      case ACTION_PANIC_ALL:
        if (workspaceID == "") {
          Serial.println("Workspace ID not set.");
          lcd.clear(); 
          lcd.print(t("no_workspace_id"));
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
          lcd.print(t("no_workspace_id"));
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
          lcd.print(t("no_workspace_id"));
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
      udp.beginPacket(targetIP, port.toInt());
      msg.send(udp);
      udp.endPacket();
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
  String wsID = workspaceID.length() ? workspaceID : "<none>";
  String ip = ipAddress.length() ? ipAddress : "<unset>";
  String portStr = port;
  String scroll = scrollLockEnabled ? "ON" : "OFF";

  const char* actionStrs[] = {
    t("action_none"), t("action_panic_all"), t("action_custom_osc"),
    t("action_goto_cue"), t("action_stop_all"), t("action_resume_all")
  };
  String action = actionStrs[thirdButtonAction];

  lcd.setCursor(0, 0); 
  lcd.print(t("version_label")); 
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
          lcd.setCursor(0, 0); lcd.print(t("workspace_id_label"));
          lcd.setCursor(0, 1); lcd.print(wsID.substring(0, 16));
          break;
        case 2:
          lcd.setCursor(0, 0); lcd.print(t("qlab_ip_label"));
          lcd.setCursor(0, 1); lcd.print(ip.substring(0, 16));
          break;
        case 3:
          lcd.setCursor(0, 0); lcd.print(t("port_label")); lcd.print(portStr);
          lcd.setCursor(0, 1); lcd.print(t("scroll_label")); lcd.print(scroll);
          break;
        case 4:
          lcd.setCursor(0, 0); lcd.print(t("third_btn_action_label"));
          lcd.setCursor(0, 1); lcd.print(action.substring(0, 16));
          break;
      }

      page++;
      lastChange = now;
    }

    if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
      lastPressTime = millis();
      break;
    }

    delay(10);
  }

  currentState = SETTINGS_MENU;
  resetMenuState();
  lcd.clear();
}

