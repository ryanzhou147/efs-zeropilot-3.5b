#include "rfd.hpp"

RFD* RFD::instance = nullptr; // global instance

RFD::RFD(UART_HandleTypeDef* huart) : huart(huart) {
    instance = this;
}

RFD::~RFD() {
    instance = nullptr;
}

void RFD::transmit(const uint8_t* data, uint16_t size) {
    if (huart) {
        HAL_UART_Transmit_DMA(huart, const_cast<uint8_t*>(data), size);
    }
}

uint16_t RFD::startReceive(uint8_t* buffer, uint16_t bufferSize) {
    if (huart) {
        HAL_UARTEx_ReceiveToIdle_DMA(huart, rxBuffer, BUFFER_SIZE);
        // __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT); // error here with DMA_IT_HT TODO
    }
}

uint16_t RFD::receive(uint8_t* buffer, uint16_t bufferSize) {
    // TODO
    return 0;
}

// Getter for writeIndex
uint16_t RFD::getWriteIndex() const {
    return writeIndex;
}

// Setter for writeIndex
void RFD::setWriteIndex(uint16_t index) {
    writeIndex = index;
}

// Getter for rxBuffer
uint16_t* RFD::getRxBuffer() {
    return rxBuffer;
}

// Getter for huart
UART_HandleTypeDef* RFD::getHuart() const {
    return huart;
}