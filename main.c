

#include "mcc_generated_files/mcc.h"    // MPLAB Configurator; contains initialization routines
#include "displayOptions.h"             // functions for modifying LCD parameters and writing to display
#define _XTAL_FREQ 500000               
#define CIRCUMFERENCE 2.096             // circumference of 700x23C bicycle wheel in meters

// constant defines for modifying LCD
#define configMode      0x7C 
#define redOff          0x80 
#define redOn           0x9D
#define greenOff        0x9E 
#define greenOn         0xBB
#define blueOff         0xBC 
#define blueOn          0xD9
#define offSet          15

int rpm      = 0 ;
int second   = 0 ;
int counter  = 0 ;
int distance = 0 ;
int speed    = 0 ;

void timerISR  ( void ) ;
void speedCalc ( void ) ;


//~~~~~ LCD strings ~~~~~~//
    char speedDisp[]     = "Speed: " ;      // need to add a variable for calculated speed
    char distanceDisp[]  = "Distance: " ;   // add var for calc dist
    char hrDisp[] = "Heart Rate: " ; // add var for calc HR
//~~~~~~~~~~~~~~~~~~~~~~~~//

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    // set timer and interrupt-on-change handlers
    TMR0_SetInterruptHandler( timerISR ) ;
    IOCCF5_SetInterruptHandler( speedCalc ) ;

    // clear screen, set cursor to origin
    resetCursor() ;
    // initialize display   //set cursor to
    setCursor(1,0) ;         // row 1, origin
    writeString(speedDisp);     
    setCursor(2,0) ;        // row 2, position 0 
    writeString(distanceDisp);
    setCursor(3,0) ;        // row 3
    writeString(hrDisp);
    setCursor(4,0) ;        // row 4
    // Enable the Global Interrupts, Enable the Peripheral Interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    while(1){
        setCursor(1,10) ;
        printf( "%d", speed ) ;
        setCursor(2,11) ;      
        printf( "%d", distance ) ;
        
        
    };
}



void timerISR ( void ){
    counter++ ;
    if ( counter % 1000 == 0 ){ // every 1000ms count 1 second
        second++ ;
    }
}

void speedCalc ( void ){
    // calculate rotations per minute by multiplying by 60 sec/min
    rpm      = second * 60 ;    
    // calculate speed in km/hr; speed = ( (2*pi*r) * rpm ) * (60 min/1 hr) * (1 km/1000 m)
    // speed = circumference * rpm * 0.06
    speed    = CIRCUMFERENCE * rpm * 0.06 ; 
    second   = 0 ;
    distance = distance + CIRCUMFERENCE ;
}


/**
 End of File
*/






