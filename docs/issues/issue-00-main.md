## タイトル
[ATOMS3R] ATOM Echo から ATOMS3R AI Chatbot Kit への移植

## ラベル
`enhancement`, `atoms3r-migration`, `high-priority`

## 説明

### 概要
AI_StackChan_Minimal を ATOM Echo から ATOMS3R AI Chatbot Kit に移植します。

### 背景
ATOMS3R AI Chatbot Kit は ATOM Echo と比べて以下の利点があります：
- より強力なプロセッサ (ESP32-S3, デュアルコア Xtensa LX7 @ 240MHz)
- より多くのメモリ (8MB Flash + 8MB PSRAM)
- 内蔵カラーディスプレイ (0.85インチ IPS TFT, 128x128)
- 内蔵センサー (9軸IMU: BMI270 + BMM150)
- 赤外線送信機
- Atomic Echo Base 統合

### 製品情報
- 製品URL: https://www.switch-science.com/products/10487
- 公式ドキュメント: https://docs.m5stack.com/en/core/AtomS3R-AI%20Chatbot

### 目標
1. ✅ 既存の機能をすべて維持
2. ✅ 内蔵ディスプレイを活用
3. ✅ 追加のセンサーとメモリを将来の拡張に備える
4. ✅ ユーザーがどちらのハードウェアでも使用できるようにする（可能な場合）

### サブイシュー

詳細な計画は `ATOMS3R_MIGRATION_PLAN.md` を参照してください。

#### フェーズ 1: 基本移植（必須）
- [ ] #XX イシュー 1: プラットフォーム設定の更新
- [ ] #XX イシュー 2: GPIO ピン定義の更新
- [ ] #XX イシュー 8: ビルド設定とライブラリ依存関係の更新
- [ ] #XX イシュー 4: オーディオ設定の検証と調整

#### フェーズ 2: ディスプレイとメモリ最適化
- [ ] #XX イシュー 3: ディスプレイ設定の更新
- [ ] #XX イシュー 5: メモリ最適化と PSRAM 活用

#### フェーズ 3: テストとドキュメント
- [ ] #XX イシュー 10: テストと検証
- [ ] #XX イシュー 9: ドキュメントの更新

#### フェーズ 4: 将来の拡張（オプション）
- [ ] #XX イシュー 6: センサー統合の準備
- [ ] #XX イシュー 7: 赤外線機能の準備
- [ ] #XX イシュー 11: 後方互換性の維持
- [ ] #XX イシュー 12: M5Burner ファームウェアの準備

### 成功基準
1. ✅ ATOMS3R で ATOM Echo の全機能が動作
2. ✅ 内蔵ディスプレイでアバターが表示される
3. ✅ 音声認識と合成が正常に動作
4. ✅ ChatGPT との連携が正常に動作
5. ✅ Web インターフェースが正常に動作
6. ✅ ドキュメントが更新され、ユーザーが簡単に使用開始できる
7. ✅ 長時間安定動作する
8. ✅ （オプション）ATOM Echo との後方互換性が維持される

### タイムライン（推定）
- **フェーズ 1**: 1週間
- **フェーズ 2**: 1週間
- **フェーズ 3**: 3-4日
- **フェーズ 4**: 1-2週間（オプション）

**合計**: 約3-5週間（オプション機能を含む）

### 参考資料
- [ATOMS3R_MIGRATION_PLAN.md](../ATOMS3R_MIGRATION_PLAN.md) - 詳細な移植計画
- [M5Stack 公式ドキュメント](https://docs.m5stack.com/en/core/AtomS3R-AI%20Chatbot)
- [ESP32-S3 技術仕様](https://www.espressif.com/en/products/socs/esp32-s3)
