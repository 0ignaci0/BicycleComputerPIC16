//test

#include "mcc_generated_files/mcc.h"    // MPLAB Configurator; contains initialization routines
#include "displayOptions.h"             // functions for modifying LCD parameters and writing to display
#define CIRCUMFERENCE 2             // circumference of 700x23C bicycle wheel is 210 cm ; rounded to 2 meters

// constant defines for modifying LCD
#define configMode      0x7C 
#define redOff          0x80 
#define redOn           0x9D
#define greenOff        0x9E 
#define greenOn         0xBB
#define blueOff         0xBC 
#define blueOn          0xD9
#define offSet          15

long int rpm      = 0 ;
long int second   = 0 ;
long int counter  = 0 ;
long int speed    = 0 ;
long int distance = 0 ;
long int km       = 0 ;

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
    IOCCF7_SetInterruptHandler( speedCalc ) ;

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
        printf( "   %d kmh    ", speed ) ;
        setCursor(2,11) ;
        if ( distance < 1000 ){
            printf("   %d m  ", distance ) ;
        }
        else if ( distance % 1000 == 0){
            km++ ;
            printf("  %d km  ", km ) ;
        }
    }
 
}

void timerISR ( void ){ // timer set to 1ms period
    counter++ ;
    if ( counter % 2 == 0 ){ // every 1 second
        second++ ;
    }  

}

void speedCalc ( void ){
    // calculate rotations per minute by multiplying by 60 sec/min
    rpm      = second * 60 ;    // counter increments every 1 milli-sec, therefore divide by 1000 to achieve seconds
    // calculate speed in mi/hr; speed = ( (2*pi*r) * rpm ) * (60 min/1 hr) * (1 km/1000 m)
    // below  circum * rpm gives m per min ; to get km/hr, multiply by (1 km / 1000 m) * ( 60 min / 1 hr ) = 6/100 = 3/50 = 0.06
    speed    = CIRCUMFERENCE * rpm * (0.06) ; // km/hr
    counter  = 0 ;
    second   = 0 ;
    distance = distance + CIRCUMFERENCE ; // distance in meters
}


/**
 End of File
*/






