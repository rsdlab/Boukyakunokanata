// カメラ関係
#include <Camera.h>
// sdカード関連
#include "sd_and_file.h"

// dnnrt関係
#include <NetPBM.h>
#include <DNNRT.h>

// カメラのファイル関係
File myFileCamera;

const int cam_width = 32;
const int cam_height = 32;
const int x1 = int((CAM_IMGSIZE_QQVGA_H - cam_width)/2);
const int x2 = int((CAM_IMGSIZE_QQVGA_H - cam_width)/2) + cam_width - 1;
const int y1 = int((CAM_IMGSIZE_QQVGA_V - cam_height)/2);
const int y2 = int((CAM_IMGSIZE_QQVGA_V - cam_height)/2) + cam_height - 1;
const int img_size = cam_width* cam_height * 3; // 内部バッファのサイズ

// NN関連
DNNRT dnnrt;
// NNの入力画像
DNNVariable input(img_size);

// Print error message
void printError(enum CamErr err)
{
  Serial.print("Error: ");
  switch (err)
    {
      case CAM_ERR_NO_DEVICE:
        Serial.println("No Device");
        break;
      case CAM_ERR_ILLEGAL_DEVERR:
        Serial.println("Illegal device error");
        break;
      case CAM_ERR_ALREADY_INITIALIZED:
        Serial.println("Already initialized");
        break;
      case CAM_ERR_NOT_INITIALIZED:
        Serial.println("Not initialized");
        break;
      case CAM_ERR_NOT_STILL_INITIALIZED:
        Serial.println("Still picture not initialized");
        break;
      case CAM_ERR_CANT_CREATE_THREAD:
        Serial.println("Failed to create thread");
        break;
      case CAM_ERR_INVALID_PARAM:
        Serial.println("Invalid parameter");
        break;
      case CAM_ERR_NO_MEMORY:
        Serial.println("No memory");
        break;
      case CAM_ERR_USR_INUSED:
        Serial.println("Buffer already in use");
        break;
      case CAM_ERR_NOT_PERMITTED:
        Serial.println("Operation not permitted");
        break;
      default:
        break;
    }
}

// get camera image
void CamCB(CamImage img)
{
  /* Check the img instance is available or not. */
  if (img.isAvailable())
    {
      // 得られた画像の切り抜きを行う
      CamImage small;
      // CamErr err = img.clipAndResizeImageByHW(small
      //                   , 32, 24
      //                   , 287, 215
      //                   , 64, 48);
      // CamErr err = img.clipAndResizeImageByHW(small
      //                   , 48, 36
      //                   , 48+64-1, 36+48-1
      //                   , 64, 48);
      CamErr err = img.clipAndResizeImageByHW(small
                      , x1, y1
                      , x2, y2
                      , cam_width, cam_height);
      // エラーしたとき
      if (err)
      {
        Serial.println("Error : " + String(err));
        return;
      }

      // RGB画像を入力とする
      small.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
      uint16_t* temp = (uint16_t*)small.getImgBuff();
      temp += cam_width * cam_height;
      float* r = input.data();
      float* g = r + cam_width * cam_height;
      float* b = g + cam_width * cam_height;
      for (int i = 0; i < cam_width * cam_height; ++i) {
        *(r++) = (float)((*temp >> 11) & 0x1F)/ 31.0; // 0x1F = 31
        *(g++) = (float)((*temp >>  5) & 0x3F)/ 63.0; // 0x3F = 64
        *(b++) = (float)((*temp)       & 0x1F)/ 31.0; // 0x1F = 31
        temp -= 1;
      }
    }
  else
    {
      Serial.println("Failed to get video stream image");
    }
}

// dnnrtの初期化
void init_dnnrt()
{
  Serial.println("init dnnrt");
  // run dnn
  File nnbfile = theSD.open("model/model_large_datasets_63kb.nnb");
  if (!nnbfile) {
    Serial.print("nnb not found");
  }
  // 推論を開始
  int ret = dnnrt.begin(nnbfile);
  // 初期化失敗
  if (ret < 0) {
    Serial.println("Runtime initialization failure.");
    if (ret == -16) {
      Serial.print("Please install bootloader!");
      Serial.println(" or consider memory configuration!");
    } 
    else {
      Serial.println(ret);
    }
  }
}

// init camera instance for setup function
void init_camera()
{
  Serial.println("init camera");
  // camera
  CamErr err;
  err = theCamera.begin(
    1, CAM_VIDEO_FPS_5, CAM_IMGSIZE_QQVGA_H, CAM_IMGSIZE_QQVGA_V
  );
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
  // camera stream
  err = theCamera.startStreaming(true, CamCB);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
  // auto balance
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
  // picture format
  err = theCamera.setStillPictureImageFormat(
                                CAM_IMGSIZE_QVGA_H,
                                CAM_IMGSIZE_QVGA_V,
                                CAM_IMAGE_PIX_FMT_JPG);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
}

// カメラからの画像を保存する
void CamSave(char *fname)
{
  // 撮影
  CamImage img = theCamera.takePicture();
  // 画像を取得しているかを確認
  if (img.isAvailable())
    {
      theSD.remove(fname);
      myFileCamera = theSD.open(fname, FILE_WRITE);
      myFileCamera.write(img.getImgBuff(), img.getImgSize());
      myFileCamera.close();
    }
  else
    {
      Serial.println("Failed to take picture");
      theCamera.end();
    }
}

// class分類結果
void Classification(int *label, float *confidence)
{
  // https://qiita.com/azarashin/items/c4e0abb8c299c8a9233d#%E3%81%AF%E3%81%98%E3%82%81%E3%81%AB
  // 入力データ
  dnnrt.inputVariable(input, 0);
  // ネットワークに通す
  dnnrt.forward();
  // 出力
  DNNVariable output = dnnrt.outputVariable(0);
  // 推論結果
  *label = output.maxIndex();
  // 自信度
  *confidence = output[*label];
  // 終了
  // dnnrt.end();
}