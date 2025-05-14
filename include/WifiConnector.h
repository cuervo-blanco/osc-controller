#pragma once

#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

void connectToWiFi(LiquidCrystal_I2C& lcd, RotaryEncoder& encoder, int encoderButtonPin);

