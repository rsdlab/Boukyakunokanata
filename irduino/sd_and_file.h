#ifndef INCLUDE_SD_AND_FILE
#define INCLUDE_SD_AND_FILE

// ファイル関係
File myFile;
// SDカード
SDClass theSD;

// 情報を書き込む変数
char info[BUFFER_SIZE*2] = "";

// sdカードがあるかを確認
void init_sd()
{
  // Initialize SD
  Serial.println("init SD card");
  while (!theSD.begin())
    {
      // wait until SD card is mounted.
      Serial.println("Insert SD card.");
    }
}

// sdカードに情報を書き込む
void write_sd(const char *times, const char *position, const int *label)
{
  // 保存する情報を書き込む
  snprintf(info, BUFFER_SIZE*2, "%s %s %d", times, position, *label);
  // sdカードpen
  myFile = theSD.open("info.txt", FILE_WRITE);
  /* If the file opened okay, write to it */
  if (myFile) {
    myFile.println(info);
    /* Close the file */
    myFile.close();
  } else {
    /* If the file didn't open, print an error */
    Serial.println("error opening info.txt");
  }
}

#endif