#include <Audio.h>
#include "sd_and_file.h"

// 保存時間などの設定
static const uint32_t recoding_sampling_rate = 16000;
static const uint8_t  recoding_cannel_number = 1;
static const uint8_t  recoding_bit_length = 16;
static const uint32_t recoding_time = 10;
static const int32_t recoding_byte_per_second = recoding_sampling_rate *
                                                recoding_cannel_number *
                                                recoding_bit_length / 8;
static const int32_t recoding_size = recoding_byte_per_second * recoding_time;

// オーディオ
AudioClass *theAudio;

// オーディオのファイル関係
File myFileAudio;

// エラー処理
bool ErrEnd = false;
static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      ErrEnd = true;
   }
}

// init audio
void init_audio()
{
  Serial.println("init audio");
  // start audio system
  theAudio = AudioClass::getInstance();
  theAudio->begin(audio_attention_cb);
  // Set output device to speaker
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
}

// record
void recorderMode(char *fname)
{
  // Select input device as analog microphone
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC, 100);
  // init recoder
  theAudio->initRecorder(AS_CODECTYPE_WAV, 
                         "/mnt/sd0/BIN", 
                         recoding_sampling_rate,
                         recoding_bit_length,
                         recoding_cannel_number);
  // Open file for data write on SD card
  myFileAudio = theSD.open(fname, FILE_WRITE);
  // Verify file open
  if (!myFileAudio)
    {
      printf("File open error\n");
      exit(1);
    }
  // audio start recoder
  theAudio->startRecorder();
}

// streaming
bool recordStream()
{
  // error変数
  err_t err;
  // recording end condition
  if (theAudio->getRecordingSize() > recoding_size)
    {
      err = theAudio->readFrames(myFileAudio);
      return true;
    }

  // Read frames to record in file
  err = theAudio->readFrames(myFileAudio);

  // error
  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("File End! =%d\n",err);
      return true;
    }

  return false;
}

// save for wav
void AudioSave()
{
  // stop recorder
  theAudio->stopRecorder();
  // save wav file
  theAudio->closeOutputFile(myFileAudio);
  // file wav close
  myFileAudio.close();
}