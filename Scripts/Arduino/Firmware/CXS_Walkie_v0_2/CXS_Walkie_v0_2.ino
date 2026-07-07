#include "config.h"
#include "types.h"
#include "audio.h"
#include "display_ui.h"
#include "radio_local.h"
#include "buttons.h"
#include "power.h"

WalkieState state;

void setup() {
  Serial.begin(115200);
  delay(1000);

  state.config.mode = MODE_LOCAL;
  state.config.talkGroup = 1;

  state.runtime.status = "Booting";
  state.runtime.txPackets = 0;
  state.runtime.rxPackets = 0;
  state.runtime.lastRxMs = 0;
  state.runtime.transmitting = false;
  state.runtime.receiving = false;

  setupPowerPins();
  setupButtons();

  enableVE();          // move this BEFORE display init
  setupDisplay();
  showStartupScreen();

  setupSpeaker();
  setupLocalRadio();

  state.runtime.status = "Ready";
  drawMainScreen(state);
}

void loop() {
  static uint32_t lastScreenMs = 0;

  handlePower(state);
  handleFunctionButton(state);
  handlePTT(state);

  if (state.config.mode == MODE_LOCAL) {
    localRadioLoop(state);
  }

  if (millis() - lastScreenMs > 200) {
    drawMainScreen(state);
    lastScreenMs = millis();
  }
}