//Header file for InstMem.cpp

#ifndef INSTMEM_H
#define INSTMEM_H

#include <stdio.h>
#include <stdlib.h>

class InstructionMemory 
{
public: 

    unsigned long* instArr; //unsigned long = 32-bit data type
    int instMemSize;

    /**
     * Creates the memory bank to be used for the system.
     * @input: size of memory in number of 20-bit words, passed in as hex.
     * */
    void create(int size);

    /**
     * Initializes and resets all the values in memory to 0x00
     * */
    void reset();

    /**
     * Dumps memory from the specified addresss up to the specified word count.
     * @input: hexAddress to begin at, hexCount number of 20-bit words to dump.
     * */
    void dump(int hexAddress, int hexCount);

    /**
     * Sets specified values of memory to user-supplied values.
     * @input: hexAddress to begin set at, file containing the memory data to be loaded, with one hex word per line.
     * */
    void set(int hexAddress, FILE* dataFile);

    void parse(FILE* inFile);
};

InstructionMemory &getInstMemory();

#endif