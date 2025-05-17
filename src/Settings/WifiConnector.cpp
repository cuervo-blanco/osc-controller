#include <Preferences.h>

#include "Settings/WifiConnector.h"
#include "Settings/AppState.h"
#include "Settings/NetworkSettings.h"
#include "Settings/LanguageManager.h"
#include "Utilities/RotaryTextInput.h"

namespace osc_controller::settings {

static bool selecting = true;
static bool enteringPassword = false;
static int selectedNetwork = 0;

static void showNetworkList(LiquidCrystal_I2C& lcd, int index, int total) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(t("select_network"));
  lcd.setCursor(0, 1);
  String ssid = WiFi.SSID(index);
  lcd.print(ssid.substring(0, 16));
}

void connectToWiFi(LiquidCrystal_I2C& lcd, RotaryEncoder& encoder, int encoderButtonPin) {
  selecting = true;
  enteringPassword = false;

  pinMode(encoderButtonPin, INPUT_PULLUP);
  encoder.setPosition(0);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(t("scanning_wifi"));
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int n = WiFi.scanNetworks();
  if (n == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(t("no_networks"));
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

    if (digitalRead(BACK_BUTTON) == LOW) {
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
    lcd.print(t("enter_password"));
    delay(1000);
    utilities::initTextInput();
    enteringPassword = true;
  }

  while (enteringPassword) {
    if (utilities::updateTextInput()) {
      enteringPassword = false;
      if (utilities::didUserCancel()) {
        lcd.clear();
        lcd.print(t("cancelled"));
        delay(500);
        return;
      }
      password = utilities::getFinalInput();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(t("connecting"));
      WiFi.begin(ssid.c_str(), password.c_str());

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(100);
      }

      lcd.clear();
      if (WiFi.status() == WL_CONNECTED) {
        saveNetworkCredentials(ssid, password);

        lcd.setCursor(0, 0);
        lcd.print(t("connected"));
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());

      } else {
        lcd.setCursor(0, 0);
        lcd.print(t("connection_failed."));
        lcd.setCursor(0, 1);
        lcd.print(t("try_again"));
      }

      delay(500);
    }
  }
}

} // namespace osc_controller::settings
