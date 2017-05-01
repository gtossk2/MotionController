#include "Message.h"

#include <string.h>
#include <stdio.h>

Message message = {
  .protocol.parseMessage = _parseMessage 
};

int _parseMessage(unsigned char* data, unsigned int dataSize) {
  int element;
  char *pch;  

  if(!data){
    printf("data is not available.\n");
    return -1;
  }

  pch = strtok(data, ";");
  while(pch != NULL){

    element = sscanf(pch, "#%2[^S]S%4[^;];", message.id, message.position);
    if(element != 2){
      // TODO Error handler
      printf("Wrong Parameter ... \n");
      return -1;
    } else {
      // TODO
      printf("Element : %d \n", element);
      printf("ID : %s , Position : %s \n", message.id, message.position);
    }
    pch = strtok(NULL, ";");
  }

  return 0;
};
