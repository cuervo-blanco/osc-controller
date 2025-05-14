#pragma once
#include <Arduino.h>

namespace osc_controller::settings {
enum Language {
  LANG_EN,
  LANG_ES
};

void setLanguage(Language lang);
Language getLanguage();
const char* t(const char* key); 
void loadLanguageSetting();
void saveLanguageSetting();
} // namespace Settings
