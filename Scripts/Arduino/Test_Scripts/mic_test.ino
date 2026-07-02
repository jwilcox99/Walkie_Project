#include <Arduino.h>
#include <driver/i2s.h>

#define VEXT_CTRL     36

#define I2S_BCLK      46
#define I2S_WS        47
#define I2S_MIC_DATA  33
#define I2S_SPK_DATA  34

#define SAMPLE_RATE   16000
#define RECORD_SECS   3
#define NUM_SAMPLES   (SAMPLE_RATE * RECORD_SECS)

int16_t audioBuffer[NUM_SAMPLES];

void enableVE() {
  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, LOW);   // VE/VEXT ON
  delay(500);
}

void setupMic() {
  i2s_driver_uninstall(I2S_NUM_0);

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
}

void setupSpeaker() {
  i2s_driver_uninstall(I2S_NUM_0);

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
}

void recordAudio() {
  Serial.println("Recording...");

  int32_t sample32;
  size_t bytesRead;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    i2s_read(I2S_NUM_0, &sample32, sizeof(sample32), &bytesRead, portMAX_DELAY);
    audioBuffer[i] = (int16_t)(sample32 >> 14);
  }

  Serial.println("Recording done");
}

void playAudio() {
  Serial.println("Playing back...");

  int16_t stereo[128];

  for (int i = 0; i < NUM_SAMPLES; i += 64) {
    for (int j = 0; j < 64; j++) {
      int16_t s = audioBuffer[i + j] * 3;  // simple volume boost
      stereo[j * 2] = s;
      stereo[j * 2 + 1] = s;
    }

    size_t bytesWritten;
    i2s_write(I2S_NUM_0, stereo, sizeof(stereo), &bytesWritten, portMAX_DELAY);
  }

  Serial.println("Playback done");
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  enableVE();

  Serial.println("=== Mic Record / Speaker Playback Test ===");
}

void loop() {
  setupMic();
  recordAudio();

  delay(500);

  setupSpeaker();
  playAudio();

  delay(1500);
}