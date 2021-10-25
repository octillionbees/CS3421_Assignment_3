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

    for (int i = 0; i < n; i++) {
        clock.counter++;
        cpu.startTick();
        memory.startTick();

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