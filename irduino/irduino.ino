// 配列サイズ
#define BUFFER_SIZE 32
// 配列宣言
static char fname[BUFFER_SIZE] = "";
// 現在時刻、過去の時刻
static char nowtimes[BUFFER_SIZE] = "";
// GNSSの座標
static char position[BUFFER_SIZE] = "";

// dnnrtの推論結果
static int label = 0;
static float confidence = 0;

// 必要ファイル
#include <stdio.h>
#include <SDHCI.h>

#include "camera_and_dnnrt_function.h"
#include "rtc_function.h"
#include "gnss_function.h"
#include "audio_function.h"

// subcoreを選択していたら終了
#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

// loopで使用
enum
{
  GetTime, 
  GNSSsaving, //GNSSの座標を取得 
  RecordReady,
  Recording, 
  AudioSaving, 
  ImageSaving, 
  FaceClassification, 
  TextSaving, 
};
// 初期モード
static int s_state = GetTime;

// setup
void setup()
{
  // ボーレート
  Serial.begin(115200);
  while (!Serial);
  // 初期化開始
  Serial.println("========== 初期化開始 ===========");
  // initialize RTC
  init_rtc();
  // Initialize SD
  init_sd();
  // Initialize audio
  init_audio();
  // init dnnrt
  init_dnnrt();
  // Initialize camera
  init_camera();
  // Initialize GNSS 
  init_gnss();
  // 初期化終了
  Serial.println("========== 初期化終了 ===========");
}

// loop
void loop()
{
  // SDカードが抜かれたとき
  while (!theSD.begin())
    {
      // wait until SD card is mounted.
      Serial.println("Insert SD card.");
    }

  // error end
  if (ErrEnd)
    {
      printf("Error End\n");
      exit(1);
    }

  // 条件切り替え
  switch (s_state)
    {
      // 現在時刻を取得
      case GetTime:
        Serial.println("====== start ======");
        get_time(nowtimes, BUFFER_SIZE);
        Serial.println(nowtimes);
        s_state = GNSSsaving;
        break;
      // gnss座標、時刻取得
      case GNSSsaving:
        GetGnss(position, BUFFER_SIZE);
        s_state = RecordReady;
        break;
      // 次のレコーダの準備（次のレコードなので現在時刻）
      case RecordReady:
        create_save_path(fname, nowtimes, BUFFER_SIZE, 0);
        recorderMode(fname);
        s_state = Recording;
        break;
      // レコード
      case Recording:
        if (recordStream())
          {
            s_state = AudioSaving;
          }
        break;
      // 音声保存
      case AudioSaving:
        AudioSave();
        theAudio->setReadyMode();
        s_state = ImageSaving;
        break;
      // 画像保存（前のレコードなので前の時刻）
      case ImageSaving:
        create_save_path(fname, nowtimes, BUFFER_SIZE, 1);
        CamSave(fname);
        s_state = FaceClassification;
        break;
      // 画像分類
      case FaceClassification:
        Classification(&label, &confidence);
        Serial.print("(0:ない, 1:ある)：");
        Serial.print(label);
        Serial.print(" 自信度：");
        Serial.print(confidence);
        Serial.println("");
        s_state = TextSaving;
        break;
      // 座標情報、時間などをtxtに保存（前のレコードなので前の時刻）
      case TextSaving:
        write_sd(nowtimes, position, &label);
        s_state = GetTime;
        Serial.println("====== end ======");
        break;
      // default
      default:
        break;
    }
}