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
  walkieDisplay.drawString(0, 13, displayModeName(state.config.mode));
  walkieDisplay.drawString(70, 13, "TG " + String(state.config.talkGroup));

  walkieDisplay.drawString(0, 29, state.runtime.status);

  String bottom = "TX " + String(state.runtime.txPackets) +
                  "  RX " + String(state.runtime.rxPackets);

  if (state.runtime.transmitting) bottom = "TRANSMITTING";
  if (state.runtime.receiving) bottom = "RECEIVING";

  walkieDisplay.drawString(0, 48, bottom);
  walkieDisplay.display();
}