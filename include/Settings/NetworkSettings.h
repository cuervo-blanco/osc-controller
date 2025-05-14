#pragma once
#include <Arduino.h>
#include "CueStorage.h"

namespace osc_controller::settings {
extern WiFiUDP udp;

void loadNetworkSettings();
void saveNetworkCredentials(const String& ssid, const String& pass);
void saveQLABIP(const String& ip);
void savePort(uint16_t port);
void savePasscode(const String& passcode);

void resetNetworkSettings();
bool hasSavedNetworkSettings();
void sendCueOSC(const Cue& cue);

extern String connectedSSID;
extern String password;
extern String passcode;
extern String ipAddress;
extern String port;
} // namespace Settings
