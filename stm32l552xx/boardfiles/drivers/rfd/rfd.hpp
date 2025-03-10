#pragma once

#include <stm32l5xx_hal_uart.h>
#include "rfd_iface.hpp"
#include "rfd_defines.hpp"

#ifdef __cplusplus
#include <cstdint>
#endif


class RFD : public IRFD {

public:
    RFD(UART_HandleTypeDef* huart);
    ~RFD();

    void transmit(const uint8_t* data, uint16_t size) override;
    uint16_t receive(uint8_t* buffer, uint16_t bufferSize) override;
private:
    UART_HandleTypeDef* huart;
};