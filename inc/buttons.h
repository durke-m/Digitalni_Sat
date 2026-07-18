#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <at89c51rc2.h>

// Pin definicije (prilagodi svojim pinovima)
#define BUTTON_SET   P0_0
#define BUTTON_INC    P0_1
#define BUTTON_DEC  P0_2
#define BUTTON_CONFIRM    P0_3

// Tipovi tastera
typedef enum {
    KEY_NONE = 0,
    KEY_SET,
    KEY_INC,
    KEY_DEC,
    KEY_CONFIRM
} Key;

// Režimi rada
typedef enum {
    MODE_NORMAL = 0,
    MODE_SET_TIME_H,
    MODE_SET_TIME_M,
    MODE_SET_TIME_S,
    MODE_SET_ALARM_H,
    MODE_SET_ALARM_M,
    MODE_SET_ALARM_S
} Mode;

// Funkcije
void keypadInit(void);
Key keypadScan(void);
Mode getMode(void);
void keypadProcess(void);
static void displayEditValue(uint8_t value);  // Poziva se iz main loop-a

#endif