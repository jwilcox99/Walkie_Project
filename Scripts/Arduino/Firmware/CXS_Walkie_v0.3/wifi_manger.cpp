#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "types.h"
#include "state_manager.h"
#include "wifi_manager.h"
#include "wifi_secrets.h"

static bool wifiConnecting = false;
static uint32_t wifiConnectStartMs = 0;
static uint32_t lastReconnectAttemptMs = 0;

#define WIFI_CONNECT_TIMEOUT_MS   12000
#define WIFI_RECONNECT_DELAY_MS   10000

void setupWifiManager() {
  WiFi.mode(WIFI_OFF);
}

void connectWifi(WalkieState &state) {
  if (WiFi.status() == WL_CONNECTED) {
    state.runtime.wifiConnected = true;
    state.runtime.wifiRSSI = WiFi.RSSI();
    state.runtime.wifiIP = WiFi.localIP().toString();
    state.runtime.status = "WiFi Connected";
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  wifiConnecting = true;
  wifiConnectStartMs = millis();

  state.runtime.wifiConnected = false;
  state.runtime.wifiRSSI = 0;
  state.runtime.wifiIP = "-";
  state.runtime.status = "WiFi Connecting";
}

void disconnectWifi(WalkieState &state) {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  wifiConnecting = false;

  state.runtime.wifiConnected = false;
  state.runtime.wifiRSSI = 0;
  state.runtime.wifiIP = "-";

  if (state.config.mode == MODE_REMOTE || state.config.mode == MODE_AUTO) {
    state.runtime.status = "WiFi Off";
  }
}

bool wifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String wifiIpAddress() {
  if (!wifiConnected()) return "-";
  return WiFi.localIP().toString();
}

int wifiSignalDbm() {
  if (!wifiConnected()) return 0;
  return WiFi.RSSI();
}

void wifiManagerLoop(WalkieState &state) {
  if (state.config.mode != MODE_REMOTE && state.config.mode != MODE_AUTO) {
    return;
  }

  if (wifiConnected()) {
    wifiConnecting = false;

    state.runtime.wifiConnected = true;
    state.runtime.wifiRSSI = WiFi.RSSI();
    state.runtime.wifiIP = WiFi.localIP().toString();
    state.runtime.status = "WiFi Connected";

    return;
  }

  state.runtime.wifiConnected = false;
  state.runtime.wifiRSSI = 0;
  state.runtime.wifiIP = "-";

  if (wifiConnecting) {
    state.runtime.status = "WiFi Connecting";

    if (millis() - wifiConnectStartMs > WIFI_CONNECT_TIMEOUT_MS) {
      wifiConnecting = false;
      state.runtime.status = "WiFi Failed";
      lastReconnectAttemptMs = millis();
    }

    return;
  }

  if (millis() - lastReconnectAttemptMs > WIFI_RECONNECT_DELAY_MS) {
    connectWifi(state);
    lastReconnectAttemptMs = millis();
  }
}
