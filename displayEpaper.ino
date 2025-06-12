#include <GxEPD2_BW.h>       // モノクロディスプレイ用ライブラリ
#include <Adafruit_GFX.h>    // グラフィックス描画用ライブラリ
#include <Fonts/FreeMonoBold9pt7b.h> // 使用するフォント1 (小さいサイズ)
#include <Fonts/FreeSansBold12pt7b.h> // 使用するフォント2 (中くらいのサイズ)

// --- E-Paperディスプレイのピン定義 ---
#define EPD_CS    7  // Chip Select (SS)      -> XIAO D5 / SCL (GPIO7) に接続
#define EPD_DC    5  // Data/Command          -> XIAO D3 / A3 (GPIO5) に接続
#define EPD_RST   2  // Reset                 -> XIAO D0 / A0 (GPIO2) に接続
#define EPD_BUSY  3  // Busy signal from E-Paper -> XIAO D1 / A1 (GPIO3) に接続

// ディスプレイオブジェクトの初期化
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(
  GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

// デモ表示用の定数文字列
const char HelloWorld[] = "Hello World!";
const char HelloWeACtStudio[] = "WeAct Studio"; // 元のデモにあった文字列

// --- 描画関数1: 基本的な「Hello World!」表示 ---
void helloWorld() {
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x_hw = ((display.width() - tbw) / 2) - tbx;
  uint16_t y_hw = ((display.height() - tbh) / 2) - tby;

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x_hw, y_hw - tbh);
    display.print(HelloWorld);

    display.getTextBounds(HelloWeACtStudio, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x_was = ((display.width() - tbw) / 2) - tbx;
    display.setCursor(x_was, y_hw + tbh); // "WeAct Studio" のカーソル位置を設定
    display.print(HelloWeACtStudio);      // "WeAct Studio" を描画
  } while (display.nextPage()); // 画面更新が完了するまで繰り返す
}

// --- 描画関数2: フルスクリーン更新モードの表示 ---
void helloFullScreenPartialMode() {
  const char fullscreen[] = "full screen update";
  const char fpm[] = "fast partial mode";
  const char spm[] = "slow partial mode";
  const char npm[] = "no partial mode";

  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(1); // 画面の向きを設定
  display.setFont(&FreeMonoBold9pt7b); // フォントを設定
  display.setTextColor(GxEPD_BLACK);    // テキスト色を黒に設定

  // ディスプレイの更新モードに関するメッセージを選択
  const char* updatemode;
  if (display.epd2.hasFastPartialUpdate) {
    updatemode = fpm;
  } else if (display.epd2.hasPartialUpdate) {
    updatemode = spm;
  } else {
    updatemode = npm;
  }

  int16_t tbx, tby; uint16_t tbw, tbh;

  display.getTextBounds(fullscreen, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t utx = ((display.width() - tbw) / 2) - tbx;
  uint16_t uty = ((display.height() / 4) - tbh / 2) - tby;

  display.getTextBounds(updatemode, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t umx = ((display.width() - tbw) / 2) - tbx;
  uint16_t umy = ((display.height() * 3 / 4) - tbh / 2) - tby;

  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t hwx = ((display.width() - tbw) / 2) - tbx;
  uint16_t hwy = ((display.height() - tbh) / 2) - tby;

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(hwx, hwy);
    display.print(HelloWorld);
    display.setCursor(utx, uty);
    display.print(fullscreen);
    display.setCursor(umx, umy);
    display.print(updatemode);
  } while (display.nextPage());
}

// --- 描画関数3: 部分更新のデモ ---
void showPartialUpdate() {
  helloWorld(); // 背景として「Hello World!」を表示

  uint16_t box_x = 10;
  uint16_t box_y = 15;
  uint16_t box_w = 150;
  uint16_t box_h = 30;
  uint16_t cursor_y = box_y + box_h - 6;

  float value = 13.95;
  uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;

  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  // 更新ボックスの位置を各回転で表示（黒い四角で場所を示す）
  for (uint16_t r = 0; r < 4; r++) {
    display.setRotation(r);
    display.setPartialWindow(box_x, box_y, box_w, box_h);
    display.firstPage();
    do {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
    } while (display.nextPage());
    delay(1000);
    display.firstPage();
    do {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    } while (display.nextPage());
    delay(500);
  }

  // 更新ボックス内の数値を動的に変更し、部分更新をデモ
  for (uint16_t r = 0; r < 4; r++) {
    display.setRotation(r);
    display.setPartialWindow(box_x, box_y, box_w, box_h);
    for (uint16_t i = 1; i <= 10; i += incr) {
      display.firstPage();
      do {
        display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
        display.setCursor(box_x, cursor_y);
        display.print(value * i, 2);
      } while (display.nextPage());
      delay(500);
    }
    delay(1000);
    display.firstPage();
    do {
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
    } while (display.nextPage());
    delay(500);
  }
}

// --- setup() 関数: プログラムの初期設定と一度だけ実行される処理 ---
void setup() {
  Serial.begin(115200); // シリアル通信を開始 (デバッグ出力用)
  Serial.println("ESP32-C3 4.2inch E-Paper Demo (SSD1683) - Final Code for XIAO ESP32C3");
  Serial.println("Starting E-Paper initialization...");

  // 元のデモコードにあったGPIO8の初期化。これはE-Paper制御とは直接関係ないかもしれません。
  // 必要に応じてこの行を削除または変更してください。
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH); // GPIO8をHIGHに設定（例: LEDを点灯）

  // E-Paperディスプレイの初期化
  // 引数: シリアル速度、リセット後に初期化するか、リセットピンのホールド時間、手動リセットを有効にするか
  display.init(115200, true, 50, false);
  Serial.println("E-Paper initialization complete.");

  // 各描画デモ関数を順番に呼び出す
  Serial.println("Running helloWorld() demo...");
  helloWorld();
  delay(3000); // 3秒間表示

  Serial.println("Running helloFullScreenPartialMode() demo...");
  helloFullScreenPartialMode();
  delay(3000); // 3秒間表示

  // ディスプレイが高速部分更新に対応している場合のみ、部分更新デモを実行
  if (display.epd2.hasFastPartialUpdate) {
    Serial.println("Running showPartialUpdate() demo (Fast Partial Update available)...");
    showPartialUpdate();
    delay(3000); // 3秒間表示
  } else {
    Serial.println("Partial Update is not available or is slow. Skipping showPartialUpdate() demo.");
  }
  
  // E-Paperディスプレイを低電力モード（休止状態）へ移行
  // これにより、表示内容は保持されたまま消費電力が抑えられます。
  display.hibernate();
  Serial.println("Display is in hibernate mode. Program finished demonstration.");
}

// --- loop() 関数: setup() 実行後に繰り返し実行される処理 ---
void loop() {
  // E-Paperは一度コンテンツを表示するとその状態を保持するため、
  // 通常はloop()で頻繁に更新する必要はありません。
  // ここでは、元のデモコードにあったGPIO8のトグル処理のみ残します。
  // E-Paperの表示とは直接関係ありません。
  digitalWrite(8, HIGH); // GPIO8をHIGH (ON)
  delay(1000);           // 1秒待機
  digitalWrite(8, LOW);  // GPIO8をLOW (OFF)
  delay(1000);           // 1秒待機
}