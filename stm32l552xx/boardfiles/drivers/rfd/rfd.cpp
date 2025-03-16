#include "rfd.hpp"

RFD::RFD(UART_HandleTypeDef* huart) : huart(huart) {}

RFD::~RFD() {}

void RFD::transmit(const uint8_t* data, uint16_t size) {
    if (huart) {
        HAL_UART_Transmit_IT(huart, const_cast<uint8_t*>(data), size);
    }
}

uint16_t RFD::startReceive(uint8_t* buffer, uint16_t bufferSize) {
    if (huart) {
        if (HAL_UART_Receive_IT(huart, buffer, bufferSize) == HAL_OK) {
            return bufferSize;
        }
    }
}

uint16_t RFD::receive(uint8_t* buffer, uint16_t bufferSize) {
    
    return 0;
}
