# Bicycle Computer: Speed, Distance and Heart Rate     

A semester project for ECE471: Embedded Systems, Spring 2019 at Purdue School of Engineering&Technology, IUPUI  

This project is written for Microchip PIC16F1619 microcontroller. The goal is to prototype and 
demonstrate proof of concept operation of a bicycle computer.

Functionality
------
The bicycle computer provides the following measurements, displayed to a handle-bar mounted 
LCD:

- Speed
- Distance travelled
- Rider heart-rate

Hardware
------
The following hardware is used for this project

- Microchip PIC16F1619
- Sparkfun 20x4 SerLCD
- World famous Electronics LLC Pulse Sensor
- AH1815 Non-latching Hall-Effect Sensor
- Magnet

Principle of Operation
------
1. Speed

   Bicycle speed calculation is acheived using interrupt-on-change function of a GPIO pin in 
   conjunction with a free-running 8-bit timer, both features of the PIC16. On each rotation, a 
   magnet attached to a spoke of the bicycle wheel passes near a stationary hall-effect sensor 
   attached to the seat-stay of the bicycle frame. On this event, an interrupt is triggered on 
   the PIC16. This routine calculates the speed of the bicycle by dividing the product of the 
   wheels circumference and its rotations per minute by the elapsed time between interrupts.

2. Distance travelled

   In the same routine as above, an ongoing count of distance travelled is acheived by summing 
   the wheel circumference on each rotation.

3. Rider heart-rate 

   Uses the World Famous Electronics LLC Pulse Sensor and the PIC16 analog-digital converter. 
   Arduino routine ported to PIC microprocessor. Adapted from work done by World Famouse Electronics LLC (http://www.pulsesensor.com), CircuitDigest 
   (https://circuitdigest.com/microcontroller-projects/heartbeat-monitoring-using-pic-microcontroller-and-pulse-sensor), and Segun Ilori (https://libstock.mikroe.com/projects/view/951/pic-heartrate-monitor).

4. Display

    All sensor data is displayed to user via a 4 row, 20 column dot-matrix LCD. The Sparkfun 
    SerLCD is a fully integrated display package with its own on-board ATmega328P controller 
    handling text formatting and display. The PIC16 simply sends sensor data and text over UART.

