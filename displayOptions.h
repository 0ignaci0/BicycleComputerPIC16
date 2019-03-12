/* 
 * File:   displayOptions.h
 * Author: R. Guerra
 * Comments: Header file for asynchronous serial communication between PIC16F1619
 *  and SparkFun SerLCD.
 * Revision history: 0.01
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#define configMode      0x7C
#define displayContrast 0x18
#define redOff          0x80 
#define redOn           0x9D
#define greenOff        0x9E 
#define greenOn         0xBB
#define blueOff         0xBC 
#define blueOn          0xD9

#include "mcc_generated_files/mcc.h"


/**
  @Summary
    Sets the specified RGB back-light to specified brightness.
  @Description
    This function sets the back-light for either red, green, or blue to the
    specified brightness.
  @Preconditions
    EUSART_Initialize() function should be called before this function.
  @Param 
    color: 1 = red, 2 = green, 3 = blue
  @Param 
    offset: integer value from 0 to 29, with 29 being 100% brightness.
  @Returns
    None
 */
void backlightBrightness( int color, uint8_t offset );


/**
  @Summary
    Resets the RGB back-light to default values
  @Description
    This function resets each RGB back-light to max brightness
  @Preconditions
    EUSART_Initialize() function should be called before this function
  @Param 
    none
  @Returns
    None
 */
void backlightReset();

/**
  @Summary
    Turns off all RGB back-lights
  @Description
    This function sets each RGB back-light to min brightness
  @Preconditions
    EUSART_Initialize() function should be called before this function
  @Param 
    none
  @Returns
    None
 */
void backlightOff();

/**
  @Summary
    Clears display, sets cursor to row and column 1.
  @Description
    This function sets each RGB back-light to min brightness
  @Preconditions
    EUSART_Initialize() function should be called before this function
  @Param 
    none
  @Returns
    None
 */
void resetCursor();

/**
  @Summary
  @Description
  @Preconditions
  @Param 
    none
  @Returns
    None
 */
void writeString();


/**
  @Summary
  @Description
  @Preconditions
  @Param a
    valued 1 - 4, corresponds to each row of LCD
  @Returns
    None
 */
void setCursor( uint8_t a, uint8_t b );

void writePrintf( char *string );

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

