#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>

/*
 *  Forward Declaration
 */
typedef struct _message Message;
typedef struct _protocol Protocol;

/*
 *  Implementation Method
 */
int _parseMessage(char* data, unsigned int dataSize);

// 1. Error detect
// 2. Easy parsse
// 3. Different commands
//
// Max 24 Servo
// Start bit : #
// ID : 00 ~ 24
// Position : 0 ~ 180
// Commnad : S/G
// End bit : ;
//
// #00S123;#01S180;#03S0;
//
// 115200 bits/second (14400 bytes)
// 1 servos = 10 bytes, 24 servos = 240 bytes (MAX)
// 240 / 14400 = 0.01666 = 16ms

// Protocol abstration
struct _protocol {
  int (*parseMessage)(char* data, unsigned int dataSize);
};

struct _message {
  Protocol protocol;

  char start[1];
  char id[3];
  char position[5];
  char end[1];
};

/*
 *  Function APIs
 */
#define PARSE_MESSAGE( obj, data, size)                        \
  do {                                                         \
    ((Message *)(obj))->protocol.parseMessage( data, size );   \
  } while(0)    

extern Message gRequest;
#endif
