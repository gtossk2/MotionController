#include "MotionManager.h"
#include "Uart.h"
#include "Message.h"

extern Message gRequest;

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
  int idx = 0;

  //USART_puts(USART1, "Motion updating ... ");
  /* Set servo poition from its property */
  for(idx = 0; idx < servoNum; ++idx)
  {
    SET_SERVO_POSITION( (Servo *)(servos+idx));
  }
  //USART_puts(USART1, "Done.\r\n");
};

void startLogging(void){
  // TODO
};

void stopLogging(void){
  // TODO
};
