#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx_conf.h"
#include "servo.h"
#include "MotionManager.h"
#include "Uart.h"
#include "pwm.h"

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

  init_PWM_Configuration();

  initUSART1(115200);
  USART_puts(USART1, "Test Uart Complete! \r\n");

  //RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  //GPIOD->MODER = (1 << 26);
  //SysTick_Config(16000000);
  
  while(1){
  };

  return 0;
}

/*
 *  Move SysTick_Handler Here.
 */
/*
void SysTick_Handler(void) {
  //GPIOD->ODR ^= (1 << 13);
  //motionManager.process(servo, 2);
}
*/

void TIM2_IRQHandler(void){
 
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET){
    /* Update servo position periodically */
    motionManager.process(servo, 2);

    /* Clear IT pending Bit */
    TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
  }
}
