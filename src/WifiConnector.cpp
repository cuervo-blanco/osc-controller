#include <Preferences.h>

#include "WiFiConnector.h"
#include "RotaryTextInput.h"
#include "NetworkSettings.h"

#define BACKSPACE_BUTTON 13

static bool selecting = true;
static bool enteringPassword = false;
static int selectedNetwork = 0;

static void showNetworkList(LiquidCrystal_I2C& lcd, int index, int total) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select Network:");
  lcd.setCursor(0, 1);
  String ssid = WiFi.SSID(index);
  lcd.print(ssid.substring(0, 16)); // truncate to fit display
}

void connectToWiFi(LiquidCrystal_I2C& lcd, RotaryEncoder& encoder, int encoderButtonPin) {
  selecting = true;
  enteringPassword = false;

  pinMode(encoderButtonPin, INPUT_PULLUP);
  encoder.setPosition(0);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Scanning WiFi...");
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks();
  if (n == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No networks!");
    delay(500);
    return;
  }

  showNetworkList(lcd, 0, n);

  static long lastPos = -999;

  while (selecting) {
    encoder.tick();
    long newPos = encoder.getPosition();

    if (newPos != lastPos) {
      lastPos = newPos;
      selectedNetwork = (newPos % n + n) % n;
      showNetworkList(lcd, selectedNetwork, n);
    }

    if (digitalRead(encoderButtonPin) == LOW) {
      delay(300);
      selecting = false;
    }

    if (digitalRead(BACKSPACE_BUTTON) == LOW) {
      delay(300);
      lcd.clear();
      delay(500);
      return; 
    }
  }

  String ssid = WiFi.SSID(selectedNetwork);
  String password = "";

  if (WiFi.encryptionType(selectedNetwork) == WIFI_AUTH_OPEN) {
    WiFi.begin(ssid.c_str());
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter password:");
    delay(1000);
    initTextInput();
    enteringPassword = true;
  }

  while (enteringPassword) {
    if (updateTextInput()) {
      enteringPassword = false;
      if (didUserCancel()) {
        lcd.clear();
        lcd.print("Cancelled");
        delay(500);
        return;
      }
      password = getFinalInput();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connecting...");
      WiFi.begin(ssid.c_str(), password.c_str());

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(100);
      }

      lcd.clear();
      if (WiFi.status() == WL_CONNECTED) {
        saveNetworkCredentials(ssid, password);

        lcd.setCursor(0, 0);
        lcd.print("Connected!");
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());

      } else {
        lcd.setCursor(0, 0);
        lcd.print("Connect failed.");
        lcd.setCursor(0, 1);
        lcd.print("Try again.");
      }

      delay(500);
    }
  }
}

