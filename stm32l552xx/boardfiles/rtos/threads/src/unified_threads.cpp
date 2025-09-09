#include "unified_threads.hpp"

// CAN task handle and attributes
osThreadId_t canTaskHandle;
const osThreadAttr_t canTask_attributes = {
  .name = "canTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh
};

void StartCanTask(void *argument) {


    // Main CAN task loop
    while(1) {
        // Call routine tasks every 10ms
        if (canHandle) {
            canHandle->routineTasks();
        }

        osDelay(500); // 10ms delay = 100Hz update rate
    }
}

void initThreads()
{
  amInitThreads();
  smInitThreads();

  canTaskHandle = osThreadNew(StartCanTask, NULL, &canTask_attributes);
}
