#pragma once

#include "rc_defines.hpp"
#include "rc_iface.hpp"
#include "stm32l5xx_hal.h"
#include "error.hpp"

typedef struct {
    int dataOffset;
    uint8_t mask;
    int bitshift;
} DataChunk_t;

class RCReceiver : public IRCReceiver {
    public:
        RCReceiver(UART_HandleTypeDef *uart);

        ZP_ERROR_e getRCData(RCControl &rcData) override;

        void init();
        void startDMA();

        void parse();
       
    private:
        UART_HandleTypeDef *uart_;
        RCControl rcData_;
        uint8_t rawSbus_[SBUS_BYTE_COUNT];

        float sbusToRCControl(uint8_t *buf, int channelMappingIdx);
};
