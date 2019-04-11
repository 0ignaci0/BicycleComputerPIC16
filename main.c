#include "mcc_generated_files/mcc.h"    // MPLAB Configurator; contains initialization routines
#include "displayOptions.h"             // functions for modifying LCD parameters and writing to display
#define CIRCUMFERENCE 2                 // circumference of 700x23C bicycle wheel is 210 cm --> rounded to 2 meters


float    volatile counter    = 0 ;            // overflow counter for 1-ms timer
float    volatile rpm        = 0 ;            // equal to interval between wheel rotations converted to minute scale
float    volatile speed      = 0 ; 
int      volatile distance   = 0 ;
float    volatile heartRate  = 0 ;

int      volatile distHi     = 0 ;
int      volatile distLo     = 0 ;
int      volatile speedLo    = 0 ;
int      volatile speedHi    = 0 ;

int      volatile hrInt      = 0 ;
long int volatile adcCounter = 0 ;

int      volatile adcVal     = 0 ;
int      volatile rate[10];                    // array to hold last ten IBI values
int      volatile P          = 512 ;                      // used to find peak in pulse wave, seeded
int      volatile T          = 512 ;                      // used to find trough in pulse wave, seeded
int      volatile thresh     = 530 ;             // used to find instant moment of heart beat, seeded
int      volatile amp        = 0;                      // used to hold amplitude of pulse waveform, seeded
int      volatile BPM;                              // int that holds raw Analog in 0. updated every 2mS
int      volatile Signal;                                   // holds the incoming raw data
int      volatile IBI        = 600;                     // int that holds the time interval between beats! Must be seeded!
unsigned long int   volatile sampleCounter     = 0;       // used to determine pulse timing
unsigned long int   volatile lastBeatTime      = 0;        // used to find IBI
bool     volatile firstBeat     = true;                     // used to seed rate array so we startup with reasonable BPM
bool     volatile secondBeat    = false;               // used to seed rate array so we startup with reasonable BPM
bool     volatile  Pulse        = false;            // "True" when User's live heartbeat is detected. "False" when not a "live beat".
bool     volatile QS            = false;            // becomes true when finds a beat.

//~~~~~ Interrupt prototypes ~~~~~~//
    void timerISR  ( void ) ;               // 1-ms timer with "counter"
    void speedCalc ( void ) ;               // Interrupt-on-change routine for hall-effect sensor
    void heartBeatCalc ( int adcVal );    
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

    // wait for LCD to boot up before sending any commands or data
    __delay_ms(1000) ;
    // clear screen, set cursor to origin    
    resetCursor() ;
    // Enable the Global Interrupts, Enable the Peripheral Interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    while(1){
        
        //refresh data labels on display
        setCursor(1,0) ;         
        writeString(speedDisp);     
        setCursor(2,0) ;         
        writeString(distanceDisp);
        setCursor(3,0) ;         
        writeString(hrDisp);
        setCursor(4,0) ;                
        
        // every 2ms calculate heart rate
        if (adcCounter == 2){ 
            // get_heart_rate
            ADC_StartConversion();
            adcVal = ADC_GetConversionResult() ;
            heartBeatCalc(adcVal) ;
            adcCounter = 0 ;
        }
        
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
    adcCounter++ ; // counter for triggering ADC routine to collect heart rate sensor data.

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

void heartBeatCalc ( int adcVal ){
    Signal = adcVal ;
    sampleCounter += 2; // keep track of the time in mS with this variable
    int N = sampleCounter - lastBeatTime; // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
    if (Signal < thresh && N > (IBI / 5)*3) { // avoid dichrotic noise by waiting 3/5 of last IBI
        if (Signal < T) { // T is the trough
            T = Signal; // keep track of lowest point in pulse wave
        }
    }
        if (Signal > thresh && Signal > P) { // thresh condition helps avoid noise
        P = Signal; // P is the peak
    } // keep track of highest point in pulse wave

    //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
    // signal surges up in value every time there is a pulse
    if (N > 250) { // avoid high frequency noise
        if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5)*3)) {
            Pulse = true; // set the Pulse flag when we think there is a pulse
            IBI = sampleCounter - lastBeatTime; // measure time between beats in mS
            lastBeatTime = sampleCounter; // keep track of time for next pulse

            if (secondBeat) { // if this is the second beat, if secondBeat == TRUE
                secondBeat = false; // clear secondBeat flag
                int i;
                for (i = 0; i <= 9; i++) { // seed the running total to get a realisitic BPM at startup
                    rate[i] = IBI;
                }
            }

            if (firstBeat) { // if it's the first time we found a beat, if firstBeat == TRUE
                firstBeat = false; // clear firstBeat flag
                secondBeat = true; // set the second beat flag
                //pulse_tmr_handle = bsp_harmony_start_tmr_cb_periodic(PULSE_CHECK_TIME_INTERVAL, 0, pulse_read_cb); // enable interrupts again
                return; // IBI value is unreliable so discard it
            }

            // keep a running total of the last 10 IBI values
            uint16_t runningTotal = 0; // clear the runningTotal variable
            int i;
            for (i = 0; i <= 8; i++) { // shift data in the rate array
                rate[i] = rate[i + 1]; // and drop the oldest IBI value
                runningTotal += rate[i]; // add up the 9 oldest IBI values
            }

            rate[9] = IBI; // add the latest IBI to the rate array
            runningTotal += rate[9]; // add the latest IBI to runningTotal
            runningTotal /= 10; // average the last 10 IBI values
            BPM = 60000 / runningTotal; // how many beats can fit into a minute? that's BPM!
            QS = true; // set Quantified Self flag
            // QS FLAG IS NOT CLEARED INSIDE THIS ISR
        }
    }
    if (Signal < thresh && Pulse == true) { // when the values are going down, the beat is over
        Pulse = false; // reset the Pulse flag so we can do it again
        amp = P - T; // get amplitude of the pulse wave
        thresh = amp / 2 + T; // set thresh at 50% of the amplitude
        P = thresh; // reset these for next time
        T = thresh;
    }

    if (N > 2500) { // if 2.5 seconds go by without a beat
        thresh = 530; // set thresh default
        P = 512; // set P default
        T = 512; // set T default
        lastBeatTime = sampleCounter; // bring the lastBeatTime up to date
        firstBeat = true; // set these to avoid noise
        secondBeat = false; // when we get the heartbeat back
    }

}


/**
 End of File
*/






