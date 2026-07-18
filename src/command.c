#include "command.h"
#include "clock.h"
#include "display.h"
#include <stdint.h>

unsigned char commandBuffer[BUFFER_SIZE];
unsigned char commandIndex = 0;
bit commandReady = 0;

void clearCommandBuffer(void){
    uint8_t i;
    for(i=0; i<BUFFER_SIZE; i++){

        commandBuffer[i] = 0;

    }
    commandIndex = 0;
    commandReady = 0;
}

void commandInit(void){

    SCON = 0X50;

    TMOD &= 0X0F;
    TMOD |= 0X20;

    TH1 = 0XFD;
    PCON |= 0X80;

    TR1 = 1;

    ES = 1;
    EA = 1;

}

bit parseTime(uint8_t startPosition, uint8_t* h, uint8_t* m, uint8_t* s){

    if(commandBuffer[startPosition] < '0' || commandBuffer[startPosition] > '9') return 0;
    if(commandBuffer[startPosition+1] < '0' || commandBuffer[startPosition+1] > '9') return 0;
    *h = (commandBuffer[startPosition] - '0') * 10 + (commandBuffer[startPosition+1] - '0');
    
    if(commandBuffer[startPosition+2] != ':') return 0;
    
    // Minuti
    if(commandBuffer[startPosition+3] < '0' || commandBuffer[startPosition+3] > '9') return 0;
    if(commandBuffer[startPosition+4] < '0' || commandBuffer[startPosition+4] > '9') return 0;
    *m = (commandBuffer[startPosition+3] - '0') * 10 + (commandBuffer[startPosition+4] - '0');
    
    if(commandBuffer[startPosition+5] != ':') return 0;
    
    // Sekunde
    if(commandBuffer[startPosition+6] < '0' || commandBuffer[startPosition+6] > '9') return 0;
    if(commandBuffer[startPosition+7] < '0' || commandBuffer[startPosition+7] > '9') return 0;
    *s = (commandBuffer[startPosition+6] - '0') * 10 + (commandBuffer[startPosition+7] - '0');
    
    if(commandBuffer[startPosition+8] != ')') return 0;
    
    // Validacija
    if(*h >= 24 || *m >= 60 || *s >= 60) return 0;
    
    return 1;

}

void parseCommand(void) {
    uint8_t h, m, s;
    Time current, alarm;
    
    // (SETTIME:HH:MM:SS)
    if(commandBuffer[0] == '(' &&
       commandBuffer[1] == 'S' &&
       commandBuffer[2] == 'E' &&
       commandBuffer[3] == 'T' &&
       commandBuffer[4] == 'T' &&
       commandBuffer[5] == 'I' &&
       commandBuffer[6] == 'M' &&
       commandBuffer[7] == 'E' &&
       commandBuffer[8] == ':') {
        
        if(parseTime(9, &h, &m, &s)) {
            setTime(h, m, s);
            
            // Prikaži na displeju
            clearDisplay();
            writeLine("Vreme:");
            newLine();
            writeChar((h / 10) + '0');
            writeChar((h % 10) + '0');
            writeChar(':');
            writeChar((m / 10) + '0');
            writeChar((m % 10) + '0');
            writeChar(':');
            writeChar((s / 10) + '0');
            writeChar((s % 10) + '0');
        } else {
            // Greška
            clearDisplay();
            writeLine("ERROR!");
        }
        
        clearCommandBuffer();
        return;
    }
    
    // (SETALARM:HH:MM:SS)
    if(commandBuffer[0] == '(' &&
       commandBuffer[1] == 'S' &&
       commandBuffer[2] == 'E' &&
       commandBuffer[3] == 'T' &&
       commandBuffer[4] == 'A' &&
       commandBuffer[5] == 'L' &&
       commandBuffer[6] == 'A' &&
       commandBuffer[7] == 'R' &&
       commandBuffer[8] == 'M' &&
       commandBuffer[9] == ':') {
        
        if(parseTime(10, &h, &m, &s)) {
            setAlarm(h, m, s);
            
            // Prikaži na displeju
            clearDisplay();
            writeLine("Alarm:");
            newLine();
            writeChar((h / 10) + '0');
            writeChar((h % 10) + '0');
            writeChar(':');
            writeChar((m / 10) + '0');
            writeChar((m % 10) + '0');
            writeChar(':');
            writeChar((s / 10) + '0');
            writeChar((s % 10) + '0');
        } else {
            clearDisplay();
            writeLine("ERROR!");
        }
        
        clearCommandBuffer();
        return;
    }
    
    // (STATUS)
    if(commandBuffer[0] == '(' &&
       commandBuffer[1] == 'S' &&
       commandBuffer[2] == 'T' &&
       commandBuffer[3] == 'A' &&
       commandBuffer[4] == 'T' &&
       commandBuffer[5] == 'U' &&
       commandBuffer[6] == 'S' &&
       commandBuffer[7] == ')') {
        
        current = getTime();
        alarm = getAlarmTime();
        
        clearDisplay();
        writeChar((current.hours / 10) + '0');
        writeChar((current.hours % 10) + '0');
        writeChar(':');
        writeChar((current.minutes / 10) + '0');
        writeChar((current.minutes % 10) + '0');
        writeChar(':');
        writeChar((current.seconds / 10) + '0');
        writeChar((current.seconds % 10) + '0');
        
        newLine();
        
    
        writeChar((alarm.hours / 10) + '0');
        writeChar((alarm.hours % 10) + '0');
        writeChar(':');
        writeChar((alarm.minutes / 10) + '0');
        writeChar((alarm.minutes % 10) + '0');
        writeChar(':');
        writeChar((alarm.seconds / 10) + '0');
        writeChar((alarm.seconds % 10) + '0');
        
        clearCommandBuffer();
        return;
    }
    
    // Nepoznata komanda
    if(commandIndex > 0) {
        clearDisplay();
        writeLine("Unknown");
        newLine();
        writeLine("command");
        clearCommandBuffer();
    }
}

void uart_rx_isr(void) interrupt 4 {
    uint8_t received;
	if(RI) {
        RI = 0;
        
       received	= SBUF;
        
        if(received == ')') {
            // Kraj komande
            commandBuffer[commandIndex++] = received;
            commandBuffer[commandIndex] = '\0';
            commandReady = 1;
        } else if(received == '(') {
            // Početak komande - resetuj buffer
            clearCommandBuffer();
            commandBuffer[commandIndex++] = received;
        } else if(commandIndex < BUFFER_SIZE - 1) {
            commandBuffer[commandIndex++] = received;
        } else {
            // Buffer overflow
            clearCommandBuffer();
        }
    }
}