#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

#define I2S_BCLK      46
#define I2S_WS        47
#define I2S_DOUT      34

#define SAMPLE_RATE   16000
#define TONE_FREQ     1000

#define VEXT_CTRL 36   // try this first for Heltec-style Vext/VE control

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, LOW);   // Heltec Vext is usually LOW = ON
  delay(500);
  Serial.println("VE/VEXT enabled");
  
  Serial.println();
  Serial.println("=== Speaker Test Starting ===");

  i2s_config_t i2s_config = {
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

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  esp_err_t err;

  err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  Serial.printf("Driver install: %d\n", err);

  err = i2s_set_pin(I2S_NUM_0, &pin_config);
  Serial.printf("Pin config: %d\n", err);

  i2s_zero_dma_buffer(I2S_NUM_0);
}

void loop() {
  static uint32_t lastPrint = 0;

  if (millis() - lastPrint > 1000) {
    Serial.println("Playing tone...");
    lastPrint = millis();
  }

  int16_t buffer[256];

  for (int i = 0; i < 128; i++) {
    float sample = sinf(2.0f * PI * TONE_FREQ * i / SAMPLE_RATE);
    int16_t value = (int16_t)(sample * 12000);

    buffer[i * 2] = value;
    buffer[i * 2 + 1] = value;
  }

  size_t bytesWritten;
  i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);
}