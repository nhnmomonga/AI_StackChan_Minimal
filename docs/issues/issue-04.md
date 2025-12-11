## タイトル
[ATOMS3R] イシュー 4: オーディオ設定の検証と調整

## ラベル
`atoms3r-migration`, `high-priority`, `phase-1`

## 説明

### 概要
ATOMS3R に統合された Atomic Echo Base のオーディオ機能を検証し、最適な設定を行います。

### 関連するメインイシュー
#XX (メインイシュー番号を記入)

### タスク
- [ ] M5Speaker の設定確認と調整
  - [ ] サンプルレート設定の確認（現在: 96000Hz）
  - [ ] ATOMS3R での最適値を検証
  - [ ] バッファサイズの調整
  - [ ] タスクピンコアの設定確認
- [ ] ボリューム設定の最適化
  - [ ] 現在の設定: 150（ATOM Echo の安全範囲）
  - [ ] ATOMS3R での安全な最大値を確認
  - [ ] 推奨ボリューム範囲を決定
  - [ ] スピーカー保護の確認
- [ ] マイク入力の検証
  - [ ] MEMS マイクの動作確認
  - [ ] SNR の測定（仕様: ≥65dB）
  - [ ] 音声認識精度の検証
  - [ ] ノイズキャンセリングの確認
- [ ] スピーカー出力の検証
  - [ ] 音質の確認
  - [ ] 音量レベルの最適化
  - [ ] Class-D アンプ (NS4150B) の動作確認
  - [ ] 歪みやクリッピングのチェック

### 実装例

```cpp
// src/main.cpp

void setup()
{
  // ...
  
  { /// オーディオのカスタム設定
    auto spk_cfg = M5.Speaker.config();
    // サンプルレート: 音質とCPU負荷のバランス
    spk_cfg.sample_rate = 96000; // 96kHz（検証後に調整可能）
    spk_cfg.task_pinned_core = APP_CPU_NUM;
    M5.Speaker.config(spk_cfg);
  }
  
  M5.Speaker.begin();
  
  // ボリューム設定（ATOMS3R での最適値）
  M5.Speaker.setVolume(150);  // 検証後に調整
  
  // マイク設定の確認
  auto mic_cfg = M5.Mic.config();
  // 必要に応じて調整
  M5.Mic.config(mic_cfg);
}
```

### テスト方法
1. スピーカー出力テスト
   - [ ] トーン出力が正常（`M5.Speaker.tone()`）
   - [ ] MP3 再生が正常
   - [ ] 音声合成（VOICEVOX）が正常
   - [ ] 音量レベルが適切
   - [ ] 歪みがない
2. マイク入力テスト
   - [ ] 音声録音が正常
   - [ ] Whisper/Google STT での認識精度
   - [ ] ノイズレベルの確認
   - [ ] 複数の距離でのテスト（10cm, 30cm, 50cm）
3. 長時間動作テスト
   - [ ] 連続使用でのパフォーマンス
   - [ ] 発熱の確認
   - [ ] スピーカーの劣化がない

### 依存関係
- #XX イシュー 1: プラットフォーム設定の更新
- #XX イシュー 2: GPIO ピン定義の更新

### 完了条件
- ✅ スピーカー出力が正常に機能する
- ✅ マイク入力が正常に機能する
- ✅ 音質が良好
- ✅ 音声認識精度が十分
- ✅ 安全な動作範囲が確認されている
- ✅ ドキュメントに推奨設定が記載されている

### 注意事項
- ATOM Echo では 150 以上のボリュームでスピーカーが破損する可能性があることが報告されています
- ATOMS3R でも同様の制限があるか確認が必要です
- テストは慎重に、段階的に行ってください

### 参考資料
- [M5Speaker クラスリファレンス](https://github.com/m5stack/M5Unified)
- [ES8311 オーディオコーデックデータシート](https://www.everest-semi.com/pdf/ES8311.pdf)
- [ATOMS3R オーディオ仕様](https://docs.m5stack.com/en/core/AtomS3R-AI%20Chatbot)
- [ATOMS3R_MIGRATION_PLAN.md - イシュー 4](../../ATOMS3R_MIGRATION_PLAN.md#イシュー-4-オーディオ設定の検証と調整)
