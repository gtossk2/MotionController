#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>
#include "stm32f4xx_conf.h"

void initUSART1(uint32_t baudrate);
void initUSART2(uint32_t baudrate);

void USART_puts(USART_TypeDef *USARTx, char *s);

/*
 * Define in startup_stm32f4xx.s, so we don't need to define again.
 * Implement USART IRQ handler in .c file.
 *
 * void USART1_IRQHandler(void);
 * void USART2_IRQHandler(void);
 */

#endif
