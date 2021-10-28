#include "CPU.h"
#include "DatMem.h"
#include "InstMem.h"
#include <cstring>
#include <iostream>
#include <fstream>



/**
 * function that contains and returns the single instance of a CPU
 * return: address of single instance of cpu object.
 * */
Cpu& getCPU() {
    static Cpu cpu; //declare a static clock which is created once on first call.

    return cpu; //return a reference to the single static clock.

}

/**
* Causes all CPU registers (including PC) to be zero
* */
void Cpu::cpuReset() {
    Cpu &cpu = getCPU();

    for (int i = 0; i < 8; i++) {
        cpu.regs[i] = 0x00;
    }

    cpu.PC = 0x00;
    cpu.state = IDLE;
}

/**
* Sets the value of the specified CPU register.
* @input: CPU register to be set (including PC), hexByte to set register value to.
* */
void Cpu::setReg(int reg, unsigned char hexByte) {
    Cpu &cpu = getCPU();

    //value of 8 to be passed in if register is PC
    if (reg == 8) {
        cpu.PC = hexByte;
        cpu.state = FETCH;
        return;
    }

    //otherwise, register value is valid.
    cpu.regs[reg] = hexByte;
}

/**
* Show the value of all the CPU registers.
* */
void Cpu::cpuDump() {
    Cpu &cpu = getCPU();

    printf("PC: 0x%02X\n", cpu.PC);
    printf("RA: 0x%02X\n", cpu.regs[0]);
    printf("RB: 0x%02X\n", cpu.regs[1]);
    printf("RC: 0x%02X\n", cpu.regs[2]);
    printf("RD: 0x%02X\n", cpu.regs[3]);
    printf("RE: 0x%02X\n", cpu.regs[4]);
    printf("RF: 0x%02X\n", cpu.regs[5]);
    printf("RG: 0x%02X\n", cpu.regs[6]);
    printf("RH: 0x%02X\n", cpu.regs[7]);
}

void Cpu::startTick() {
    Cpu &cpu = getCPU();
    //printf("cpu: tick!\n");

    if(cpu.state == DOING_WORK) {
        cpu.tickCounter++;
    }
    if(cpu.state == DOING_WORK && cpu.tickCounter == cpu.waitCycles) {
        cpu.state = FETCH;
    }
    if(cpu.state == IDLE) {
        cpu.state = FETCH;
    }
}

void Cpu::doCycleWork() {
    Cpu &cpu = getCPU();
    DataMemory &memory = getDataMemory();
    InstructionMemory &imemory = getInstMemory();

    if (cpu.state == FETCH) {
        unsigned long instruction = imemory.instArr[cpu.PC]; //get instruction from imemory at address pointed to by PC;
        unsigned char instEnc = (unsigned char) (instruction >> 17); //bit shift right 17 to get last bits
        if (instEnc == 5) {
            //lw
            loadWord(instruction);
        } else if (instEnc == 6) {
            //sw
            storeWord(instruction);
        } else if (instEnc == 0) {
            //add
            add(instruction);
        } else if (instEnc == 1) {
            //addi
            addi(instruction);
        } else if (instEnc == 2) {
            //mul
            mul(instruction);
        } else if (instEnc == 3) {
            //inv
            inv(instruction);
        } else if (instEnc == 4) {
            //branches
            unsigned int branchEnc = (instruction & 0x1C000) >> 14;
            if (branchEnc == 0) {
                //beq
                beq(instruction);
            } else if (branchEnc == 1) {
                //bneq
                bneq(instruction);
            } else if (branchEnc == 2) {
                //blt
                blt(instruction);
            }
        } else if (instEnc == 7) {
            //halt
            cpu.state = HALTED;
            cpu.PC++;
        }

    } else if (cpu.state == WAIT) {
        if (cpu.memDone) {
            cpu.state = IDLE;
        }

    }

}

void Cpu::add(unsigned long instruction) {
    Cpu &cpu = getCPU();

    unsigned int dest = (instruction & 0x1C000) >> 14;
    unsigned int src = (instruction & 0x03800) >> 11;
    unsigned int target = (instruction & 0x00700) >> 8;

    cpu.regs[dest] = ((signed char)cpu.regs[src]) + ((signed char)cpu.regs[target]);

    //completes in one cycle
    cpu.waitCycles = 1;
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
    cpu.PC++;
}

void Cpu::addi(unsigned long instruction) {
    Cpu &cpu = getCPU();

    unsigned int dest = (instruction & 0x1C000) >> 14;
    unsigned int src = (instruction & 0x03800) >> 11;
    int imm = (instruction & 0x000FF);

    cpu.regs[dest] = ((signed char)cpu.regs[src]) + imm;

    //completes in one cycle
    cpu.waitCycles = 1;
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
    cpu.PC++;
}

void Cpu::mul(unsigned long instruction) {
    Cpu &cpu = getCPU();

    unsigned int dest = (instruction & 0x1C000) >> 14;
    unsigned int src = (instruction & 0x03800) >> 11;

    unsigned int srcTop = cpu.regs[src] >> 4;
    unsigned int srcBot = (cpu.regs[src] & 0x0F);

    cpu.regs[dest] = srcTop * srcBot;

    //completes in two cycles
    cpu.waitCycles = 2;
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
    cpu.PC++;
}

void Cpu::inv(unsigned long instruction){
    Cpu &cpu = getCPU();

    unsigned int dest = (instruction & 0x1C000) >> 14;
    unsigned int src = (instruction & 0x03800) >> 11;

    cpu.regs[dest] = ~cpu.regs[src];

    //completes in one cycle
    cpu.waitCycles = 1;
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
    cpu.PC++;
}

void Cpu::beq(unsigned long instruction) {
    Cpu &cpu = getCPU();

    unsigned int src = (instruction & 0x03800) >> 11;
    unsigned int target = (instruction & 0x00700) >> 8;
    unsigned char imm = (instruction & 0x000FF);

    if (cpu.regs[src] == cpu.regs[target]) {
        cpu.PC = imm;
        cpu.waitCycles = 2;
    } else {
        cpu.PC++;
        cpu.waitCycles = 1;
    }
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
}

void Cpu::bneq(unsigned long instruction) {
    Cpu &cpu = getCPU();

    unsigned int src = (instruction & 0x03800) >> 11;
    unsigned int target = (instruction & 0x00700) >> 8;
    unsigned char imm = (instruction & 0x000FF);

    if (cpu.regs[src] != cpu.regs[target]) {
        cpu.PC = imm;
        cpu.waitCycles = 2;
    } else {
        cpu.PC++;
        cpu.waitCycles = 1;
    }
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
}

void Cpu::blt(unsigned long instruction) {
    Cpu &cpu = getCPU();

    unsigned int src = (instruction & 0x03800) >> 11;
    unsigned int target = (instruction & 0x00700) >> 8;
    unsigned char imm = (instruction & 0x000FF);

    if (cpu.regs[src] < cpu.regs[target]) {
        cpu.PC = imm;
        cpu.waitCycles = 2;
    } else {
        cpu.PC++;
        cpu.waitCycles = 1;
    }
    cpu.tickCounter = 0;
    cpu.state = DOING_WORK;
}

void Cpu::loadWord(unsigned long instruction) {
    //instruction structure:
    //101 ddd --- ttt --------
    //load a word into register $d from data memory at the address specified in register $t
    Cpu &cpu = getCPU();
    DataMemory &memory = getDataMemory();

    //mask instruction to get destination register:
    //0x1C000 = 0b00011100000000000000
    unsigned int dest = (instruction & 0x1C000) >> 14;
    //mask instruction to get target register:
    //0x00700 = 0b00000000011100000000
    unsigned int target = (instruction & 0x00700) >> 8;

    memory.startFetch(cpu.regs[target], 1, &(cpu.regs[dest]), &cpu.memDone);
    cpu.memDone = false;
    cpu.state = WAIT;
    cpu.PC++;
}

void Cpu::storeWord(unsigned long instruction) {
    //instruction structure:
    //110 --- sss ttt --------
    //store the value in register $s into data memory at the address specified in register $t
    Cpu &cpu = getCPU();
    DataMemory &memory = getDataMemory();
    //mask instruction to get source register:
    //0x03800 = 0b00000011100000000000
    unsigned int src = (instruction & 0x03800) >> 11;
    //mask instruction to get target register:
    //0x00700 = 0b00000000011100000000
    unsigned int target = (instruction & 0x00700) >> 8;

    memory.startStore(cpu.regs[target], 1, &(cpu.regs[src]), &cpu.memDone);
    cpu.memDone = false;
    cpu.state = WAIT;
    cpu.PC++;
}

bool Cpu::moreCycleWorkNeeded() {
    Cpu &cpu = getCPU();

    return (cpu.memDone && cpu.state == WAIT);
}

void Cpu::cpuParse(FILE* inFile) {
    Cpu &cpu = getCPU();

    char* cpuCmd;
    fscanf(inFile, "%ms", &cpuCmd);
    if(!strcmp("reset", cpuCmd)) {
        cpu.cpuReset();
    } else if (!strcmp("set", cpuCmd)) {
        fscanf(inFile, "%ms", &cpuCmd); //push past the "reg" immediately following "set"

        fscanf(inFile, "%ms", &cpuCmd); //reassign cpuCmd to hold the value of the register
        //determine what value to pass in based on register string
        int reg;
        if (!strcmp("PC", cpuCmd)) {
            reg = 8;
        } else if (!strcmp("RA", cpuCmd)) {
            reg = 0;
        } else if (!strcmp("RB", cpuCmd)) {
            reg = 1;
        } else if (!strcmp("RC", cpuCmd)) {
            reg = 2;
        } else if (!strcmp("RD", cpuCmd)) {
            reg = 3;
        } else if (!strcmp("RE", cpuCmd)) {
            reg = 4;
        } else if (!strcmp("RF", cpuCmd)) {
            reg = 5;
        } else if (!strcmp("RG", cpuCmd)) {
            reg = 6;
        } else if (!strcmp("RH", cpuCmd)) {
            reg = 7;
        }

        fscanf(inFile, "%ms", &cpuCmd); //reassign cpuCmd to hold the value of the hexByte
        unsigned char hexByte = (unsigned char)strtol(cpuCmd, NULL, 0);

        cpu.setReg(reg, hexByte);
        
    } else if (!strcmp("dump", cpuCmd)) {
        cpu.cpuDump();
    }

    free(cpuCmd);
}