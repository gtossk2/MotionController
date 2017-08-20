#include "schedule.h"
#include "system.h"
#include "Uart.h"

#include <stddef.h>
#include <stdlib.h>

/*
 *  MACRO for converting the desired period time
 */
#define TASK_PEROID_HZ(hz)  (1000000 / (hz))
#define TASK_PEROID_MS(ms)  ((ms) * 1000)
#define TASK_PEROID_US(us)  (us)

#define STACK_SIZE  128
#define THREAD_PSP  0XFFFFFFFD

volatile static int first = 1;
volatile static int gTaskID = 0;

// TODO : Debug
static void __delay(volatile int count){
  count *= 2500;
  while(count--){
  }
}
static void task1(timeUs time){
  while(1){
    USART_puts(USART1, "Task1 is running\r\n");
    __delay(1);
  }
}

// TODO : Debug
static void task2(timeUs time){
  while(1){
    USART_puts(USART1, "Task2 is running\r\n");
    __delay(1);
  }
}

/*
 *  Initializing global tasks
 */
static Task gTasks[TASK_TOTAL] = {
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

/* FIXME: Without naked attribute, GCC will corrupt r7 which is used for stack
 * pointer. If so, after restoring the tasks' context, we will get wrong stack
 * pointer.
 */
void __attribute__((naked)) PendSV_Handler(){
  /* Save the old task's context */
  asm volatile("mrs   r0, psp\n"
      "stmdb r0!, {r4-r11, lr}\n");
  /* To get the task pointer address from result r0 */
  asm volatile("mov   %0, r0\n" : "=r"(gTasks[gTaskID].stack));

  /* Find a new task to run */
  while (1){
    gTaskID++;
    if (gTaskID == TASK_TOTAL){
      gTaskID = 0;
    }
    if(gTasks[gTaskID].in_use){
      /* Move the task's stack pointer address into r0 */
      // DEBUG : fail to switch task stack
      asm volatile("mov r0, %0\n" : : "r" (gTasks[gTaskID].stack));
      /* Restore the new task's context and jump to the task */
      asm volatile("ldmia r0!, {r4-r11, lr}\n"
          "msr psp, r0\n"
          "bx lr\n");
    }
  }
}

static void thread_self_terminal(){
  while(1);
}

void thread_start(void)
{
  gTaskID = 0;
  CONTROL_Type user_ctx = {
    .b.nPRIV = 1,
    .b.SPSEL = 1
  };

  /* Save kernel context */
  asm volatile("mrs ip, psr\n"
      "push {r4-r11, ip, lr}\n");

  /* Load user task's context and jump to the task */
  __set_PSP((uint32_t)gTasks[gTaskID].stack);
  __set_CONTROL(user_ctx.w);
  __ISB();

  asm volatile("pop {r4-r11, lr}\n"
      "pop {r0}\n"
      "bx lr\n");
}

int schedulerInit(void){
  int idx = 0;
  uint32_t *stack;

  for(idx = 0; idx < TASK_TOTAL; ++idx){
    // Create task stack
    stack = (uint32_t *) malloc(STACK_SIZE * sizeof(uint32_t));
    if(!stack) {
      return 0;
    }

    gTasks[idx].orig_stack = stack;

    stack += STACK_SIZE - 32; // End of task, minus what we are about to push
    if(first){
      stack[8] = (unsigned int) gTasks[idx].taskFunc;
      stack[9] = (unsigned int) gTasks[idx].taskName;
      first = 0;
    } else {
      stack[8] = (unsigned int) THREAD_PSP;
      stack[9] = (unsigned int) gTasks[idx].taskName;
      stack[14] = (unsigned int) &thread_self_terminal;
      stack[15] = (unsigned int) gTasks[idx].taskFunc;
      stack[16] = (unsigned int) 0x01000000; // PSR Thumb bit
    }

    gTasks[idx].stack = stack;
    gTasks[idx].in_use = 1;
  }

  return 1;
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
