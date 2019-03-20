#include "interrupt_manager.h"
#include "mcc.h"

void __interrupt() INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.TMR0IE == 1 && INTCONbits.TMR0IF == 1)
    {
        TMR0_ISR();
    }
    else if(INTCONbits.IOCIE == 1 && INTCONbits.IOCIF == 1)
    {
        PIN_MANAGER_IOC();
    }
    else
    {
        //Unhandled Interrupt
    }
}
