#ifndef _SERVO_H_
#define _SERVO_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define PWM_PEROID              10000
#define MAX_SERVO_DEGREE        180
#define MIN_SERVO_DEGREE        0
#define SERVO_CENTER_US         1500
#define SERVO_MAX_US            2100
#define SERVO_MIN_US            900
#define SERVO_US_TO_PERIOD      2       // 5000us = 10000 (period), 1us = 2(period)

#define MIN_PULSE_WIDTH         544
#define MAX_PULSE_WIDTH         2400
#define CENTER_PULSE_WIDTH      1500

/*
 * Enum macro
 */
typedef enum {
  false, 
  true
} bool;

/*
enum {
  DISABLE,
  ENABLE
};
*/

enum {
  PRIORITY_LOW    = 1,
  PRIORITY_MID    = 2,
  PRIORITY_HIGH   = 3,
};

/*
 *  Forward declaration
 */
typedef struct _header      Header;
typedef struct _operator    Operator;
typedef struct _servo       Servo;

/*
 *  Function APIs
 */

#define SET_SERVO_POSITION( obj, pos )                          \
  do {                                                          \
    ((Servo *)(obj))->op->setPosition((Servo *)(obj), pos);     \
  } while(0)

#define GET_SERVO_POSITION( obj, pos )                          \
  do {                                                          \
    ((Servo *)(obj))->op->getPosition((Servo *)(obj), pos);  \
  } while(0)


/*
 *  Abstraction Interface
 */
struct _operator {
  void  (*setPosition)(Servo *servo, int position);
  void  (*getPosition)(Servo *servo, int *position);
  void  (*validatePosition)(Servo *servo, int position, bool *valid);

  void  (*setMaxMinPosition)(Servo *servo, int max_pos, int min_pos);
  void  (*getMaxMinPosition)(Servo *servo, int *max_pos, int *min_pos);

  void  (*getPriority)(Servo *servo, int *priority);
  void  (*setPriority)(Servo *servo, int priority);

  void  (*getOffset)(Servo *servo, int *offset);
  void  (*setOffset)(Servo *servo, int offset);
  
  void  (*getStatus)(Servo *servo, int id, bool *staus);
  void  (*setStatus)(Servo *servo, int id, bool status);

  void  (*setID)(Servo *servo, int id);
};

struct _servo {
  Operator    *op;
  
  uint8_t     id;
  uint8_t     priority;
  int         position;
  int         max_position;
  int         min_position;
  bool        status;
  int         offset;
};

/*
 * Misc Function
 */
int map(int pos, int servo_min, int servo_max);
int parseCmd(char *cmd);
void updatePositionFromCmd(char *cmd);

/*
 * Implementation Method
 */
void setPosition(Servo *servo, int position);
void getPosition(Servo *servo, int *position);
void validatePosition(Servo *servo, int position, bool *valid);

void setMaxMinPosition(Servo *servo, int max_pos, int min_pos);
void getMaxMinPosition(Servo *servo, int *max_pos, int *min_pos);

void getPriority(Servo *servo, int *priority);
void setPriority(Servo *servo, int priority);

void getOffset(Servo *servo, int *offset);
void setOffset(Servo *servo, int offset);
  
void getStatus(Servo *servo, int id, bool *staus);
void setStatus(Servo *servo, int id, bool status);

void setID(Servo *servo, int id);

/*
 * Globale Variable
 * */
extern Operator default_OP;
int gPosition;
char gCmd[50];

#endif
