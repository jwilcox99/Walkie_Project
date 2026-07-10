#pragma once

#include "types.h"

void setupRemoteRelay();

void remoteRelayLoop(WalkieState &state);

bool relayConnected();
String relayStatus();
