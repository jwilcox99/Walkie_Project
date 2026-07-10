#pragma once

#include "types.h"

void setupSettings();

void loadSettings(WalkieState &state);
void saveSettingsNow(const WalkieState &state);

void markSettingsDirty();
void saveSettingsIfDirty(const WalkieState &state);

void resetSettings();
