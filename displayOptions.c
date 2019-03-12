/* 
 * File:   displayOptions.c
 * Author: igna
 *
 * Created on March 10, 2019, 2:01 PM
 */

#include "displayOptions.h"
#include "mcc_generated_files/mcc.h"

void backlightBrightness( int color, uint8_t offSet ) {
    EUSART_Write(configMode);
    if( offSet <= 29 ){
        switch (color){
            case 1: //red
            {
                while( !EUSART_is_tx_ready() ) ;
                EUSART_Write( redOff + offSet );
                __delay_us(104) ;
                break;
            }
            case 2: //green
            {
                while( !EUSART_is_tx_ready() ) ;
                EUSART_Write( greenOff + offSet );
                __delay_us(104) ;
                break;
            }
            case 3: //blue
            {
                while( !EUSART_is_tx_ready() ) ;
                EUSART_Write( blueOff + offSet );
                __delay_us(104) ;
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
    while( !EUSART_is_tx_ready() ) ;
    puts( string );
    __delay_us(200);
}

void setCursor( uint8_t a, uint8_t b  ){
    while( !EUSART_is_tx_ready() ) ;
    EUSART_Write( 254 );
    __delay_us(200);
    while( !EUSART_is_tx_ready() ) ;
    EUSART_Write( 128 + a + b );
    __delay_us(200);
}
/*
 * 
 */


