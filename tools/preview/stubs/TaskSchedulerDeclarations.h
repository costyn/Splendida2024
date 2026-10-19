// No-op TaskScheduler shim for the desktop preview renderer.
#pragma once
#include <stdint.h>
#define TASK_MILLISECOND 1UL
#define TASK_SECOND 1000UL
#define TASK_FOREVER (-1)
typedef void (*TaskCallback)();
class Scheduler;
class Task
{
public:
    Task(unsigned long = 0, long = 0, TaskCallback = nullptr, Scheduler * = nullptr, bool = false) {}
    bool enable() { return true; }
    bool enableIfNot() { return true; }
    void enableDelayed(unsigned long = 0) {}
    void delay(unsigned long = 0) {}
    bool disable() { return true; }
    bool isEnabled() { return false; }
    bool isFirstIteration() { return false; }
    unsigned long getRunCounter() { return 0; }
    void setIterations(long) {}
    void setOnDisable(TaskCallback) {}
    void restart() {}
};
class Scheduler
{
public:
    void init() {}
    void addTask(Task &) {}
    bool execute() { return false; }
};
