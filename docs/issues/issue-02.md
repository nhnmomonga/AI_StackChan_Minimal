## タイトル
[ATOMS3R] イシュー 2: GPIO ピン定義の更新

## ラベル
`atoms3r-migration`, `high-priority`, `phase-1`

## 説明

### 概要
ATOMS3R のハードウェアに合わせて GPIO ピン定義を更新します。

### 関連するメインイシュー
#XX (メインイシュー番号を記入)

### タスク
- [ ] I2C ピンの変更
  - [ ] SDA: GPIO21 → GPIO38
  - [ ] SCL: GPIO25 → GPIO39
  - [ ] または Grove ポート (G1/G2) の使用を検討
- [ ] LED ピンの確認と更新
  - [ ] 現在: GPIO27
  - [ ] ATOMS3R の内蔵 LED ピンを調査
  - [ ] データシートで確認
- [ ] ボタンピンの確認
  - [ ] ATOMS3R の画面下部ボタンのピン番号を確認
  - [ ] M5Unified での BtnA の動作確認
- [ ] オーディオ関連ピンの確認
  - [ ] Atomic Echo Base のピン配置を確認
  - [ ] 変更が不要か確認

### 実装例

```cpp
// src/main.cpp

#ifdef ATOMS3R
  // ATOMS3R AI Chatbot Kit の設定
  #define I2C_SDA_PIN 38  // G38 (I2C SDA)
  #define I2C_SCL_PIN 39  // G39 (I2C SCL)
  #define LED_PIN     35  // 要確認
#else
  // ATOM Echo の設定（後方互換性）
  #define I2C_SDA_PIN 21
  #define I2C_SCL_PIN 25
  #define LED_PIN     27
#endif
```

### テスト方法
1. I2C デバイス（SSD1306）が正しく認識されるか確認
   - `Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN)` が成功
   - I2C スキャンでデバイスが見つかる
2. LED が正しく点灯するか確認
   - `neopixelWrite(LED_PIN, ...)` で点灯
   - 各色（赤、緑、青）が正しく表示される
3. ボタン入力が正しく検出されるか確認
   - `M5.BtnA.wasSingleClicked()` が動作
   - `M5.BtnA.wasDoubleClicked()` が動作

### 依存関係
- #XX イシュー 1: プラットフォーム設定の更新

### 完了条件
- ✅ すべてのピン定義が ATOMS3R に対応している
- ✅ I2C デバイスが正しく動作する
- ✅ LED が正しく点灯する
- ✅ ボタン入力が正しく検出される
- ✅ コードに適切なコメントが追加されている

### 参考資料
- [ATOMS3R ピン配置図](https://docs.m5stack.com/en/core/AtomS3R)
- [ATOMS3R データシート (PDF)](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/static/pdf/static/en/core/AtomS3R.pdf)
- [M5Unified GPIO リファレンス](https://github.com/m5stack/M5Unified)
- [ATOMS3R_MIGRATION_PLAN.md - イシュー 2](../../ATOMS3R_MIGRATION_PLAN.md#イシュー-2-gpio-ピン定義の更新)
