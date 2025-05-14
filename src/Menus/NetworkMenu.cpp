#include <Preferences.h>
#include <ESP32Ping.h>
#include <WiFi.h>

#include "Globals.h"
#include "Menus/MainMenu.h"
#include "Menus/NetworkMenu.h"
#include "Settings/WiFiConnector.h"
#include "Settings/NetworkSettings.h"
#include "Settings/AppState.h"
#include "Settings/LanguageManager.h"
#include "Utilities/RotaryTextInput.h"

namespace osc_controller::menus {

using namespace osc_controller;

void handleNetworkTest() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(settings::t("pinging"));
  delay(500);

  IPAddress targetIP;
  if (!targetIP.fromString(settings::ipAddress)) {
    lcd.clear();
    lcd.print(settings::t("invalid_ip"));
    delay(1500);
    currentState = settings::NETWORK_MENU;
    return;
  }

  int successCount = 0;
  for (int i = 0; i < 4; ++i) {
    bool success = Ping.ping(targetIP, 1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ping " + String(i + 1) + ": " + (success ? "OK" : settings::t("fail")));
    if (success) successCount++;
    delay(800);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(settings::t("success"));
  lcd.print(successCount);
  lcd.print("/4");
  delay(1500);
  currentState = settings::NETWORK_MENU;
}

void handleNetworkMenu() {
  const char* items[] = {
    settings::t("scan_item"),
    settings::t("port_item"),
    settings::t("passcode_item"),
    settings::t("qlab_ip_item"),
    settings::t("info_item"),
    settings::t("reset_item"),
    settings::t("test_item"),
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
      case 0:
        settings::connectToWiFi(lcd, encoder, settings::ENCODER_SW);
        settings::connectedSSID = WiFi.SSID();
        settings::ipAddress = WiFi.localIP().toString();
        currentState = settings::NETWORK_MENU;
        break;
      case 1:
        utilities::initPrefilledInput(settings::port, true, false, 16);
        currentState = settings::ENTER_PORT;
        break;
      case 2:
        utilities::initPrefilledInput(settings::passcode, false, false, 16);
        currentState = settings::ENTER_PASSCODE;
        break;
      case 3:
        utilities::initPrefilledInput(settings::ipAddress, false, true, 15);
        currentState = settings::ENTER_QLAB_IP;
        break;
      case 4:
        currentState = settings::SHOW_INFO;
        break;
      case 5:
        lcd.clear();
        lcd.print(settings::t("reset_notify"));
        settings::resetNetworkSettings();
        delay(500);
        lcd.clear();
        lcd.print(settings::t("done_notify"));
        delay(500);
        break;
      case 6:
        handleNetworkTest();
        break;
    }
    resetMenuState();
    lastDrawnIndex = -1;
  }

  if (digitalRead(settings::BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = settings::SETTINGS_MENU;
    resetMenuState();
    lastDrawnIndex = -1;
    lcd.clear();
  }
}

void handleEnterQLabIP() {
  if (utilities::updateTextInput()) {
    if (!utilities::didUserCancel()) {
      settings::ipAddress = utilities::getFinalInput();
      settings::saveQLABIP(settings::ipAddress);
    }
    currentState = settings::NETWORK_MENU;
    resetMenuState();
  }
}

void handleShowInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SSID: ");
  lcd.print(settings::connectedSSID.substring(0, 9));
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.print(settings::ipAddress);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String(settings::t("port_item")) + ": ");
  lcd.print(settings::port);
  lcd.setCursor(0, 1);
  lcd.print(String(settings::t("passcode_item")) + ":");
  lcd.print(settings::passcode);
  delay(2000);
  currentState = settings::NETWORK_MENU;
}

} // namespace osc_controller::menus
