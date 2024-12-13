// tasks.h - NEW FILE
#ifndef TASKS_H
#define TASKS_H

#include <TaskScheduler.h>

extern Scheduler _runner;
extern Task _taskChangeToBrightness;
extern Task _taskRunPattern;
extern Task _taskChangePalette;
extern Task _taskChangePattern;
extern Task _taskBlendPalette;
extern Task _taskFade;
extern Task _taskReadEncoders;

#endif