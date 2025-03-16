#include "rfd.hpp"

RFD::RFD(UART_HandleTypeDef* huart) : huart(huart) {}

RFD::~RFD() {}

void RFD::transmit(const uint8_t* data, uint16_t size) {
    if (huart) {
        HAL_UART_Transmit_DMA(huart, const_cast<uint8_t*>(data), size);
    }
}

uint16_t RFD::startReceive(uint8_t* buffer, uint16_t bufferSize) {
    if (huart) {
        HAL_UARTEx_ReceiveToIdle_DMA(huart, rxBuffer, BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT); // error here with DMA_IT_HT TODO
    }
}

uint16_t RFD::receive(uint8_t* buffer, uint16_t bufferSize) {
    
    return 0;
}
