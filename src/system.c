#include "system.h"
#include "stm32f4xx_conf.h"

static uint32_t usTicks = 0;
static volatile uint32_t sysTickUptime = 0;
static volatile int sysTickPending = 0;

void SysTick_Handler(void){
  // TODO ATOMIC BLOCK
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

  sysTickUptime++;
  //sysTickPending = 0;
  //(void)(SysTick->CTRL);
};

void systemInit(void){
  RCC_ClocksTypeDef clocks;
  uint32_t sysCoreClock = 0;
  uint32_t ticks = 0xFFFFFF;

  RCC_GetClocksFreq(&clocks);
  sysCoreClock = clocks.SYSCLK_Frequency;

  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
  // DBG : This will make -g0 fail ...
  //NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Cortex-M0 System Interrupts */
  SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */

  // Init usTicks
  cycleCounterInit();
};

void delayMicroSeconds(uint32_t us){
  uint32_t now = micros();
  while(micros() - now < us);
};

void delay(uint32_t ms){
  while(ms--){
    delayMicroSeconds(1000);
  }
};

void cycleCounterInit(void){
  RCC_ClocksTypeDef clocks;

  RCC_GetClocksFreq(&clocks);
  usTicks = clocks.SYSCLK_Frequency / 1000000;
};

uint32_t micros(void){
  volatile uint32_t ms, cycle_cnt;

  //TODO : Add micros in interrupt and elevated BASEPRI context
  #if 0
  if((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) || (__get_BASEPRI())){
    return microsISR();
  }
  #endif

  do{
    ms = sysTickUptime;
    cycle_cnt = SysTick->VAL;

    /*
     * If the SysTick time expired during the previous instruction,
     * we need to give it a little time for that interrupt to be deliverable before we can recheck sysTickUptime:
     */
    asm volatile("\tnop\n");
  }while(ms != sysTickUptime);

  return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
};

uint32_t microsISR(void){
  //TODO Implement me
  return 0;
};

uint32_t millis(void){
  return sysTickUptime;
};

