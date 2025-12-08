# ATOMS3R 移植プロジェクト - ドキュメントインデックス

このドキュメントは、ATOMS3R 移植プロジェクトのすべてのドキュメントへの入り口です。

## 📚 クイックナビゲーション

### 🎯 まずはここから
1. **[SUMMARY.md](../SUMMARY.md)** ← 👈 **最初にお読みください**
   - プロジェクト概要
   - 使い方ガイド
   - 次のステップ

### 📋 計画を理解する
2. **[ATOMS3R_MIGRATION_PLAN.md](../ATOMS3R_MIGRATION_PLAN.md)**
   - 移植プロジェクト全体の詳細計画
   - 12のサブイシュー定義
   - リスク分析と対策
   - タイムライン

3. **[VISUAL_GUIDE.md](./VISUAL_GUIDE.md)**
   - ビジュアルガイド
   - 図表とマトリックス
   - FAQ

### 🔧 実装するとき
4. **[ATOMS3R_QUICK_REFERENCE.md](./ATOMS3R_QUICK_REFERENCE.md)** ← 👈 **実装時に参照**
   - ハードウェア比較表
   - ピンマッピング一覧
   - コード変更テンプレート
   - トラブルシューティング

### 🎫 Issues を作成するとき
5. **[issues/README.md](./issues/README.md)**
   - Issue 作成ガイド
   - ラベルの説明

6. **個別 Issue テンプレート**
   - [issue-00-main.md](./issues/issue-00-main.md) - メインイシュー
   - [issue-01.md](./issues/issue-01.md) - プラットフォーム設定
   - [issue-02.md](./issues/issue-02.md) - GPIO ピン定義
   - [issue-03.md](./issues/issue-03.md) - ディスプレイ設定
   - [issue-04.md](./issues/issue-04.md) - オーディオ設定
   - [issue-09.md](./issues/issue-09.md) - ドキュメント更新
   - [issue-10.md](./issues/issue-10.md) - テストと検証

---

## 🎯 あなたの役割別ガイド

### プロジェクトオーナー / マネージャー
```
1. SUMMARY.md を読む
2. ATOMS3R_MIGRATION_PLAN.md で全体計画を確認
3. GitHub Issues を作成（issues/ のテンプレートを使用）
4. マイルストーンを設定
5. 貢献者を募集
```

### 開発者（実装担当）
```
1. SUMMARY.md でプロジェクト概要を把握
2. ATOMS3R_QUICK_REFERENCE.md を手元に置く
3. 担当する Issue を選択
4. ATOMS3R_QUICK_REFERENCE.md を参照しながら実装
5. 困ったら VISUAL_GUIDE.md で全体像を確認
```

### レビュアー
```
1. SUMMARY.md で背景を理解
2. ATOMS3R_MIGRATION_PLAN.md で計画を確認
3. VISUAL_GUIDE.md のテストマトリックスを参照
4. プルリクエストをレビュー
```

### テスター
```
1. VISUAL_GUIDE.md のテストマトリックスを確認
2. issue-10.md のテストチェックリストを使用
3. ATOMS3R_QUICK_REFERENCE.md のトラブルシューティングを参照
4. 問題を発見したら新しい Issue を作成
```

---

## 📊 ドキュメント概要

| ドキュメント | 主な内容 | 対象者 | 読む順序 |
|------------|---------|-------|---------|
| SUMMARY.md | プロジェクト概要、使い方 | 全員 | 1 |
| ATOMS3R_MIGRATION_PLAN.md | 詳細計画、12サブイシュー | PM、開発者 | 2 |
| VISUAL_GUIDE.md | 図表、マトリックス、FAQ | 全員 | 3 |
| ATOMS3R_QUICK_REFERENCE.md | 実装リファレンス | 開発者 | 実装時 |
| issues/ | Issue テンプレート集 | PM、開発者 | Issue作成時 |

---

## 🗺️ プロジェクトロードマップ

```
現在地: 📋 計画フェーズ完了 ✅

次へ: 🎫 GitHub Issues 作成
      ↓
      🔧 フェーズ 1: 基本移植（1週間）
      ↓
      ⚡ フェーズ 2: 最適化（1週間）
      ↓
      🧪 フェーズ 3: テスト（3-4日）
      ↓
      🎁 フェーズ 4: 拡張（オプション、1-2週間）
      ↓
      🚀 リリース
```

---

## 📖 ドキュメント詳細

### SUMMARY.md
**概要**: プロジェクトの全体像と使い方  
**サイズ**: 3.9KB  
**内容**:
- プロジェクト概要
- 作成されたドキュメント一覧
- 使い方（3ステップ）
- 主要な変更点
- 注意事項

**こんなときに読む**:
- 初めてプロジェクトに参加するとき
- 全体像を把握したいとき

---

### ATOMS3R_MIGRATION_PLAN.md
**概要**: 移植プロジェクトの詳細計画書  
**サイズ**: 8.4KB  
**内容**:
- 12のサブイシュー定義
- 4つの実装フェーズ
- 実装順序
- リスクと対策
- 成功基準
- タイムライン（3-5週間）

**こんなときに読む**:
- 詳細な計画を知りたいとき
- 各イシューの内容を確認するとき
- リスク対策を検討するとき

---

### VISUAL_GUIDE.md
**概要**: ビジュアルガイド（図表、マトリックス）  
**サイズ**: 6.4KB  
**内容**:
- プロジェクト構造図
- ハードウェア比較マトリックス
- 依存関係グラフ
- テストマトリックス
- リスクマトリックス
- FAQ
- 用語集

**こんなときに読む**:
- 視覚的に全体を理解したいとき
- ハードウェアの違いを確認するとき
- テスト項目を確認するとき
- よくある質問を見たいとき

---

### ATOMS3R_QUICK_REFERENCE.md
**概要**: 開発者向けクイックリファレンス  
**サイズ**: 4.8KB  
**内容**:
- ハードウェア比較表
- ピンマッピング変更一覧
- PlatformIO 設定例
- コード変更テンプレート
- ビルドコマンド
- テストチェックリスト
- トラブルシューティング

**こんなときに読む**:
- 実装を開始するとき
- ピン番号を確認するとき
- コード例が欲しいとき
- ビルドエラーが出たとき

---

### issues/
**概要**: GitHub Issue テンプレート集  
**内容**:
- README.md - 使い方ガイド
- issue-00-main.md - メインイシュー
- issue-01.md ～ issue-10.md - サブイシュー

**こんなときに使う**:
- GitHub で Issue を作成するとき
- Issue の内容をコピー&ペースト

---

## 🔍 キーワード検索

### ハードウェア関連
- **ピン番号を知りたい** → [ATOMS3R_QUICK_REFERENCE.md](./ATOMS3R_QUICK_REFERENCE.md#ピンマッピング変更)
- **ハードウェアの違いを見たい** → [VISUAL_GUIDE.md](./VISUAL_GUIDE.md#ハードウェア比較マトリックス)
- **センサーについて知りたい** → [ATOMS3R_MIGRATION_PLAN.md](../ATOMS3R_MIGRATION_PLAN.md#イシュー-6-センサー統合の準備)

### ソフトウェア関連
- **PlatformIO設定を知りたい** → [ATOMS3R_QUICK_REFERENCE.md](./ATOMS3R_QUICK_REFERENCE.md#platformio-設定変更)
- **コード例が欲しい** → [ATOMS3R_QUICK_REFERENCE.md](./ATOMS3R_QUICK_REFERENCE.md#コード変更テンプレート)
- **ビルド方法を知りたい** → [ATOMS3R_QUICK_REFERENCE.md](./ATOMS3R_QUICK_REFERENCE.md#ビルドコマンド)

### プロジェクト管理
- **全体スケジュールを見たい** → [VISUAL_GUIDE.md](./VISUAL_GUIDE.md#実装マイルストーン)
- **リスクを確認したい** → [VISUAL_GUIDE.md](./VISUAL_GUIDE.md#リスクマトリックス)
- **Issue を作りたい** → [issues/README.md](./issues/README.md)

### テスト関連
- **テスト項目を見たい** → [VISUAL_GUIDE.md](./VISUAL_GUIDE.md#テストマトリックス)
- **テスト方法を知りたい** → [issues/issue-10.md](./issues/issue-10.md)

### トラブルシューティング
- **ビルドエラー** → [ATOMS3R_QUICK_REFERENCE.md](./ATOMS3R_QUICK_REFERENCE.md#トラブルシューティング)
- **FAQ** → [VISUAL_GUIDE.md](./VISUAL_GUIDE.md#よくある質問faq)

---

## 📞 サポート

### 質問がある場合
1. まず [VISUAL_GUIDE.md の FAQ](./VISUAL_GUIDE.md#よくある質問faq) を確認
2. 該当するドキュメントを読む
3. それでも解決しない場合は GitHub Issue を作成

### バグを見つけた場合
1. [ATOMS3R_QUICK_REFERENCE.md のトラブルシューティング](./ATOMS3R_QUICK_REFERENCE.md#トラブルシューティング) を確認
2. 既知の問題でない場合は GitHub Issue を作成

### 改善提案がある場合
1. GitHub Issue を作成
2. または Pull Request を送信

---

## 📝 更新履歴

| 日付 | バージョン | 内容 |
|-----|----------|------|
| 2025-12-08 | 1.0 | 初版作成（全ドキュメント） |

---

## 🔗 外部リンク

### 製品情報
- [ATOMS3R 製品ページ](https://www.switch-science.com/products/10487)
- [ATOMS3R 公式ドキュメント](https://docs.m5stack.com/en/core/AtomS3R-AI%20Chatbot)

### 技術資料
- [ESP32-S3 仕様](https://www.espressif.com/en/products/socs/esp32-s3)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)

### リポジトリ
- [AI_StackChan_Minimal](https://github.com/nhnmomonga/AI_StackChan_Minimal)
- [元のAIスタックチャン](https://github.com/robo8080/AI_StackChan2)

---

**最終更新**: 2025-12-08  
**管理者**: @nhnmomonga
