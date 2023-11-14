/*
 * File:   newmain.c
 * Author: sirdep
 *
 * Created on November 3, 2023, 11:54 AM
 */

// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ      8000000 // Used by the XC8 delay_ms(x) macro
// 'C' source line config statements

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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void init_timers(void);

#define IR_LED LATCbits.LATC6
#define BUTTON PORTAbits.RA2

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
    init_timers();

    GIE = 1; // Enable General Interrupt
    INTE = 1; // Enable Interrupt pin
    PEIE = 1; // Enable Peripheral Interrupt for Timer2
}

void init_gpio(void)
{
    TRISAbits.TRISA2 = 1;
    TRISCbits.TRISC6 = 0;
    ANSELA = 0;
    ANSELC = 0;
    IR_LED = 0;
}

void init_osc(void)
{
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b00; // Clk determined by FOSC in Conf-1
    // Internal Resistor-Capacitor Frequency select (IRCF)sadasdasdasd
    OSCCONbits.IRCF = 0b1110; // 8 MHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
}

void init_timers(void)
{
    //Timer2 Registers Prescaler= 1 - TMR2 PostScaler = 1 - PR2 = 52 - Freq = 38461.54 Hz - Period = 0.000026 seconds
    T2CON |= 0; // bits 6-3 Post scaler 1:1 thru 1:16
    TMR2ON = 1; // bit 2 turn timer2 on;
    T2CKPS1 = 0; // bits 1-0  Prescaler Rate Select bits
    T2CKPS0 = 0;
    PR2 = 52; // PR2 (Timer2 Match value)
    TMR2IE = 1;
}

void __interrupt() isr(void)
{
    switch (on) {
        case 0:
            if (TMR2IF) {
                TMR2IF = 0; // maak flag 0
                IR_LED = 0; // Zet IR led uit
            }
            break;
        case 1:
            if (TMR2IF) {
                IR_LED = 0; // zet IR led uit
                TMR2IF = 0; // maak flag 0
                IR_LED = 1; // zet IR led aan
            }
            break;
    }
    // button handler:
    if (INTCONbits.INTF) {
        ButtonTrigger ^= 1; // xor uitvoeren:
        switch (ButtonTrigger) {
            case 0:

                OPTION_REGbits.INTEDG = 0; // rising edge instelling
                INTCONbits.INTF = 0;
                on = 0;
                break;
            case 1:
                on = 1;
                OPTION_REGbits.INTEDG = 1; // rising edge instelling
                INTCONbits.INTF = 0;
                break;
        }
    }
}