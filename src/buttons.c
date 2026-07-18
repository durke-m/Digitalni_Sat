#include "buttons.h"
#include "clock.h"
#include "display.h"


static Mode currentMode = MODE_NORMAL;
static uint8_t editValueH = 0;
static uint8_t editValueM = 0;
static uint8_t editValueS = 0;


void keypadInit(void) {
    
    P0 |= 0x0F;  
}

// Debouncing + detekcija tastera
Key keypadScan(void) {
    static uint8_t lastKey = KEY_NONE;
    static uint8_t debounceCounter = 0;
    
    Key currentKey = KEY_NONE;
    
    // Čitaj tastere (active LOW)
    if(BUTTON_SET == 0) currentKey = KEY_SET;
    else if(BUTTON_INC == 0) currentKey = KEY_INC;
    else if(BUTTON_DEC == 0) currentKey = KEY_DEC;
    else if(BUTTON_CONFIRM == 0) currentKey = KEY_CONFIRM;
    
    // Debouncing logika
    if( currentKey != KEY_NONE && currentKey == lastKey) {
        debounceCounter++;
        if(debounceCounter > 3) {  // ~30ms debounce
            debounceCounter = 0;
            
            // Čekaj release
            while(BUTTON_SET == 0 || BUTTON_INC == 0 || BUTTON_DEC == 0 || BUTTON_CONFIRM == 0);
            
            return currentKey;
        }
    } else {
        debounceCounter = 0;
    }
    
    lastKey = currentKey;
    return KEY_NONE;
}

Mode getMode(void) {
    return currentMode;
}

static void displayEditValue(uint8_t value) {
    setCursor(1, 0);
    writeChar((value / 10) + '0');
    writeChar((value % 10) + '0');
}

void keypadProcess(void) {
    Key key = keypadScan();
    Time currentTime, currentAlarm;
    
    if(key == KEY_NONE) return;
    
    // State machine
    switch(currentMode) {
        
        case MODE_NORMAL:
            if(key == KEY_SET) {
                // Ulazak u edit režim
                currentTime = getTime();
                editValueH = currentTime.hours;
                editValueM = currentTime.minutes;
                editValueS = currentTime.seconds;
                currentMode = MODE_SET_TIME_H;
                
                clearDisplay();
                
            }
            break;
        
        case MODE_SET_TIME_H:
            if(key == KEY_INC) {
                editValueH++;
                if(editValueH >= 24) editValueH = 0;
                displayEditValue(editValueH);
            }
            else if(key == KEY_DEC) {
                if(editValueH == 0) editValueH = 23;
                else editValueH--;
                displayEditValue(editValueH);
            }
            else if(key == KEY_CONFIRM) {
                currentMode = MODE_SET_TIME_M;
                clearDisplay();
                
            }
            
            // Prikaži trenutnu vrednost
          //  setCursor(1, 0);
            //writeChar((editValueH / 10) + '0');
           // writeChar((editValueH % 10) + '0');
            break;
        
        case MODE_SET_TIME_M:
            if(key == KEY_INC) {
                editValueM++;
                if(editValueM >= 60) editValueM = 0;
                displayEditValue(editValueM);
            }
            else if(key == KEY_DEC) {
                if(editValueM == 0) editValueM = 59;
                else editValueM--;
                displayEditValue(editValueM);
            }
            else if(key == KEY_CONFIRM) {
                currentMode = MODE_SET_TIME_S;
                clearDisplay();
            
            }
            
       //     setCursor(1, 0);
         //   writeChar((editValueM / 10) + '0');
           // writeChar((editValueM % 10) + '0');
            break;
        
        case MODE_SET_TIME_S:
            if(key == KEY_INC) {
                editValueS++;
                if(editValueS >= 60) editValueS = 0;
                displayEditValue(editValueS);
            }
            else if(key == KEY_DEC) {
                if(editValueS == 0) editValueS = 59;
                else editValueS--;
                displayEditValue(editValueS);
            }
            else if(key == KEY_CONFIRM) {
                // Potvrdi vreme
                setTime(editValueH, editValueM, editValueS);
                
                // Pređi na podešavanje alarma
                currentAlarm = getAlarmTime();
                editValueH = currentAlarm.hours;
                editValueM = currentAlarm.minutes;
                editValueS = currentAlarm.seconds;
                currentMode = MODE_SET_ALARM_H;
                
                clearDisplay();
                break;
            }
            
           // setCursor(1, 0);
            //writeChar((editValueS / 10) + '0');
            //writeChar((editValueS % 10) + '0');
            
            break;
        
        case MODE_SET_ALARM_H:
            if(key == KEY_INC) {
                editValueH++;
                if(editValueH >= 24) editValueH = 0;
                displayEditValue(editValueH);
            }
            else if(key == KEY_DEC) {
                if(editValueH == 0) editValueH = 23;
                else editValueH--;
                displayEditValue(editValueH);
            }
            else if(key == KEY_CONFIRM) {
                currentMode = MODE_SET_ALARM_M;
                clearDisplay();
            }
            
         //   setCursor(1, 0);
           // writeChar((editValueH / 10) + '0');
            //writeChar((editValueH % 10) + '0');
            break;
        
        case MODE_SET_ALARM_M:
            if(key == KEY_INC) {
                editValueM++;
                if(editValueM >= 60) editValueM = 0;
                displayEditValue(editValueM);
            }
            else if(key == KEY_DEC) {
                if(editValueM == 0) editValueM = 59;
                else editValueM--;
                displayEditValue(editValueM);
            }
            else if(key == KEY_CONFIRM) {
                currentMode = MODE_SET_ALARM_S;
                clearDisplay();
            }
            
         //   setCursor(1, 0);
           // writeChar((editValueM / 10) + '0');
           // writeChar((editValueM % 10) + '0');
            break;
        
        case MODE_SET_ALARM_S:
            if(key == KEY_INC) {
                editValueS++;
                if(editValueS >= 60) editValueS = 0;
                displayEditValue(editValueS);
            }
            else if(key == KEY_DEC) {
                if(editValueS == 0) editValueS = 59;
                else editValueS--;
                displayEditValue(editValueS);
            }
            else if(key == KEY_CONFIRM) {
                // Potvrdi alarm
                setAlarm(editValueH, editValueM, editValueS);
                
                // Vrati se u normalan režim
                currentMode = MODE_NORMAL;
                
                clearDisplay();
                writeLine("Sacuvano!");
                wait1s();
            }
            
            currentTime = getTime();
            currentAlarm = getAlarmTime();
            clearDisplay();
                
            setCursor(0, 0);
            writeChar((currentTime.hours / 10) + '0');
            writeChar((currentTime.hours % 10) + '0');
            writeChar(':');
            writeChar((currentTime.minutes / 10) + '0');
            writeChar((currentTime.minutes % 10) + '0');
            writeChar(':');
            writeChar((currentTime.seconds / 10) + '0');
            writeChar((currentTime.seconds % 10) + '0');
                
            setCursor(1, 0);
            writeChar((currentAlarm.hours / 10) + '0');
            writeChar((currentAlarm.hours % 10) + '0');
            writeChar(':');
            writeChar((currentAlarm.minutes / 10) + '0');
            writeChar((currentAlarm.minutes % 10) + '0');
            writeChar(':');
            writeChar((currentAlarm.seconds / 10) + '0');
            writeChar((currentAlarm.seconds % 10) + '0');
            break;
    }
}