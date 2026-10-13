#ifndef TIME_H
#define TIME_H

#include <stdint.h>

extern volatile uint32_t ticks; 
extern uint32_t freq;
void setPitPeriodic(uint32_t count);
void sleep(uint32_t ms);
void initTimeIntrHandler();

#endif