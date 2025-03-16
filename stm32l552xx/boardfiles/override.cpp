#include "main.h"
#include "museq.hpp"
#include "stm32l552xx.h" // delete eventually TODO

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
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart->Instance == UART4) {  // Ensure callback is for UART4
      writeIndex = Size % BUFFER_SIZE;
      HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rxBuffer, BUFFER_SIZE);  // Restart DMA reception
      __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
  }
}
}
