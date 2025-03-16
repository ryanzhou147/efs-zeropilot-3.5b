#pragma once
#include "stm32l552xx.h"

#ifdef __cplusplus
#include <cstdint>
#endif

static constexpr uint32_t DELAY = 100000000; // not used so far
static constexpr uint32_t BUFFER_SIZE = 256;
// static constexpr #define RFD_UART UART4; // TODO fix this global uart, and also fix this in class
