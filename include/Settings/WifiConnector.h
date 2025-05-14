#pragma once

#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

namespace osc_controller::settings {
void connectToWiFi(LiquidCrystal_I2C& lcd, RotaryEncoder& encoder, int encoderButtonPin);
} // namespace Settings

