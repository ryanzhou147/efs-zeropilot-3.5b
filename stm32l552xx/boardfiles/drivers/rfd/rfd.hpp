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

    void startReceive(uint8_t* buffer, uint16_t bufferSize) override;
    void transmit(const uint8_t* data, uint16_t size) override;
    uint16_t receive(uint8_t* buffer, uint16_t bufferSize) override;

    static RFD* instance; // assumes only one instance defined at a time

    // Getter and Setters
    uint16_t RFD::getReadIndex() const;

    uint16_t getWriteIndex() const;
    void setWriteIndex(uint16_t index);

    uint8_t* getRxBuffer();

    UART_HandleTypeDef* getHuart() const;
    
    bool isOverlapped() const;
    void setOverlapped(bool value);

    uint16_t getPrevWriteIndex() const;
    void setPrevWriteIndex(uint16_t index);

    void setErrorFlag(bool flag);
private:
    UART_HandleTypeDef* huart;
    uint8_t rxBuffer[BUFFER_SIZE];
    uint16_t writeIndex = 0;
    uint16_t readIndex = 0;
    bool overlapped = false; // if writeIndex overlaps buffer_size and readindex hasn't
    uint16_t prevWriteIndex = 0;
    bool errorFlag = false;
};
