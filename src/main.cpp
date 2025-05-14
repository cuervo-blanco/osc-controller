#include <WiFiUdp.h>
#include <Wire.h>
#include <OSCMessage.h>
#include "Menus/MainMenu.h"
#include "Menus/NetworkMenu.h"
#include "Menus/CuesMenu.h"
#include "Menus/SettingsMenu.h"
#include "Menus/RunShowMenu.h"
#include "Settings/AppState.h"
#include "Settings/WiFiConnector.h"
#include "Settings/NetworkSettings.h"
#include "Settings/LanguageManager.h"
#include "Utilities/RotaryTextInput.h"
#include "Globals.h"
#include "CueStorage.h"

void handleEnterText(String& targetField, osc_controller::settings::MenuState returnState) {
  if (osc_controller::utilities::updateTextInput()) {
    if (!osc_controller::utilities::didUserCancel()) {
      targetField = osc_controller::utilities::getFinalInput();
      if (returnState == osc_controller::settings::NETWORK_MENU && &targetField == &osc_controller::settings::port) {
        osc_controller::settings::savePort(osc_controller::settings::port.toInt());
      }
    }

    osc_controller::currentState = returnState;
    osc_controller::menus::resetMenuState();
  }
}

void setup() {
  using namespace osc_controller;
  pinMode(settings::ENCODER_SW, INPUT_PULLUP);
  pinMode(settings::BACK_BUTTON, INPUT_PULLUP);
  pinMode(settings::FIRE_BUTTON, INPUT_PULLUP);
  pinMode(settings::THIRD_BUTTON, INPUT_PULLUP);
  Wire.begin(settings::SDA_PIN, settings::SCL_PIN);

  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting...");

  settings::loadLanguageSetting();
  settings::loadNetworkSettings();

  if (settings::connectedSSID != "" && settings::password != "") {
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to ");
    Serial.println(settings::connectedSSID);
    WiFi.begin(settings::connectedSSID.c_str(), settings::password.c_str());

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 8000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.println(WiFi.localIP());
      settings::udp.begin(8888);
    } else {
      Serial.println("\nWiFi connect failed!");
    }
  } else {
    Serial.println("No stored network credentials.");
  }

  Serial.println("UDP started on port 8888");

  menus::loadSettings();
  loadCues(); 

  lcd.init(); 
  lcd.backlight();
  encoder.setPosition(0);
}


void loop() {
  using namespace osc_controller;
  encoder.tick();
  static settings::MenuState lastState = settings::MAIN_MENU;
  if (currentState != lastState) {
    forceRedraw = true;
    lastState = currentState;
  }
  menus::handleThirdButton();

  switch (currentState) {
    case settings::MAIN_MENU: menus::handleMainMenu(); break;

    case settings::CUES_MENU: menus::handleCuesMenu(); break;
    case settings::ADD_CUE_MENU: menus::handleAddCueMenu(); break;
    case settings::EDIT_CUE_MENU: menus::handleEditCueMenu(); break;
    case settings::DELETE_CUE_MENU: menus::handleDeleteCueMenu(); break;
    case settings::CUE_CONTROL_MENU: menus::handleCueControlMenu(); break;
    case settings::REORDER_CUE_MENU: menus::handleReorderCueMenu(); break;

    case settings::RUN_SHOW_MENU: menus::handleRunShowMenu(); break;

    case settings::NETWORK_MENU: menus::handleNetworkMenu(); break;
    case settings::ENTER_PORT: handleEnterText(settings::port, settings::NETWORK_MENU); break;
    case settings::ENTER_PASSCODE: handleEnterText(settings::passcode, settings::NETWORK_MENU); break;
    case settings::ENTER_QLAB_IP: menus::handleEnterQLabIP(); break;
    case settings::SHOW_INFO: menus::handleShowInfo(); break;

    case settings::SETTINGS_MENU: menus::handleSettingsMenu(); break;

  }

}
