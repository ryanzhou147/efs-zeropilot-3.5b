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

// make call back logic into own function then just call it here TODO
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (RFD::instance && RFD::instance->getHuart() == huart) { // globally set a uart here? TODO
      uint16_t prevWriteIndex = RFD::instance->getWriteIndex();

      RFD::instance->setWriteIndex(Size % BUFFER_SIZE); // don't need % BUFFER_SIZE here

      // assumes that Size < BUFFER_LENGTH always
      // also ignoring case where writeIndex then overtakes readIndex
      if (RFD::instance->getWriteIndex() < RFD::instance->getPrevWriteIndex()) RFD::instance->setOverlapped(true);
      RFD::instance->setPrevWriteIndex(prevWriteIndex);

      // TODO convvert to cpp
      if (RFD::instance->isOverlapped() && (RFD::instance->getWriteIndex() > RFD::instance->getReadIndex())) {
    	  RFD::instance->setErrorFlag(true);
    	  RFD::instance->setOverlapped(true);
      }

      HAL_UARTEx_ReceiveToIdle_DMA(RFD::instance->getHuart(), RFD::instance->getRxBuffer(), BUFFER_SIZE);
      // __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
  }
}
}
