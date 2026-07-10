#pragma once

#define FW_VERSION "v0.3"

#define VEXT_CTRL     36

#define CH_PIN        1
#define PTT_PIN       4
#define PTT_LED_PIN   6
#define PWR_SW_PIN    7

#define I2S_BCLK      46
#define I2S_WS        47
#define I2S_MIC_DATA  33 //Update based on Prototype Josh=33 Snake=35
#define I2S_SPK_DATA  34

#define SAMPLE_RATE          16000
#define SAMPLES_PER_PACKET   80
#define VOLUME_BOOST         3

#define MAX_TALK_GROUPS      5
#define LONG_PRESS_MS        800
#define VERY_LONG_PRESS_MS   2500
#define DEBOUNCE_MS          40