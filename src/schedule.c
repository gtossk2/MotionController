#include "schedule.h"
#include "system.h"
#include "Uart.h"

/*
 *  MACRO for converting the desired period time
 */
#define TASK_PEROID_HZ(hz)  (1000000 / (hz))
#define TASK_PEROID_MS(ms)  ((ms) * 1000)
#define TASK_PEROID_US(us)  (us)

// TODO : Debug
static void task1(timeUs time){
  USART_puts(USART1, "Task1 is running\r\n");
}

// TODO : Debug
static void task2(timeUs time){
  USART_puts(USART1, "Task2 is running\r\n");
}

/*
 *  Initializing global tasks
 */
Task gTasks[TASK_TOTAL] = {
  [TASK_MOTION_MANAGER] = {
    .taskName = "MotionManager",
    .taskFunc = task1,
    .periodExecuteTime = TASK_PEROID_MS(500), 
    .staticPriority = PRIORITY_MEDIUM
  },
  [TASK_TEST] = {
    .taskName = "Test",
    .taskFunc = task2,
    .periodExecuteTime = TASK_PEROID_HZ(1),
    .staticPriority = PRIORITY_MEDIUM
  },
};

void schedulerInit(void){
  // TODO
}

void scheduler(void){
  const timeUs currentTimeUs = micros();
  Task *currentTask, *selectedTask;
  uint16_t selectedTaskDynamicPriority = 0;

  // Check for realtime task
  bool realtimeGuard = true;
  int idx = 0;
  for(idx = 0; idx < TASK_TOTAL; ++idx){
    currentTask = &gTasks[idx];

    if( currentTask->staticPriority < PRIORITY_HIGH ) continue;

    const timeUs nextExecuteAt = currentTask->lastExecuteTime + currentTask->periodExecuteTime;
    if( (currentTimeUs - nextExecuteAt) >= 0){
      realtimeGuard = false;
      break;
    }
  }

  // The task to be invoked
  uint16_t waitTaskCount = 0;
  for(idx = 0; idx < TASK_TOTAL; ++idx){
    currentTask = &gTasks[idx];
    // TODO event task
    //if(currentTask->checkFunc)

    currentTask->taskAge = ((currentTimeUs - currentTask->lastExecuteTime) / currentTask->periodExecuteTime);
    if(currentTask->taskAge > 0){
      currentTask->dynamicPriority = 1 + currentTask->staticPriority * currentTask->taskAge;
      waitTaskCount++;
    }

    // Compared the urgent task
    if(currentTask->dynamicPriority > selectedTaskDynamicPriority){
      const bool taskForScheduling = 
        ((realtimeGuard) || (currentTask->taskAge > 1) || (currentTask->staticPriority == PRIORITY_HIGH));

      // Update the select task and dynamic priority
      if(taskForScheduling){
        selectedTaskDynamicPriority = currentTask->dynamicPriority;
        selectedTask = currentTask;
      }
    }
  }

  // Task run
  if(selectedTask){
    selectedTask->lastExecuteTime = currentTimeUs;
    selectedTask->dynamicPriority = 0;
    
    selectedTask->taskFunc(currentTimeUs);
  }
}


