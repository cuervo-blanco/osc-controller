#include <Preferences.h>
#include <OSCMessage.h>
#include <WiFiUdp.h>

#include "Settings/NetworkSettings.h"
#include "Settings/WifiConnector.h"

namespace osc_controller::settings {
Preferences prefs;
WiFiUDP udp;

String connectedSSID = "";
String password = "";
String passcode = "";
String ipAddress = "";
String port = "53000";

void loadNetworkSettings() {
   if (!prefs.begin("net", false)) {
    Serial.println("Failed to open preferences in read/write mode.");
    return;
  }
  connectedSSID = prefs.getString("ssid", "");
  password = prefs.getString("password", "");
  passcode = prefs.getString("passcode", "");
  ipAddress = prefs.getString("ip", "");
  port = String(prefs.getUInt("port", 53000));
  prefs.end();

  Serial.println("Loaded Network Credentials:");
  Serial.println("SSID: " + connectedSSID);
  Serial.println("Password: " + password);
  Serial.println("IP: " + ipAddress);
  Serial.println("Port: " + port);
}

void saveNetworkCredentials(const String& ssid, const String& password) {
  if (!prefs.begin("net", false)) {
    Serial.println("Failed to open preferences for writing.");
    return;
  }
  if (ssid != "") prefs.putString("ssid", ssid);
  if (password != "") prefs.putString("password", password);
  prefs.end();

  Serial.println("Saved Network Credentials:");
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);
}

void saveQLABIP(const String& ip) {
  if (!prefs.begin("net", false)) {
    Serial.println("Failed to open preferences for saving IP.");
    return;
  }
  prefs.putString("ip", ip);
  prefs.end();

  Serial.println("Saved QLab IP:");
  Serial.println("IP: " + ip);
}

void savePort(uint16_t portNum) {
  if (!prefs.begin("net", false)) {
    Serial.println("Failed to open preferences for saving port.");
    return;
  }
  prefs.putUInt("port", portNum);
  prefs.end();
}

void savePasscode(const String& passcode) {
  if (!prefs.begin("net", false)) {
    Serial.println("Failed to open preferences for saving passcode.");
    return;
  }

  prefs.putString("passcode", passcode);
  prefs.end();
}

void resetNetworkSettings() {
  if (!prefs.begin("net", false)) {
    Serial.println("Failed to open preferences for reset.");
    return;
  }
  prefs.clear();
  prefs.end();

  connectedSSID = "";
  password = "";
  ipAddress = "";
  port = "53000";
  passcode = "";
}

bool hasSavedNetworkSettings() {
  if (!prefs.begin("net", false)) {
    Serial.println("Failed to check for saved network settings.");
    return false;
  }
  bool exists = prefs.isKey("ssid");
  prefs.end();
  return exists;
}

void sendCueOSC(const Cue& cue) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Skipping OSC send: Not connected to Wi-Fi.");
    return;
  }

  if (ipAddress.isEmpty()) {
    Serial.println("Empty IP address");
    return;
  }

  IPAddress targetIP;
  if (!targetIP.fromString(ipAddress)) {
    Serial.println("Invalid IP address: " + ipAddress);
    return;
  }

  int portNum = port.toInt();
  if (portNum <= 0 || portNum > 65535) {
    Serial.println("Skipping OSC send: Invalid port number: " + port);
    return;
  }

  Serial.print("Sending OSC to ");
  Serial.print(targetIP);
  Serial.print(":");
  Serial.print(portNum);
  Serial.print(" -> ");
  Serial.println(cue.oscCommand);

  if (!udp.beginPacket(targetIP, portNum)) {
    Serial.println("Failed to begin UDP packet.");
    return;
  }

  OSCMessage msg(cue.oscCommand.c_str());
  msg.send(udp);
  udp.endPacket();
  msg.empty();
}

} // namespace osc_controller::settings
