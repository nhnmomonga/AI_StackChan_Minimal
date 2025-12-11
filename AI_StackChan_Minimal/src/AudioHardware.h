#pragma once

// Shared pin definitions for the ES8311 codec on the Atomic Echo Base.
// These values come from the official M5Stack SDK wiring for ATOMS3R + Atomic Echo Base.
// Override via build flags if you need to adapt to a custom wiring harness.
#ifndef ES8311_BCLK_PIN
#define ES8311_BCLK_PIN     8   // I2S BCLK
#endif
#ifndef ES8311_LRCK_PIN
#define ES8311_LRCK_PIN     6   // I2S WS/LRCK
#endif
#ifndef ES8311_DATA_OUT_PIN
#define ES8311_DATA_OUT_PIN 5   // I2S DATA_OUT (speaker)
#endif
#ifndef ES8311_DATA_IN_PIN
#define ES8311_DATA_IN_PIN  7   // I2S DATA_IN (microphone)
#endif
#ifndef ES8311_MCLK_PIN
#define ES8311_MCLK_PIN     2   // I2S MCLK (AtomS3R GPIO2)
#endif
#ifndef ES8311_MCLK_FREQ
#define ES8311_MCLK_FREQ    12288000 // 12.288 MHz external master clock
#endif
