#include "stm32f4xx_conf.h"
#include "pwm.h"

/*
 *  Based on the time2 to time5 from Cortex-M4,
 *  Implemented 16-channels PWM to control servos.
 *  (Each timer for 4 channels)
 *
 *  Servo ID will be matched to the each channel sequentially
 *    id 0 -> time2-channel-1
 *    id 1 -> time2-channel-2
 *    ...
 *    ...
 *    id 3 -> time3-channel-1
 *    id 4 -> time3-channel-2
 *    ...
 *    ...
 *
 *  PWM timer pinout is below
 *    timer2 : PA5/PB3/PB10/PB11
 *    timer3 : PB4/PB5/PC8/PC9
 *    timer4 : PD12/PD13/PD14/PD15
 *    timer5 : PH10/PH11/PH12/PI0         <--- TODO: No pinout for STM32F429
 *  PWM Scheduler is based on time14
 */

/* 
 * Assume system clock is running at 84M Hz and Servo is running at 200Hz (5ms)
 *  1. System clock / (Period * Prescaler) = 200 Hz
 *  2. duty ranges from 0 to 65536
 */
#define TIM_PERIOD        (10000 - 1)
#define TIM_PRESCALER     (42 - 1)
#define TIM_PULSE         (3000 -1)

#define SERVO_12          12
#define SERVO_8           8
#define SERVO_4           4

#define GROUP_CFG_1       1
#define GROUP_CFG_2       2
#define GROUP_CFG_3       3
#define GROUP_CFG_4       4

static void TIM2_Ch1_Setting(int value){ TIM2->CCR1 = value; };
static void TIM2_Ch2_Setting(int value){ TIM2->CCR2 = value; };
static void TIM2_Ch3_Setting(int value){ TIM2->CCR3 = value; };
static void TIM2_Ch4_Setting(int value){ TIM2->CCR4 = value; };

static void TIM3_Ch1_Setting(int value){ TIM3->CCR1 = value; };
static void TIM3_Ch2_Setting(int value){ TIM3->CCR2 = value; };
static void TIM3_Ch3_Setting(int value){ TIM3->CCR3 = value; };
static void TIM3_Ch4_Setting(int value){ TIM3->CCR4 = value; };

static void TIM4_Ch1_Setting(int value){ TIM4->CCR1 = value; };
static void TIM4_Ch2_Setting(int value){ TIM4->CCR2 = value; };
static void TIM4_Ch3_Setting(int value){ TIM4->CCR3 = value; };
static void TIM4_Ch4_Setting(int value){ TIM4->CCR4 = value; };

static void TIM5_Ch1_Setting(int value){ TIM5->CCR1 = value; };
static void TIM5_Ch2_Setting(int value){ TIM5->CCR2 = value; };
static void TIM5_Ch3_Setting(int value){ TIM5->CCR3 = value; };
static void TIM5_Ch4_Setting(int value){ TIM5->CCR4 = value; };


static void PWM_RCC_TIM2_Configuration(void){
  /* GPIOA & GPIOB Clock Enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

  /* TIM2 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
};

static void PWM_RCC_TIM3_Configuration(void){
  /* GPIOB & GPIOC Clock Enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

  /* TIM3 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
};

static void PWM_RCC_TIM4_Configuration(void){
  /* GPIOD Clock Enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* TIM4 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
};

static void PWM_RCC_TIM5_Configuration(void){
  /* GPIOH & GPIOI Clock Enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);

  /* TIM5 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
};


static void PWM_GPIO_TIM2_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Setting GPIOA */
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIO A5 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Setting GPIOB */
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIO B3/B10/B11 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_TIM2);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
};

static void PWM_GPIO_TIM3_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Setting GPIOB */
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIO B4/B5 */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Setting GPIOC */
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIO C8/C9 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM3);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOC, &GPIO_InitStructure);
};

static void PWM_GPIO_TIM4_Configuration(void){
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
};

static void PWM_GPIO_TIM5_Configuration(void){
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Setting GPIOH */
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIO H10/H11/H12 */
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_TIM5);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_TIM5);
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource12, GPIO_AF_TIM5);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOH, &GPIO_InitStructure);

  /* Setting GPIOI */
  GPIO_StructInit(&GPIO_InitStructure);
  /* GPIO I0 */
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_TIM5);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_Init(GPIOI, &GPIO_InitStructure);
};

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

static void PWM_TIM2_Configuration(void){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;

  // Let PWM Frequency equal 200Hz
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_Period = TIM_PERIOD; // 84000000/420000=200Hz 5ms for cycle
  TIM_TimeBaseInitStruct.TIM_Prescaler = TIM_PRESCALER;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

  // Initial duty cycle equals 0%. Value can range from zero to 65535
  // TIM_Pulse = TIM2_CCR1 register (16 bits)
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = TIM_PULSE; //1 = duty 0%,10000 = duty 100%
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
 
  TIM_OC1Init( TIM2, &TIM_OCInitStruct ); // Channel 1
  TIM_OC2Init( TIM2, &TIM_OCInitStruct ); // Channel 2
  TIM_OC3Init( TIM2, &TIM_OCInitStruct ); // Channel 3
  TIM_OC4Init( TIM2, &TIM_OCInitStruct ); // Channel 4
 
  TIM_Cmd(TIM2, ENABLE);
}

static void PWM_TIM3_Configuration(void){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;

  // Let PWM Frequency equal 200Hz
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_Period = TIM_PERIOD; // 84000000/420000=200Hz 5ms for cycle
  TIM_TimeBaseInitStruct.TIM_Prescaler = TIM_PRESCALER;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

  // Initial duty cycle equals 0%. Value can range from zero to 65535
  // TIM_Pulse = TIM3_CCR1 register (16 bits)
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = TIM_PULSE; //1 = duty 0%,10000 = duty 100%
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  
  TIM_OC1Init( TIM3, &TIM_OCInitStruct ); // Channel 1
  TIM_OC2Init( TIM3, &TIM_OCInitStruct ); // Channel 2
  TIM_OC3Init( TIM3, &TIM_OCInitStruct ); // Channel 3
  TIM_OC4Init( TIM3, &TIM_OCInitStruct ); // Channel 4
 
  TIM_Cmd(TIM3, ENABLE);
}

static void PWM_TIM4_Configuration(void){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;

  // Let PWM Frequency equal 200Hz
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_Period = TIM_PERIOD; // 84000000/420000=200Hz 5ms for cycle
  TIM_TimeBaseInitStruct.TIM_Prescaler = TIM_PRESCALER;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

  // Initial duty cycle equals 0%. Value can range from zero to 65535
  // TIM_Pulse = TIM4_CCR1 register (16 bits)
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = TIM_PULSE; //1 = duty 0%,10000 = duty 100%
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  
  TIM_OC1Init( TIM4, &TIM_OCInitStruct ); // Channel 1
  TIM_OC2Init( TIM4, &TIM_OCInitStruct ); // Channel 2
  TIM_OC3Init( TIM4, &TIM_OCInitStruct ); // Channel 3
  TIM_OC4Init( TIM4, &TIM_OCInitStruct ); // Channel 4
 
  TIM_Cmd(TIM4, ENABLE);
}

static void PWM_TIM5_Configuration(void){
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;

  // Let PWM Frequency equal 200Hz
  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_Period = TIM_PERIOD; // 84000000/420000=200Hz 5ms for cycle
  TIM_TimeBaseInitStruct.TIM_Prescaler = TIM_PRESCALER;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);

  // Initial duty cycle equals 0%. Value can range from zero to 65535
  // TIM_Pulse = TIM5_CCR1 register (16 bits)
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_Pulse = TIM_PULSE; //1 = duty 0%,10000 = duty 100%
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  
  TIM_OC1Init( TIM5, &TIM_OCInitStruct ); // Channel 1
  TIM_OC2Init( TIM5, &TIM_OCInitStruct ); // Channel 2
  TIM_OC3Init( TIM5, &TIM_OCInitStruct ); // Channel 3
  TIM_OC4Init( TIM5, &TIM_OCInitStruct ); // Channel 4
 
  TIM_Cmd(TIM5, ENABLE);
}

void PWM_SCHEDULE_Configuration(void) {
  TIM_TimeBaseInitTypeDef TIM_ScheduleStructure;

  TIM_ScheduleStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_ScheduleStructure.TIM_Period = 10000 - 1;
  TIM_ScheduleStructure.TIM_Prescaler = 4200 - 1;
  TIM_ScheduleStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_ScheduleStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM14, &TIM_ScheduleStructure);

  TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM14, ENABLE);
}

void PWM_NVIC_Configuration(void) {
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void init_PWM_Configuration(int servo_number){
  int group = (servo_number > SERVO_12) ? GROUP_CFG_4 :
              (servo_number > SERVO_8)  ? GROUP_CFG_3 :
              (servo_number > SERVO_4)  ? GROUP_CFG_2 : GROUP_CFG_1;

  switch(group){
    case GROUP_CFG_4:
      PWM_RCC_TIM5_Configuration();
      PWM_GPIO_TIM5_Configuration();
      PWM_TIM5_Configuration();
    case GROUP_CFG_3:
      PWM_RCC_TIM4_Configuration();
      PWM_GPIO_TIM4_Configuration();
      PWM_TIM4_Configuration();
    case GROUP_CFG_2:
      PWM_RCC_TIM3_Configuration();
      PWM_GPIO_TIM3_Configuration();
      PWM_TIM3_Configuration();
    case GROUP_CFG_1:
      PWM_RCC_TIM2_Configuration();
      PWM_GPIO_TIM2_Configuration();
      PWM_TIM2_Configuration();
      break;
    default:
      // Error-Handle
      break;
  }

  /* PWM Scheduler Configuration */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
  PWM_NVIC_Configuration();
  PWM_SCHEDULE_Configuration();

  return;
}

PWM_CHANNEL* gChannel[4][4] = {
  {TIM2_Ch1_Setting, TIM2_Ch2_Setting, TIM2_Ch3_Setting, TIM2_Ch4_Setting},
  {TIM3_Ch1_Setting, TIM3_Ch2_Setting, TIM3_Ch3_Setting, TIM3_Ch4_Setting},
  {TIM4_Ch1_Setting, TIM4_Ch2_Setting, TIM4_Ch3_Setting, TIM4_Ch4_Setting},
  {TIM5_Ch1_Setting, TIM5_Ch2_Setting, TIM5_Ch3_Setting, TIM5_Ch4_Setting},
};

