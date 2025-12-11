/***
 - GitHub AI_StackChan_Minimal
 https://github.com/A-Uta/AI_StackChan_Minimal

 - AIｽﾀｯｸﾁｬﾝ - ミニマル(Minimal)で利用しているWebサービスと、その料金は下記を参照ください
 1. OpenAI API
 https://platform.openai.com/docs/overview
   A. Pricing (利用料金) - GPT-XXX / Audio models-Whisperをご確認ください
   https://openai.com/api/pricing/

 2. WEB版VOICEVOX API（高速）（無料）
 https://voicevox.su-shiki.com/su-shikiapis/

 3. Google Speech-to-Text
 https://cloud.google.com/speech-to-text?hl=ja
   A. Speech-to-Text の料金 - Speech-to-Text V1 API をご確認ください
   https://cloud.google.com/speech-to-text/pricing?hl=ja
***/

#include <Arduino.h>
#include <Wire.h>         // Add for I2C (ES8311)
#include "driver/i2c.h"   // ESP-IDF I2C driver for ES8311
#ifndef ATOMS3R
#include <M5UnitGLASS2.h> // Add for SSD1306 (only for ATOM Echo with external display)
#endif
#include <SPIFFS.h>       // Add for Web Setting
#include <M5Unified.h>
#include <nvs.h>          // Add for Web Setting
#include <Avatar.h>       // Add for M5 avatar
#include <AudioOutput.h>
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputM5Speaker.h"
#include "AudioFileSourceHTTPSStream.h"
#include "WebVoiceVoxTTS.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "rootCACertificate.h"
#include "rootCAgoogle.h"
#include <ArduinoJson.h>
#include "AudioWhisper.h"
#include "Whisper.h"
#include "Audio.h"
#include "CloudSpeechClient.h"
#include "AudioHardware.h"
#include <deque>
#include <ESP32WebServer.h> // Add for Web Setting

#include <AudioFileSourcePROGMEM.h> // Add for Global language
#include <google-tts.h>             // Add for Global language

/// Web or SSL server
ESP32WebServer server(80);  // Add for Web server setting

using namespace m5avatar;   // Add for M5 avatar
Avatar avatar;              // Add for M5 avatar

/// set M5Speaker virtual channel (0-7)
static constexpr uint8_t m5spk_virtual_channel = 0;
static AudioOutputM5Speaker out(&M5.Speaker, m5spk_virtual_channel);
AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;

AudioFileSourcePROGMEM *file1 = nullptr; // Add for Global language
// uint8_t mp3buff[1024*50];             // Add for Global language
uint8_t mp3buff[1024*20];                // Add for Global language
TTS tts;                                 // Add for Global language
// String LANG_CODE = "zh-CN";
String LANG_CODE = "ja-jp";
// String LANG_CODE = "en-US";

String message_help_you = ""; // Add for Global language
String message_You_said = ""; // Add for Global language
String message_tinking = "";  // Add for Global language
String message_error = "";    // Add for Global language
String message_cant_hear = ""; // Add for Global language
String message_dont_understand = ""; // Add for Global language

/// 接続：Wifiは[スマホアプリ(EspTouch)]、また[APIキーはブラウザ]から設定します。

/// I2C接続のピン番号 // Add for SSD1306
#ifdef ATOMS3R
  // ATOMS3R + Atomic Echo Base の設定
  #define I2C_SDA_PIN 38  // G38 (I2C SDA) - ES8311制御用
  #define I2C_SCL_PIN 39  // G39 (I2C SCL)
  /// LEDストリップのピン番号
  #define LED_PIN     35  // G35 (内蔵 RGB LED)
  /// Groveピン (Serial1用) - Atomic Echo Base 経由
  #define GROVE_RX_PIN 5  // G5 (Atomic Echo BaseのGrove)
  #define GROVE_TX_PIN 6  // G6 (Atomic Echo BaseのGrove)
  /// 内蔵ディスプレイを使用するかどうか（外部ディスプレイ M5UnitGLASS2 を使用しない場合はtrue）
  #define USE_INTERNAL_DISPLAY true
  /// Atomic Echo Base (ES8311) を使用 - platformio.iniで定義されていない場合のデフォルト
  #ifndef USE_ATOMIC_ECHO_BASE
    #define USE_ATOMIC_ECHO_BASE true
  #endif
#else
  // ATOM Echo の設定（後方互換性）
  #define I2C_SDA_PIN 21  // G21 (I2C SDA)
  #define I2C_SCL_PIN 25  // G25 (I2C SCL)
  /// LEDストリップのピン番号
  #define LED_PIN     27  // G27 (内蔵 RGB LED)
  /// Groveピン (Serial1用)
  #define GROVE_RX_PIN 32
  #define GROVE_TX_PIN 26
  /// 外部ディスプレイを使用
  #define USE_INTERNAL_DISPLAY false
  /// 内蔵スピーカー・マイクを使用
  #ifndef USE_ATOMIC_ECHO_BASE
    #define USE_ATOMIC_ECHO_BASE false
  #endif
#endif
/// LEDストリップのLED数
#define NUM_LEDS    1
/// LED明るさ
#define BRIGHTNESS 64 // Adjust for Atom Echo
/// LEDストリップの色の並び順
// #define COLOR_ORDER GRB

#if USE_ATOMIC_ECHO_BASE
#include "es8311.h"

// ES8311 I2C address (CE pin low = 0x18, CE pin high = 0x19)
#define ES8311_ADDR 0x18

#define ES8311_I2C_PORT     I2C_NUM_1
#define ES8311_I2C_FREQ     400000
#define ES8311_SAMPLE_RATE  16000

// ES8311ハンドル
static es8311_handle_t es8311_handle = nullptr;

// ES8311初期化関数
bool init_es8311() {
  Serial.println("Initializing ES8311...");
  
  // I2Cドライバをインストール
  i2c_config_t i2c_conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = (gpio_num_t)I2C_SDA_PIN,
    .scl_io_num = (gpio_num_t)I2C_SCL_PIN,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {
      .clk_speed = ES8311_I2C_FREQ,
    },
    .clk_flags = 0,
  };
  
  esp_err_t ret = i2c_param_config(ES8311_I2C_PORT, &i2c_conf);
  if (ret != ESP_OK) {
    Serial.println("ES8311: I2C param config failed");
    return false;
  }
  
  ret = i2c_driver_install(ES8311_I2C_PORT, i2c_conf.mode, 0, 0, 0);
  if (ret != ESP_OK) {
    Serial.println("ES8311: I2C driver install failed");
    return false;
  }
  
  // ES8311ハンドルを作成
  es8311_handle = es8311_create(ES8311_I2C_PORT, ES8311_ADDRRES_0);
  if (es8311_handle == nullptr) {
    Serial.println("ES8311: Failed to create handle");
    return false;
  }
  
  // ES8311クロック設定
  es8311_clock_config_t clk_cfg = {
    .mclk_inverted = false,
    .sclk_inverted = false,
    .mclk_from_mclk_pin = true,
    .mclk_frequency = ES8311_MCLK_FREQ,
    .sample_frequency = ES8311_SAMPLE_RATE,
  };
  
  ret = es8311_init(es8311_handle, &clk_cfg, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
  if (ret != ESP_OK) {
    Serial.println("ES8311: init failed");
    return false;
  }
  
  // 音量設定
  es8311_voice_volume_set(es8311_handle, 80, NULL);
  
  // マイク設定 (false = ゲイン低め、true = ゲイン高め)
  es8311_microphone_config(es8311_handle, false);
  es8311_microphone_gain_set(es8311_handle, ES8311_MIC_GAIN_42DB);
  
  Serial.println("ES8311: Initialized successfully");

  // この時点でクロックやゲイン設定は完了しているため、
  // M5Unified 側の I2C 初期化と競合しないようドライバを解放する。
  i2c_driver_delete(ES8311_I2C_PORT);
  es8311_delete(es8311_handle);
  es8311_handle = nullptr;
  return true;
}
#endif

/// 保存する質問と回答の最大数
const int MAX_HISTORY = 2; // Adjust for Atom Echo
/// 過去の質問と回答を保存するデータ構造
std::deque<String> chatHistory;
///---------------------------------------------
String OPENAI_API_KEY = "";
String VOICEVOX_API_KEY = "";
String STT_API_KEY = "";
String TTS_SPEAKER = "&speaker=";
String TTS_PARMS = TTS_SPEAKER;
String speech_text_buffer = "";

/// キャラクターの音声構造
class CHARACTER_VOICE {
  public:
    // uint8_t normal, happy, sasa_yaki;
    uint16_t normal, happy, sasa_yaki;
};

/// 選択:キャラクター
String CHARACTER = "";
uint8_t character;

/// 選択:Chat GPTのモデルVersion
String MODEL_VER = "";
uint8_t model_ver;

/// 入力:Webからのテキスト入力
String TEXTAREA = "";

/// 入力:Webからの入力
String Toio_ActionID = "";

// DynamicJsonDocument chat_doc(1024*10);
StaticJsonDocument<768> chat_doc; // Adjust for Atom Echo
String json_ChatString = "{\"model\": \"gpt-3.5-turbo\",\"messages\": [{\"role\": \"user\", \"content\": \"""\"}]}";
// String json_ChatString =
// "{\"model\": \"gpt-3.5-turbo\",\
//  \"messages\": [\
//                 {\"role\": \"user\", \"content\": \"" + text + "\"},\
//                 {\"role\": \"system\", \"content\": \"あなたは「スタックちゃん」と言う名前の小型ロボットとして振る舞ってください。\"},\
//                 {\"role\": \"system\", \"content\": \"あなたはの使命は人々の心を癒すことです。\"},\
//                 {\"role\": \"system\", \"content\": \"幼い子供の口調で話してください。\"},\
//                 {\"role\": \"system\", \"content\": \"語尾には「だよ｝をつけて話してください。\"}\
//               ]}";
String Role_JSON = "";
String InitBuffer = "";

static const char HEAD[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html lang="ja">
<head>
  <title>AIｽﾀｯｸﾁｬﾝ</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      textarea {
        width: 80%;
        height: 200px;
        resize: both;
      }
    </style>  
</head>)KEWL";

static const char APIKEY_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
  <head>
    <title>AIｽﾀｯｸﾁｬﾝ - APIキー設定</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      textarea {
        width: 80%;
        height: 200px;
        resize: both;
      }
    </style>
  </head>
  <body>
    <h1>APIキー設定</h1>
    <form>
      <input type="text" id="openai" name="openai" oninput="adjustSize(this)">
      <label for="role1">OpenAI(必須)</label></br>
      <input type="text" id="voicevox" name="voicevox" oninput="adjustSize(this)">
      <label for="role2">VoiceVox(必須)</label></br>
      <input type="text" id="sttapikey" name="sttapikey" oninput="adjustSize(this)">
      <label for="role3">Google Speech to Text(未入力時,OpenAI Whisperを使用)</label></br>
      <button type="button" onclick="sendData()">送信する</button>
      <button type="button" onclick="history.back()">戻る</button>
    </form>
    <script>
      function adjustSize(input) {
        input.style.width = ((input.value.length + 1) * 8) + 'px';
      }
      function sendData() {
        // FormDataオブジェクトを作成
        const formData = new FormData();

        // 各ロールの値をFormDataオブジェクトに追加
        const openaiValue = document.getElementById("openai").value;
        if (openaiValue !== "") formData.append("openai", openaiValue);

        const voicevoxValue = document.getElementById("voicevox").value;
        if (voicevoxValue !== "") formData.append("voicevox", voicevoxValue);

        const sttapikeyValue = document.getElementById("sttapikey").value;
        if (sttapikeyValue !== "") {
          formData.append("sttapikey", sttapikeyValue);
        } else {
          formData.append("sttapikey", openaiValue);
        }

	    // POSTリクエストを送信
	    const xhr = new XMLHttpRequest();
	    xhr.open("POST", "/apikey_set");
	    xhr.onload = function() {
	      if (xhr.status === 200) {
	        alert("設定を送信しました！");
	      } else {
	        alert("設定の送信に失敗しました。");
	      }
	    };
	    xhr.send(formData);
	  }
	</script>
  </body>
</html>)KEWL";

static const char CHARACTER_VOICE_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
  <head>
    <title>AIｽﾀｯｸﾁｬﾝ - 言語/キャラクター音声の設定</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      textarea {
        width: 80%;
        height: 200px;
        resize: both;
      }
    </style>
  </head>
  <body>
    <h1>言語/キャラクター音声の設定</h1>
    <form>
      <select id="character" onchange="sendData()">
        <optgroup label="日本語">
          <option value="ja-03">ずんだもん</option>
          <option value="ja-02">四国めたん</option>
          <option value="ja-13">青山龍星</option>
          <option value="ja-23">WhiteCUL</option>
        </optgroup>
        <optgroup label="English">
          <option value="en-01">Normal</option>
        </optgroup>
        <optgroup label="中文">
          <option value="zh-01">Normal</option>
        </optgroup>
      </select></br>
      <button type="button" onclick="history.back()">戻る</button></br></br>
      <li><a href='https://voicevox.hiroshiba.jp/'>VOICEVOX</a>のキャラクター音声を利用しています</li>
      <li><label for="role1">クレジット表記 - VOICEVOX:ずんだもん,VOICEVOX:四国めたん,VOICEVOX:青山龍星,VOICEVOX:WhiteCUL</label></li>
    </form>
    <script>
      function sendData() {
        // FormDataオブジェクトを作成
        const formData = new FormData();

        // 各ロールの値をFormDataオブジェクトに追加
        const characterValue = document.getElementById("character").value;
        if (characterValue !== "") {
          formData.append("character", characterValue);
        }

	    // POSTリクエストを送信
	    const xhr = new XMLHttpRequest();
	    xhr.open("POST", "/character_voice_set");
	    xhr.onload = function() {
	      if (xhr.status === 200) {
	        alert("設定を送信しました！");
	      } else {
	        alert("設定の送信に失敗しました。");
	      }
	    };
	    xhr.send(formData);
	  }
	</script>
  </body>
</html>)KEWL";

static const char MODEL_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
  <head>
    <title>AIｽﾀｯｸﾁｬﾝ - 対話型AIモデルの設定</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      textarea {
        width: 80%;
        height: 200px;
        resize: both;
      }
    </style>
  </head>
  <body>
    <h1>対話型AIモデルの設定</h1>
    <form>
      <select id="model_ver" onchange="sendData()">
        <optgroup label="ChatGPT">
          <option value="">選択してください</option>
          <option value="1">gpt-3.5-turbo</option>
          <option value="2">gpt-4o-mini</option>
          <option value="3">gpt-4o</option>
        </optgroup>
      </select></br>
      <button type="button" onclick="history.back()">戻る</button>
    </form>
    <script>
      function sendData() {
        // FormDataオブジェクトを作成
        const formData = new FormData();

        // 各ロールの値をFormDataオブジェクトに追加
        const model_verValue = document.getElementById("model_ver").value;
        if (model_verValue !== "") {
          formData.append("model_ver", model_verValue);
        }

	    // POSTリクエストを送信
	    const xhr = new XMLHttpRequest();
	    xhr.open("POST", "/model_ver_set");
	    xhr.onload = function() {
	      if (xhr.status === 200) {
	        alert("設定を送信しました！");
	      } else {
	        alert("設定の送信に失敗しました。");
	      }
	    };
	    xhr.send(formData);
	  }
	</script>
  </body>
</html>)KEWL";

static const char TEXT_CHAT_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
<head>
	<title>AIｽﾀｯｸﾁｬﾝ - テキストで会話</title>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<style>
		textarea {
			width: 80%;
			height: 200px;
			resize: both;
		}
	</style>
</head>
<body>
	<h1>テキストで会話</h1>
	<form onsubmit="postData(event)">
		<label for="textarea">以下に会話を入力してください。</label><br>
		<textarea id="textarea" name="textarea"></textarea><br><br>
    <button type="submit">送信する</button>
    <button type="reset">リセットする</button>
    <button type="button" onclick="history.back()">戻る</button>
	</form>
	<script>
		function postData(event) {
			event.preventDefault();
			const textAreaContent = document.getElementById("textarea").value.trim();
			if (textAreaContent.length > 0) {
				const xhr = new XMLHttpRequest();
				xhr.open("POST", "/text_chat_set");
				xhr.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
				xhr.send(textAreaContent);
        alert("会話を送信しました！");
			} else {
        alert("何か会話を入力してください");
			}
		}
	</script>
</body>
</html>)KEWL";

static const char TOIO_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
<head>
	<title>AIｽﾀｯｸﾁｬﾝ - Toio</title>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<style>
	</style>
</head>
<body>
	<h1>Toioを操作</h1>
<!-- Sample
https://mukudori-noise.skr.jp/esp1.html
https://qiita.com/suzakutakumi3/items/bbd24ff6b8e6ca0310a9
https://tekuteku-embedded.xyz/2021/08/21/esp32%E3%81%AEweb%E3%82%B5%E3%83%BC%E3%83%90%E3%82%92%E3%83%87%E3%82%B6%E3%82%A4%E3%83%B3%E3%81%99%E3%82%8B/
https://swift-smooth.com/character-list/
-->
  <form>
		<label for="textarea">操作ボタン</label><br>
    <table>
      <tr>
        <th></th>
        <th><button type="button" onclick='sendData("F")'>▲</button></th>
        <th></th>
      </tr>
      <tr>
        <td><button type="button" onclick='sendData("L")'>⫍</button></td>
        <td></td>
        <td><button type="button" onclick='sendData("R")'>⫎</button></td>
      </tr>
      <tr>
        <td></td>
        <td><button type="button" onclick='sendData("B")'>▼</button></td>
        <td></td>
      </tr>
    </table>
    <br>
    <button type="button" onclick="history.back()">戻る</button>    
  </form>
	<script>
    function sendData(btn) {
      // FormDataオブジェクトを作成
      const formData = new FormData();

      // 各ロールの値をFormDataオブジェクトに追加
      const set_button_Value = btn;
      if (set_button_Value !== "") {
        formData.append("toio_action", set_button_Value);
      }

      // POSTリクエストを送信
      const xhr = new XMLHttpRequest();
      xhr.open("POST", "/toio_action_set");
      xhr.onload = function() {
        // if (xhr.status === 200) {
        //   alert("操作を送信しました！");
        // } else {
        //   alert("操作の送信に失敗しました。");
        // }
      };
      xhr.send(formData);
    }
	</script>
</body>
</html>)KEWL";

void handleRoot() {
  String message = "";
  message += "<h1>設定メニュー</h1>";
  message += "\n<ul>";
  message += "\n  <li><a href='apikey'>APIキー設定</a></li>";
  message += "\n  <li><a href='character_voice'>言語/キャラクター音声の設定</a></li>";
  message += "\n  <li><a href='model_ver'>対話型AIモデルの設定</a></li>";
  message += "\n  <li><a href='text_chat'>テキストで会話</a></li>";
  message += "\n  <li><a href='toio_action'>Toioを操作</a></li>";
  message += "\n</ul>";
  server.send(200, "text/html", String(HEAD) + String("<body>") + message + String("</body>"));
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/html", String(HEAD) + String("<body>") + message + String("</body>"));
}

void handle_apikey() {
  /// ファイルを読み込み、クライアントに送信する
  server.send(200, "text/html", APIKEY_HTML);
}

void handle_apikey_set() {
  /// POST以外は拒否
  if (server.method() != HTTP_POST) {
    return;
  }
  /// openai
  String openai = server.arg("openai");
  /// voicetxt
  String voicevox = server.arg("voicevox");
  /// voicetxt
  String sttapikey = server.arg("sttapikey");
 
  OPENAI_API_KEY = openai;
  VOICEVOX_API_KEY = voicevox;
  STT_API_KEY = sttapikey;
  Serial.println(openai);
  Serial.println(voicevox);
  Serial.println(sttapikey);

  uint32_t nvs_handle;
  if (ESP_OK == nvs_open("apikey", NVS_READWRITE, &nvs_handle)) {
    nvs_set_str(nvs_handle, "openai", openai.c_str());
    nvs_set_str(nvs_handle, "voicevox", voicevox.c_str());
    nvs_set_str(nvs_handle, "sttapikey", sttapikey.c_str());
    nvs_close(nvs_handle);
  }
  neopixelWrite(LED_PIN, 0, BRIGHTNESS, 0);  // LED:Green
  avatar.setExpression(Expression::Happy);
  avatar.setSpeechText("APIキーが、セットされました");
  server.send(200, "text/plain", String("OK"));
  delay(3000);
  avatar.setExpression(Expression::Neutral);
  avatar.setSpeechText("");
  neopixelWrite(LED_PIN, 0, 0, 0);          // LED:Off / black
}

void handle_character_voice() {
  /// ファイルを読み込み、クライアントに送信する
  server.send(200, "text/html", CHARACTER_VOICE_HTML);
}
void handle_character_voice_set() {
  /// POST以外は拒否
  if (server.method() != HTTP_POST) {
    return;
  }

  /// language
  String _LANG_CODE = server.arg("character").substring(0,2);
  Serial.print("_LANG_CODE:");  Serial.println(_LANG_CODE);
  if (_LANG_CODE == "zh") {
    LANG_CODE = "zh-CN";
  } else if (_LANG_CODE == "en") {
    LANG_CODE = "en-US";
  } else {
    LANG_CODE = "ja-jp";
  }
  Serial.print("LANG_CODE:");  Serial.println(LANG_CODE);
  /// character
  String character = server.arg("character").substring(3,5); 
  CHARACTER = character;
  Serial.print("CHARACTER:"); Serial.println(character);

  uint32_t nvs_handle;
  if (ESP_OK == nvs_open("setting", NVS_READWRITE, &nvs_handle)) {
    nvs_set_str(nvs_handle, "character", character.c_str());
    nvs_close(nvs_handle);
  }
  neopixelWrite(LED_PIN, 0, BRIGHTNESS, 0);  // LED:Green
  avatar.setExpression(Expression::Happy);
  avatar.setSpeechText("キャラクター音声が、変更されました");
  server.send(200, "text/plain", String("OK"));
  delay(3000);
  avatar.setExpression(Expression::Neutral);
  avatar.setSpeechText("");
  neopixelWrite(LED_PIN, 0, 0, 0);          // LED:Off / black
}

void handle_model() {
  /// ファイルを読み込み、クライアントに送信する
  server.send(200, "text/html", MODEL_HTML);
}
void handle_model_set() {
  /// POST以外は拒否
  if (server.method() != HTTP_POST) {
    return;
  }
  /// model_ver
  String model_ver = server.arg("model_ver");
 
  if (model_ver == "2") {
    MODEL_VER = "gpt-4o-mini";
  } else if (model_ver == "3")  {
    MODEL_VER = "gpt-4o";
  } else {
    MODEL_VER = "gpt-3.5-turbo";
  }

  Serial.print("MODEL_VER: "); Serial.println(MODEL_VER);

  uint32_t nvs_handle;
  if (ESP_OK == nvs_open("setting", NVS_READWRITE, &nvs_handle)) {
    nvs_set_str(nvs_handle, "model_ver", model_ver.c_str());
    nvs_close(nvs_handle);
  }
  neopixelWrite(LED_PIN, 0, BRIGHTNESS, 0);  // LED:Green
  avatar.setExpression(Expression::Happy);
  avatar.setSpeechText("モデルが、変更されました");
  server.send(200, "text/plain", String("OK"));
  delay(3000);
  avatar.setExpression(Expression::Neutral);
  avatar.setSpeechText("");
  neopixelWrite(LED_PIN, 0, 0, 0);          // LED:Off / black
}

void handle_toio_action() {
  /// ファイルを読み込み、クライアントに送信する
  server.send(200, "text/html", TOIO_HTML);
}
void handle_toio_action_set() {
  /// POST以外は拒否
  if (server.method() != HTTP_POST) {
    return;
  }
  /// toio_action
  String toio_action = server.arg("toio_action");
 
  Toio_ActionID = toio_action;
  Serial.println(toio_action);
  String toio_action_msg = "Toio操作：" + toio_action;

  /// Toio Test
  while (Serial1.available() > 0) {//受信バッファクリア
    char t = Serial1.read();
  }
  Serial.println("Toio-Actin:Start");
    Serial1.println(Toio_ActionID);
    Serial1.flush();
  Serial.println("Toio-Actin:End");

  neopixelWrite(LED_PIN, 0, BRIGHTNESS, 0);  // LED:Green
  avatar.setExpression(Expression::Happy);
  avatar.setSpeechText(toio_action_msg.c_str());
  server.send(200, "text/plain", String("OK"));
  // delay(3000);
  delay(500);
  avatar.setExpression(Expression::Neutral);
  avatar.setSpeechText("");
  neopixelWrite(LED_PIN, 0, 0, 0);          // LED:Off / black
}

bool init_chat_doc(const char *data)
{
  DeserializationError error = deserializeJson(chat_doc, data);
  if (error) {
    Serial.println("DeserializationError");
    return false;
  }
  String json_str; //= JSON.stringify(chat_doc);
  serializeJsonPretty(chat_doc, json_str);  // 文字列をシリアルポートに出力する
//  Serial.println(json_str);
    return true;
}

String https_post_json(const char* url, const char* json_string, const char* root_ca) {
  String payload = "";
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(root_ca);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      https.setTimeout( 65000 ); 
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, url)) {  // HTTPS
        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Authorization", String("Bearer ") + OPENAI_API_KEY);
        int httpCode = https.POST((uint8_t *)json_string, strlen(json_string));
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
            Serial.println("//////////////");
            Serial.println(payload);
            Serial.println("//////////////");
          }
        } else {
          Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return payload;
}

String chatGpt(String json_string) {
  String response = "";
  Serial.print("chatGpt = ");
  Serial.println(json_string);
  avatar.setExpression(Expression::Doubt);
  // avatar.setSpeechText("考え中…");
  avatar.setSpeechText(message_tinking.c_str());

  String ret = https_post_json("https://api.openai.com/v1/chat/completions", json_string.c_str(), root_ca_openai);
  avatar.setExpression(Expression::Neutral);
  avatar.setSpeechText("");
  Serial.println(ret);
  if(ret != ""){
    DynamicJsonDocument doc(2000);
    // StaticJsonDocument<2000> doc;  // Adjust for Atom Echo
    DeserializationError error = deserializeJson(doc, ret.c_str());
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      avatar.setExpression(Expression::Sad);
      avatar.setSpeechText("Error");
      response = "Error";

      // delay(1000);
      delay(700); // Adjust for Atom Echo
      avatar.setSpeechText("");
      avatar.setExpression(Expression::Neutral);
    }else{
      const char* data = doc["choices"][0]["message"]["content"];
      Serial.println(data);
      response = String(data);
      std::replace(response.begin(),response.end(),'\n',' ');
    }
  } else {
    avatar.setExpression(Expression::Sad);
    avatar.setSpeechText(message_dont_understand.c_str());
    response = message_dont_understand.c_str();
    // delay(1000);
    delay(700); // Adjust for Atom Echo
    avatar.setSpeechText("");
    avatar.setExpression(Expression::Neutral);
  }
  return response;
}

String exec_chatGPT(String text) {
  static String response = "";
  Serial.print("InitBuffer1: "); Serial.println(InitBuffer);

  /// model 切替え
  if (MODEL_VER == "gpt-4o") {
    InitBuffer = "{\"model\": \"gpt-4o\",\"messages\": [{\"role\": \"user\", \"content\": \"""\"},\{\"role\": \"system\", \"content\": \"150文字以内で回答する。\"}\]}";
    // InitBuffer = "{\"model\": \" + MODEL_VER + "\,\"messages\": [{\"role\": \"user\", \"content\": \"""\"},\{\"role\": \"system\", \"content\": \"150文字以内で回答する。\"}\]}";
  } else if (MODEL_VER == "gpt-4o-mini") {
    InitBuffer = "{\"model\": \"gpt-4o-mini\",\"messages\": [{\"role\": \"user\", \"content\": \"""\"},\{\"role\": \"system\", \"content\": \"150文字以内で回答する。\"}\]}";
  } else {
    InitBuffer = "{\"model\": \"gpt-3.5-turbo\",\"messages\": [{\"role\": \"user\", \"content\": \"""\"}]}";
  }
  Serial.print("InitBuffer2: "); Serial.println(InitBuffer);

  init_chat_doc(InitBuffer.c_str());
  /// 質問をチャット履歴に追加
  chatHistory.push_back(text);
  /// チャット履歴が最大数を超えた場合、古い質問と回答を削除
  if (chatHistory.size() > MAX_HISTORY * 2)
  {
    chatHistory.pop_front();
    chatHistory.pop_front();
  }

  for (int i = 0; i < chatHistory.size(); i++)
  {
    JsonArray messages = chat_doc["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    if(i % 2 == 0) {
      systemMessage1["role"] = "user";
    } else {
      systemMessage1["role"] = "assistant";
    }
    systemMessage1["content"] = chatHistory[i];
  }

  String json_string;
  serializeJson(chat_doc, json_string);
  response = chatGpt(json_string);
  /// 返答をチャット履歴に追加
  chatHistory.push_back(response);
  // Serial.printf("chatHistory.max_size %d \n",chatHistory.max_size());
  // Serial.printf("chatHistory.size %d \n",chatHistory.size());
  // for (int i = 0; i < chatHistory.size(); i++)
  // {
  //   Serial.print(i);
  //   Serial.println("= "+chatHistory[i]);
  // }
  serializeJsonPretty(chat_doc, json_string);
  Serial.println("====================");
  Serial.println(json_string);
  Serial.println("====================");

  return response;
}

void playMP3(AudioFileSourceBuffer *buff){
  mp3->begin(buff, &out);
}

String SpeechToText(bool isGoogle){
  Serial.println("\r\nRecord start!\r\n");

  String ret = "";
  if( isGoogle) {
    Audio* audio = new Audio();
    audio->Record();  
    Serial.println("Record end\r\n");
    Serial.println("Start - Voice recognition");
    // avatar.setSpeechText("わかりました");  
    neopixelWrite(LED_PIN, BRIGHTNESS, 30, 0);  // LED:Orange
    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(root_ca_google, STT_API_KEY.c_str());
    // ret = cloudSpeechClient->Transcribe(audio);
    ret = cloudSpeechClient->Transcribe(audio, LANG_CODE);
    delete cloudSpeechClient;
    delete audio;
  } else {
    // AudioWhisper* audio = new AudioWhisper();
    AudioWhisper* audio = new AudioWhisper();
    audio->Record();  
    Serial.println("Record end\r\n");
    Serial.println("Start - Voice recognition");
    // avatar.setSpeechText("わかりました"); 
    neopixelWrite(LED_PIN, BRIGHTNESS, 30, 0);  // LED:Orange
    Whisper* cloudSpeechClient = new Whisper(root_ca_openai, OPENAI_API_KEY.c_str());
    // ret = cloudSpeechClient->Transcribe(audio);
    ret = cloudSpeechClient->Transcribe(audio, LANG_CODE.substring(0,2));
    delete cloudSpeechClient;
    delete audio;
  }
  return ret;
}

// void google_tts(char *text, char *lang) {
void google_tts(char *text, String lang) {
  Serial.println("tts Start");
  String link =  "http" + tts.getSpeechUrl(text, lang).substring(5);   
  Serial.println(link);

  HTTPClient http;    // Add for Global language
  WiFiClient client;  // Add for Global language

  http.begin(client, link);
  http.setReuse(true);
  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    http.end();
//    cb.st(STATUS_HTTPFAIL, PSTR("Can't open HTTP request"));
    return ;
  }

  WiFiClient *ttsclient = http.getStreamPtr();
  if (ttsclient->available() > 0) {
    int i = 0;
    int len = sizeof(mp3buff);
    // int len = sizeof(buff);
    int count = 0;
    while (ttsclient->available() > 0) {
      int bytesread = ttsclient->read(&mp3buff[i], len);
//     Serial.printf("%d Bytes Read\n",bytesread);
      i = i + bytesread;
      if(i > sizeof(mp3buff))
      {
        break;
      } else {
        len = len - bytesread;
        if(len <= 0) break;
      }
      delay(100);
    }
    Serial.printf("Total %d Bytes Read\n",i);
    ttsclient->stop();
    http.end();
    file1 = new AudioFileSourcePROGMEM(mp3buff, i);
    mp3->begin(file1, &out);
  }
}

void setlang_messege()
{
  if (LANG_CODE == "zh-CN") {
    M5.Lcd.setFont(&fonts::efontCN_12); // Adjust for SSD1306 (Connect info)
    avatar.setSpeechFont(&fonts::efontCN_12);  // Adjust for SSD1306
    message_help_you = "我可以帮你吗？";
    message_You_said = "知道了:";
    message_tinking = "我在思考";
    message_cant_hear = "我听不到";
    message_dont_understand = "我不明白";

  } else if (LANG_CODE == "en-US") {
    // M5.Lcd.setFont(&fonts::lgfxJapanGothic_12); // Adjust for SSD1306 (Connect info)
    // avatar.setSpeechFont(&fonts::lgfxJapanGothic_16);  // Adjust for SSD1306
    message_help_you = "May I help you?";
    message_You_said = "You said:";
    message_tinking = "Thinking...";
    message_cant_hear = "I can't hear you.";
    message_dont_understand = "I don't understand.";

  } else {  // LANG_CODE = "ja-jp";
    M5.Lcd.setFont(&fonts::lgfxJapanGothic_12); // Adjust for SSD1306 (Connect info)
    avatar.setSpeechFont(&fonts::lgfxJapanGothic_12);  // Adjust for SSD1306
    message_help_you = "御用でしょうか？";
    message_You_said = "認識:";
    message_tinking = "考え中…";
    message_cant_hear = "聞き取れませんでした";
    message_dont_understand = "わかりません";

  }

}

/// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  /// Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

/// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  /// Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

void lipSync(void *args)  // Add for M5 avatar
{
  float gazeX, gazeY;
  int level = 0;
  DriveContext *ctx = (DriveContext *)args;
  Avatar *avatar = ctx->getAvatar();
  for (;;)
  {
    level = abs(*out.getBuffer());
    if(level<100) level = 0;
    if(level > 15000)
    {
      level = 15000;
    }
    float open = (float)level/15000.0;
    avatar->setMouthOpenRatio(open);
    // avatar->getGaze(&gazeY, &gazeX);
    // avatar->setRotation(gazeX * 5);
    delay(50);
  }
}

void Wifi_setup() { // Add for Web Setting (SmartConfig)
  Serial.println("Connect:WiFi"); M5.Display.println("Connect:WiFi");
  
  // WiFi初期化
  WiFi.disconnect(true, true);  // 切断してNVSのWiFi設定もクリア
  delay(500);
  WiFi.mode(WIFI_OFF);
  delay(500);
  WiFi.mode(WIFI_STA);
  delay(500);
  
  // 保存されたWiFi情報があるか確認
  String savedSSID = WiFi.SSID();
  Serial.printf("Saved SSID: [%s]\n", savedSSID.c_str());
  
  if (savedSSID.length() > 0) {
    // 保存されたWiFi情報で接続を試みる
    Serial.println("Trying saved WiFi credentials...");
    WiFi.begin();

    /// 前回接続時情報で接続する
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print("."); M5.Display.print(".");
      delay(500);
      /// 10秒以上接続できなかったら抜ける
      if ( millis() - startTime > 10000 ) {
        break;
      }
    }
    Serial.println(""); M5.Display.println("");
  }
  
  /// 未接続の場合にはSmartConfig待受
  if ( WiFi.status() != WL_CONNECTED ) {
    Serial.println("Starting SmartConfig...");
    Serial.println("Use EspTouch app to configure WiFi");
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.beginSmartConfig();
    Serial.println("Connect:SmartConfig"); M5.Display.println("Connect:SmartConfig");
    
    unsigned long startTime = millis();
    while (!WiFi.smartConfigDone()) {
      delay(500);
      Serial.print("#"); M5.Display.print("#");
      /// 60秒以上接続できなかったら抜ける（時間を延長）
      if ( millis() - startTime > 60000 ) {
        Serial.println("");
        Serial.println("SmartConfig timeout. Reset");
        ESP.restart();
      }
    }
    Serial.println("\nSmartConfig received!");
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    
    /// Wi-fi接続
    Serial.println(""); M5.Display.println("");
    Serial.println("Connect:WiFi"); M5.Display.println("Connect:WiFi");
    startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("."); M5.Display.print(".");
      /// 60秒以上接続できなかったら抜ける
      if ( millis() - startTime > 60000 ) {
        Serial.println("");
        Serial.println("WiFi connection timeout. Reset");
        ESP.restart();
      }
    }
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

/// 会話ロジック
void start_talking() {
  Serial.println("start_talking() called");
  Serial.flush();
  
  M5.Speaker.tone(1000, 100);
  delay(200);
  
  Serial.println("Setting avatar expression...");
  Serial.flush();
  
  avatar.setExpression(Expression::Happy);
  avatar.setSpeechText(message_help_you.c_str());
  neopixelWrite(LED_PIN, 0, 0, BRIGHTNESS);  // LED:Blue
  
  if (M5.Speaker.isEnabled()) {
    Serial.println("Stopping speaker before recording...");
    Serial.flush();
    
    // トーン再生が完全に終わるまで待つ
    while (M5.Speaker.isPlaying()) {
      delay(10);
    }
    
    // スピーカーを停止（タイムアウト付き）
    M5.Speaker.stop();
    delay(50);
    M5.Speaker.end();
    delay(100);  // ES8311の切り替え待ち
  } else {
    Serial.println("Speaker already disabled, skipping stop.");
    Serial.flush();
  }
  
  Serial.println("Speaker stopped, starting STT...");
  Serial.flush();
  
  String ret;

  CHARACTER_VOICE Chr_Vic;
  ///  最新話者IDの取得ページ(ただしハミングは非対応2024/6/14現在) <https://www.voicevox.su-shiki.com/voicevox-id>
  if (CHARACTER == "02") {
    /// [四国めたん]
    Chr_Vic.normal    =  2; // ノーマル
    Chr_Vic.happy     =  0; // あまあま
    Chr_Vic.sasa_yaki = 36; // ささやき
  } else if (CHARACTER == "13") {
    /// [青山龍星]
    Chr_Vic.normal    = 13;
    Chr_Vic.happy     = 83;
    Chr_Vic.sasa_yaki = 86;
  } else if (CHARACTER == "23") {
    /// [WhiteCUL]
    Chr_Vic.normal    = 23;
    Chr_Vic.happy     = 24;
    Chr_Vic.sasa_yaki = 25; // かなしい
  } else {
    /// Default : [ずんだもん]
    Chr_Vic.normal    =  3;
    Chr_Vic.happy     =  1;
    Chr_Vic.sasa_yaki = 22;
  }
  TTS_PARMS = TTS_SPEAKER + String(Chr_Vic.normal) ; // ノーマル
  
  Serial.println("Checking TEXTAREA and API keys...");
  Serial.flush();
  
  /// 音声認識：テキスト入力がある場合は、テキストを優先する
  if ( TEXTAREA == "" ) {
    if(OPENAI_API_KEY == ""){  // Add for Web Setting
      Serial.println("Error: No API-Key Setting");
      ret = "Error: No API-Key Setting";
    } else if(OPENAI_API_KEY != STT_API_KEY){
      Serial.println("Google STT - calling SpeechToText(true)");
      Serial.flush();
      ret = SpeechToText(true);
    } else {
      Serial.println("Whisper STT - calling SpeechToText(false)");
      Serial.flush();
      ret = SpeechToText(false);
    }
  } else {
      ret = TEXTAREA;
  }
  // テキスト入力を初期化
  TEXTAREA = "";
  
  String You_said;
  // You_said = "認識:" +  ret;
  You_said = message_You_said +  ret; 

  avatar.setSpeechText(You_said.c_str()); // 認識した文字を表示
  delay(2500);
  // Serial.println("音声認識終了"); Serial.println("音声認識結果");
  M5.Speaker.begin();
  if(ret != "") {
    neopixelWrite(LED_PIN, 0, BRIGHTNESS, 0);  // LED:Green
    Serial.println(ret);
    if (!mp3->isRunning()) {
      String response = ""; // Add for Web Setting
      if(OPENAI_API_KEY == ""){
        response = "初めに、URL：" + WiFi.localIP().toString() + "をブラウザに入力し、APIキーを設定してください";
      } else {
        response = exec_chatGPT(ret);
      }

      // if(response == "エラーです") {
      if(response == "Error") {
        neopixelWrite(LED_PIN, BRIGHTNESS, 0, 0);  // LED:Red
        // avatar.setSpeechText("エラーです");        // Add for M5 avatar
        avatar.setSpeechText("Error");        // Add for M5 avatar
        avatar.setExpression(Expression::Sad);
        TTS_PARMS = TTS_SPEAKER + String(Chr_Vic.sasa_yaki) ; // ささやき
        if (LANG_CODE == "ja-jp") {
          Voicevox_tts((char*)response.c_str(), (char*)TTS_PARMS.c_str());
        } else {
          google_tts((char*)response.c_str(), LANG_CODE);
        }        
      } else if (response != "") {
        neopixelWrite(LED_PIN, 0, 0, BRIGHTNESS);  // LED:Blue
        avatar.setSpeechText(response.c_str());    // Add for M5 avatar
        avatar.setExpression(Expression::Happy);
        // Voicevox_tts((char*)response.c_str(), (char*)TTS_PARMS.c_str());
        // google_tts((char*)response.c_str(), LANG_CODE);
        if (LANG_CODE == "ja-jp") {
          Voicevox_tts((char*)response.c_str(), (char*)TTS_PARMS.c_str());
        } else {
          google_tts((char*)response.c_str(), LANG_CODE);
        }
      }
    }
  } else {
    Serial.println("音声認識失敗");
    avatar.setExpression(Expression::Sad);
    avatar.setSpeechText(message_cant_hear.c_str());  // 聞き取れませんでした
    String response = message_cant_hear.c_str();
    TTS_PARMS = TTS_SPEAKER + String(Chr_Vic.sasa_yaki) ; // ささやき
    if (LANG_CODE == "ja-jp") {
      Voicevox_tts((char*)response.c_str(), (char*)TTS_PARMS.c_str());
    } else {
      google_tts((char*)response.c_str(), LANG_CODE);
    }
    neopixelWrite(LED_PIN, BRIGHTNESS, 0, 0); // LED:Red
    delay(2000);
    neopixelWrite(LED_PIN, 0, 0, 0);          // LED:Off / black
    avatar.setSpeechText("");
    avatar.setExpression(Expression::Neutral);
  } 
  TTS_PARMS = TTS_SPEAKER + String(Chr_Vic.normal) ; // ノーマル
}

void handle_text_chat() {
  /// ファイルを読み込み、クライアントに送信する
  server.send(200, "text/html", TEXT_CHAT_HTML);
}
void handle_text_chat_set() {
  /// POST以外は拒否
  if (server.method() != HTTP_POST) {
    return;
  }
  /// TEXTAREA
  TEXTAREA = server.arg("plain");
  Serial.print("TEXTAREA: "); Serial.println(TEXTAREA);

  /// おしゃべり開始
  start_talking();
}


void setup()
{
  // ESP32-S3 USB CDC用に少し待機
  delay(1000);
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== AI StackChan Minimal ===");
#ifdef ATOMS3R
  Serial.println("Board: ATOMS3R");
#else
  Serial.println("Board: ATOM Echo");
#endif
#if USE_ATOMIC_ECHO_BASE
  Serial.println("Audio: Atomic Echo Base (ES8311)");
#else
  Serial.println("Audio: Internal");
#endif

  auto mem0 = esp_get_free_heap_size(); // check memory size
  Serial.printf("Free heap: %u bytes\n", mem0);

  auto cfg = M5.config();

#if USE_ATOMIC_ECHO_BASE
  // ES8311を初期化（espressif/es8311ライブラリ使用）
  if (init_es8311()) {
    Serial.println("ES8311 codec initialized");
  } else {
    Serial.println("ERROR: ES8311 codec initialization failed!");
  }
#endif

#if !USE_INTERNAL_DISPLAY
  // 外部ディスプレイ（M5UnitGLASS2）を使用する場合
  cfg.unit_glass2.pin_sda= I2C_SDA_PIN; // Add for SSD1306
  cfg.unit_glass2.pin_scl= I2C_SCL_PIN; // Add for SSD1306
#endif

#if USE_ATOMIC_ECHO_BASE
  // Atomic Echo Base (ES8311) 用の設定
  cfg.internal_imu = false;  // 内部IMUを無効化
  cfg.internal_rtc = false;  // 内部RTCを無効化
  cfg.internal_mic = false;  // 内蔵マイクを無効化
  cfg.internal_spk = false;  // 内蔵スピーカーを無効化
  
  // Atomic Echo Base (ES8311) を有効化
  cfg.external_speaker.atomic_spk = true;
  cfg.external_speaker.atomic_echo = true;
#endif

  auto mem1 = esp_get_free_heap_size(); // check memory size

  Serial.println("Calling M5.begin()...");
  M5.begin(cfg);
  Serial.println("M5.begin() done.");
  Serial.flush();  // 確実に出力を送信
  delay(100);
  
  // M5Unifiedが認識したボード情報を表示
  Serial.print("M5 Board: ");
  Serial.println((int)M5.getBoard());
  Serial.flush();
  
#if USE_ATOMIC_ECHO_BASE
  // M5.begin後にスピーカーとマイクの状態を確認
  Serial.print("Speaker enabled: ");
  Serial.println(M5.Speaker.isEnabled());
  Serial.print("Mic enabled: ");
  Serial.println(M5.Mic.isEnabled());
  Serial.flush();
  
  // Speaker/Micが無効な場合、手動で初期化を試みる
  // 注意: ES8311は半二重なので、SpeakerとMicを同時に初期化しない
  // 起動時はSpeakerのみ初期化し、録音時にMicに切り替える
  // ピン配置はM5Stack公式SDKより: https://github.com/m5stack/openai-realtime-embedded-sdk
  if (!M5.Speaker.isEnabled()) {
    Serial.println("Trying to manually initialize speaker...");
    Serial.flush();
    
    // スピーカーを手動で開始
    M5.Speaker.end();
    auto spk_cfg = M5.Speaker.config();
    spk_cfg.sample_rate = 16000;   // ES8311は8KHzをサポートしない
  spk_cfg.pin_bck = ES8311_BCLK_PIN;
  spk_cfg.pin_ws = ES8311_LRCK_PIN;
  spk_cfg.pin_data_out = ES8311_DATA_OUT_PIN;
  spk_cfg.pin_mck = ES8311_MCLK_PIN;
    spk_cfg.i2s_port = I2S_NUM_1;
    M5.Speaker.config(spk_cfg);
    M5.Speaker.begin();
    
    Serial.print("After manual init - Speaker enabled: ");
    Serial.println(M5.Speaker.isEnabled());
    Serial.flush();
    
    // マイクの設定は保存するが、開始はしない（録音時に開始する）
    auto mic_cfg = M5.Mic.config();
    mic_cfg.sample_rate = 16000;   // ES8311は8KHzをサポートしない
  mic_cfg.pin_bck = ES8311_BCLK_PIN;
  mic_cfg.pin_ws = ES8311_LRCK_PIN;
  mic_cfg.pin_data_in = ES8311_DATA_IN_PIN;
  mic_cfg.pin_mck = ES8311_MCLK_PIN;
    mic_cfg.i2s_port = I2S_NUM_1;
    mic_cfg.stereo = false;
    M5.Mic.config(mic_cfg);
    // M5.Mic.begin() は録音時に呼ぶ
    
    Serial.println("Mic configured (will start when recording)");
    Serial.println("Pin config: BCLK=8, LRCK=6, DATA_OUT=5, DATA_IN=7");
    Serial.flush();
  }
#endif

#if !USE_INTERNAL_DISPLAY
  // 外部ディスプレイをプライマリに設定
  M5.setPrimaryDisplayType({m5::board_t::board_M5UnitGLASS2}); // Add for M5 avatar
#endif
  { /// custom setting
    auto spk_cfg = M5.Speaker.config();
    /// Increasing the sample_rate will improve the sound quality instead of increasing the CPU load.
#if USE_ATOMIC_ECHO_BASE
  // CodecとI2Sを同期させるため16kHzに揃える
  spk_cfg.sample_rate = ES8311_SAMPLE_RATE;
#else
    spk_cfg.sample_rate = 96000; // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 , 96000 , 100000 , 128000 , 144000 , 192000 , 200000
#endif
    spk_cfg.task_pinned_core = APP_CPU_NUM;
#if USE_ATOMIC_ECHO_BASE
  spk_cfg.pin_bck = ES8311_BCLK_PIN;
  spk_cfg.pin_ws = ES8311_LRCK_PIN;
  spk_cfg.pin_data_out = ES8311_DATA_OUT_PIN;
  spk_cfg.pin_mck = ES8311_MCLK_PIN;
#endif
    M5.Speaker.config(spk_cfg);
  }
  M5.Speaker.begin();
  /// set master volume (0~255)
#if USE_ATOMIC_ECHO_BASE
  M5.Speaker.setVolume(200);  // Atomic Echo Base は高品質なので音量を上げても問題なし
#else
  M5.Speaker.setVolume(150);  // Adjust for Atom Echo (DO NOT set over 150. This echo Speaker will be broken.)
#endif

#if USE_ATOMIC_ECHO_BASE
  // Atomic Echo Base (ES8311) のマイク設定
  {
    auto mic_cfg = M5.Mic.config();
    mic_cfg.sample_rate = 16000;  // 音声認識用に16kHz
    mic_cfg.stereo = false;       // モノラル
  mic_cfg.pin_bck = ES8311_BCLK_PIN;
  mic_cfg.pin_ws = ES8311_LRCK_PIN;
  mic_cfg.pin_data_in = ES8311_DATA_IN_PIN;
  mic_cfg.pin_mck = ES8311_MCLK_PIN;
  mic_cfg.i2s_port = I2S_NUM_1;
    M5.Mic.config(mic_cfg);
  }
  // マイクが正しく初期化されたか確認
  if (M5.Mic.isEnabled()) {
    Serial.println("Mic: Enabled (ES8311)");
  } else {
    Serial.println("Mic: NOT Enabled - Check Atomic Echo Base connection!");
  }
#endif

  /// シリアル通信機能の設定 via Grove Pin
  Serial1.begin(19200, SERIAL_8N1, GROVE_RX_PIN, GROVE_TX_PIN); // RX,TX - Grove Pin

  neopixelWrite(LED_PIN, 0, BRIGHTNESS, 0);  // Green

  mp3 = new AudioGeneratorMP3();

  /// ESP32本体フラッシュメモリより 保存した値を読み取り
  {
    uint32_t nvs_handle1;
    if (ESP_OK == nvs_open("setting", NVS_READONLY, &nvs_handle1)) {
      nvs_get_u8(nvs_handle1, "character", &character);
      nvs_get_u8(nvs_handle1, "model_ver", &model_ver);
      nvs_close(nvs_handle1);
    } else {
      if (ESP_OK == nvs_open("setting", NVS_READWRITE, &nvs_handle1)) {
        nvs_set_u8(nvs_handle1, "character", character);
        nvs_set_u8(nvs_handle1, "model_ver", model_ver);
        nvs_close(nvs_handle1);
      }
    }
  }

  {
    uint32_t nvs_handle;
    if (ESP_OK == nvs_open("apikey", NVS_READONLY, &nvs_handle)) {
      Serial.println("nvs_open");

      size_t length1;
      size_t length2;
      size_t length3;
      if(ESP_OK == nvs_get_str(nvs_handle, "openai", nullptr, &length1) && 
         ESP_OK == nvs_get_str(nvs_handle, "voicevox", nullptr, &length2) && 
         ESP_OK == nvs_get_str(nvs_handle, "sttapikey", nullptr, &length3) && 
        length1 && length2 && length3) {
        Serial.println("nvs_get_str");
        char openai_apikey[length1 + 1];
        char voicevox_apikey[length2 + 1];
        char stt_apikey[length3 + 1];
        if(ESP_OK == nvs_get_str(nvs_handle, "openai", openai_apikey, &length1) && 
           ESP_OK == nvs_get_str(nvs_handle, "voicevox", voicevox_apikey, &length2) &&
           ESP_OK == nvs_get_str(nvs_handle, "sttapikey", stt_apikey, &length3)) {
          OPENAI_API_KEY = String(openai_apikey);
          VOICEVOX_API_KEY = String(voicevox_apikey);
          STT_API_KEY = String(stt_apikey);
        }
      }
      nvs_close(nvs_handle);
    }
  }

  /// ネットワークに接続
  Wifi_setup(); // Add for Web Setting
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Connect OK-WiFi");

  auto mem2 = esp_get_free_heap_size(); // check memory size

  server.on("/", handleRoot);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  /// And as regular external functions:
  server.on("/apikey", handle_apikey);
  server.on("/apikey_set", HTTP_POST, handle_apikey_set);
  server.on("/character_voice", handle_character_voice);
  server.on("/character_voice_set", HTTP_POST, handle_character_voice_set);
  server.on("/model_ver", handle_model);
  server.on("/model_ver_set", HTTP_POST, handle_model_set);
  server.on("/text_chat", handle_text_chat);
  server.on("/text_chat_set", HTTP_POST, handle_text_chat_set);
  server.on("/toio_action", handle_toio_action);
  server.on("/toio_action_set", HTTP_POST, handle_toio_action_set);
  server.onNotFound(handleNotFound);

  init_chat_doc(json_ChatString.c_str());
  /// SPIFFSをマウントする
  if(SPIFFS.begin(true)){
    /// JSONファイルを開く
    File file = SPIFFS.open("/data.json", "r");
    if(file){
      DeserializationError error = deserializeJson(chat_doc, file);
      if(error){
        Serial.println("Failed to deserialize JSON");
        init_chat_doc(json_ChatString.c_str());
      }
      serializeJson(chat_doc, InitBuffer);
      Role_JSON = InitBuffer;
      String json_str; 
      serializeJsonPretty(chat_doc, json_str);  // 文字列をシリアルポートに出力する
      Serial.println(json_str);
    } else {
      Serial.println("Failed to open file for reading");
      init_chat_doc(json_ChatString.c_str());
    }
  } else {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }  

  server.begin(); // Add for Web Setting
  Serial.println("Setting URL:"); M5.Lcd.println("Setting URL:");   // 設定URL
  M5.Lcd.setTextSize(1.3);            // Adjust for SSD1306 (Connect info)
  Serial.print(WiFi.localIP()); M5.Lcd.print(WiFi.localIP());
  delay(6000);

#if USE_INTERNAL_DISPLAY
  // ATOMS3R 内蔵ディスプレイ用設定 (128x128)
  avatar.setScale(0.5);               // Adjust for ATOMS3R internal display
  avatar.setPosition(-64, -80);       // Adjust for ATOMS3R internal display
#else
  // 外部ディスプレイ（M5UnitGLASS2/SSD1306）用設定
  avatar.setScale(.32);               // Adjust for SSD1306
  avatar.setPosition(-92,-100);       // Adjust for SSD1306
#endif
  avatar.init();                      // Add for M5 avatar
  avatar.addTask(lipSync, "lipSync"); // Add for M5 avatar

  neopixelWrite(LED_PIN, 0, 0, 0);    // LED:Off / black

  setlang_messege();  // Set message based on language

  auto mem3 = esp_get_free_heap_size();
  printf("Heap available : %u : %u : %u : %u\n", mem0, mem1, mem2, mem3);

}


void loop()
{
  M5.update();

  /// 設定表示：URL,モデル
  if (M5.BtnA.wasDoubleClicked())
  {
      neopixelWrite(LED_PIN, 0, 0, BRIGHTNESS);  // LED:Blue
      M5.Speaker.tone(500, 100);
      delay(100);
      M5.Speaker.end();
      delay(100);
      M5.Speaker.begin();
      String response2 = ""; 
      response2 = "Setting URL:" + WiFi.localIP().toString();
      if (MODEL_VER == "gpt-4o") {
      } else {
        MODEL_VER = "gpt-3.5-turbo";
      }
      response2 = response2 + " Model:" + MODEL_VER;
      avatar.setExpression(Expression::Happy);
      avatar.setSpeechText(response2.c_str());
      delay(7000);
      neopixelWrite(LED_PIN, 0, 0, 0);  // LED:Off / black
      avatar.setSpeechText("");
      avatar.setExpression(Expression::Neutral);
  }

  if (M5.BtnA.wasSingleClicked())
  {
    /// 言語別メッセージ切替
    setlang_messege();
    /// おしゃべり開始
    start_talking();    
  }

  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      if(file != nullptr){delete file; file = nullptr;}
      Serial.println("mp3 stop");
      avatar.setExpression(Expression::Neutral);
      speech_text_buffer = "";
      neopixelWrite(LED_PIN, 0, 0, 0);  // LED:Off / black
      avatar.setSpeechText("");
      delay(5);
    }
  } else {
    server.handleClient();  // Add for Web Setting
  }
}