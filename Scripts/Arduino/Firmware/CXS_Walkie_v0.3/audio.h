#pragma once

#include "types.h"

void enableVE();
void disableVE();

void setupMic();
void setupSpeaker();
void stopI2S();

void readMicPacket(AudioPacket &packet);
void playAudioPacket(const AudioPacket &packet);
