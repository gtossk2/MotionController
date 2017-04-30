#ifndef _MOTION_MANAGER_H_
#define _MOTION_MANAGER_H_

#include <stdio.h>
#include <stdlib.h>

#include "servo.h"

#define SCHEDULE_INTERVAL_MS       20

/*
 *  Forward declaration
 */
typedef struct _motionManager MotionManager;

/*
 *  Abstraction Interface
 */
struct _motionManager {
  void (*process)(Servo* servos, unsigned int servoNum);
  void (*startLogging)(void);
  void (*stopLogging)(void);
};

/*
 *  Implementation Method
 */
void schedule_process(Servo* servos, unsigned int servoNum);
void startLogging(void);
void stopLogging(void);

/*
 *  Singleton - Motion Manager
 */
extern MotionManager motionManager;

/*
 * #define SERVO_NUM 10
 *
 * Servo servos[SERVO_NUM];
 *
 * motionManager.process(servos, SERVO_NUM);
 *
 */

#endif
