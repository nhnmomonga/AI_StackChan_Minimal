# ATOMS3R 移植クイックリファレンス

このドキュメントは、開発者が ATOMS3R への移植作業を行う際のクイックリファレンスです。

## ハードウェア比較

| 項目 | ATOM Echo | ATOMS3R AI Chatbot Kit |
|------|-----------|------------------------|
| **プロセッサ** | ESP32-PICO-D4 | ESP32-S3-PICO-1-N8R8 |
| **CPU** | デュアルコア @ 240MHz | デュアルコア Xtensa LX7 @ 240MHz |
| **Flash** | 4MB | 8MB |
| **PSRAM** | なし | 8MB PSRAM |
| **ディスプレイ** | 外部SSD1306 (96x64 OLED) | 内蔵 0.85" IPS TFT (128x128) |
| **ディスプレイドライバ** | SSD1306 (I2C) | GC9107 (SPI) |
| **オーディオ** | Atomic Echo | Atomic Echo Base（統合） |
| **オーディオコーデック** | ES8311 | ES8311 |
| **マイク** | MEMS デジタル | MEMS デジタル (SNR ≥65dB) |
| **スピーカー** | 1W 8Ω | 1W 8Ω |
| **センサー** | なし | IMU (BMI270), 磁気 (BMM150) |
| **IR送信機** | なし | あり |
| **LED** | GPIO27 | 要確認 |
| **ボタン** | トップボタン | 画面下部ボタン |
| **サイズ** | 24x24x14mm | 24x24x12.9mm (本体) |
| **重量** | - | 6.6g (本体) + 6.3g (Echo Base) |

## ピンマッピング変更

### I2C
| 機能 | ATOM Echo | ATOMS3R |
|------|-----------|---------|
| SDA | GPIO21 | GPIO38 (または G1) |
| SCL | GPIO25 | GPIO39 (または G2) |

### その他
| 機能 | ATOM Echo | ATOMS3R | 備考 |
|------|-----------|---------|------|
| LED | GPIO27 | GPIO35? | 要確認 |
| ボタン | M5.BtnA | M5.BtnA | ピン番号は M5Unified が管理 |
| Serial1 RX | GPIO32 | GPIO32? | Grove ポート（要確認） |
| Serial1 TX | GPIO26 | GPIO26? | Grove ポート（要確認） |

### 拡張 GPIO（ATOMS3R）
- G5: GPIO5
- G6: GPIO6
- G7: GPIO7
- G8: GPIO8
- G38: GPIO38 (I2C SDA 推奨)
- G39: GPIO39 (I2C SCL 推奨)

## PlatformIO 設定変更

```ini
[env:m5stack-atoms3]
platform = espressif32@6.2.0
board = esp32-s3-devkitc-1
framework = arduino
board_build.partitions = no_ota.csv
upload_speed = 1500000
monitor_speed = 115200

; ESP32-S3 specific settings
board_build.mcu = esp32s3
board_build.f_cpu = 240000000L
board_build.flash_mode = qio
board_build.psram_type = opi

; Build flags
build_flags = 
  -DATOMS3R
  -DBOARD_HAS_PSRAM
  -DUSE_INTERNAL_DISPLAY  ; オプション：内蔵ディスプレイ使用

lib_deps = 
  m5stack/M5Unified @ 0.1.16  ; または最新版
  earlephilhower/ESP8266Audio @ ^1.9.7
  bblanchon/ArduinoJson @ ^6
  ESP32WebServer
  horihiro/esp8266-google-tts @ 1.1.0
```

## コード変更テンプレート

### ピン定義

```cpp
// src/main.cpp

#ifdef ATOMS3R
  // ATOMS3R AI Chatbot Kit
  #define I2C_SDA_PIN 38
  #define I2C_SCL_PIN 39
  #define LED_PIN     35  // 要確認
  #define USE_INTERNAL_DISPLAY  // 内蔵ディスプレイを使用
#else
  // ATOM Echo（後方互換性）
  #define I2C_SDA_PIN 21
  #define I2C_SCL_PIN 25
  #define LED_PIN     27
#endif
```

### ディスプレイ初期化

```cpp
void setup()
{
  auto cfg = M5.config();
  
#ifdef USE_INTERNAL_DISPLAY
  // ATOMS3R 内蔵ディスプレイ
  cfg.internal_imu = true;  // IMU も有効化
  M5.begin(cfg);
  M5.setPrimaryDisplayType({m5::board_t::board_M5AtomS3});
#else
  // 外部 SSD1306
  cfg.unit_glass2.pin_sda = I2C_SDA_PIN;
  cfg.unit_glass2.pin_scl = I2C_SCL_PIN;
  M5.begin(cfg);
  M5.setPrimaryDisplayType({m5::board_t::board_M5UnitGLASS2});
#endif

  // 以降は共通
  // ...
}
```

### メモリ最適化（PSRAM活用）

```cpp
#ifdef ATOMS3R
  // より大きなバッファ（PSRAM あり）
  const int MAX_HISTORY = 5;
  uint8_t mp3buff[1024*50];
#else
  // 制限されたバッファ（PSRAM なし）
  const int MAX_HISTORY = 2;
  uint8_t mp3buff[1024*20];
#endif
```

## ビルドコマンド

```bash
# ATOMS3R 用ビルド
pio run -e m5stack-atoms3

# ATOM Echo 用ビルド（既存）
pio run -e m5stack-atom

# アップロード
pio run -e m5stack-atoms3 -t upload

# モニター
pio run -e m5stack-atoms3 -t monitor
```

## テストチェックリスト

### 基本動作
- [ ] ビルド成功
- [ ] アップロード成功
- [ ] 起動成功
- [ ] WiFi 接続成功
- [ ] ディスプレイ表示成功

### オーディオ
- [ ] マイク入力動作
- [ ] スピーカー出力動作
- [ ] 音声認識動作
- [ ] 音声合成動作

### 表示
- [ ] アバター表示
- [ ] テキスト表示
- [ ] カラー表示（ATOMS3R のみ）
- [ ] スクロール動作

### 入力
- [ ] ボタン単押し動作
- [ ] ボタン二度押し動作
- [ ] LED 点灯動作

### 機能
- [ ] ChatGPT 連携動作
- [ ] Web インターフェース動作
- [ ] 設定保存・読込動作

## トラブルシューティング

### ビルドエラー

**エラー**: `board 'm5stack-atoms3' not found`  
**解決**: PlatformIO の ESP32 プラットフォームを最新版に更新

```bash
pio platform update espressif32
```

**エラー**: `PSRAM not found`  
**解決**: `platformio.ini` に PSRAM 設定を追加

```ini
board_build.psram_type = opi
build_flags = -DBOARD_HAS_PSRAM
```

### ディスプレイが表示されない

**症状**: 画面が真っ暗  
**確認点**:
1. `M5.setPrimaryDisplayType()` が正しく設定されているか
2. ディスプレイドライバが初期化されているか
3. バックライトが有効か

### オーディオが動作しない

**症状**: 音が出ない  
**確認点**:
1. `M5.Speaker.begin()` が呼ばれているか
2. ボリュームが設定されているか
3. Atomic Echo Base が正しく接続されているか

### メモリ不足

**症状**: 動作中にクラッシュ  
**解決**:
1. PSRAM が有効化されているか確認
2. バッファサイズを削減
3. `esp_get_free_heap_size()` でメモリ使用量を監視

## 参考リンク

### 公式ドキュメント
- [ATOMS3R 公式ページ](https://docs.m5stack.com/en/core/AtomS3R-AI%20Chatbot)
- [ATOMS3R データシート](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/static/pdf/static/en/core/AtomS3R.pdf)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [ESP32-S3 仕様](https://www.espressif.com/en/products/socs/esp32-s3)

### プロジェクト内
- [ATOMS3R_MIGRATION_PLAN.md](../ATOMS3R_MIGRATION_PLAN.md) - 詳細な移植計画
- [docs/issues/](./issues/) - GitHub Issues テンプレート

## 最終更新
2025-12-08
