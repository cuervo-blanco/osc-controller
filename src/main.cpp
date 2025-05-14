#include <WiFiUdp.h>
#include <Wire.h>
#include <OSCMessage.h>
#include "MainMenu.h"
#include "NetworkMenu.h"
#include "CuesMenu.h"
#include "WiFiConnector.h"
#include "RotaryTextInput.h"
#include "AppState.h"
#include "NetworkSettings.h"
#include "SettingsMenu.h"
#include "RunShowMenu.h"
#include "CueStorage.h"
#include "LanguageManager.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
RotaryEncoder encoder(ROTARY_DT, ROTARY_CLK);

MenuState currentState = MAIN_MENU;

int selectedIndex = 0;

bool forceRedraw = true;
unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 300;

void setup() {
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);
  pinMode(FIRE_BUTTON, INPUT_PULLUP);
  pinMode(THIRD_BUTTON, INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  loadLanguageSetting();
  loadNetworkSettings();

  if (connectedSSID != "" && password != "") {
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to ");
    Serial.println(connectedSSID);
    WiFi.begin(connectedSSID.c_str(), password.c_str());

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 8000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.println(WiFi.localIP());
      udp.begin(8888);
    } else {
      Serial.println("\nWiFi connect failed!");
    }
  } else {
    Serial.println("No stored network credentials.");
  }

  Serial.println("UDP started on port 8888");

  loadSettings();
  loadCues(); 

  lcd.init(); lcd.backlight();
  encoder.setPosition(0);
}

void handleEnterText(String& targetField, MenuState returnState) {
  if (updateTextInput()) {
    if (!didUserCancel()) {
      targetField = getFinalInput();
      if (returnState == NETWORK_MENU && &targetField == &port) {
        savePort(port.toInt());
      }
    }

    currentState = returnState;
    resetMenuState();
  }
}

void loop() {
  encoder.tick();
  static MenuState lastState = MAIN_MENU;
  if (currentState != lastState) {
    forceRedraw = true;
    lastState = currentState;
  }
  handleThirdButton();

  switch (currentState) {
    case MAIN_MENU: handleMainMenu(); break;

    case CUES_MENU: handleCuesMenu(); break;
    case ADD_CUE_MENU: handleAddCueMenu(); break;
    case EDIT_CUE_MENU: handleEditCueMenu(); break;
    case DELETE_CUE_MENU: handleDeleteCueMenu(); break;
    case CUE_CONTROL_MENU: handleCueControlMenu(); break;
    case REORDER_CUE_MENU: handleReorderCueMenu(); break;

    case RUN_SHOW_MENU: handleRunShowMenu(); break;

    case NETWORK_MENU: handleNetworkMenu(); break;
    case ENTER_PORT: handleEnterText(port, NETWORK_MENU); break;
    case ENTER_PASSCODE: handleEnterText(passcode, NETWORK_MENU); break;
    case ENTER_QLAB_IP: handleEnterQLabIP(); break;
    case SHOW_INFO: handleShowInfo(); break;

    case SETTINGS_MENU: handleSettingsMenu(); break;

  }

}

