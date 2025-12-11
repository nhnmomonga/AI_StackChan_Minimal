#include <M5Unified.h>
#include "AudioHardware.h"
#include "Audio.h"

Audio::Audio() {
//  wavData = (typeof(wavData))heap_caps_malloc(record_size * sizeof(int16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  wavData = (typeof(wavData))heap_caps_malloc(record_size * sizeof(int16_t),  MALLOC_CAP_8BIT);
  memset(wavData, 0 , record_size * sizeof(int16_t));
}

Audio::~Audio() {
  delete wavData;
}

void Audio::CreateWavHeader(byte* header, int waveDataSize){
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSizeMinus8 = waveDataSize + 44 - 8;
  header[4] = (byte)(fileSizeMinus8 & 0xFF);
  header[5] = (byte)((fileSizeMinus8 >> 8) & 0xFF);
  header[6] = (byte)((fileSizeMinus8 >> 16) & 0xFF);
  header[7] = (byte)((fileSizeMinus8 >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;  // linear PCM
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;  // linear PCM
  header[21] = 0x00;
  header[22] = 0x01;  // monoral
  header[23] = 0x00;
  header[24] = 0x80;  // sampling rate 16000
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;  // Byte/sec = 16000x2x1 = 32000
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;  // 16bit monoral
  header[33] = 0x00;
  header[34] = 0x10;  // 16bit
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(waveDataSize & 0xFF);
  header[41] = (byte)((waveDataSize >> 8) & 0xFF);
  header[42] = (byte)((waveDataSize >> 16) & 0xFF);
  header[43] = (byte)((waveDataSize >> 24) & 0xFF);
}

void Audio::Record() {
  Serial.println("Audio::Record() - Starting...");
  Serial.flush();
  
  CreateWavHeader(paddedHeader, wavDataSize);
  
  // ES8311ではSpeakerとMicが同じI2Sポートを共有するため、
  // 録音前にSpeakerを停止し、録音後に復帰させる必要がある
  const bool speaker_was_enabled = M5.Speaker.isEnabled();
  auto restart_speaker_if_needed = [&]() {
    if (!speaker_was_enabled) {
      return;
    }
    Serial.println("Restarting speaker after recording...");
    Serial.flush();
    M5.Speaker.begin();
  };

  if (speaker_was_enabled) {
    Serial.println("Stopping speaker for recording...");
    Serial.flush();
    M5.Speaker.end();
    delay(100);
  }
  
  // マイクの設定を明示的に行う (M5Stack公式SDKより)
  auto mic_cfg = M5.Mic.config();
  mic_cfg.sample_rate = 16000;   // ES8311は8KHzをサポートしない
  mic_cfg.pin_bck = ES8311_BCLK_PIN;
  mic_cfg.pin_ws = ES8311_LRCK_PIN;
  mic_cfg.pin_data_in = ES8311_DATA_IN_PIN;
  mic_cfg.pin_mck = ES8311_MCLK_PIN;
  mic_cfg.i2s_port = I2S_NUM_1;
  mic_cfg.stereo = false;
  M5.Mic.config(mic_cfg);
  Serial.printf("Mic config -> SR:%d, BCK:%d, WS:%d, DIN:%d, MCLK:%d, I2S:%d\n",
                mic_cfg.sample_rate,
                mic_cfg.pin_bck,
                mic_cfg.pin_ws,
                mic_cfg.pin_data_in,
                mic_cfg.pin_mck,
                mic_cfg.i2s_port);
  Serial.println("Starting mic with explicit pin config...");
  Serial.flush();
  
  if (!M5.Mic.begin()) {
    Serial.println("ERROR: Mic.begin() failed! Restoring speaker state.");
    Serial.flush();
    restart_speaker_if_needed();
    return;
  }
  
  Serial.print("Mic enabled: ");
  Serial.print(M5.Mic.isEnabled());
  Serial.print(", recording ");
  Serial.print(record_number * record_length);
  Serial.println(" samples...");
  Serial.flush();
  
  int32_t maxLevel = 0;
  int32_t minLevel = 0;
  int rec_record_idx;
  
  for (rec_record_idx = 0; rec_record_idx < record_number; rec_record_idx++) {
    auto data = &wavData[rec_record_idx * record_length];
    
    // 進捗表示（10フレームごと）
    if (rec_record_idx % 10 == 0) {
      Serial.print("Recording frame ");
      Serial.print(rec_record_idx);
      Serial.print("/");
      Serial.println(record_number);
      Serial.flush();
    }
    
    M5.Mic.record(data, record_length, record_samplerate);
    
    // 録音レベルをチェック（最初の数フレームのみ）
    if (rec_record_idx < 5) {
      for (int i = 0; i < record_length; i++) {
        if (data[i] > maxLevel) maxLevel = data[i];
        if (data[i] < minLevel) minLevel = data[i];
      }
    }
  }
  
  Serial.print("Recording done. Audio level: min=");
  Serial.print(minLevel);
  Serial.print(", max=");
  Serial.println(maxLevel);
  Serial.flush();
  
  // 録音レベルが極端に低い場合は警告
  if (maxLevel - minLevel < 100) {
    Serial.println("WARNING: Audio level very low - mic may not be working!");
  }
  
  M5.Mic.end();
  restart_speaker_if_needed();
}
