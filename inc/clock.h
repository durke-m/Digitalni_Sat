#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <at89c51rc2.h>
typedef struct {

        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
} Time;


extern Time currentTime;
extern Time alarmTime;
extern bit alarmFlag;

void clockInit(void);

Time getTime(void);
Time getAlarmTime(void);

void setTime(uint8_t h, uint8_t m, uint8_t s);
void setAlarm(uint8_t h, uint8_t m, uint8_t s);

bit checkAlarm(void);

void clockTick(void);

#endif