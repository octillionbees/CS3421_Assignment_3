//Header File for Clock.cpp
#ifndef CLOCK_H
#define CLOCK_H

#include <stdio.h>
#include <stdlib.h>

class Clock
{

public:
  unsigned short counter;

   /**
 * Resets the internal counter.
 * */
   void clockReset();

   /**
 * Increments internal counter by specified number of ticks.
 * input: positive decimal integer number of ticks.
 */
   void tick(int n);

   /**
 * Prints "Clock: " followed by the value of the internal counter.
 */
   void clockDump();

   void clockParse(FILE* inFile);
};

#endif

/**
 * function that contains and returns the single instance of a Clock
 * return: address of single instance of clock object.
 * */
Clock &getClock();