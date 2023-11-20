/***********************************************************************
 * 
 * PIC   : 16F1829
 * Date  : A long time ago, in a galaxy far away (20-11-'23)
 * Author: SirDeP
 * 
 **********************************************************************/

#include <xc.h>                 // PIC hardware mapping
#include "spi.h"

#define _XTAL_FREQ      4000000  // Used by the XC8 delay_ms(x) macro (4 MHz)

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

// CONFIG WORD 2 @ flash program memory location 0x8008
#pragma config WRT      = OFF
#pragma config PLLEN    = OFF
#pragma config STVREN   = OFF
#pragma config BORV     = LO
//#pragma config DEBUG    = OFF   // Pre-processor short-circuits !
#pragma config LVP      = OFF // Low voltage programming

#define LED LATCbits.LATC6

void pic_init(void);
void init_gpio(void);
void init_osc(void);

char spi_buffer = 0;
int ButtonTrigger = 1;
int on = 0;

void main(void)
{
    pic_init();
    while (1) {
    }
}

void pic_init(void)
{
    init_osc();
    init_gpio();
    //init_timers();
    spi_init(MST_OSC_DIV_04, SMP0_SDI_MIDDLE, CKP0_CPOL0_IDLE_LOW, CKE0_CPHA1_TRSMIT_IDL2ACT);
}

void init_osc(void)
{
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b00; // Clk determined by FOSC in Conf-1
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b1101; // 4 MHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
}

void init_gpio(void)
{
    ANSELC = 0;
    ANSELB = 0;
    ANSELA = 0;
    TRISCbits.TRISC6 = 0; // led OUTPUT
    LATCbits.LATC6 = 1;
    GIE = 1;
    INTE = 1;
}


void __interrupt() isr(void)
{
    switch (on) {
//        case 0: // RELEASED
//
//            break;
        case 1: // PRESSED
            spi_transmit(0b01010101);
            
            spi_buffer = spi_read();
            if (spi_buffer & 0b11110000) {
                LED = 0;
                __delay_ms(500);
                LED = 1;
            }
            ButtonTrigger = 1;
            break;
    }
    // button handler:
    if (INTCONbits.INTF) {
        ButtonTrigger ^= 1;
        switch (ButtonTrigger) {
            case 0:
                OPTION_REGbits.INTEDG = 0;
                INTCONbits.INTF = 0;
                on = 1;
                break;
            case 1:
                on = 0;
                OPTION_REGbits.INTEDG = 1;
                INTCONbits.INTF = 0;
                break;
        }
    }
}
/* End-of Interrupt Service Routine (ISR) -------------------------------------------*/