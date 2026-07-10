#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "config.h"
#include "types.h"
#include "audio.h"
#include "radio_local.h"

static uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
static uint16_t txSeq = 0;

extern WalkieState state;

void onLocalReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(AudioPacket)) return;
  if (state.config.mode != MODE_LOCAL) return;
  if (state.runtime.transmitting) return;

  AudioPacket packet;
  memcpy(&packet, data, sizeof(packet));

  if (packet.version != 1) return;
  if (packet.talkGroup != state.config.talkGroup) return;

  playAudioPacket(packet);

  state.runtime.rxPackets++;
  state.runtime.lastRxMs = millis();
  state.runtime.receiving = true;
  state.runtime.status = "Receiving";
}

void setupLocalRadio() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    state.runtime.status = "LOCAL failed";
    return;
  }

  esp_now_register_recv_cb(onLocalReceive);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    state.runtime.status = "Peer failed";
    return;
  }

  state.runtime.status = "Ready";
}

void localRadioLoop(WalkieState &state) {
  if (!state.runtime.transmitting) {
    if (state.runtime.receiving && millis() - state.runtime.lastRxMs > 1000) {
      state.runtime.receiving = false;
      state.runtime.status = "Ready";
    }

    return;
  }

  AudioPacket packet;
  packet.version = 1;
  packet.mode = MODE_LOCAL;
  packet.talkGroup = state.config.talkGroup;
  packet.seq = txSeq++;

  readMicPacket(packet);

  esp_err_t result = esp_now_send(
    broadcastAddress,
    (uint8_t *)&packet,
    sizeof(packet)
  );

  if (result == ESP_OK) {
    state.runtime.txPackets++;
    state.runtime.status = "Transmitting";
  } else {
    state.runtime.status = "TX failed";
  }
}