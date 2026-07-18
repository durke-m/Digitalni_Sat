#include "clock.h"

Time currentTime = {0, 0 ,0};
Time alarmTime = {0, 0, 0};
bit alarmFlag = 0;
static volatile uint16_t timerCount = 0;


void clockInit(void){

     T2CON = 0x00;  // Timer2 mode: 16-bit auto-reload
    
    // Za 11.0592 MHz kristal: 50ms delay
    // Preload: 65536 - 46080 = 19456 = 0x4C00
    RCAP2H = 0x4C;  // Reload High
    RCAP2L = 0x00;  // Reload Low
    
    TH2 = 0x4C;     // Initial value
    TL2 = 0x00;
    
    // Omogući Timer2 interrupt
    ET2 = 1;
    EA = 1;
    
    // Startuj Timer2
    TR2 = 1;  // T2CON.2
    
    timerCount = 0;
    alarmFlag = 0;
}

void clockTick(void){
    TH1 = 0x4C; // 11.0592
    TL1 = 0x00;
    
    timerCount++;
    
    // 20 x 50ms = 1000ms = 1 sekunda
    if(timerCount >= 20) {
        timerCount = 0;
        
        currentTime.seconds++;
        
        if(currentTime.seconds>= 60) {
            currentTime.seconds = 0;
            currentTime.minutes++;
            
            if(currentTime.minutes >= 60) {
                currentTime.minutes = 0;
                currentTime.hours++;
                
                if(currentTime.hours >= 24) {
                    currentTime.hours = 0;
                }
            }
        }
    }
}

Time getTime(void){
    
    Time temporary;
    EA = 0;
    temporary.hours = currentTime.hours;
    temporary.minutes = currentTime.minutes;
    temporary.seconds = currentTime.seconds;
    EA = 1;

    return temporary;
}

Time getAlarmTime(void){

    Time temporary;
    
    temporary.hours = alarmTime.hours;
    temporary.minutes = alarmTime.minutes;
    temporary.seconds = alarmTime.seconds;

	return temporary;
}

void setTime(uint8_t h, uint8_t m, uint8_t s){
    if (h < 24 && m < 60 && s < 60){

        EA = 0;
        currentTime.hours = h;
        currentTime.minutes = m;
        currentTime.seconds = s;
        timerCount = 0;
        EA = 1;

    }
}

void setAlarm(uint8_t h, uint8_t m, uint8_t s){
    if (h < 24 && m < 60 && s < 60){

        
        alarmTime.hours = h;
        alarmTime.minutes = m;
        alarmTime.seconds = s;
        alarmFlag  = 0;

    }
}

bit checkAlarm(void){

    if(currentTime.hours == alarmTime.hours &&
       currentTime.minutes == alarmTime.minutes &&
       currentTime.seconds == alarmTime.seconds){

            if(!alarmFlag){
                alarmFlag = 1;
                return 1;
            }
       }
    else{
        alarmFlag = 0;
    }

    return 0;
}