#pragma once

#include "types.h"

void setupRemoteRelay();

void remoteRelayLoop(WalkieState &state);

bool relayConnected();
String relayStatus();

void remoteRelayPttStart(WalkieState &state);
void remoteRelayPttStop(WalkieState &state);

void remoteRelaySendBinaryTest(WalkieState &state);