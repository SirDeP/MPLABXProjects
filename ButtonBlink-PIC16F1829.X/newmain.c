/***********************************************************************
 * Universal startup code structure for new and simple projects
 * 
 * PIC   : 16F1829
 * Board : PICkit 3 Low Pin Count Demo Board (PK3LPCDB) or breadboard
 * Date  : A long time ago, in a galaxy far away
 * 
 **********************************************************************/

#include <xc.h>                 // PIC hardware mapping
// This auto-determines (using sub-includes, #pragma's, and IDE-
// controlled data sources) that 
// /opt/microchip/xc8/v<compiler_version>/pic/include/proc/pic16f1829.h
// should be included ...
// As for Windoze, `/opt' is probably somewhere in the user's AppData 
// folder ..



#define _XTAL_FREQ      500000  // Used by the XC8 delay_ms(x) macro

// CONFIG WORD 1 @ flash program memory location 0x8007
// For acceptable values use both datasheet and this file:
// /opt/microchip/xc8/v<compiler_version>/docs/pic_chipinfo.html

#pragma config FOSC     = 0x4   // xor INTOSC (name from IDE database)
#pragma config WDTE     = 0x0   // OFF
#pragma config PWRTE    = 0x1   // OFF
#pragma config MCLRE    = OFF   // etc.
#pragma config CP       = OFF   // text is the most readable option
#pragma config CPD      = OFF
#pragma config BOREN    = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO     = OFF
#pragma config FCMEN    = OFF

// XOR (not recommended)
// #pragma config CONFIG1 = 0b (00)00 1111 1010 0100
// i.e. (don't do this)
// #pragma config CONFIG1 = 0x0FA4

// CONFIG WORD 2 @ flash program memory location 0x8008
#pragma config WRT      = OFF
#pragma config PLLEN    = OFF
#pragma config STVREN   = OFF
#pragma config BORV     = LO
//#pragma config DEBUG    = OFF   // Pre-processor short-circuits !
#pragma config LVP      = OFF

/* Hardware / state naming scheme ------------------------------------*/
#define LED LATCbits.LATC0
#define BUTTON PORTAbits.RA2
// ...

/* End-of hardware / state naming scheme -----------------------------*/

//>>

/* Forward function declarations + main function hereafter -----------*/

void pic_init(void);
void init_osc(void);
void led_blink(int counter);
// ... more init function declarations here, one per each subsystem !
void init_gpio(void);
// ... and even more function declarations here, for module behaviors !

void main(void)
    {
    // Do all initialisation here
    pic_init();
    int counter = 0;
    while (1)
        {
        // Main program loop here
        switch (BUTTON)
            {
            case 0:
                counter++;
                led_blink(counter);
                if (counter > 9)
                    {
                    counter = 0;
                    }
                break;
            }
        }
    }

/* End-of function declarations + main function --------------------- */

//>>

/* Init functions hereafter -- may be modularized later --------------*/

void pic_init(void)
    {
    init_osc();
    init_gpio();
    }

void init_osc(void)
    {
    // Do it like this, it's easily compared to the datasheet:
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b00; // Clk determined by FOSC in Conf-1
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b0111; // 500 kHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
    // XOR (Don't do this, it's unmaintainable)
    // OSCCON = 0b00111000;
    }

void init_gpio(void)
    {
    TRISCbits.TRISC0 = 0;
    TRISAbits.TRISA2 = 1;
    ANSELAbits.ANSA2 = 0;
    LED = 0;
    }
/* End-of Init functions -------------------------------------------- */

//>>

/* Utility functions per module hereafter -- may be modularized later!*/
void led_blink(int counter)
    {
    for (int i = 0; i < counter; i++)
        {
        LED = 1;
        __delay_ms(250); // 1sec delay
        LED = 0;
        __delay_ms(250); // 1sec delay
        }
    }
// ...

/* End-of utility functions per module -------------------------------*/

//>>

/* Interrupt Service Routine (ISR) hereafter -- keep with main module!*/

void __interrupt() isr();

/* End-of Interrupt Service Routine (ISR) ----------------------------*/

//>>
