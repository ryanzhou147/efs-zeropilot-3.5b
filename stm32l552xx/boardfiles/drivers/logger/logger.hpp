#pragma once

#include "logger_iface.hpp"
#include "app_fatfs.h"
#include "logger_config.h"

class Logger : public ILogger {
    private:
        FATFS FatFs;
        FIL fil;
        char file[100];

    public:
        Logger() = default;

        /**
         * @brief logs a single message to the SD Card
         * @param message: data to be written
         * @retval DRESULT: Operation result
         */
        int log(char message[100]);

        /**
         * @brief logs multiple messages to the SD card
         * @param messages: data to be written
         * @retval DRESULT: Operation result
         */
        int log(char messages[][100], int count);

        /**
         * @brief Reads a specific value from a csv style text file with of the given key
         * @param valueBuf: location for the read value
         * @param bufSize: size of valueBuf in bytes
         * @param key: the key for the csv key value pair
         * @retval int: 0 for success 1 for not success
         */
        int read(char *valueBuf, size_t bufSize, const char *key);

        /**
         * @brief mounts SD card and selects file to write to, call before starting kernel
         */
        int init();

};

