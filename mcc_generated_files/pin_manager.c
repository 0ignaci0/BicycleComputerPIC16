#include "pin_manager.h"

void (*IOCCF7_InterruptHandler)(void);


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATA = 0x00;
    LATB = 0x00;
    LATC = 0x00;

    /**
    TRISx registers
    */
    TRISA = 0x37;
    TRISB = 0x70;
    TRISC = 0xFF;

    /**
    ANSELx registers
    */
    ANSELC = 0x4F;
    ANSELB = 0x70;
    ANSELA = 0x17;

    /**
    WPUx registers
    */
    WPUB = 0x00;
    WPUA = 0x00;
    //set weal pull up for port c 7
    WPUC = 0x80;
    OPTION_REGbits.nWPUEN = 0;

    /**
    ODx registers
    */
    ODCONA = 0x00;
    ODCONB = 0x00;
    ODCONC = 0x00;

    /**
    SLRCONx registers
    */
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;


    /**
    IOCx registers 
    */
    //interrupt on change for group IOCCF - flag
    IOCCFbits.IOCCF7 = 0;
    //interrupt on change for group IOCCN - negative
    IOCCNbits.IOCCN7 = 0;
    //interrupt on change for group IOCCP - positive
    IOCCPbits.IOCCP7 = 1;

    // set slew rate to max for port c 7
    SLRC7 = 0;
    
    // set TTL for PORTC7
    INLVLC7 = 1 ;

    // register default IOC callback functions at runtime; use these methods to register a custom function
    IOCCF7_SetInterruptHandler(IOCCF7_DefaultInterruptHandler);
   
    // Enable IOCI interrupt 
    INTCONbits.IOCIE = 1; 
    
	
    RB7PPS = 0x12;   //RB7->EUSART:TX;    
}
  
void PIN_MANAGER_IOC(void)
{   
	// interrupt on change for pin IOCCF7
    if(IOCCFbits.IOCCF7 == 1)
    {
        IOCCF7_ISR();  
    }	
}

/**
   IOCCF7 Interrupt Service Routine
*/
void IOCCF7_ISR(void) {

    // Add custom IOCCF7 code

    // Call the interrupt handler for the callback registered at runtime
    if(IOCCF7_InterruptHandler)
    {
        IOCCF7_InterruptHandler();
    }
    IOCCFbits.IOCCF7 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF7 at application runtime
*/
void IOCCF7_SetInterruptHandler(void (* InterruptHandler)(void)){
    IOCCF7_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF7
*/
void IOCCF7_DefaultInterruptHandler(void){
    // add your IOCCF7 interrupt custom code
    // or set custom function using IOCCF7_SetInterruptHandler()
}

/**
 End of File
*/