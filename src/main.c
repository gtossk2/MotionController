#include <stdio.h>
#include <stdlib.h>

#include "stm32f4xx_conf.h"
#include "servo.h"
#include "MotionManager.h"

// TODO Add timer to EXIT for toggling LED that make sure system is alive
// TODO Add EXIT to handle UART message  

extern MotionManager motionManager;
extern Operator default_OP;

Servo servo[2] = {
  {
    .op = &default_OP,
    .id = 0,
    .priority = PRIORITY_MID,
    .position = 24,
    .max_position = 180,
    .min_position = -180,
    .status = ENABLE,
    .offset = 0 },
  {
    .op = &default_OP,
    .id = 1,
    .priority = PRIORITY_MID,
    .position = 0,
    .max_position = 180,
    .min_position = -180,
    .status = ENABLE,
    .offset = 0 }
};

int main(){
  int pos = 0;

  SET_SERVO_POSITION(&servo[0], 122);
  GET_SERVO_POSITION(&servo[0], &pos);

  motionManager.process(servo, 2);

  return 0;
}
