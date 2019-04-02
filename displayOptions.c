/* 
 * File:   displayOptions.c
 * Author: igna
 *
 * Created on March 10, 2019, 2:01 PM
 */

#include "displayOptions.h"
#include "mcc_generated_files/mcc.h"


void backlightBrightness( int color, uint8_t offSet ) {
    
    if( offSet <= 29 ){
        switch (color){
            case 1: //red
            {
                EUSART_Write(configMode);
                EUSART_Write( redOff + offSet );
                break;
            }
            case 2: //green
            {
                EUSART_Write(configMode);
                EUSART_Write( greenOff + offSet );
                break;
            }
            case 3: //blue
            {
                EUSART_Write(configMode);
                EUSART_Write( blueOff + offSet );
                break;
            }
            default:
            {
            
            }
        }
    } 
}

void backlightReset(){
    backlightBrightness(1,29);
    backlightBrightness(2,29);
    backlightBrightness(3,29);
}

void backlightOff(){
    backlightBrightness(1,0);
    backlightBrightness(2,0);
    backlightBrightness(3,0);
}

void resetCursor(){
    puts("|-");
}

void writeString( char *string ){
    puts( string );
}

void writePrintf( char *string ){
    while( !EUSART_is_tx_ready() ) ;
    printf( string ) ;
}

void setCursor( uint8_t a, uint8_t b  ){
    switch ( a ){
        case 1: // row 1
        {
            EUSART_Write( 254 );
            EUSART_Write(128 + 0 + b) ;
            break ;
        }
        case 2: // row 2
        {
           EUSART_Write( 254 );
           EUSART_Write(128 + 64 + b) ;
           break ;
        }
        case 3: // row 3
        {
            EUSART_Write( 254 );
            EUSART_Write(128 + 20 + b) ; 
            break ;
        }
        case 4: // row 4
        {
            EUSART_Write( 254 );
            EUSART_Write(128 + 84 + b) ; 
            break ;
        }
    }
}
/*
 * 
 */


