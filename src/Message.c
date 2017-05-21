#include "Message.h"
#include "Uart.h"
#include "servo.h"

#include <string.h>
#include <stdio.h>

Message gRequest = {
  .protocol.parseMessage = _parseMessage 
};

int _parseMessage(char* data, unsigned int dataSize) {
  int element;
  char *pch;

  // Debug
  USART_puts(USART1, data);
  USART_puts(USART1, "\r\n");

  if(!data){
    USART_puts(USART1, "data is not available.\r\n");
    return -1;
  }

  pch = strtok((char *)data, ";");
  while(pch != NULL){

    // DEBUG: Cannot detect +/- for sscanf
    element = sscanf(pch, "#%2[^S]S%4[^;];", gRequest.id, gRequest.position);
    if(element != 2){
      // TODO Error handler
      USART_puts(USART1, "\r\nWrong Parameter ... \r\n");
      return -1;
    } else {
      // TODO
      USART_puts(USART1, gRequest.id);
      USART_puts(USART1, " : ");
      USART_puts(USART1, gRequest.position);
      USART_puts(USART1, "\r\n");

      // Set Servo Position
      updatePositionFromCmd(gRequest.id, gRequest.position);

      USART_puts(USART1, "Parse incoming request successfully \r\n");
    }
    pch = strtok(NULL, ";");
  }

  return 0;
};


