// CXS Walkie Button Test - Heltec WiFi LoRa 32 V4
// Buttons use internal pullups: pressed = LOW

#define PIN_PTT_SW   4
#define PIN_CH_SW    1
#define PIN_PTT_LED  6
#define PIN_PWR_SW   7   // optional test input if wired

bool lastPtt = HIGH;
bool lastCh  = HIGH;
bool lastPwr = HIGH;

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_PTT_SW, INPUT_PULLUP);
  pinMode(PIN_CH_SW, INPUT_PULLUP);
  pinMode(PIN_PWR_SW, INPUT_PULLUP);

  pinMode(PIN_PTT_LED, OUTPUT);
  digitalWrite(PIN_PTT_LED, LOW);

  Serial.println();
  Serial.println("=== CXS Walkie Button Test ===");
  Serial.println("PTT: GPIO4");
  Serial.println("CH:  GPIO5");
  Serial.println("LED: GPIO6");
  Serial.println("PWR: GPIO7");
  Serial.println("Pressed = LOW");
}

void loop() {
  bool ptt = digitalRead(PIN_PTT_SW);
  bool ch  = digitalRead(PIN_CH_SW);
  bool pwr = digitalRead(PIN_PWR_SW);

  // LED follows PTT button
  digitalWrite(PIN_PTT_LED, ptt == LOW ? HIGH : LOW);

  if (ptt != lastPtt) {
    Serial.print("PTT ");
    Serial.println(ptt == LOW ? "PRESSED" : "released");
    lastPtt = ptt;
    delay(30); // simple debounce
  }

  if (ch != lastCh) {
    Serial.print("CHANNEL ");
    Serial.println(ch == LOW ? "PRESSED" : "released");
    lastCh = ch;
    delay(30);
  }

  if (pwr != lastPwr) {
    Serial.print("POWER SWITCH ");
    Serial.println(pwr == LOW ? "ON/LOW" : "OFF/HIGH");
    lastPwr = pwr;
    delay(30);
  }

  // Heartbeat every 2 sec so you know firmware is alive
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();
    Serial.print("State | PTT=");
    Serial.print(ptt == LOW ? "DOWN" : "UP");
    Serial.print(" CH=");
    Serial.print(ch == LOW ? "DOWN" : "UP");
    Serial.print(" PWR=");
    Serial.println(pwr == LOW ? "LOW" : "HIGH");
  }
}
