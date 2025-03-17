#include "main.h"
#include "museq.hpp"
// #include "stm32l552xx.h" // delete eventually TODO
#include "rfd.hpp"

extern "C"
{
/* overriding _write to redirect puts()/printf() to SWO */
int _write(int file, char *ptr, int len)
{
  if( osMutexAcquire(itmMutex, osWaitForever) == osOK )
  {
    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
      ITM_SendChar(ptr[DataIdx]);
    }
    osMutexRelease(itmMutex);
  }
  return len;
}

/* interrupt callback functions */

// rfd interrupt callback
// FIX CALL BACK TODO
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
//   if (huart->Instance == UART4) {  // Ensure callback is for UART4
//       writeIndex = Size % BUFFER_SIZE;
//       HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rxBuffer, BUFFER_SIZE);  // Restart DMA reception
//       __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
//   }
// }
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (RFD::instance && RFD::instance->getHuart() == huart) { // globally set a uart here? TODO
      uint16_t prevWriteIndex = RFD::instance->getWriteIndex();

      RFD::instance->setWriteIndex(Size % BUFFER_SIZE); // don't need % BUFFER_SIZE here

      // assumes that Size < BUFFER_LENGTH always
      // also ignoring case where writeIndex then overtakes readIndex
      if (RFD::instance->getWriteIndex() < RFD::instance->getPrevWriteIndex()) RFD::instance->setOverlapped(true);
      RFD::instance->setPrevWriteIndex(prevWriteIndex);

      HAL_UARTEx_ReceiveToIdle_DMA(RFD::instance->getHuart(), RFD::instance->getRxBuffer(), BUFFER_SIZE);
      // __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
  }
}
}
