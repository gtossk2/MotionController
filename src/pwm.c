#include "stm32f4xx_conf.h"
#include "pwm.h"

void init_PWM_Configuration(void){

  PWM_RCC_Configuration();
  PWM_GPIO_Configuration();
  PWM_NVIC_Configuration();
  PWM_TIM_Configuration();
  PWM_SCHEDULE_Configuration();

  return;
}

void PWM_RCC_Configuration(void){
  /* GPIOD Clock Enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* TIM4 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* TIM2 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
};

void PWM_GPIO_Configuration(void){
  GPIO_InitTypeDef GPIOD_InitStructure;
  GPIO_StructInit(&GPIOD_InitStructure);
  
  /* GPIOD */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

  GPIOD_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIOD_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIOD_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIOD_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIOD_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_Init(GPIOD, &GPIOD_InitStructure);
}

void PWM_TIM_Configuration(void){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;

  // Let PWM Frequency equal 200Hz
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1; // 42000000/210000=200Hz 5ms for cycle
  TIM_TimeBaseInitStruct.TIM_Prescaler = 42 - 1;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

  // Initial duty cycle equals 0%. Value can range from zero to 65535
  // TIM_Pulse = TIM4_CCR1 register (16 bits)
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = 3000 - 1; //1 = duty 0%,10000 = duty 100%
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  
  TIM_OC1Init( TIM4, &TIM_OCInitStruct ); // Channel 1  LED
  TIM_OC2Init( TIM4, &TIM_OCInitStruct ); // Channel 2  LED
  TIM_OC3Init( TIM4, &TIM_OCInitStruct ); // Channel 3  LED
  TIM_OC4Init( TIM4, &TIM_OCInitStruct ); // Channel 4  LED
 
  TIM_Cmd(TIM4, ENABLE);
}

void PWM_SCHEDULE_Configuration(void) {
  TIM_TimeBaseInitTypeDef TIM_ScheduleStructure;

  TIM_ScheduleStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_ScheduleStructure.TIM_Period = 10000 - 1;
  TIM_ScheduleStructure.TIM_Prescaler = 4200 - 1;
  TIM_ScheduleStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_ScheduleStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_ScheduleStructure);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
}

void PWM_NVIC_Configuration(void) {
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

