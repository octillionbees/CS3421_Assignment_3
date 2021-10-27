#include "Clock.h"
#include "CPU.h"
#include "DatMem.h"
#include <cstring>
#include <iostream>
#include <fstream>

/**
 * function that contains and returns the single instance of a Clock
 * return: address of single instance of clock object.
 * */
Clock& getClock() {
   static Clock clock; //declare a static clock which is created once on first call.

   return clock; //return a reference to the single static clock.
}

/**
* Resets the internal counter.
* */
void Clock::clockReset() {
    Clock &clock = getClock();

    clock.counter = 0;
}

/**
 * Increments internal counter by specified number of ticks.
 * input: positive decimal integer number of ticks.
 */
void Clock::tick(int n) {
    Clock &clock = getClock();
    Cpu &cpu = getCPU();
    DataMemory &memory = getDataMemory();

    bool workToDo = true;

    for (int i = 0; i < n; i++) {
        clock.counter++;
        //tell all devices we are starting a new tick, allowing them to change state,
        //set counters, etc.
        cpu.startTick();
        memory.startTick();

        //Continue to loop while ANY device has more work to do
        //on this cycle. Devices often have more work to do to finish an instruction,
        //but it may need to wait for a new tick, and nothing remains for this cycle.
        while(workToDo) {
            //give each deice a chance to do some work such as
            //issue requests, process results, change state, etc.
            memory.doCycleWork();
            cpu.doCycleWork();

            //Poll all devices. Will be true if ANY device has more work to do on THIS cycle.
            workToDo = cpu.moreCycleWorkNeeded() || memory.moreCycleWorkNeeded();
        }
    }
 }

 /**
 * Prints "Clock: " followed by the value of the internal counter.
 */
void Clock::clockDump() {
    Clock &clock = getClock();

    printf("Clock: %d\n", clock.counter);
}

void Clock::clockParse(FILE* inFile) {
    Clock &clock = getClock();


    char* clockCmd;
    fscanf(inFile, "%ms", &clockCmd);
    if(!strcmp("reset", clockCmd)) {
        clock.clockReset();
    } else if (!strcmp("tick", clockCmd)) {
        fscanf(inFile, "%ms", &clockCmd); //reassign clockCmd to be the number of ticks
        int n = atoi(clockCmd);
        clock.tick(n);
    } else if (!strcmp("dump", clockCmd)) {
        clock.clockDump();
    }

    free(clockCmd);
}