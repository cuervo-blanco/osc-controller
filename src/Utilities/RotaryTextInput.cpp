#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

#include "Utilities/RotaryTextInput.h"
#include "Settings/AppState.h"

namespace osc_controller::utilities {

using namespace osc_controller;

static LiquidCrystal_I2C lcd(0x27, 16, 2);
static RotaryEncoder encoder(settings::ROTARY_DT, settings::ROTARY_CLK);

static String inputBuffer = "";
static const int maxVisibleChars = 16;

static const char charSetText[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 .,;'`~!?@#$%&*()-_=+<>[]{}/";
static const char charSetNumbers[] = "0123456789";
static const char charSetIP[] = "0123456789.";

static const char* activeCharSet = charSetText;
static int numChars = sizeof(charSetText) - 1;

static int cursorPos = 0;
static int currentCharIndex = 0;
static long lastPos = -999;
static int scrollOffset = 0;
static int maxInputLength = 16;

static bool finished = false;
static bool isNumberInput = false;
static bool isIPInput = false;
static bool userCanceled = false;

bool didUserCancel() {
  return userCanceled;
}

void updateLCDLine() {
  lcd.setCursor(0, 0);
  lcd.print("                "); 

  int start = scrollOffset;
  int end = min(scrollOffset + maxVisibleChars, (int)inputBuffer.length());
  String visible = inputBuffer.substring(start, end);

  while (visible.length() < maxVisibleChars) visible += ' ';
  lcd.setCursor(0, 0);
  lcd.print(visible);

  int cursorInWindow = inputBuffer.length() - scrollOffset; 
  if (cursorInWindow < maxVisibleChars) { 
    lcd.setCursor(cursorInWindow, 0);
    lcd.print('_');
  }
}

void initTextInput() {
  userCanceled = false;
  isNumberInput = false;
  isIPInput = false;
  maxInputLength = 16;
  activeCharSet = charSetText;
  numChars = sizeof(charSetText) - 1;

  pinMode(settings::FIRE_BUTTON, INPUT_PULLUP);
  pinMode(settings::ENCODER_SW, INPUT_PULLUP);
  pinMode(settings::BACK_BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  encoder.setPosition(0);

  inputBuffer = "";
  cursorPos = 0;
  scrollOffset = 0;
  currentCharIndex = 0;
  lastPos = -999;
  finished = false;

  lcd.clear();
  updateLCDLine();
}

void initNumberInput(int maxLen) {
  userCanceled = false;
  isNumberInput = true;
  maxInputLength = maxLen;
  activeCharSet = charSetNumbers;
  numChars = sizeof(charSetNumbers) - 1;

  pinMode(settings::FIRE_BUTTON, INPUT_PULLUP);
  pinMode(settings::ENCODER_SW, INPUT_PULLUP);
  pinMode(settings::BACK_BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  encoder.setPosition(0);

  inputBuffer = "";
  cursorPos = 0;
  scrollOffset = 0;
  currentCharIndex = 0;
  lastPos = -999;
  finished = false;

  lcd.clear();
  updateLCDLine();
}

void initIPInput() {
  userCanceled = false;
  isIPInput = true;
  maxInputLength = 15;
  activeCharSet = charSetIP;
  numChars = sizeof(charSetIP) - 1;

  pinMode(settings::FIRE_BUTTON, INPUT_PULLUP);
  pinMode(settings::ENCODER_SW, INPUT_PULLUP);
  pinMode(settings::BACK_BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  encoder.setPosition(0);

  inputBuffer = "";
  cursorPos = 0;
  scrollOffset = 0;
  currentCharIndex = 0;
  lastPos = -999;
  finished = false;

  lcd.clear();
  updateLCDLine();
}

void initPrefilledInput(const String& initialText, bool numberOnly, bool ipOnly, int maxLen) {
  userCanceled = false;
  isNumberInput = numberOnly;
  isIPInput = ipOnly;
  maxInputLength = maxLen;

  if (numberOnly) {
    activeCharSet = charSetNumbers;
    numChars = sizeof(charSetNumbers) - 1;
  } else if (ipOnly) {
    activeCharSet = charSetIP;
    numChars = sizeof(charSetIP) - 1;
  } else {
    activeCharSet = charSetText;
    numChars = sizeof(charSetText) - 1;
  }

  pinMode(settings::FIRE_BUTTON, INPUT_PULLUP);
  pinMode(settings::ENCODER_SW, INPUT_PULLUP);
  pinMode(settings::BACK_BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  inputBuffer = initialText;
  cursorPos = initialText.length();
  scrollOffset = max(0, cursorPos - maxVisibleChars + 1);
  currentCharIndex = 0;
  lastPos = -999;
  encoder.setPosition(0);
  finished = false;

  lcd.clear();
  updateLCDLine();
}

bool updateTextInput() {
  if (finished) return true;

  encoder.tick();
  long newPos = encoder.getPosition();

  if (newPos != lastPos) {
    lastPos = newPos;
    currentCharIndex = (newPos % numChars + numChars) % numChars;

    lcd.setCursor(cursorPos, 0);
    lcd.print(activeCharSet[currentCharIndex]);
    lcd.setCursor(cursorPos, 0);
  }

  if (digitalRead(settings::ENCODER_SW) == LOW) {
    delay(200);

    if ((int)inputBuffer.length() < maxInputLength) {
      inputBuffer += activeCharSet[currentCharIndex];

      int totalLength = inputBuffer.length();
      scrollOffset = max(0, totalLength - maxVisibleChars);
      cursorPos = min(totalLength - scrollOffset, maxVisibleChars - 1);

      encoder.setPosition(0);
      currentCharIndex = 0;
      lastPos = 0;

      updateLCDLine();
    }
  }

  if (digitalRead(settings::BACK_BUTTON) == LOW) {
    delay(200);

    if (inputBuffer.length() > 0) {
      inputBuffer.remove(inputBuffer.length() - 1);

      if (scrollOffset > 0 && inputBuffer.length() <= scrollOffset) {
        scrollOffset--;
      }

      cursorPos = min((int)inputBuffer.length() - scrollOffset, maxVisibleChars - 1);
      if (cursorPos < 0) cursorPos = 0;

    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      delay(500);
      userCanceled = true;
      finished = true;
      return true;
    }
    updateLCDLine();
  }

  if (digitalRead(settings::FIRE_BUTTON) == LOW) {
    delay(200);
    lcd.clear();
    finished = true;
    return true;
  }

  return false;
}

String getFinalInput() {
  return inputBuffer;
}

} // namespace osc_controller::utilities
