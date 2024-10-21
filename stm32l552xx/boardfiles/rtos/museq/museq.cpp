#include "museq.hpp"

/* --- mutexes --- */
/* define mutexes begin */
osMutexId_t itmMutex;

static const osMutexAttr_t itmMutexAttr = {
  "itmMutex",
  osMutexPrioInherit,
  NULL,
  0U
};
/* define mutexes end */

void initMutexes()
{
  itmMutex = osMutexNew(&itmMutexAttr);
}

/* --- sempahores --- */
/* define semaphores begin */
/* define semaphores end */

void initSemphrs()
{

}

/* --- queues --- */
/* define queues begin */
/* define queues end */

void initQueues()
{

}
