#include <Arduino.h>

#include "state_manager.h"
#include "settings.h"

void setStatus(WalkieState &state, const String &status) {
  state.runtime.status = status;
}

void setMode(WalkieState &state, WalkieMode mode) {
  state.config.mode = mode;
  markSettingsDirty();
}

void setTalkGroup(WalkieState &state, uint16_t talkGroup) {
  state.config.talkGroup = talkGroup;
  markSettingsDirty();
}

void beginTransmit(WalkieState &state) {
  state.runtime.transmitting = true;
  state.runtime.receiving = false;
  state.runtime.status = "Transmitting";
}

void endTransmit(WalkieState &state) {
  state.runtime.transmitting = false;
  state.runtime.status = "Ready";
}

void beginReceive(WalkieState &state) {
  state.runtime.receiving = true;
  state.runtime.lastRxMs = millis();
  state.runtime.status = "Receiving";
}

void endReceive(WalkieState &state) {
  state.runtime.receiving = false;
  state.runtime.status = "Ready";
}
