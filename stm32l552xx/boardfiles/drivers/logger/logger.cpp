#include "logger.hpp"
#include <cstring>
#include <cstdio>
#include <string>

#define MAX_LINE_LENGTH 128
#define MAX_VALUE_LENGTH 32
#define MAX_KEY_LENGTH (MAX_LINE_LENGTH-MAX_VALUE_LENGTH)

int Logger::init() {
#if defined(SD_CARD_LOGGING)
    HAL_Delay(1000);

    FRESULT res;

    res = f_mount(&FatFs, "", 1);
    if (res != FR_OK) {
        return res;
    }

    FRESULT exist = FR_OK;
    FILINFO fno;
    int count = 1;

    while (exist == FR_OK) {
        snprintf(file, 100, "log%d.txt", count);
        exist = f_stat(file, &fno);
        count++;
    }

    return res;
#elif defined(SWO_LOGGING)
    return 0;
#endif

}

int Logger::log(char message[100]) {
    char msgToSend[112]; //10 for timestamp, 100 for message, 2 for new line

    uint32_t ts = (uint32_t)(osKernelGetTickCount() * 1.0 / osKernelGetTickFreq());
    int tsStrLen = snprintf(msgToSend, 10, "%lus: ", ts);

#if defined(SD_CARD_LOGGING)
    FRESULT res;
    res = f_open(&fil, file, FA_WRITE | FA_OPEN_APPEND);

    snprintf(msgToSend + tsStrLen, 100, message);
    snprintf(msgToSend + tsStrLen + strlen(message), 3, "\r\n");
    res = (FRESULT)f_puts(msgToSend, &fil);

    res = f_close(&fil);

    return res;
#elif defined(SWO_LOGGING)
    snprintf(msgToSend + tsStrLen, 100, message);
    snprintf(msgToSend + tsStrLen + strlen(message), 3, "\r\n");
    printf("%s", msgToSend);
    return 0;
#endif
}

int Logger::log(char message[][100], int count) {
    char msgToSend[112]; //10 for timestamp, 100 for message, 2 for new line

    uint32_t ts = (uint32_t)(osKernelGetTickCount() * 1.0 / osKernelGetTickFreq());
    int tsStrLen = snprintf(msgToSend, 10, "%lus: ", ts);

#if defined(SD_CARD_LOGGING)
    FRESULT res;
    res = f_open(&fil, file, FA_WRITE | FA_OPEN_APPEND);

    for (int i = 0; i < count; i++) {
      snprintf(msgToSend + tsStrLen, 100, message[i]);
      snprintf(msgToSend + tsStrLen + strlen(message[i]), 3, "\r\n");
      res = (FRESULT)f_puts(msgToSend, &fil);
    }

    res = f_close(&fil);

    return res;
#elif defined(SWO_LOGGING)
    for (int i = 0; i < count; i++) {
      snprintf(msgToSend + tsStrLen, 100, message[i]);
      snprintf(msgToSend + tsStrLen + strlen(message[i]), 3, "\r\n");
      printf("%s", msgToSend);
    }
    return 0;
#endif
}

int Logger::findParam(const char param[100], float &val) {
  char *key, *value;
  char msg[100];
  int length;

  while (f_gets(msg, sizeof(char[100]), &fil)) {
    key = strtok(msg, ",\n");
    value = strtok(NULL, ",\n");

    if (!strcmp(param, key)) break;
  }

  if (strcmp(param, key)) return 2;

  length = (f_eof(&fil) != 0) ? strlen(value) : strlen(value) + 2; //accounting for \n

  f_lseek(&fil, f_tell(&fil) - length);

  val = std::atof(value);

  return 0;
}

int Logger::readParam(const char param[100], float &val) {
  if (f_open(&fil, "csvedit.txt", FA_READ | FA_WRITE) != 0 ) {
      return 1;
  }

  int res = findParam(param, val);

  f_close(&fil);

  if (res != 0) {
    return 2;
  }

  return 0;
}

int Logger::writeParam(const char param[100], float newValue) {
  char strvalue[MAX_LINE_LENGTH];
  if (f_open(&fil, "csvedit.txt", FA_READ | FA_WRITE) != 0 ) {
    return 1;
  }

  float val;
  findParam(param, val);

  snprintf(strvalue, MAX_LINE_LENGTH, "%g", newValue);

  if (strlen(strvalue) > MAX_VALUE_LENGTH) {
    strvalue[MAX_VALUE_LENGTH] = '\0';
  }

  f_puts(strvalue, &fil);

  if (strlen(strvalue) < MAX_VALUE_LENGTH) {
    for (int i = 0; i < 8 - static_cast<int>(strlen(strvalue)); i++) {
      f_puts(" ", &fil);
    }
  }

  f_close(&fil);

  return 0;
}

#if defined(SWO_LOGGING)
extern "C" {
  int __io_putchar(int ch) {
    return ITM_SendChar(ch);
  }
}
#endif