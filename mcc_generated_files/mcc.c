#include "mcc.h"


void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    TMR0_Initialize();
    EUSART_Initialize();
}

void OSCILLATOR_Initialize(void)
{
    // SCS INTOSC; SPLLEN disabled; IRCF 16MHz_HF; 
    OSCCON = 0x7A;
    // TUN 0; 
    OSCTUNE = 0x00;
    // SBOREN disabled; BORFS disabled; 
    BORCON = 0x00;
}



/**
 End of File
*/
