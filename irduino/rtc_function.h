#include <RTC.h>

#ifndef INCLUDE_RTC_FILE
#define INCLUDE_RTC_FILE

// init rtc
void init_rtc()
{
  // Initialize RTC at first
  Serial.println("init RTC");
  RTC.begin();
}

// get time
void get_time(char *times, const int buffer_size)
{
  RtcTime rtc = RTC.getTime();

  snprintf(times, buffer_size, "%04d-%02d-%02d-%02d-%02d-%02d", 
        rtc.year(), rtc.month(), rtc.day(),
        rtc.hour(), rtc.minute(), rtc.second());
}

// create time path
void create_save_path(char *fname, char *times, const int buffer_size, int type)
{
  // Display only when the second is updated
  if (type == 0) {
    snprintf(fname, buffer_size, "wav/%s.wav", times);
  }
  else if (type == 1) {
    snprintf(fname, buffer_size, "jpg/%s.JPG", times);
  }
}

// save path
void getsavepath(char *fname, int type)
{
  RtcTime rtc = RTC.getTime();

  // Display only when the second is updated
  if (type == 0) {
    snprintf(fname, 64, "wav/%04d-%02d-%02d-%02d-%02d-%02d.wav", 
        rtc.year(), rtc.month(), rtc.day(),
        rtc.hour(), rtc.minute(), rtc.second());
  }
  else if (type == 1) {
    snprintf(fname, 64, "jpg/%04d-%02d-%02d-%02d-%02d-%02d.JPG", 
        rtc.year(), rtc.month(), rtc.day(),
        rtc.hour(), rtc.minute(), rtc.second());
  }
}

#endif