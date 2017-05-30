#ifndef _PWN_H_
#define _PWM_H_

typedef void PWM_CHANNEL(int value);

void init_PWM_Configuration(int servo_number);

void PWM_RCC_Configuration(void);
void PWM_GPIO_Configuration(void);
void PWM_TIM_Configuration(void);

void PWM_SCHEDULE_Configuration(void);
void PWM_NVIC_Configuration(void);

#endif
