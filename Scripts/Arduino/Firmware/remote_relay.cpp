#define DEBUG_WEBSOCKETS

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

#include "config.h"
#include "types.h"
#include "wifi_manager.h"
#include "wifi_secrets.h"
#include "remote_relay.h"

static WebSocketsClient relayClient;

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
      String msg = String((char *)payload);
      msg.trim();

      Serial.print("Relay RX: ");
      Serial.println(msg);

      if (msg == "OK") {
        lastRelayStatus = "Relay OK";
      } else if (msg == "PONG") {
        lastRelayStatus = "Relay PONG";
      } else {
        lastRelayStatus = msg;
      }
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

  lastRelayStatus = "Relay ping";
  state.runtime.status = "Relay ping";
}

void remoteRelayLoop(WalkieState &state) {
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

  if (lastRelayStatus.length() > 0) {
    state.runtime.status = lastRelayStatus;
  }

  if (millis() - lastHeartbeatMs >= RELAY_HEARTBEAT_MS) {
    lastHeartbeatMs = millis();
    sendHeartbeat(state);
  }
}