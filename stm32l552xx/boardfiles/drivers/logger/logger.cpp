#include "logger.hpp"
#include <cstring>
#include <cstdio>

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

#if defined(SWO_LOGGING)
extern "C" {
  int __io_putchar(int ch) {
    return ITM_SendChar(ch);
  }
}
#endif

int Logger::read(char *valueBuf, size_t bufSize, const char *key) {
#if defined(MAX_LINE_LENGTH)
	char line[MAX_LINE_LENGTH];
	  FRESULT res;
	  res = f_open(&fil, "csvfile.txt", FA_READ); //name subject to change
	  if (res) {
	    printf("Could not open file: %s", file);
	    return 1;
	  }
	  while (f_gets(line, sizeof(line), &fil)) {
	    char *readKey = strtok(line, ",\r\n");
	    char *readValue = strtok(NULL, ",\r\n");

	    if (strcmp(readKey, key) == 0) {
	      strncpy(valueBuf, readValue, bufSize - 1);
	      valueBuf[bufSize - 1] = '\0';
	      f_close(&fil);
	      return 0; // Key found and value put in buffer
	    }
	  }

	  f_close(&fil);
	  return 1; // Key not found
#elif
	  return 1;
#endif
}
