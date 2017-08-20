#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx_conf.h"
#include "servo.h"
#include "MotionManager.h"
#include "Uart.h"
#include "pwm.h"
#include "system.h"
#include "schedule.h"

/*
 *  TODO :
 *    1. Context switch (O)
 *    2. TASK priority and interrupt
 *    3. Mutex/Semaphore/Critical Section
 *    4. IPC: Shared memory/Message Queue/Pipe
 *
 */

extern MotionManager  motionManager;
extern Operator       default_OP;
extern Servo          servo[2];

/*
Servo servo[2] = {
  {
    .op = &default_OP,
    .id = 0,
    .priority = PRIORITY_MID,
    .position = 24,
    .max_position = 180,
    .min_position = -180,
    .status = ENABLE,
    .offset = 0 },
  {
    .op = &default_OP,
    .id = 1,
    .priority = PRIORITY_MID,
    .position = 0,
    .max_position = 180,
    .min_position = -180,
    .status = ENABLE,
    .offset = 0 }
};
*/

int main(){

  initUSART1(115200);
  USART_puts(USART1, "Test Uart Complete! \r\n");
  
  //systemInit();
  //init_PWM_Configuration(2);

  if(schedulerInit()){
    systemInit();
    thread_start();
  } else {
    USART_puts(USART1, "Scheduler initialize fails ... end \n");
  }

  while(1){
    //scheduler();
  };

  return 0;
}

void TIM8_TRG_COM_TIM14_IRQHandler(void){

  if(TIM_GetITStatus(TIM14, TIM_IT_Update) == SET){
    /* Update servo position periodically */
    //motionManager.process(servo, 2);

    /* Clear IT pending Bit */
    TIM_ClearITPendingBit(TIM14, TIM_FLAG_Update);

    //USART_puts(USART1, "Set pendsv ... \r\n");
    //SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
}
