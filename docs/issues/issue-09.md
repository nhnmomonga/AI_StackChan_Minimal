## タイトル
[ATOMS3R] イシュー 9: ドキュメントの更新

## ラベル
`atoms3r-migration`, `medium-priority`, `phase-3`, `documentation`

## 説明

### 概要
ATOMS3R AI Chatbot Kit 対応に伴い、README.md およびその他のドキュメントを更新します。

### 関連するメインイシュー
#XX (メインイシュー番号を記入)

### タスク

#### README.md の更新
- [ ] 冒頭の説明に ATOMS3R 対応を追加
- [ ] 必要なパーツリストの更新
  - [ ] ATOMS3R AI Chatbot Kit の情報追加
  - [ ] 価格情報の追加
  - [ ] 購入リンクの追加
  - [ ] ATOM Echo との比較表
- [ ] 組み立て手順の更新
  - [ ] ATOMS3R は組み立て不要（一体型）
  - [ ] ATOM Echo 用の手順は残す
- [ ] ピン接続図の更新
  - [ ] ATOMS3R は外部ディスプレイ不要
  - [ ] オプションの外部接続方法
- [ ] インストール手順の確認
  - [ ] M5Burner での書き込み手順
  - [ ] ボード選択の説明追加
  - [ ] PlatformIO でのビルド手順
  - [ ] 環境選択の説明追加

#### 新しいセクションの追加
- [ ] ATOMS3R AI Chatbot Kit 対応セクション
```markdown
### ATOMS3R AI Chatbot Kit 対応

AI_StackChan_Minimal は ATOMS3R AI Chatbot Kit にも対応しています。

#### 必要な物
- ATOMS3R AI Chatbot Kit（内蔵ディスプレイ＆オーディオ付き）
  - [スイッチサイエンス：XX,XXX円](https://www.switch-science.com/products/10487)
  
#### 特徴
- 内蔵0.85インチカラーディスプレイ（128x128）
- より高速なプロセッサ（ESP32-S3 @ 240MHz）
- 8MB PSRAM で大きなモデルに対応
- センサー内蔵（9軸IMU、磁気センサー）
- 一体型で組み立て不要

#### 必要な物（ATOM Echo と比較）
| 項目 | ATOM Echo | ATOMS3R |
|------|-----------|---------|
| マイコン本体 | ATOM Echo | ATOMS3R Kit |
| ディスプレイ | 外部SSD1306必要 | 内蔵 |
| ジャンパーワイヤ | 必要 | 不要 |
| 外装ケース | 3Dプリント | 未定 |
| 組み立て | 必要 | 不要 |
| 価格 | 約3,000円台 | 約X,XXX円 |
```

#### スクリーンショットと写真の追加
- [ ] ATOMS3R の外観写真
- [ ] ATOMS3R での動作画面
- [ ] カラーディスプレイでのアバター表示
- [ ] Web インターフェースのスクリーンショット
- [ ] M5Burner でのボード選択画面

#### FAQ の更新
- [ ] ATOMS3R 特有の質問を追加
  - Q: ATOM Echo と ATOMS3R の違いは？
  - Q: どちらを購入すべきですか？
  - Q: 外部ディスプレイは必要ですか？
  - Q: ATOMS3R で外部センサーは使えますか？
- [ ] トラブルシューティングの追加
  - ATOMS3R でビルドできない場合
  - ディスプレイが表示されない場合
  - オーディオが動作しない場合

#### その他のドキュメント
- [ ] CONTRIBUTING.md の作成（コントリビューションガイド）
- [ ] CHANGELOG.md の更新（変更履歴）
- [ ] LICENSE の確認（変更なし）

### ドキュメント構造（案）

```
AI_StackChan_Minimal/
├── README.md （メインドキュメント）
├── ATOMS3R_MIGRATION_PLAN.md （移植計画）
├── CHANGELOG.md （変更履歴）
├── CONTRIBUTING.md （コントリビューションガイド）
├── docs/
│   ├── issues/ （GitHub Issues テンプレート）
│   ├── images/ （画像ファイル）
│   ├── atom-echo-guide.md （ATOM Echo 専用ガイド）
│   └── atoms3r-guide.md （ATOMS3R 専用ガイド）
└── ...
```

### テスト方法
- [ ] リンク切れのチェック
- [ ] 画像の表示確認
- [ ] コードブロックの構文確認
- [ ] 日本語の表現確認
- [ ] 英語版の必要性検討

### 依存関係
- すべてのコード実装イシューが完了していること
- #XX イシュー 10: テストと検証が完了していること

### 完了条件
- ✅ README.md が ATOMS3R に対応している
- ✅ 新規ユーザーが簡単に始められる
- ✅ ATOM Echo ユーザーも情報を見つけられる
- ✅ スクリーンショットが最新
- ✅ リンク切れがない
- ✅ FAQ が充実している
- ✅ 日本語として自然

### 参考資料
- [既存の README.md](../../README.md)
- [良いREADMEの書き方](https://docs.github.com/ja/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/about-readmes)
- [ATOMS3R_MIGRATION_PLAN.md - イシュー 9](../../ATOMS3R_MIGRATION_PLAN.md#イシュー-9-ドキュメントの更新)
