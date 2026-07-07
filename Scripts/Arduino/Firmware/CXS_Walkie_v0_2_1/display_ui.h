#pragma once

#include "types.h"

void setupDisplay();
void showStartupScreen();
void showPoweringDownScreen();
void drawMainScreen(const WalkieState &state);
String displayModeName(WalkieMode mode);