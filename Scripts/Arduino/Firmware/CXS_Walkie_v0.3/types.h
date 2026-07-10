#pragma once
#include <Arduino.h>
#include "config.h"

enum WalkieMode {
  MODE_LOCAL,
  MODE_REMOTE,
  MODE_AUTO
};

enum AudioMode {
  AUDIO_NONE,
  AUDIO_MIC,
  AUDIO_SPEAKER
};

struct WalkieConfig {
  WalkieMode mode;
  uint16_t talkGroup;
};

struct WalkieRuntime {
  bool transmitting;
  bool receiving;

  bool wifiConnected;
  int wifiRSSI;
  String wifiIP;

  uint32_t txPackets;
  uint32_t rxPackets;
  uint32_t lastRxMs;

  String status;
};

struct WalkieState {
  WalkieConfig config;
  WalkieRuntime runtime;
};

struct AudioPacket {
  uint8_t version;
  uint8_t mode;
  uint16_t talkGroup;
  uint16_t seq;
  int16_t samples[SAMPLES_PER_PACKET];
};
