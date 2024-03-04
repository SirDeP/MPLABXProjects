/*
 * File:   newmain.c
 * Author: sirdep
 *
 * Created on March 4, 2024, 10:30 AM
 */



// PIC16F1827 Configuration Bit Settings

// 'C' source line config statements
#define _XTAL_FREQ      500000  // Used by the XC8 delay_ms(x) macro (125 kHz)
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config DEBUG = OFF      // In-Circuit Debugger Mode (In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead o500000  //f #define for ON and OFF.

#include <xc.h>

#define BUTTON1 PORTBbits.RB0
#define LED1 PORTBbits.RB4

void pic_init(void);
void gpio_init(void);
void osc_init(void);

void main(void)
{
    pic_init();
    while(1)
    {
        
        if (BUTTON1)
            LED1 = 1;
        else
            LED1= 0;
    }
}



void gpio_init(void)
{
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB4 = 0;
    LED1 = 0;
    
}

void osc_init(void)
{
    OSCCONbits.SCS = 0b10;
    OSCCONbits.IRCF = 0b0111; // Clockspeed of 500 kHz
    OSCCONbits.SPLLEN = 0;
}

void pic_init(void)
{
    gpio_init();
    osc_init();
}

void __interrupt() isr(void)
{
    
}