//parse.cpp
#include "Clock.h"
#include "CPU.h"
#include "DatMem.h"
#include "InstMem.h"
#include <cstring>

int main(int argc, char* argv[]){
    //using namespace std;

    if (argc < 2) return 1; //ensure we have an argument

    FILE* inFile = fopen(argv[1], "r");
    char* devName;

    //Create references to device objects and get them
    Cpu &cpu = getCPU(); 
    Clock &clock = getClock();
    DataMemory &dmemory = getDataMemory();
    InstructionMemory &imemory = getInstMemory();
    bool datMemCreated = false;
    bool instMemCreated = false;

    while (fscanf(inFile, "%ms", &devName) > 0) {
        if (!strcmp("cpu", devName)) {
            cpu.cpuParse(inFile);
            
        }
        if (!strcmp("memory", devName)) {
            datMemCreated = true;
            dmemory.parse(inFile);
        }
        if (!strcmp("clock", devName)) {
            clock.clockParse(inFile);
        }
        if (!strcmp("imemory", devName)) {
            instMemCreated = true;
            imemory.parse(inFile);
        }
    }
    free(devName);
    if (datMemCreated) {
        free(dmemory.memArr);
    }
    if (instMemCreated) {
        free(imemory.instArr);
    }
    fclose(inFile);
}
