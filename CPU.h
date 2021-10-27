//Header file for CPU.cpp
#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdio.h>

class Cpu
{
    //state definitions
    const int IDLE = 0;
    const int FETCH = 1;
    const int WAIT = 2;

public:
    unsigned char regs[8];
    unsigned char PC;
    int state;

    /**
     * Causes all CPU registers (including PC) to be zero
     * */
    void cpuReset();

    /**
     * Sets the value of the specified CPU register.
     * @input: CPU register to be set (including PC), hexByte to set register value to.
     * */
    void setReg(int reg, unsigned char hexByte);

    /**
     * Show the value of all the CPU registers.
     * */
    void cpuDump();

    void startTick();

    void doCycleWork();

    bool moreCycleWorkNeeded();

    void cpuParse(FILE* inFile);
};

#endif

/**
 * function that contains and returns the single instance of a CPU
 * return: address of single instance of cpu object.
 * */
Cpu &getCPU();

