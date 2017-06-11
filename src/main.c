#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx_conf.h"
#include "servo.h"
#include "MotionManager.h"
#include "Uart.h"
#include "pwm.h"
#include "system.h"

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
  unsigned int currentTime = 0,
               previousTime = 0;

  systemInit();
  init_PWM_Configuration(2);
  initUSART1(115200);
  USART_puts(USART1, "Test Uart Complete! \r\n");
  
  while(1){
    currentTime = micros();

    if((currentTime - previousTime) > 2000000){
      USART_puts(USART1, "Timesup for 2 seconds\r\n");

      previousTime = currentTime;
    }
  };

  return 0;
}

void TIM8_TRG_COM_TIM14_IRQHandler(void){
  if(TIM_GetITStatus(TIM14, TIM_IT_Update) == SET){
    /* Update servo position periodically */
    motionManager.process(servo, 2);

    /* Clear IT pending Bit */
    TIM_ClearITPendingBit(TIM14, TIM_FLAG_Update);
  }
}
