## タイトル
[ATOMS3R] イシュー 1: プラットフォーム設定の更新

## ラベル
`atoms3r-migration`, `high-priority`, `phase-1`

## 説明

### 概要
PlatformIO の設定を ATOM Echo (ESP32-PICO) から ATOMS3R (ESP32-S3) に対応させます。

### 関連するメインイシュー
#XX (メインイシュー番号を記入)

### タスク
- [ ] `platformio.ini` のボード設定を更新
  - [ ] `board` を `m5stack-atom` から ESP32-S3 対応に変更
  - [ ] ESP32-S3 用の MCU 設定を追加
  - [ ] CPU 周波数を 240MHz に設定
  - [ ] Flash モードを QIO に設定
  - [ ] PSRAM タイプを OPI に設定
- [ ] プラットフォームバージョンの確認
  - [ ] `espressif32@6.2.0` が ESP32-S3 をサポートしているか確認
  - [ ] 必要に応じて最新版に更新
- [ ] パーティション設定の確認
  - [ ] `no_ota.csv` が ESP32-S3 で動作するか確認
- [ ] ビルドフラグの追加
  - [ ] ESP32-S3 固有の最適化フラグを追加
  - [ ] ATOMS3R 識別用のフラグを追加 (`-DATOMS3R`)

### 実装例

```ini
[env:m5stack-atoms3]
platform = espressif32@6.2.0
board = esp32-s3-devkitc-1
framework = arduino
board_build.partitions = no_ota.csv
upload_speed = 1500000
monitor_speed = 115200
board_build.mcu = esp32s3
board_build.f_cpu = 240000000L
board_build.flash_mode = qio
board_build.psram_type = opi
build_flags = 
  -DATOMS3R
  -DBOARD_HAS_PSRAM
lib_deps = 
  m5stack/M5Unified @ 0.1.16
  earlephilhower/ESP8266Audio @ ^1.9.7
  bblanchon/ArduinoJson @ ^6
  ESP32WebServer
  horihiro/esp8266-google-tts @ 1.1.0
```

### テスト方法
1. `pio run -e m5stack-atoms3` でビルドが成功することを確認
2. ビルドログで ESP32-S3 が認識されていることを確認
3. PSRAM が有効になっていることを確認

### 依存関係
なし（最初に実装すべきイシュー）

### 完了条件
- ✅ ESP32-S3 用のビルド設定が追加されている
- ✅ ビルドが成功する
- ✅ ビルドログに警告やエラーが出ない

### 参考資料
- [PlatformIO ESP32-S3 ドキュメント](https://docs.platformio.org/en/latest/boards/espressif32/esp32-s3-devkitc-1.html)
- [ESP32-S3 技術仕様](https://www.espressif.com/en/products/socs/esp32-s3)
- [ATOMS3R_MIGRATION_PLAN.md - イシュー 1](../../ATOMS3R_MIGRATION_PLAN.md#イシュー-1-プラットフォーム設定の更新)
