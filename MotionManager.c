#include "MotionManager.h"

/*
 *  Singleton Instance
 */
MotionManager motionManager = {
  .process = schedule_process,
  .startLogging = NULL,
  .stopLogging = NULL
};

/*
 *  Implementation Method
 */
void schedule_process(Servo* servos, unsigned int servoNum){
  // TODO
  int idx;

  printf("Motion updating ... \n");
  for(idx = 0; idx < servoNum; ++idx){
    printf("\t");
    SET_SERVO_POSITION( (Servo *)(servos+idx), 100);
  }
  printf("Done.\n");
};

void startLogging(void){
  // TODO
};

void stopLogging(void){
  // TODO
};
