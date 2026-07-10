#include <Arduino.h>
#include <Preferences.h>

#include "config.h"
#include "types.h"
#include "settings.h"

static Preferences prefs;

static bool settingsDirty = false;
static uint32_t lastSettingsChangeMs = 0;

#define SETTINGS_SAVE_DELAY_MS 2000

void setupSettings() {
  prefs.begin("cxs", false);
}

void loadSettings(WalkieState &state) {
  state.config.mode = (WalkieMode)prefs.getUChar("mode", MODE_LOCAL);
  state.config.talkGroup = prefs.getUShort("tg", 1);

  if (state.config.mode > MODE_AUTO) {
    state.config.mode = MODE_LOCAL;
  }

  if (state.config.talkGroup < 1 || state.config.talkGroup > MAX_TALK_GROUPS) {
    state.config.talkGroup = 1;
  }
}

void saveSettingsNow(const WalkieState &state) {
  prefs.putUChar("mode", (uint8_t)state.config.mode);
  prefs.putUShort("tg", state.config.talkGroup);

  settingsDirty = false;
}

void markSettingsDirty() {
  settingsDirty = true;
  lastSettingsChangeMs = millis();
}

void saveSettingsIfDirty(const WalkieState &state) {
  if (!settingsDirty) return;

  if (millis() - lastSettingsChangeMs >= SETTINGS_SAVE_DELAY_MS) {
    saveSettingsNow(state);
  }
}

void resetSettings() {
  prefs.clear();
  settingsDirty = false;
}