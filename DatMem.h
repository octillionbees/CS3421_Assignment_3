//Header file for DatMem.cpp

#ifndef DATMEM_H
#define DATMEM_H

#include <stdio.h>
#include <stdlib.h>

class DataMemory
{
public:

    unsigned char* memArr;
    int memSize;
    int state;

    /**
     * Creates the memory bank to be used for the system.
     * @input: size of memory in number of bytes, passed in as hex.
     * */
    void create(int size);

    /**
     * Initializes and resets all the values in memory to 0x00
     * */
    void reset();

    /**
     * Dumps memory from the specified addresss up to the specified byte count.
     * @input: Hex Address to begin dump at, hex count to dump.
     * */
    void dump(int hexAddress, int hexCount);

    /**
     * Sets specified values of memory to user-supplied values.
     * @input: hexAddress to begin set at, hexCount number of bytes to assign, hexBytes array of Hex Bytes to assign to memory.
     * */
    void set(int hexAddress, int hexCount, unsigned char *hexBytes);

    void parse(FILE* inFile);

    void StartFetch(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* donePtr);

    void startTick();

    void doCycleWork();

    bool moreCycleWorkNeeded();
};

#endif

/**
 * function that contains and returns the single instance of a Memory object
 * return: address of single instance of memory object.
 * */
DataMemory &getDataMemory();
