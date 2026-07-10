#include <Arduino.h>
#include "HT_SSD1306Wire.h"
#include "config.h"
#include "types.h"
#include "display_ui.h"

static SSD1306Wire walkieDisplay(
  0x3c,
  500000,
  SDA_OLED,
  SCL_OLED,
  GEOMETRY_128_64,
  RST_OLED
);

String displayModeName(WalkieMode mode) {
  switch (mode) {
    case MODE_LOCAL:  return "LOCAL";
    case MODE_REMOTE: return "REMOTE";
    case MODE_AUTO:   return "AUTO";
  }

  return "?";
}

void setupDisplay() {
  walkieDisplay.init();
  walkieDisplay.flipScreenVertically();
  walkieDisplay.clear();
  walkieDisplay.display();
}

void showStartupScreen() {
  walkieDisplay.clear();
  walkieDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
  walkieDisplay.setFont(ArialMT_Plain_16);
  walkieDisplay.drawString(64, 8, "CXS WALKIE");

  walkieDisplay.setFont(ArialMT_Plain_10);
  walkieDisplay.drawString(64, 30, FW_VERSION);
  walkieDisplay.drawString(64, 46, "Initializing...");

  walkieDisplay.display();
  delay(700);
}

void showPoweringDownScreen() {
  walkieDisplay.clear();
  walkieDisplay.setTextAlignment(TEXT_ALIGN_CENTER);
  walkieDisplay.setFont(ArialMT_Plain_16);
  walkieDisplay.drawString(64, 18, "Powering");
  walkieDisplay.drawString(64, 36, "Down...");
  walkieDisplay.display();
}

void drawMainScreen(const WalkieState &state) {
  walkieDisplay.clear();
  walkieDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
  walkieDisplay.setFont(ArialMT_Plain_10);

  walkieDisplay.drawString(0, 0, "CXS " FW_VERSION);
  walkieDisplay.drawString(0, 12, "Mode: " + displayModeName(state.config.mode));
  walkieDisplay.drawString(0, 24, "TG: " + String(state.config.talkGroup));

if (state.config.mode == MODE_REMOTE || state.config.mode == MODE_AUTO) {
  if (state.runtime.transmitting) {
    walkieDisplay.drawString(0, 36, "Status: Remote TX");
  } else if (state.runtime.receiving) {
    walkieDisplay.drawString(0, 36, "Status: Remote RX");
  } else {
    walkieDisplay.drawString(0, 36, "Status: " + state.runtime.status);
  }

  if (state.runtime.wifiConnected) {
    walkieDisplay.drawString(
      0,
      50,
      "RSSI: " + String(state.runtime.wifiRSSI) + " dBm"
    );
  } else {
    walkieDisplay.drawString(0, 50, "No WiFi");
  }
  } else {
    walkieDisplay.drawString(0, 36, "Status: " + state.runtime.status);

    String bottom = "TX:" + String(state.runtime.txPackets) +
                    " RX:" + String(state.runtime.rxPackets);

    if (state.runtime.transmitting) bottom = "TRANSMIT";
    if (state.runtime.receiving) bottom = "RECEIVE";

    walkieDisplay.drawString(0, 50, bottom);
  }

  walkieDisplay.display();
}