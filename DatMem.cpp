#include "DatMem.h"
#include <iostream>
#include <fstream>
#include <stdbool.h>
#include <cstring>


using namespace std;





/**
 * function that contains and returns the single instance of a Memory object
 * return: address of single instance of memory object.
 * */
DataMemory &getDataMemory() {
    static DataMemory memory; //declare a static Memory which is created once on first call

    return memory; //return a reference to the single static memory
}


/**
* Dumps memory from the specified addresss up to the specified byte count.
* @input: Hex Address to begin dump at, hex count to dump.
* */
void DataMemory::dump(int hexAddress, int hexCount)
{
    DataMemory &memory = getDataMemory();
  
    printf("Addr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    for (int i = 0; i < hexAddress; i++) {
        if (i % 0x10 == 0) {
            int leadingDigit = (i / 0x10);
            printf("0x%d0    ", leadingDigit);
        } else if (i % 0x10 == 0xF) {
            printf("  \n");
        } else {
            printf("   ");
        }
    }
    
    for (int i = hexAddress; i < (hexAddress + hexCount); i++) {
        if (i % 0x10 == 0) {
            int leadingDigit = (i / 0x10);
            printf("0x%d0 %02X ", leadingDigit, memory.memArr[i]);
        } else if (i % 0x10 == 0xF) {
            printf("%02X\n", memory.memArr[i]);
        } else {
            printf("%02X ", memory.memArr[i]);
        }
    }
    printf("\n");
}

void DataMemory::set(int hexAddress, int hexCount, unsigned char hexBytes[]){
    DataMemory &memory = getDataMemory();

    int j = 0;

    for (int i = hexAddress; i < (hexAddress + hexCount); i++) {
        memory.memArr[i] = hexBytes[j];
        j++;
    }
}

/**
* Creates the memory bank to be used for the system.
* @input: size of memory in number of bytes, passed in as hex.
* */
void DataMemory::create(int size)
{
    DataMemory &memory = getDataMemory();

    memory.memArr = (unsigned char *)malloc(sizeof(unsigned char) * size);

    memory.memSize = size;
}

/**
* Initializes and resets all the values in memory to 0x00
* */
void DataMemory::reset()
{
    DataMemory &memory = getDataMemory();
    for (int i = 0; i < memory.memSize; i++)
    {
        memory.memArr[i] = 0x00;
    }
    memory.state = IDLE;
}

void DataMemory::startTick() {
    DataMemory &memory = getDataMemory();

    //printf("dMem: tick!\n");
    if (memory.state == WAIT) {
        memory.tickCounter++;
    }
    if (memory.state == WAIT && memory.tickCounter == MEM_TICK_DELAY) {
        memory.state = MOVE_DATA;
    }
}

/**
 *
 * @param address the offset in memory where the read/write should begin
 * @param count  the number of bytes that should be read/written
 * @param dataPtr a pointer to where the data should be placed (read) or the source of data(write)
 * @param donePtr a pointer to a boolean that the memory device will set to true when the data transfer has completed`
 */
void DataMemory::startFetch(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* donePtr) {
    DataMemory &memory = getDataMemory();

    memory.state = WAIT;
    memory.address = address;
    memory.count = count;
    memory.answerPtr = dataPtr;
    memory.memDonePtr = donePtr;
    memory.tickCounter = 1;
    memory.readWrite = FETCH;
    //Cannot finish work now (need wait cycles), so all done
}
void DataMemory::startStore(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* donePtr){
    DataMemory &memory = getDataMemory();

    memory.state = WAIT;
    memory.address = address;
    memory.count = count;
    memory.answerPtr = dataPtr;
    memory.memDonePtr = donePtr;
    memory.tickCounter = 1;
    memory.readWrite = STORE;
    //Cannot finish work now (need wait cycles), so all done
}

void DataMemory::doCycleWork() {
    DataMemory &memory = getDataMemory();

    //finished wait, and moved to MOVE_DATA state?
    if (memory.state == MOVE_DATA) {
        if (memory.readWrite == FETCH) {
            //copy data back to caller
            memcpy(memory.answerPtr, memory.memArr + memory.address, memory.count);
            //tell caller memory operation is complete
            *(memory.memDonePtr) = true;
            memory.state = IDLE;
        } else if (memory.readWrite == STORE) {
            //copy data back to caller
            memcpy(memory.memArr + memory.address, memory.answerPtr, memory.count);

            //tell caller memory operation is complete
            *(memory.memDonePtr) = true;
            memory.state = IDLE;
        }
    }
}

bool DataMemory::moreCycleWorkNeeded() {
    DataMemory &memory = getDataMemory();

    return (memory.state != WAIT && memory.state != IDLE);
}

/**
 * Parses the input file to determine what function should be executed/.
 * @input: inFile file that was passed into parse.cpp
 * */
void DataMemory::parse(FILE* inFile) {
    DataMemory &memory = getDataMemory();
    char* memCmd;
    fscanf(inFile, "%ms", &memCmd);

    if (!strcmp("create", memCmd)) {
        fscanf(inFile, "%ms", &memCmd); //reassign memCmd to be the size
        int size = (int)strtol(memCmd, NULL, 0);
        memory.create(size);
    } else if (!strcmp("reset", memCmd)) {
        memory.reset();
    } else if (!strcmp("dump", memCmd)) {
        char* memCmd2;
        fscanf(inFile, "%ms", &memCmd); //reassign memCmd to be the hexAddress to start dump at
        fscanf(inFile, "%ms", &memCmd2); //memCmd2 represents the hexCount

        int hexAddress = (int)strtol(memCmd, NULL, 0);
        int hexCount = (int)strtol(memCmd2, NULL, 0);

        memory.dump(hexAddress, hexCount);

        free(memCmd2);
    } else if (!strcmp("set", memCmd)) {
        DataMemory &memory = getDataMemory();

        char* memCmd2;
        fscanf(inFile, "%ms", &memCmd); //reassign memCmd to be the hexAddress to start set at
        fscanf(inFile, "%ms", &memCmd2); //memCmd2 represents the hexCount

        int hexAddress = (int)strtol(memCmd, NULL, 0);
        int hexCount = (int)strtol(memCmd2, NULL, 0);
        unsigned char* hexBytes = (unsigned char*)malloc(sizeof(unsigned char) * hexCount);

        //read in number of bytes to the array
        char* hexString;
        unsigned char hexByte;

        for (int i = 0; i < hexCount; i++) {
            fscanf(inFile, "%ms", &hexString); //store next argument in hexString
            hexByte = (unsigned char)strtol(hexString, NULL, 0); //parse from the hexString the value of the char
            hexBytes[i] = hexByte;
        }

        memory.set(hexAddress, hexCount, hexBytes);

        free(memCmd2);
        free(hexString);
        free(hexBytes);
    }
    free(memCmd);
}