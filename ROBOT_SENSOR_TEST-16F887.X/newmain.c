/*
 * File:   newmain.c
 * Author: sirdep
 *
 * Created on November 3, 2023, 11:54 AM
 */

// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ      8000000 // Used by the XC8 delay_ms(x) macro
// 'C' source line config statements

// CONFIG1
#pragma config FOSC  = INTRC_NOCLKOUT // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE  = OFF            // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON             // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF            // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP    = OFF            // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD   = OFF            // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON             // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO  = OFF            // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF            // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP   = OFF            // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define LED PORTAbits.RA0

#include <xc.h>

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void init_timers(void);


void main(void)
{
    pic_init();
    while (1) {
        if (PORTDbits.RD1 == 1)
        {
            PORTDbits.RD2 = 1;
        } else {
            PORTDbits.RD2 = 0;
        }
    }
}

void pic_init(void)
{
    init_osc();
    init_gpio();
    init_timers();
}

void init_gpio(void)
{
    TRISDbits.TRISD1 = 1;
    TRISDbits.TRISD2 = 0;
    //TRISBbits.TRISB0 = 1;
    ANSEL = 0;
    LED = 1;
}

void init_osc(void)
{

}

void init_timers(void)
{

}

void __interrupt() isr(void)
{

}