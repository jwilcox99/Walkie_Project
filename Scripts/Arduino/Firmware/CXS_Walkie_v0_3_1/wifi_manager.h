#pragma once

#include "types.h"

void setupWifiManager();

void connectWifi(WalkieState &state);
void disconnectWifi(WalkieState &state);

bool wifiConnected();
String wifiIpAddress();
int wifiSignalDbm();

void wifiManagerLoop(WalkieState &state);