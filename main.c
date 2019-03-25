#include "mcc_generated_files/mcc.h"    // MPLAB Configurator; contains initialization routines
#include "displayOptions.h"             // functions for modifying LCD parameters and writing to display
#define CIRCUMFERENCE 2                 // circumference of 700x23C bicycle wheel is 210 cm --> rounded to 2 meters

// constant defines for modifying LCD
#define configMode      0x7C 
#define redOff          0x80 
#define redOn           0x9D
#define greenOff        0x9E 
#define greenOn         0xBB
#define blueOff         0xBC 
#define blueOn          0xD9
#define offSet          15

long int volatile counter  = 0 ;            // overflow counter for 1-ms timer
float    volatile rpm      = 0 ;            // equal to interval between wheel rotations converted to minute scale
float    volatile speed    = 0 ;            // 
float    volatile distance = 0 ;

//~~~~~ Interrupt prototypes ~~~~~~//
    void timerISR  ( void ) ;               // 1-ms timer with "counter"
    void speedCalc ( void ) ;               // Interrupt-on-change routine for hall-effect sensor
//~~~~~~~~~~~~~~~~~~~~~~~~//


//~~~~~ LCD strings ~~~~~~//
    char speedDisp[]     = "Speed: " ;      
    char distanceDisp[]  = "Distance: " ;   
    char hrDisp[]        = "Heart Rate: " ; 
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
    
    // initialize display    //set cursor to
    setCursor(1,0) ;         // row 1, origin
    writeString(speedDisp);     
    setCursor(2,0) ;         // row 2, position 0 
    writeString(distanceDisp);
    setCursor(3,0) ;         // row 3
    writeString(hrDisp);
    setCursor(4,0) ;         // row 4
    
    // Enable the Global Interrupts, Enable the Peripheral Interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    while(1){
        clearLine(1);
        setCursor(1,10) ;
        int speedInt = speed ;
        printf( "%d kmh", speedInt) ;
        
        clearLine(2);
        setCursor(2,11) ;
        int distInt = distance ; 
        printf("%d m", distInt ) ;


    }
 
}

void timerISR ( void ){ // timer set to 1ms period
    counter++ ;
}

void speedCalc ( void ){
    // calculate rotations per minute by multiplying by 60 sec/min
    rpm      = ( counter / 1000 ) * 60 ;            // counter increments every 1 milli-sec, therefore divide by 1000 to achieve seconds
    
    //  speed = ( (2*pi*r) * rpm ) * (60 min/1 hr) * (1 km/1000 m) ; w/ circumference = 2*pi*r
    //  up-convert to kilometer, hour: multiply by (1 km / 1000 m) * ( 60 min / 1 hr ) = 6/100 = 3/50 = 0.06
    speed    = CIRCUMFERENCE * rpm * (0.06) ; // km/hr
    
    // reset counter, keep track of distance
    counter  = 0 ;
    distance = distance + CIRCUMFERENCE ; // distance in meters
}


/**
 End of File
*/






