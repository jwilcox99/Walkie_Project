#include <Arduino.h>

#include "config.h"
#include "types.h"
#include "audio.h"
#include "buttons.h"

void setupButtons() {
  pinMode(CH_PIN, INPUT_PULLUP);
  pinMode(PTT_PIN, INPUT_PULLUP);
  pinMode(PWR_SW_PIN, INPUT_PULLUP);
  pinMode(PTT_LED_PIN, OUTPUT);

  digitalWrite(PTT_LED_PIN, LOW);
}

static void nextTalkGroup(WalkieState &state) {
  state.config.talkGroup++;

  if (state.config.talkGroup > MAX_TALK_GROUPS) {
    state.config.talkGroup = 1;
  }

  state.runtime.status = "Talk Group " + String(state.config.talkGroup);
}

static void nextMode(WalkieState &state) {
  if (state.config.mode == MODE_LOCAL) {
    state.config.mode = MODE_REMOTE;
    state.runtime.status = "Remote soon";
    stopI2S();
  } else if (state.config.mode == MODE_REMOTE) {
    state.config.mode = MODE_AUTO;
    state.runtime.status = "Auto soon";
    stopI2S();
  } else {
    state.config.mode = MODE_LOCAL;
    state.runtime.status = "Ready";
    setupSpeaker();
  }
}

void handleFunctionButton(WalkieState &state) {
  static bool lastReading = HIGH;
  static bool stableState = HIGH;
  static uint32_t lastDebounceMs = 0;
  static uint32_t pressStartMs = 0;
  static bool longHandled = false;

  bool reading = digitalRead(CH_PIN);

  if (reading != lastReading) {
    lastDebounceMs = millis();
  }

  if ((millis() - lastDebounceMs) > DEBOUNCE_MS) {
    if (reading != stableState) {
      stableState = reading;

      if (stableState == LOW) {
        pressStartMs = millis();
        longHandled = false;
      } else {
        uint32_t pressDuration = millis() - pressStartMs;

        if (!longHandled && pressDuration < LONG_PRESS_MS) {
          nextTalkGroup(state);
        }
      }
    }

    if (
      stableState == LOW &&
      !longHandled &&
      millis() - pressStartMs >= LONG_PRESS_MS
    ) {
      nextMode(state);
      longHandled = true;
    }
  }

  lastReading = reading;
}

void handlePTT(WalkieState &state) {
  bool ptt = digitalRead(PTT_PIN) == LOW;

  digitalWrite(PTT_LED_PIN, ptt ? HIGH : LOW);

  if (state.config.mode != MODE_LOCAL) {
    state.runtime.transmitting = false;
    return;
  }

  if (ptt && !state.runtime.transmitting) {
    state.runtime.transmitting = true;
    state.runtime.receiving = false;
    state.runtime.status = "Transmitting";
    setupMic();
  }

  if (!ptt && state.runtime.transmitting) {
    state.runtime.transmitting = false;
    state.runtime.status = "Ready";
    setupSpeaker();
  }
}