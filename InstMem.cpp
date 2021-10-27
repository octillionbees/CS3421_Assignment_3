#include "InstMem.h"
#include <iostream>
#include <fstream>
#include <stdbool.h>
#include <cstring>

using namespace std;

InstructionMemory &getInstMemory() {
    static InstructionMemory memory;

    return memory;
}

void InstructionMemory::create(int size) {
    InstructionMemory &memory = getInstMemory();

    memory.instArr = (unsigned long*) malloc(sizeof(unsigned long) * size);
    //creates an array of char pointers to 20-bit words represented by longs
    //(the last 12 bits of the data type will be ignored)

    memory.instMemSize = size;
}

void InstructionMemory::reset() {
    InstructionMemory &memory = getInstMemory();

    for (int i = 0; i < memory.instMemSize; i++) {
        memory.instArr[i] = 0x00000000;
    }
}

void InstructionMemory::dump(int hexAddress, int hexCount) {
    InstructionMemory &memory = getInstMemory();

    printf("Addr     0     1     2     3     4     5     6     7\n");
    for (int i = 0; i < hexAddress; i++) {
        if (i % 0x08 == 0) {
            printf("0x%02X       ", i); //7 spaces
        } else if (i % 0x08 == 0x07) {
            printf("     \n"); //5 spaces
        } else {
            printf("      "); //6 spaces
        }
    }

    for (int i = hexAddress; i < (hexAddress + hexCount); i++) {
        if (i % 0x08 == 0) {
            printf("0x%02X %05X ", i, memory.instArr[i]);
        } else if (i % 0x08 == 0x07) {
            printf("%05X\n", memory.instArr[i]);
        } else {
            printf("%05X ", memory.instArr[i]);
        }
    }
    printf("\n");
}

/**
* Sets specified values of memory to user-supplied values.
* @input: hexAddress to begin set at, file containing the memory data to be loaded, with one hex word per line.
* */
void InstructionMemory::set(int hexAddress, FILE* dataFile) {
    InstructionMemory &memory = getInstMemory();
    
    int i = 0;
    char* instruction;
    while (fscanf(dataFile, "%ms", instruction) > 0) {
        memory.instArr[hexAddress + i] = (unsigned long) strtol(instruction, NULL, 16);
        i++;
    }

    free(instruction);
}

void InstructionMemory::parse(FILE* inFile) {
    InstructionMemory &memory = getInstMemory();
    char* memCmd;

    fscanf(inFile, "%ms", memCmd); //load second word on line into memCmd

    if (!strcmp("create", memCmd)) {
        fscanf(inFile, "%ms", memCmd); //reassign memCmd to be the size
        int size = (int)strtol(memCmd, NULL, 0);
        memory.create(size);
    } else if (!strcmp("reset", memCmd)) {
        memory.reset();
    } else if (!strcmp("dump", memCmd)) {
        fscanf(inFile, "%ms", memCmd); //reassign memCmd to be the hexAddress;
        int hexAddress = (int)strtol(memCmd, NULL, 0);

        fscanf(inFile, "%ms", memCmd); //reassign memCmd to be the hexCount;
        int hexCount = (int)strtol(memCmd, NULL, 0);

        memory.dump(hexAddress, hexCount);
    } else if (!strcmp("set", memCmd)) {
        fscanf(inFile, "%ms", memCmd); //reassign memCmd to be the hexAddress;
        int hexAddress = (int)strtol(memCmd, NULL, 0);

        fscanf(inFile, "%ms", memCmd); //burning through the "file" keyword
        fscanf(inFile, "%ms", memCmd); //reassign memCmd to be the filepath

        FILE* dataFile = fopen(memCmd, "r");
        memory.set(hexAddress, dataFile);

        //once done with the set instructions in the file,
        fclose(dataFile);
    }

    free(memCmd);
}