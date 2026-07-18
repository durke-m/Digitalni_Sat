#include <at89c51rc2.h>
#include "clock.h"
#include "display.h"
#include "command.h"
#include "buttons.h"

// Timer2 ISR - interrupt 5
void timer2_isr(void) interrupt 5 {
    clockTick();
}

// Helper funkcija za prikaz vremena
void displayTime(uint8_t h, uint8_t m, uint8_t s, uint8_t row) {
    setCursor(row, 0);
    writeChar((h / 10) + '0');
    writeChar((h % 10) + '0');
    writeChar(':');
    writeChar((m / 10) + '0');
    writeChar((m % 10) + '0');
    writeChar(':');
    writeChar((s / 10) + '0');
    writeChar((s % 10) + '0');
}

void main(void) {
    Time current, alarm;
    uint8_t lastSec = 0xFF;
    
    // Inicijalizacija
    initDisplay();
    commandInit();
    clockInit();
    keypadInit();
    
    // Postavi default vreme (preko tastera ili UART ce se promeniti)
    setTime(0, 0, 0);
    setAlarm(0, 0, 0);
    
    // Inicijalni prikaz
    clearDisplay();
    displayTime(0, 0, 0, 0);  // Red 1: 00:00:00
    displayTime(0, 0, 0, 1);  // Red 2: 00:00:00
    
    while(1) {
        // Proveri tastere (ako je u edit režimu, preskoči normalan prikaz)
        if(getMode() != MODE_NORMAL) {
            keypadProcess();
            continue;
        }
        
        // NORMALAN REŽIM - prikaz sata
        current = getTime();
        alarm = getAlarmTime();
        
        // Ažuriraj displej SAMO kad se sekunda promeni
        if(current.seconds != lastSec) {
            lastSec = current.seconds;
            
            displayTime(current.hours, current.minutes, current.seconds, 0);  // Red 1
            displayTime(alarm.hours, alarm.minutes, alarm.seconds, 1);        // Red 2
        }
        
        // Parsiranje UART komandi
        if(commandReady) {
            parseCommand();
            
            // Osveži displej nakon komande
            current = getTime();
            alarm = getAlarmTime();
            clearDisplay();
            displayTime(current.hours, current.minutes, current.seconds, 0);
            displayTime(alarm.hours, alarm.minutes, alarm.seconds, 1);
        }
        
        // Tastatura (procesiranje u normalnom režimu)
        keypadProcess();
        
        // Provera alarma
        if(checkAlarm()) {
            clearDisplay();
            writeLine("!ALARM!");
            wait1s();
            wait1s();
            
            // Vrati normalan prikaz
            clearDisplay();
            displayTime(current.hours, current.minutes, current.seconds, 0);
            displayTime(alarm.hours, alarm.minutes, alarm.seconds, 1);
        }
    }
}
