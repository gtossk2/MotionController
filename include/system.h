#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>

void systemInit(void);
void delayMicroSeconds(uint32_t us);
void delay(uint32_t ms);
void cycleCounterInit(void);

uint32_t micros(void);
uint32_t microsISR(void);
uint32_t millis(void);

#endif
