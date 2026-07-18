#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <at89c51rc2.h>

#define BUFFER_SIZE 20

extern unsigned char commandBuffer[BUFFER_SIZE];
extern unsigned char commandIndex;
extern bit commandReady;

void commandInit(void);
void parseCommand(void);
void clearCommandBuffer(void);
void uart_rx_isr(void);

#endif
