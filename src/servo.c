#include "servo.h"
#include "stm32f4xx_conf.h"

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

Servo servo[2] = {
  {
    .op = &default_OP,
    .id = 0,
    .priority = PRIORITY_MID,
    .position = 0,
    .max_position = MAX_SERVO_DEGREE,
    .min_position = MIN_SERVO_DEGREE,
    .status = ENABLE,
    .offset = 0 },
  {
    .op = &default_OP,
    .id = 1,
    .priority = PRIORITY_MID,
    .position = 0,
    .max_position = MAX_SERVO_DEGREE,
    .min_position = MIN_SERVO_DEGREE,
    .status = ENABLE,
    .offset = 0 }
};

extern int gPosition;
extern char gCmd[50];

void setPosition(Servo *servo){

  switch(servo->id){
    case 0:
      TIM4->CCR1 = map(servo->position, servo->min_position, servo->max_position);
    case 1:
      TIM4->CCR2 = map(servo->position, servo->min_position, servo->max_position);
    case 2:
      TIM4->CCR3 = map(servo->position, servo->min_position, servo->max_position);
    case 3:
      TIM4->CCR4 = map(servo->position, servo->min_position, servo->max_position);
    default:
      break;
  }
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

static int isValidated(int idx, int position){
  if(idx > -1 && idx < MAX_SERVO_NUMBER){

    if(position > servo[idx].min_position || position < servo[idx].max_position)
      return 1;

    return 0;
  }

  return 0;
}

static void updatePosition(Servo *servo, int position){
  servo->position = position;
  return;
}

void updatePositionFromCmd(char *idx, char *position){
  int _position = atoi(position);
  int _idx = atoi(idx);

  //TODO : validate the incoming request
  if(!isValidated(_idx, _position))
    return;
  
  //enterCritical();
  updatePosition(&servo[_idx], _position);
  //exitCritical();

 return;
}

