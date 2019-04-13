#include "mcc_generated_files/mcc.h"    // MPLAB Configurator; contains initialization routines
#include "displayOptions.h"             // functions for modifying LCD parameters and writing to display
#define CIRCUMFERENCE 2                 // circumference of 700x23C bicycle wheel is 210 cm --> rounded to 2 meters
#define threshold 512                   // threshold value for detecting heart rate
#define peak 512                        // peak value for pulse wave
#define trough 512                      // trough value for pulse wave

//~~~~~ Variables associated with Speedometer Function ~~~~~~//
float    volatile counter    = 0 ;            // overflow counter for 1-ms timer
float    volatile rpm        = 0 ;            // equal to interval between wheel rotations converted to minute scale
float    volatile speed      = 0 ;            // floating point representation of calculated speed
int      volatile distance   = 0 ;            // raw integer value for distance calculation

int      volatile distHi     = 0 ;            // up-converted distance to kilometers
int      volatile distLo     = 0 ;            // contains fractional kilometer distance
int      volatile speedLo    = 0 ;            // integer representation of speed, fractional kilometers per hour
int      volatile speedHi    = 0 ;            // integer representation of speed, whole kilometers per hour
//~~~~~~~~~~~~~~~~~~~~~~~~//        //~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~ Variables associated with Heart Rate Function ~~~~~~//
int                 volatile rate[10];                          // array to hold last ten IBI values
adc_result_t        volatile P                 = peak ;         // used to find peak in pulse wave, seeded
adc_result_t        volatile T                 = trough ;       // used to find trough in pulse wave, seeded
int                 volatile thresh            = threshold ;    // used to find instant moment of heart beat, seeded
int                 volatile amp               = 100;           // used to hold amplitude of pulse waveform, seeded
int                 volatile BPM;                               // int that holds raw Analog in 0. updated every 2mS
adc_result_t        volatile Signal;                            // holds the incoming raw data
int                 volatile IBI               = 600;           // int that holds the time interval between beats! Must be seeded!
unsigned long int   volatile sampleCounter     = 0;             // used to determine pulse timing
unsigned long int   volatile lastBeatTime      = 0;             // used to find IBI
bool                volatile firstBeat         = true;          // used to seed rate array so we startup with reasonable BPM
bool                volatile secondBeat        = false;         // used to seed rate array so we startup with reasonable BPM
bool                volatile  Pulse            = false;         // "True" when User's live heartbeat is detected. "False" when not a "live beat".
//~~~~~~~~~~~~~~~~~~~~~~~~//        //~~~~~~~~~~~~~~~~~~~~~~~~//

//~~~~~ Interrupt prototypes ~~~~~~//
    void timerISR  ( void ) ;               // 1-ms timer with "counter"
    void speedCalc ( void ) ;               // Interrupt-on-change routine for hall-effect sensor
    void heartRateISR ( void ) ;
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

    TMR0_SetInterruptHandler( timerISR ) ;      // set 1-ms timer interrupt routine for speedometer interval
    TMR1_SetInterruptHandler( heartRateISR ) ;  // set 2-ms timer interrupt routine for heart rate calculation
    IOCCF7_SetInterruptHandler( speedCalc ) ;   // set interrupt-on-change routine for speedometer calculation

    // wait for LCD to boot up before sending any commands or data
    __delay_ms(2000) ;
    
    // clear screen, set cursor to origin    
    resetCursor() ;
    
    // Enable the Global Interrupts, Enable the Peripheral Interrupts, start 2ms timer for heart rate sensor
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    TMR1_StartTimer ; 
    
    while(1){
        
        //refresh data labels on display
        setCursor(1,0) ;         
        writeString(speedDisp);     
        setCursor(2,0) ;         
        writeString(distanceDisp);
        setCursor(3,0) ;         
        writeString(hrDisp);
        setCursor(4,0) ;                

        // print speed
        setCursor(1,7) ;
        if( counter <= 15000 ) {  // minimum speed displayed is 1/2 kph, upper counter limit is 15000    
            printf( "%d.%d kph     ", speedHi, speedLo ) ;
        }   
        else { // if counter increments to 15000 or larger, 15 seconds have passed with no wheel rotation, assume zero speed.
            printf( "0 kph        " ) ;  
        }
        
        // print distance
        setCursor(2,10) ;
        printf( "%d.%d km" , distHi, distLo );
        
        // print heart-rate
        setCursor(3,13) ;
        printf( "%d bpm   " , BPM ) ;
    }
 
}

void timerISR ( void ){ // timer set to 1ms period, count contains number of milliseconds between wheel rotations
    
    counter++ ;
}

void speedCalc ( void ){
    
    // counter increments every 1 milli-sec, divide 1000 by counter for scaling to RPM
    rpm      = ( ( 1000 / counter ) * 60 )  ;            
    // up-convert to kilometer per hour: multiply by (1 km / 1000 m) * ( 60 min / 1 hr ) = 6/100 = 3/50 = 0.06
    speed    = CIRCUMFERENCE * rpm * (0.06) ; // km/hr
    
    // convert float to integer, save decimation for display
    speedHi = speed ;
    speedLo = ( (int)(speed*10.0) % 10 ) ; // preserve fractional speed to the tens place
    
    // reset counter, keep track of distance
    counter  = 0 ;
    distance = distance + CIRCUMFERENCE ; // distance in meters
    // convert distance to decimated kilometers for display
    distLo = distance % 1000 ;
    distHi = distance / 1000 ; 

}

void heartRateISR ( void ){
    
    Signal = ADC_GetConversion( channel_AN4 ) ;
    sampleCounter += 2;                                 // increments every 2-ms
    int N = sampleCounter - lastBeatTime;               // interval between detected heart beats

    //  find the peak and trough of the pulse wave
    if ( Signal < thresh && N > ( IBI / 5 ) * 3 ) {     // avoid dichrotic noise by waiting 3/5 of last IBI
        if ( Signal < T ) {                             // T is the trough
            T = Signal;                                 // keep track of lowest point in pulse wave
        }
    }
        if ( Signal > thresh && Signal > P ) {          // thresh condition helps avoid noise
        P = Signal;                                     // P is the peak
    }                                                   // keep track of highest point in pulse wave

   
    // look for increasing ADC values, which indicate a sensed pulse
    if ( N > 250 ) {                                    // avoid high frequency noise by waiting for 250-ms
        if ( ( Signal > thresh ) && ( Pulse == false ) && ( N > ( IBI / 5 ) * 3 ) ) {
            Pulse = true;                               // set pulse flag to true when ADC signal exceeds threshold
            IBI = sampleCounter - lastBeatTime;         // measure time between beats in ms
            lastBeatTime = sampleCounter;               // keep track of time for next pulse

            if ( secondBeat ) {                         // if this is the second beat, if secondBeat == TRUE
                secondBeat = false;                     // clear secondBeat flag
                int i;
                for (i = 0; i <= 9; i++) {              // seed the running total to get a realistic BPM at startup
                    rate[i] = IBI;
                }
            }

            if ( firstBeat ) {                          // if it's the first time we found a beat
                firstBeat = false; 
                secondBeat = true; 
                return;                                 // IBI value is unreliable so discard it
            }

            // keep a running total of the last 10 IBI values
            int runningTotal = 0; // clear the runningTotal variable
            int i;
            for (i = 0; i <= 8; i++) {                  // shift data in the rate array
                rate[i] = rate[i + 1];                  // and drop the oldest IBI value
                runningTotal += rate[i];                // add up the 9 oldest IBI values
            }

            rate[9] = IBI;                              // add the latest IBI to the rate array
            runningTotal += rate[9];                    // add the latest IBI to runningTotal
            runningTotal /= 10;                         // average the last 10 IBI values
            BPM = 60000 / runningTotal;                 // up-convert running total to beats per minute
        }
    }
    
    // look for decreasing ADC values, which indicate the end of a pulse interval
    if (Signal < thresh && Pulse == true) { 
        Pulse = false;                                  // reset the Pulse flag for next interval
        amp = P - T;                                    // pulse amplitude is equal to difference between peak and trough
        thresh = amp / 2 + T;                           // set threshold at 50% of the amplitude
        P = thresh;                                     // reinitialize peak and trough for next pulse interval
        T = thresh;
    }
    
    // if 2.5 seconds go by without a beat, reset peak, trough, threshold and first/second beat flags
    if (N > 2500) { 
        thresh = threshold ; 
        P = peak ; 
        T = trough ; 
        lastBeatTime = sampleCounter; 
        firstBeat = true; 
        secondBeat = false;    
    }
}


/**
 End of File
*/






