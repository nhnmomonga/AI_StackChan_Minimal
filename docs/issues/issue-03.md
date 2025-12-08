## タイトル
[ATOMS3R] イシュー 3: ディスプレイ設定の更新

## ラベル
`atoms3r-migration`, `medium-priority`, `phase-2`

## 説明

### 概要
ATOMS3R の内蔵 TFT ディスプレイ（0.85インチ IPS, 128x128, GC9107）をサポートし、外部 SSD1306 との共存も可能にします。

### 関連するメインイシュー
#XX (メインイシュー番号を記入)

### タスク
- [ ] 内蔵 TFT ディスプレイのサポート追加
  - [ ] GC9107 ドライバの設定を確認
  - [ ] 128x128 解像度への対応
  - [ ] M5Unified での TFT 初期化コードの実装
  - [ ] カラー表示の動作確認
- [ ] 外部 SSD1306 との共存オプション
  - [ ] ビルド時またはランタイムで切り替え可能にする
  - [ ] デフォルトは内蔵 TFT を使用
  - [ ] 設定ファイルまたはフラグでの切り替え
- [ ] アバター表示の調整
  - [ ] カラーディスプレイに適した配色
  - [ ] 解像度の違いへの対応（96x64 → 128x128）
  - [ ] テキスト表示領域の調整
  - [ ] アバターライブラリの互換性確認

### 実装例

```cpp
// src/main.cpp

void setup()
{
  auto cfg = M5.config();
  
#ifdef USE_INTERNAL_DISPLAY
  // ATOMS3R 内蔵 TFT ディスプレイを使用
  cfg.internal_imu = true;
  M5.begin(cfg);
  M5.setPrimaryDisplayType({m5::board_t::board_M5AtomS3});
#else
  // 外部 SSD1306 OLED を使用（後方互換性）
  cfg.unit_glass2.pin_sda = I2C_SDA_PIN;
  cfg.unit_glass2.pin_scl = I2C_SCL_PIN;
  M5.begin(cfg);
  M5.setPrimaryDisplayType({m5::board_t::board_M5UnitGLASS2});
#endif

  // アバターの初期化
  avatar.init();
  // カラーディスプレイ用の設定
  avatar.setColorDepth(16); // 16ビットカラー
}
```

### テスト方法
1. 内蔵 TFT でのテスト
   - [ ] アバターが正しく表示される
   - [ ] テキストが読みやすい
   - [ ] 色の表現が適切
   - [ ] スクロールが滑らか
2. 外部 SSD1306 でのテスト（後方互換性）
   - [ ] 従来通り動作する
   - [ ] 表示が崩れていない
3. 長時間表示テスト
   - [ ] 画面の焼き付きがない
   - [ ] メモリリークがない

### 依存関係
- #XX イシュー 2: GPIO ピン定義の更新

### 完了条件
- ✅ 内蔵 TFT ディスプレイでアバターが表示される
- ✅ カラー表示が適切に機能する
- ✅ 外部 SSD1306 も選択可能（オプション）
- ✅ テキスト表示が読みやすい
- ✅ パフォーマンスが良好

### 参考資料
- [GC9107 データシート](https://github.com/m5stack/M5GFX)
- [M5Unified ディスプレイ設定](https://github.com/m5stack/M5Unified/blob/master/docs/README_ja.md)
- [M5Stack-Avatar カラー対応](https://github.com/meganetaaan/m5stack-avatar)
- [ATOMS3R_MIGRATION_PLAN.md - イシュー 3](../../ATOMS3R_MIGRATION_PLAN.md#イシュー-3-ディスプレイ設定の更新)
