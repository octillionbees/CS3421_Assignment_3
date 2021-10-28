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
    const int DOING_WORK = 3;
    const int HALTED = 4;

public:
    unsigned char regs[8];
    unsigned char PC;
    int state;
    bool memDone;
    int waitCycles;
    int tickCounter;

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

    //Assembly Commands:

    void loadWord(unsigned long instruction);

    void storeWord(unsigned long instruction);

    void add(unsigned long instruction);

    void addi(unsigned long instruction);

    void mul(unsigned long instruction);

    void inv(unsigned long instruction);

    void beq(unsigned long instruction);

    void bneq(unsigned long instruction);

    void blt(unsigned long instruction);
};

#endif

/**
 * function that contains and returns the single instance of a CPU
 * return: address of single instance of cpu object.
 * */
Cpu &getCPU();

