#include <string.h>

#include "Message.h"
#include "Uart.h"

#define MESSAGE_SIZE  100

extern Message gRequest;

char gMessage[MESSAGE_SIZE];

void initUSART1(uint32_t baudrate){
  
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;

  /* RCC GPIO/USART Configuration */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* GPIO Configuration 
   *
   *  GPIOA_9   --> USART1 TX
   *  GPIOA_10  --> USART1 RX
   */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  //GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART Configuration 
   *
   *  Baudrate = User-define
   *  Word Length = 8 bits    
   *  One stop bit
   *  No Parity
   *  Hardware Flow Control disable (RTS and CTS signals)
   *  Receive and transmit enable
   */
  USART_InitStruct.USART_BaudRate = baudrate;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE);

  /* NVIC Configuration */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStruct);
};

void USART_puts(USART_TypeDef *USARTx, char *s){
  while(*s){
    // Wait until the data register is empty
    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
    
    USART_SendData(USART1, *s);
    s++;
  } 
};

void initUSART2(uint32_t baudrate){
  // TODO
  return;
};

static char* clearMessage(char* message, uint8_t* cnt){
  memset(message, 0, MESSAGE_SIZE);
  *cnt = 0;

  return message;
}

void USART1_IRQHandler(){
  static uint8_t messageCnt = 0;
  static char *messagePtr = gMessage;
  char incomingByte;

  if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
    incomingByte = (uint8_t) USART_ReceiveData(USART1);
    USART_puts(USART1, &incomingByte); // Do echo-ing
  } 
  
  if( incomingByte == '\n' || incomingByte == '\r'){
    USART_puts(USART1, "\r\nRec : ");
    USART_puts(USART1, gMessage);
    USART_puts(USART1, "\r\n");

    // TODO : Message Process Here
    PARSE_MESSAGE(&gRequest, (char *) &gMessage[0], messageCnt);
    messagePtr = clearMessage(gMessage, &messageCnt);
  } else {
    *messagePtr++ = incomingByte;
    messageCnt++;
  };

  // Make sure the message size is enough
  if(messageCnt > MESSAGE_SIZE){
    USART_puts(USART1, "\r\nReceive too many message ... clear previous message\r\n");
    messagePtr = clearMessage(gMessage, &messageCnt);
  }

  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  return;
};

void USART2_IRQHandler(){
  // TODO
  return;
};
