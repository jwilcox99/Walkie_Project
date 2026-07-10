#define DEBUG_WEBSOCKETS

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <cstring>

#include "audio.h"
#include "config.h"
#include "types.h"
#include "wifi_manager.h"
#include "wifi_secrets.h"
#include "remote_relay.h"

static WebSocketsClient relayClient;

static WalkieState *relayState = nullptr;

static bool connectedToRelay = false;
static String lastRelayStatus = "Relay idle";

static uint32_t lastConnectAttemptMs = 0;
static uint32_t lastHeartbeatMs = 0;

#define RELAY_CONNECT_INTERVAL_MS  5000
#define RELAY_HEARTBEAT_MS         3000

static String relayUrl() {
  return String("ws://") + CXS_RELAY + ":" + String(CXS_RELAY_PORT) + CXS_RELAY_PATH;
}

static void relayEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
   case WStype_DISCONNECTED:
  connectedToRelay = false;
  lastRelayStatus = "Relay closed";
  Serial.println("Relay closed");
  delay(1000);
  break;

  case WStype_CONNECTED:
  connectedToRelay = true;
  lastRelayStatus = "Relay connected";
  Serial.println("Relay connected");

  relayClient.sendTXT("HELLO CXS TG1");
  Serial.println("Relay TX: HELLO CXS TG1");
  break;

case WStype_TEXT: {
  String msg((char *)payload, length);
  msg.trim();

  Serial.print("Relay RX: ");
  Serial.println(msg);

  if (msg == "OK") {
    lastRelayStatus = "Relay OK";
  }
  else if (msg == "PONG") {
    lastRelayStatus = "Relay PONG";
  }
  else if (msg.startsWith("PTT_START")) {
    Serial.println("REMOTE PTT START");
    lastRelayStatus = "Remote RX";

    if (relayState != nullptr) {
      relayState->runtime.receiving = true;
      relayState->runtime.transmitting = false;
      relayState->runtime.status = "Remote RX";
    }
  }
  else if (msg.startsWith("PTT_STOP")) {
    Serial.println("REMOTE PTT STOP");
    lastRelayStatus = "Remote ready";

    if (relayState != nullptr) {
      relayState->runtime.receiving = false;
      relayState->runtime.status = "Remote ready";
    }
  }
  else {
    lastRelayStatus = msg;
  }

  break;
}

case WStype_BIN: {
  Serial.print("Relay RX audio: ");
  Serial.print(length);
  Serial.println(" bytes");

  if (length != sizeof(AudioPacket)) {
    Serial.print("Unexpected binary packet size. Expected ");
    Serial.print(sizeof(AudioPacket));
    Serial.print(", received ");
    Serial.println(length);
    break;
  }

  AudioPacket packet;
  memcpy(&packet, payload, sizeof(AudioPacket));

  if (relayState == nullptr) {
    Serial.println("Audio RX ignored: state unavailable");
    break;
  }

  if (packet.talkGroup != relayState->config.talkGroup) {
    Serial.println("Audio RX ignored: wrong talk group");
    break;
  }

  relayState->runtime.receiving = true;
  relayState->runtime.transmitting = false;
  relayState->runtime.status = "Remote RX";

  playAudioPacket(packet);

  break;
}

case WStype_ERROR:
  connectedToRelay = false;
  lastRelayStatus = "Relay error";
  Serial.println("Relay error");
  break;

default:
  break;
  }
}
void setupRemoteRelay() {
  connectedToRelay = false;
  lastRelayStatus = "Relay idle";

  relayClient.onEvent(relayEvent);
  relayClient.setReconnectInterval(0);
}

bool relayConnected() {
  return connectedToRelay;
}

String relayStatus() {
  return lastRelayStatus;
}

static void disconnectRelay() {
  relayClient.disconnect();
  connectedToRelay = false;
  lastRelayStatus = "Relay disconnected";
}

static void connectRelay(WalkieState &state) {
  if (!wifiConnected()) {
    disconnectRelay();
    lastRelayStatus = "No WiFi";
    state.runtime.status = "No WiFi";
    return;
  }

  connectedToRelay = false;
  lastRelayStatus = "Relay connecting";
  state.runtime.status = "Relay connecting";

  Serial.print("Connecting relay: ");
  Serial.println(relayUrl());
  Serial.print("Host: ");
  Serial.println(CXS_RELAY);
  Serial.print("Port: ");
  Serial.println(CXS_RELAY_PORT);
  Serial.print("Path: ");
  Serial.println(CXS_RELAY_PATH);

  relayClient.disconnect();

  relayClient.beginSSL(CXS_RELAY, CXS_RELAY_PORT, CXS_RELAY_PATH);
}

static void sendHello(WalkieState &state) {
  if (!connectedToRelay) return;

  String hello = "HELLO CXS TG" + String(state.config.talkGroup);

  Serial.print("Relay TX: ");
  Serial.println(hello);

  relayClient.sendTXT(hello);
}

static void sendHeartbeat(WalkieState &state) {
  if (!connectedToRelay) return;

  String msg = "PING TG" + String(state.config.talkGroup);

  Serial.print("Relay TX: ");
  Serial.println(msg);

  relayClient.sendTXT(msg);
}

void remoteRelayPttStart(WalkieState &state) {
  if (!relayConnected()) {
    Serial.println("PTT_START not sent: relay disconnected");
    return;
  }

  String msg = "PTT_START TG" + String(state.config.talkGroup);

  Serial.print("Relay TX: ");
  Serial.println(msg);

  relayClient.sendTXT(msg);
}

void remoteRelayPttStop(WalkieState &state) {
  if (!relayConnected()) {
    Serial.println("PTT_STOP not sent: relay disconnected");
    return;
  }

  String msg = "PTT_STOP TG" + String(state.config.talkGroup);

  Serial.print("Relay TX: ");
  Serial.println(msg);

  relayClient.sendTXT(msg);
}

static void sendRemoteAudioPacket(WalkieState &state) {
  static uint16_t sequence = 0;

  if (!relayConnected()) {
    return;
  }

  AudioPacket packet = {};

  packet.version = 1;
  packet.mode = static_cast<uint8_t>(MODE_REMOTE);
  packet.talkGroup = state.config.talkGroup;
  packet.seq = sequence++;

  readMicPacket(packet);

  bool sent = relayClient.sendBIN(
    reinterpret_cast<uint8_t *>(&packet),
    sizeof(AudioPacket)
  );

  if (!sent) {
    Serial.println("Audio packet send failed");
    state.runtime.status = "Audio TX failed";
    return;
  }

  state.runtime.status = "Remote TX";
}

void remoteRelayLoop(WalkieState &state) {
  relayState = &state;
  static bool helloSent = false;

  if (state.config.mode != MODE_REMOTE && state.config.mode != MODE_AUTO) {
    helloSent = false;
    disconnectRelay();
    return;
  }

  if (!wifiConnected()) {
    helloSent = false;
    disconnectRelay();
    state.runtime.status = "No WiFi";
    return;
  }

  relayClient.loop();

  if (!connectedToRelay) {
    helloSent = false;

    if (millis() - lastConnectAttemptMs >= RELAY_CONNECT_INTERVAL_MS) {
      lastConnectAttemptMs = millis();
      connectRelay(state);
    }

    return;
  }

  if (!helloSent) {
    helloSent = true;
    sendHello(state);
  }

  if (
  !state.runtime.receiving &&
  !state.runtime.transmitting &&
  lastRelayStatus.length() > 0
) {
  state.runtime.status = lastRelayStatus;
}
  if (state.runtime.transmitting) {
  sendRemoteAudioPacket(state);
  return;
}
  if (millis() - lastHeartbeatMs >= RELAY_HEARTBEAT_MS) {
    lastHeartbeatMs = millis();
    sendHeartbeat(state);
  }
}