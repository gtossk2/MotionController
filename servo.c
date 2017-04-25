#include "servo.h"

#include <stdlib.h>

Operator default_OP = {
  .setPosition = setPosition,
  .getPosition = getPosition,
  .validatePosition = NULL,

  .setMaxMinPosition = NULL,
  .getMaxMinPosition = NULL,

  .setPriority = NULL,
  .getPriority = NULL,

  .setOffset = NULL,
  .getOffset = NULL,

  .setStatus = NULL,
  .getStatus = NULL,

  .setID = NULL
};

extern int gPosition;
extern char gCmd[50];

void setPosition(Servo *servo, int position){
  servo->position = position;
  printf("Set ID: %d to %d \n", servo->id, servo->position);
}

void getPosition(Servo *servo, int *position){
  *position = servo->position;
  printf("Get ID: %d to %d \n", servo->id, servo->position);
}

/*
 *  Return to position (degree) to the period
 */
int map(int pos, int servo_min, int servo_max){
  if(pos > servo_max || pos < servo_min)
    return 0;

  int servo_degree_diff = servo_max - servo_min;
  int servo_us_diff = SERVO_MAX_US - SERVO_MIN_US;
  
  double degree_to_us = (double) servo_us_diff / servo_degree_diff;
  
  int value = (int)(pos * degree_to_us) + SERVO_MIN_US;

  return value * SERVO_US_TO_PERIOD;
};

void validatePosition(Servo *servo, int position, bool *valid){
  /*
  if(position < MIN_SERVO_DEGREE || position > MAX_SERVO_DEGREE)
    return 0;
  else
    return 1;
  */
}

int parseCmd(char *cmd){
  int res = 0;

  //TODO: Error Handler & Check Error

  /* Cmd: T180 */
  if( cmd[0] == 'T' ){
    res = atoi(&cmd[1]);
  }

  return res;
}

void updatePositionFromCmd(char *cmd){
  int position = 0;

  position = parseCmd(cmd);
  //if(!validatePosition(position))
  //  return;
  
  //enterCritical();
  gPosition = position;
  //exitCritical();

 return;
}

