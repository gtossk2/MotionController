#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
 * Define
 */
typedef uint32_t timeUs;
typedef uint32_t timeMs;

/*
 * ENUM for task priority
 */
typedef enum {
  PRIORITY_IDLE = 0,
  PRIORITY_LOW = 1,
  PRIORITY_MEDIUM = 2,
  PRIORITY_HIGH = 3,
  PRIORITY_MAX = 4
}TaskPriority;

/*
 * ENUM for TASK ID
 */
typedef enum {
  TASK_MOTION_MANAGER = 0,
  TASK_TEST,
  TASK_TOTAL
}TaskID;

/*
 * Task information struct
 */
typedef struct {
  const char *taskName;
  bool       isEnabled;
  uint8_t    staticPriority;
  timeUs     periodExecuteTime;
  timeUs     lastExecuteTime;
  timeUs     maxExecuteTime;
}TaskInfo;

/*
 * Task struct
 */
typedef struct {
  const char *taskName;
  // Event-driven TASK
  bool       (*checkFunc)(timeUs currentTime, timeUs currentDeltaTime);
  // Time-driven TASK
  void       (*taskFunc)(timeUs currentTime);

  // Time information
  timeUs     periodExecuteTime;
  timeUs     lastExecuteTime;
  timeUs     maxExecuteTime;

  // Task priority
  uint8_t    staticPriority;
  uint8_t    dynamicPriority;
  uint8_t    taskAge;  

  // Task stack
  void *stack;
  void *orig_stack;
}Task;

/*
 *  Scheduler APIs
 */
void schedulerInit(void);
void scheduler(void);
void thread_start(void);

#endif
