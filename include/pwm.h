#ifndef _PWN_H_
#define _PWM_H_

void init_PWM_Configuration(void);

void PWM_RCC_Configuration(void);
void PWM_GPIO_Configuration(void);
void PWM_TIM_Configuration(void);

void PWM_SCHEDULE_Configuration(void);
void PWM_NVIC_Configuration(void);

#endif
