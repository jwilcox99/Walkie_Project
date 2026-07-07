#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "esp_sleep.h"

#include "config.h"
#include "types.h"
#include "audio.h"
#include "display_ui.h"
#include "power.h"

void setupPowerPins() {
  pinMode(PWR_SW_PIN, INPUT_PULLUP);
}

void enterDeepSleep(WalkieState &state) {
  state.runtime.status = "Powering Down";
  showPoweringDownScreen();

  delay(700);

  digitalWrite(PTT_LED_PIN, LOW);

  esp_now_deinit();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  stopI2S();
  disableVE();

  // Wake when power switch returns to ON/LOW
  esp_sleep_enable_ext0_wakeup((gpio_num_t)PWR_SW_PIN, 0);

  delay(100);
  esp_deep_sleep_start();
}

void handlePower(WalkieState &state) {
  static bool offTiming = false;
  static uint32_t offStartedMs = 0;
  static bool sleepTriggered = false;

  bool switchIsOff = digitalRead(PWR_SW_PIN) == HIGH;

  if (!switchIsOff) {
    offTiming = false;
    sleepTriggered = false;
    return;
  }

  if (!offTiming) {
    offTiming = true;
    offStartedMs = millis();
    state.runtime.status = "Power Off...";
    return;
  }

  if (!sleepTriggered && millis() - offStartedMs >= 500) {
    sleepTriggered = true;
    enterDeepSleep(state);
  }
}