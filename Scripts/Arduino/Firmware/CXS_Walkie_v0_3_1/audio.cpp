#include <Arduino.h>
#include <driver/i2s.h>
#include "config.h"
#include "types.h"
#include "audio.h"

static AudioMode currentAudioMode = AUDIO_NONE;

void enableVE() {
  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, LOW);
  delay(500);
}

void disableVE() {
  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, HIGH);
  delay(100);
}

void stopI2S() {
  if (currentAudioMode != AUDIO_NONE) {
    i2s_driver_uninstall(I2S_NUM_0);
  }

  currentAudioMode = AUDIO_NONE;
}

void setupMic() {
  if (currentAudioMode == AUDIO_MIC) return;

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

  currentAudioMode = AUDIO_MIC;
}

void setupSpeaker() {
  if (currentAudioMode == AUDIO_SPEAKER) return;

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

  currentAudioMode = AUDIO_SPEAKER;
}

void readMicPacket(AudioPacket &packet) {
  setupMic();

  for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
    int32_t sample32;
    size_t bytesRead;

    i2s_read(
      I2S_NUM_0,
      &sample32,
      sizeof(sample32),
      &bytesRead,
      portMAX_DELAY
    );

    packet.samples[i] = (int16_t)(sample32 >> 14);
  }
}

void playAudioPacket(const AudioPacket &packet) {
  setupSpeaker();

  int16_t stereo[SAMPLES_PER_PACKET * 2];

  for (int i = 0; i < SAMPLES_PER_PACKET; i++) {
    int16_t s = packet.samples[i] * VOLUME_BOOST;
    stereo[i * 2] = s;
    stereo[i * 2 + 1] = s;
  }

  size_t bytesWritten;

  i2s_write(
    I2S_NUM_0,
    stereo,
    sizeof(stereo),
    &bytesWritten,
    5
  );
}