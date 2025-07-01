#pragma once

#include "logger_iface.hpp"
#include "config_iface.hpp"
#include "app_fatfs.h"
#include "logger_config.h"

class Logger : public ILogger, public IConfig {
    private:
        FATFS FatFs;
        FIL fil;
        char file[100];

        /**
         * @brief Moves read/write pointer to parameter
         * @param param parameter to find
         * @param val buffer to store value at parameter
         * @retval Operation success
         */
        int findParam(const char param[100], float &val);

    public:
        Logger() = default;

        /**
         * @brief logs a single message to the SD Card
         * @param message data to be written
         * @retval DRESULT: Operation result
         */
        int log(const char message[100]);

        /**
         * @brief logs multiple messages to the SD card
         * @param messages data to be written
         * @param count number of messages to write
         * @retval DRESULT: Operation result
         */
        int log(const char messages[][100], int count);

        /**
         * @brief reads parameter from config file
         * @param param parameter to read
         * @param val buffer to store value
         * @retval Operation success
         */
        int readParam(const char param[100], float &val);

        /**
         * @brief writes parameter from config file
         * @param param parameter to write too
         * @param newValue updated value
         * @retval Operation success
         */
        int writeParam(const char param[100], float newValue);

        /**
         * @brief mounts SD card and selects file to write to, call before starting kernel
         */
        int init();

};