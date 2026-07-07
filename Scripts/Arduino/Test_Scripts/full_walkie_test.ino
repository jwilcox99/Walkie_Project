#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <driver/i2s.h>
#include "HT_SSD1306Wire.h"

// ---------------- PINS ----------------

#define VEXT_CTRL     36

#define CH_PIN        1
#define PTT_PIN       4
#define PTT_LED_PIN   6
#define PWR_SW_PIN    7

#define I2S_BCLK      46
#define I2S_WS        47
#define I2S_MIC_DATA  33
#define I2S_SPK_DATA  34

// ---------------- AUDIO ----------------

#define SAMPLE_RATE          16000
#define SAMPLES_PER_PACKET   80
#define VOLUME_BOOST         3

enum AudioMode {
  MODE_NONE,
  MODE_MIC,
  MODE_SPEAKER
};

AudioMode currentAudioMode = MODE_NONE;

typedef struct {
  uint16_t seq;
  int16_t samples[SAMPLES_PER_PACKET];
} AudioPacket;

uint16_t txSeq = 0;
uint32_t txCount = 0;
uint32_t rxCount = 0;
uint32_t lastRxMs = 0;
String statusLine = "Booting";

uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// ---------------- OLED ----------------

SSD1306Wire walkieDisplay(
  0x3c,
  500000,
  SDA_OLED,
  SCL_OLED,
  GEOMETRY_128_64,
  RST_OLED
);

void enableVE() {
  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, LOW);
  delay(500);
}

void drawScreen() {
  bool ptt = digitalRead(PTT_PIN) == LOW;
  bool ch = digitalRead(CH_PIN) == LOW;

  walkieDisplay.clear();
  walkieDisplay.setTextAlignment(TEXT_ALIGN_LEFT);
  walkieDisplay.setFont(ArialMT_Plain_10);

  walkieDisplay.drawString(0, 0, "CXS Voice Test");
  walkieDisplay.drawString(0, 12, "Status: " + statusLine);
  walkieDisplay.drawString(0, 26, String("PTT: ") + (ptt ? "DOWN" : "up"));
  walkieDisplay.drawString(0, 38, String("CH: ") + (ch ? "DOWN" : "up"));
  walkieDisplay.drawString(0, 50, "TX:" + String(txCount) + " RX:" + String(rxCount));

  walkieDisplay.display();
}

// ---------------- I2S ----------------

void stopI2S() {
  if (currentAudioMode != MODE_NONE) {
    i2s_driver_uninstall(I2S_NUM_0);
  }

  currentAudioMode = MODE_NONE;
}

void setupMic() {
  if (currentAudioMode == MODE_MIC) return;

  stopI2S();

  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pins = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_DATA
  };

  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);

  currentAudioMode = MODE_MIC;
}

void setupSpeaker() {
  if (currentAudioMode == MODE_SPEAKER) return;

  stopI2S();

  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pins = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_SPK_DATA,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  i2s_zero_dma_buffer(I2S_NUM_0);

  currentAudioMode = MODE_SPEAKER;
}

// ---------------- RADIO ----------------

void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(AudioPacket)) return;

  bool ptt = digitalRead(PTT_PIN) == LOW;
  if (ptt) return; // half-duplex: ignore RX while transmitting

  AudioPacket packet;
  memcpy(&packet, data, sizeof(packet));

  setupSpeaker();

  int16_t stereo[SAMPLES_PER_PACKET * 2];

  for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
    int16_t s = packet.samples[i] * VOLUME_BOOST;
    stereo[i * 2] = s;
    stereo[i * 2 + 1] = s;
  }

  size_t bytesWritten;
  i2s_write(I2S_NUM_0, stereo, sizeof(stereo), &bytesWritten, 5);

  rxCount++;
  lastRxMs = millis();
  statusLine = "Receiving";
}

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    statusLine = "ESP-NOW failed";
    drawScreen();
    while (true) delay(100);
  }

  esp_now_register_recv_cb(onReceive);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    statusLine = "Peer failed";
    drawScreen();
    while (true) delay(100);
  }

  statusLine = "Ready";
}

// ---------------- MAIN ----------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  enableVE();

  pinMode(CH_PIN, INPUT_PULLUP);
  pinMode(PTT_PIN, INPUT_PULLUP);
  pinMode(PWR_SW_PIN, INPUT_PULLUP);
  pinMode(PTT_LED_PIN, OUTPUT);
  digitalWrite(PTT_LED_PIN, LOW);

  walkieDisplay.init();
  walkieDisplay.flipScreenVertically();
  walkieDisplay.clear();
  walkieDisplay.drawString(0, 0, "Starting voice...");
  walkieDisplay.display();

  setupSpeaker();
  setupEspNow();

  drawScreen();
}

void loop() {
  static bool wasPtt = false;
  static uint32_t lastScreenMs = 0;

  bool ptt = digitalRead(PTT_PIN) == LOW;
  digitalWrite(PTT_LED_PIN, ptt ? HIGH : LOW);

  if (ptt && !wasPtt) {
    setupMic();
    statusLine = "Transmitting";
  }

  if (!ptt && wasPtt) {
    setupSpeaker();
    statusLine = "Ready";
  }

  wasPtt = ptt;

  if (ptt) {
    AudioPacket packet;
    packet.seq = txSeq++;

    for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
      int32_t sample32;
      size_t bytesRead;
      i2s_read(I2S_NUM_0, &sample32, sizeof(sample32), &bytesRead, portMAX_DELAY);
      packet.samples[i] = (int16_t)(sample32 >> 14);
    }

    esp_err_t result = esp_now_send(
      broadcastAddress,
      (uint8_t *)&packet,
      sizeof(packet)
    );

    if (result == ESP_OK) {
      txCount++;
      statusLine = "Transmitting";
    } else {
      statusLine = "TX failed";
    }
  }

  if (!ptt && millis() - lastRxMs > 1000 && statusLine == "Receiving") {
    statusLine = "Ready";
  }

  if (millis() - lastScreenMs > 200) {
    drawScreen();
    lastScreenMs = millis();
  }
}
