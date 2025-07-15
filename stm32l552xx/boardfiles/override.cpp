#include "cmsis_os2.h"
#include "main.h"
#include "museq.hpp"
#include "rfd.hpp"
#include "drivers.hpp"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void HAL_Delay(uint32_t Delay) {
  if (osKernelGetState() == osKernelRunning) {
    osDelayUntil(osKernelGetTickCount() + timeToTicks(Delay));
  } else {
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    if (wait < HAL_MAX_DELAY) {
      wait += (uint32_t)uwTickFreq;
    }

    while ((HAL_GetTick() - tickstart) < wait) {}
  }
}

#ifdef __cplusplus
}
#endif

/* interrupt callback functions */


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == UART4){
        rcHandle->parse();
        rcHandle->startDMA();
    } else if (RFD::instance && RFD::instance->getHuart() == huart) {
      RFD::instance->receiveCallback(Size);
    }
    // GPS dma callback
    else if (huart->Instance == USART2) {
      gpsHandle->processGPSData();
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  if(huart->Instance == UART4){
    uint32_t error = HAL_UART_GetError(huart);

    if (error & HAL_UART_ERROR_PE) {
      __HAL_UART_CLEAR_PEFLAG(huart);
    }

    if (error & HAL_UART_ERROR_NE){
      __HAL_UART_CLEAR_FEFLAG(huart);
    }

    if (error & HAL_UART_ERROR_FE){
      __HAL_UART_CLEAR_NEFLAG(huart);
    }

    if (error & HAL_UART_ERROR_ORE){
      __HAL_UART_CLEAR_OREFLAG(huart);
    }

    rcHandle->startDMA();
  }
}
