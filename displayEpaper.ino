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
const char HelloWeACtStudio[] = "WeAct Studio";

// --- ボタン制御用の変数 ---
#define BUTTON_PIN_1 8  // 1つ目のボタンを接続するGPIOピン (XIAO D8 / SCK)
#define BUTTON_PIN_2 10 // 2つ目のボタンを接続するGPIOピン (XIAO D10 / MOSI)

long lastButton1PressTime = 0; // 1つ目のボタン用デバウンス変数
long lastButton2PressTime = 0; // 2つ目のボタン用デバウンス変数
long lastSimultaneousPressTime = 0; // 同時押し検出用のタイムスタンプ
const long debounceDelay = 200; // デバウンス時間 (ミリ秒)
const long simultaneousThreshold = 100; // 同時押しと見なす時間差の閾値 (ミリ秒)

int displayMode = 0; // 表示モードを管理する変数 (0: helloWorld, 1: helloFullScreenPartialMode, 2: showPartialUpdate)

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
    display.setCursor(x_was, y_hw + tbh);
    display.print(HelloWeACtStudio);
  } while (display.nextPage());
  Serial.println("Displaying: Hello World!");
}

// --- 描画関数2: フルスクリーン更新モードの表示 ---
void helloFullScreenPartialMode() {
  const char fullscreen[] = "full screen update";
  const char fpm[] = "fast partial mode";
  const char spm[] = "slow partial mode";
  const char npm[] = "no partial mode";

  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

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
  Serial.println("Displaying: Full Screen Partial Mode Info!");
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
  Serial.println("Displaying: Partial Update Demo!");
}

// 描画関数を呼び出すヘルパー関数
void updateDisplayContent() {
  switch (displayMode) {
    case 0:
      helloWorld();
      break;
    case 1:
      helloFullScreenPartialMode();
      break;
    case 2:
      if (display.epd2.hasFastPartialUpdate) {
        showPartialUpdate();
      } else {
        Serial.println("Partial Update not available for this display, cannot switch to mode 2.");
        // 部分更新が利用できない場合は、別のモードに設定し直す
        displayMode = 0; // または1など
        helloWorld(); // スキップした場合は最初の画面を表示
      }
      break;
    case 99: // 同時押し用の特別なモード
      display.setRotation(1);
      display.setFont(&FreeSansBold12pt7b);
      display.setTextColor(GxEPD_BLACK);
      display.setFullWindow();
      display.firstPage();
      do {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(10, display.height() / 2);
        display.print("Both Buttons Pressed!");
      } while (display.nextPage());
      Serial.println("Displaying: Simultaneous Press Action!");
      break;
  }
}

// --- setup() 関数: プログラムの初期設定と一度だけ実行される処理 ---
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-C3 E-Paper Demo with Dual Button Control and Simultaneous Press");
  Serial.println("Starting E-Paper initialization...");

  // ボタンピンをプルアップ入力として設定
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);

  // E-Paperディスプレイの初期化
  display.init(115200, true, 50, false);
  Serial.println("E-Paper initialization complete.");

  // 初期表示モード
  helloWorld(); // まずはHello Worldを表示
}

// --- loop() 関数: setup() 実行後に繰り返し実行される処理 ---
void loop() {
  int button1State = digitalRead(BUTTON_PIN_1);
  int button2State = digitalRead(BUTTON_PIN_2);

  // 同時押し検出ロジック
  if (button1State == LOW && button2State == LOW && millis() - lastSimultaneousPressTime > debounceDelay) {
    // ボタン1とボタン2が両方押されており、かつデバウンス時間以上経過
    // さらに、同時押しの閾値内で両方が押されているか（オプション：より厳密な検出）
    // ここではシンプルに、両方がLOWになった時点で判定
    
    lastSimultaneousPressTime = millis(); // 同時押しが検出された時刻を更新

    // ここで同時押し時の挙動を定義
    if (displayMode != 99) { // 既に同時押しモードでなければ
      displayMode = 99; // 特殊なモードに設定
      Serial.println("Simultaneous press detected!");
      updateDisplayContent();
    }
  } 
  // 単独押し検出ロジック
  else if (button1State == LOW && millis() - lastButton1PressTime > debounceDelay) {
    lastButton1PressTime = millis();
    
    // 同時押し判定の後に単独押し判定を行うことで、同時押しを優先する
    // ただし、もし同時押しモード中にボタン1が単独で離された場合など、
    // 挙動をどうするかは要件次第

    // 同時押しモードから抜け出すための処理をここに入れることもできる
    if (displayMode == 99) { // 同時押しモードから抜ける場合
      displayMode = 0; // あるいは前のモードに戻す
      Serial.println("Exiting simultaneous mode via Button 1.");
      updateDisplayContent();
    } else {
      displayMode++;
      if (displayMode > 2) {
        displayMode = 0;
      }
      Serial.print("Button 1 pressed! Changing display mode to: ");
      Serial.println(displayMode);

      updateDisplayContent(); // 表示を更新する関数を呼び出す
    }
  } 
  else if (button2State == LOW && millis() - lastButton2PressTime > debounceDelay) {
    lastButton2PressTime = millis();

    // 同時押し判定の後に単独押し判定を行う
    if (displayMode == 99) { // 同時押しモードから抜ける場合
      displayMode = 0; // あるいは前のモードに戻す
      Serial.println("Exiting simultaneous mode via Button 2.");
      updateDisplayContent();
    } else {
      // 例: ボタン2が押されたら、常にPartialUpdateデモに切り替える
      // もしくは、特定のモードに戻る、など自由に設定できます。
      if (displayMode != 2 && display.epd2.hasFastPartialUpdate) {
        displayMode = 2; // Partial Updateモードへ直接ジャンプ
        Serial.println("Button 2 pressed! Jumping to Partial Update Demo.");
      updateDisplayContent(); // 表示を更新する関数を呼び出す
      } else {
      // もしすでにPartial Updateモードなら、最初のモードに戻すなど
      displayMode = 0; // 例として最初のモードに戻す
        Serial.println("Button 2 pressed! Already in Partial Update or not available, returning to Hello World.");
        updateDisplayContent();
      }
    }
  }
}