/* include the GNSS library */
#include <GNSS.h>

#define MY_TIMEZONE_IN_SECONDS (9 * 60 * 60) // JST

static SpGnss Gnss;                   /**< SpGnss object */

// gps options
enum ParamSat {
  eSatGps,            /**< GPS                     World wide coverage  */
  eSatGlonass,        /**< GLONASS                 World wide coverage  */
  eSatGpsSbas,        /**< GPS+SBAS                North America        */
  eSatGpsGlonass,     /**< GPS+Glonass             World wide coverage  */
  eSatGpsBeidou,      /**< GPS+BeiDou              World wide coverage  */
  eSatGpsGalileo,     /**< GPS+Galileo             World wide coverage  */
  eSatGpsQz1c,        /**< GPS+QZSS_L1CA           East Asia & Oceania  */
  eSatGpsGlonassQz1c, /**< GPS+Glonass+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsBeidouQz1c,  /**< GPS+BeiDou+QZSS_L1CA    East Asia & Oceania  */
  eSatGpsGalileoQz1c, /**< GPS+Galileo+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsQz1cQz1S,    /**< GPS+QZSS_L1CA+QZSS_L1S  Japan                */
};

// for gps
// Set this parameter depending on your current region.
static enum ParamSat satType =  eSatGpsQz1cQz1S;

// GPS取得中のLed点滅
static void Led_isActive(void)
{
  static int state = 1;
  if (state == 1)
  {
    ledOn(PIN_LED0);
    state = 0;
  }
  else
  {
    ledOff(PIN_LED0);
    state = 1;
  }
}

// GPSが取得されているとき
static void Led_isPosfix(bool state)
{
  if (state)
  {
    ledOn(PIN_LED1);
  }
  else
  {
    ledOff(PIN_LED1);
  }
}

// GPS error led
static void Led_isError(bool state)
{
  if (state)
  {
    ledOn(PIN_LED3);
  }
  else
  {
    ledOff(PIN_LED3);
  }
}

// GNSSから取得した時間を標準時とする
void set_time(SpNavData *pNavData)
{
  SpGnssTime *time = &pNavData->time;
  // Check if the acquired UTC time is accurate
  if (time->year >= 2000) {
    RtcTime now = RTC.getTime();
    // Convert SpGnssTime to RtcTime
    RtcTime gps(time->year, time->month, time->day,
                time->hour, time->minute, time->sec, time->usec * 1000);
#ifdef MY_TIMEZONE_IN_SECONDS
    // Set the time difference
    gps += MY_TIMEZONE_IN_SECONDS;
#endif
    int diff = now - gps;
    if (abs(diff) >= 1) {
      RTC.setTime(gps);
    }
  }
}

#define STRING_BUFFER_SIZE  128       /**< %Buffer size */
/**
 * @brief %Print position information.
 */
static void print_pos(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];

  /* print time */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%04d/%02d/%02d ", pNavData->time.year, pNavData->time.month, pNavData->time.day);
  Serial.print(StringBuffer);

  snprintf(StringBuffer, STRING_BUFFER_SIZE, "%02d:%02d:%02d.%06ld, ", pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec);
  Serial.print(StringBuffer);

  /* print satellites count */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "numSat:%2d, ", pNavData->numSatellites);
  Serial.print(StringBuffer);

  /* print position data */
  if (pNavData->posFixMode == FixInvalid)
  {
    Serial.print("No-Fix, ");
  }
  else
  {
    Serial.print("Fix, ");
  }
  if (pNavData->posDataExist == 0)
  {
    Serial.print("No Position");
  }
  else
  {
    Serial.print("Lat=");
    Serial.print(pNavData->latitude, 6);
    Serial.print(", Lon=");
    Serial.print(pNavData->longitude, 6);
  }

  Serial.println("");
}

// spresense起動時に位置情報を取得するまでloopに行かないようにする
void init_fix_pos()
{
  // 初期化が完了するまで回り続ける
  while (true) {
    // sleepを入れないと動かないことがある(メモリのアクセスが速すぎる？)
    sleep(1);
    // Blink LED.
    Led_isActive();
    /* Check update. */
    if (Gnss.waitUpdate())
    {
      /* Get NaviData. */
      SpNavData NavData;
      Gnss.getNavData(&NavData);

      /* Set posfix LED. */
      bool LedSet = (NavData.posDataExist && (NavData.posFixMode != FixInvalid));
      Led_isPosfix(LedSet);

      /* Print position information. */
      print_pos(&NavData);

      // 自身の場所を取得出来たらwhileループからぬける
      if (NavData.posDataExist == 1) {
        // 絶対時刻変更
        set_time(&NavData);
        break;
      }
    }
  }
}

// GNSSの初期設定
void init_gnss()
{
  Serial.println("init gnss");
  
  // put your setup code here, to run once
  int error_flag = 0;
  int result;

  // Turn on all LED:Setup start.
  ledOn(PIN_LED0);
  ledOn(PIN_LED1);
  ledOn(PIN_LED2);
  ledOn(PIN_LED3);

  /* Set Debug mode to Info */
  Gnss.setDebugMode(PrintInfo);

  /* Activate GNSS device */
  result = Gnss.begin();

  if (result != 0)
  {
    Serial.println("Gnss begin error!!");
    error_flag = 1;
  }
  else
  {
    switch (satType)
    {
    case eSatGps:
      Gnss.select(GPS);
      break;

    case eSatGpsSbas:
      Gnss.select(GPS);
      Gnss.select(SBAS);
      break;

    case eSatGlonass:
      Gnss.select(GLONASS);
      break;

    case eSatGpsGlonass:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      break;

    case eSatGpsBeidou:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      break;

    case eSatGpsGalileo:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      break;

    case eSatGpsQz1c:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsQz1cQz1S:
      Gnss.select(GPS);
      Gnss.select(QZ_L1CA);
      Gnss.select(QZ_L1S);
      break;

    case eSatGpsBeidouQz1c:
      Gnss.select(GPS);
      Gnss.select(BEIDOU);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGalileoQz1c:
      Gnss.select(GPS);
      Gnss.select(GALILEO);
      Gnss.select(QZ_L1CA);
      break;

    case eSatGpsGlonassQz1c:
    default:
      Gnss.select(GPS);
      Gnss.select(GLONASS);
      Gnss.select(QZ_L1CA);
      break;
    }

    /* Start positioning */
    result = Gnss.start(COLD_START);
    if (result != 0)
    {
      Serial.println("Gnss start error!!");
      error_flag = 1;
    }
    else
    {
      Serial.println("Gnss setup OK");
    }
  }

  /* Start 1PSS output to PIN_D02 */
  //Gnss.start1PPS();

  /* Turn off all LED:Setup done. */
  ledOff(PIN_LED0);
  ledOff(PIN_LED1);
  ledOff(PIN_LED2);
  ledOff(PIN_LED3);

  /* Set error LED. */
  if (error_flag == 1)
  {
    Led_isError(true);
    exit(0);
  }

  // sleep入れないとGNSSが動かない
  sleep(3);
  // positionを得る
  init_fix_pos();
  // sleepを入れないと動かないことがある(メモリのアクセスが速すぎる？)
  sleep(3);
}

// gnssから座標を取得する
void get_pos(char *StringBuffer, const int buffer_size, SpNavData *pNavData)
{
  if (pNavData->posDataExist == 0) {
    snprintf(StringBuffer, 
              buffer_size, 
              "NoFix-None-None"
              );
  }
  else {
    if (pNavData->posFixMode == FixInvalid)
    {
      snprintf(StringBuffer, 
                buffer_size, 
                "Fix-%f-%f",
                pNavData->latitude, 
                pNavData->longitude
                );
    }
    else
    {
      snprintf(StringBuffer, 
                buffer_size, 
                "NoFix-%f-%f",
                pNavData->latitude, 
                pNavData->longitude
                );
    }
  }
}

// get gnss
void GetGnss(char *StringBuffer, const int buffer_size)
{
  // Blink LED.
  Led_isActive();
  /* Check update. */
  if (Gnss.waitUpdate())
  {
    /* Get NaviData. */
    SpNavData NavData;
    Gnss.getNavData(&NavData);

    /* Set posfix LED. */
    bool LedSet = (NavData.posDataExist && (NavData.posFixMode != FixInvalid));
    Led_isPosfix(LedSet);

    /* Print position information. */
    get_pos(StringBuffer, buffer_size, &NavData);

    // 絶対時刻変更
    set_time(&NavData);
  }
  else
  {
    /* Not update. */
    Serial.println("data not update");
    snprintf(StringBuffer, buffer_size, "NoFix-None-None");
  }
}