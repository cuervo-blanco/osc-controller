#include <Preferences.h>
#include <ESP32Ping.h>
#include <WiFi.h>

#include "NetworkMenu.h"
#include "WiFiConnector.h"
#include "RotaryTextInput.h"
#include "MainMenu.h"
#include "NetworkSettings.h"
#include "AppState.h"
#include "LanguageManager.h"

void handleNetworkTest() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(t("pinging"));
  delay(500);

  IPAddress targetIP;
  if (!targetIP.fromString(ipAddress)) {
    lcd.clear();
    lcd.print(t("invalid_ip"));
    delay(1500);
    currentState = NETWORK_MENU;
    return;
  }

  int successCount = 0;
  for (int i = 0; i < 4; ++i) {
    bool success = Ping.ping(targetIP, 1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ping " + String(i + 1) + ": " + (success ? "OK" : t("fail")));
    if (success) successCount++;
    delay(800);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(t("success"));
  lcd.print(successCount);
  lcd.print("/4");
  delay(1500);
  currentState = NETWORK_MENU;
}

void handleNetworkMenu() {
  const char* items[] = {
    t("scan_item"),
    t("port_item"),
    t("passcode_item"),
    t("qlab_ip_item"),
    t("info_item"),
    t("reset_item"),
    t("test_item"),
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
      case 0:
        connectToWiFi(lcd, encoder, ENCODER_SW);
        connectedSSID = WiFi.SSID();
        ipAddress = WiFi.localIP().toString();
        currentState = NETWORK_MENU;
        break;
      case 1:
        initPrefilledInput(port, true, false, 16);
        currentState = ENTER_PORT;
        break;
      case 2:
        initPrefilledInput(passcode, false, false, 16);
        currentState = ENTER_PASSCODE;
        break;
      case 3:
        initPrefilledInput(ipAddress, false, true, 15);
        currentState = ENTER_QLAB_IP;
        break;
      case 4:
        currentState = SHOW_INFO;
        break;
      case 5:
        lcd.clear();
        lcd.print(t("reset_notify"));
        resetNetworkSettings();
        delay(500);
        lcd.clear();
        lcd.print(t("done_notify"));
        delay(500);
        break;
      case 6:
        handleNetworkTest();
        break;
    }
    resetMenuState();
    lastDrawnIndex = -1;
  }

  if (digitalRead(BACK_BUTTON) == LOW && millis() - lastPressTime > debounceDelay) {
    lastPressTime = millis();
    currentState = SETTINGS_MENU;
    resetMenuState();
    lastDrawnIndex = -1;
    lcd.clear();
  }
}

void handleEnterQLabIP() {
  if (updateTextInput()) {
    if (!didUserCancel()) {
      ipAddress = getFinalInput();
      saveQLABIP(ipAddress);
    }
    currentState = NETWORK_MENU;
    resetMenuState();
  }
}

void handleShowInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SSID: ");
  lcd.print(connectedSSID.substring(0, 9));
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.print(ipAddress);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String(t("port_item")) + ": ");
  lcd.print(port);
  lcd.setCursor(0, 1);
  lcd.print(String(t("passcode_item")) + ":");
  lcd.print(passcode);
  delay(2000);
  currentState = NETWORK_MENU;
}

