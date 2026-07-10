#pragma once

#include "types.h"

void setStatus(WalkieState &state, const String &status);

void setMode(
    WalkieState &state,
    WalkieMode mode
);

void setTalkGroup(
    WalkieState &state,
    uint16_t talkGroup
);

void beginTransmit(WalkieState &state);
void endTransmit(WalkieState &state);

void beginReceive(WalkieState &state);
void endReceive(WalkieState &state);