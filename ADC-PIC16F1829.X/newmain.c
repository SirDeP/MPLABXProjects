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



#define _XTAL_FREQ      5000000  // Used by the XC8 delay_ms(x) macro

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
// #pragma config CONFIG1 = 0b (00)00 1111 1010 0500
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
#define LED1 LATCbits.LATC0
#define LED2 LATCbits.LATC1
#define LED3 LATCbits.LATC2
#define LED4 LATCbits.LATC3

#define DEADZONE 20
#define N 4
#define TOTAL 1023
#define BUTTON PORTAbits.RA2


// ...

/* End-of hardware / state naming scheme -----------------------------*/

//>>

/* Forward function declarations + main function hereafter -----------*/

void pic_init(void);
void init_osc(void);
// ... more init function declarations here, one per each subsystem !
void init_gpio(void);
// ... and even more function declarations here, for module behaviors !

void main(void)
    {
    ADCON0 = 0b00001111;
    ADCON1bits.ADFM = 1; // right justified

    // Do all initialisation here
    pic_init();
    int ADR[N] = {0};
    ADR[0] = TOTAL / (N + 1); //12800 25600 38400 51200
    for (int i = 1; i < N; i++)
        {
        ADR[i] = ADR[0] * (i + 1);
        }
    while (1)
        {
        __delay_us(5); //wait for ADC charging cap to settle
        GO = 1; //ADCON0.GO

        while (GO)continue; //wait voor conversion to be finished
        for (int i = 0; i < N; i++)
            {
            if (ADRES > (ADR[i] - DEADZONE))
                {
                LATC = (unsigned char)(LATC | (1 << i));
                }
            else if (ADRES <= (ADR[i] - DEADZONE))
                {
                LATC = (unsigned char)(LATC & ~(1 << i));
                }
            }
        }
    }

        /*Dead zone inbouwen
         * enum bitstates
         * {
         *  LOW,
         *  HIGH
         * } bitstate
         *  __delay_us(5);//wait for ADC charging cap to settle
            GO = 1;//ADCON0.GO
            while (GO)continue; //wait vor conversion to be finished
         * if (ADRES > 522)
         *      bitstate = HIGH;
         * else if (ADRES < 520)
         *      bitstate = LOW;
         * LATCbits.LATC0 = bitstate;
         */
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
    OSCCONbits.IRCF = 0b0111; // 5000 kHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
    // XOR (Don't do this, it's unmaintainable)
    // OSCCON = 0b00115000;
    }

void init_gpio(void)
    {
    LED1 = 0;
    //    / A lijn = input (knop/pot-meter)/
    TRISAbits.TRISA2 = 1; // a = knop in
    TRISAbits.TRISA4 = 1; // a = pot-meter in

    ANSELAbits.ANSA2 = 0; // analog uit voor knop
    ANSELAbits.ANSA4 = 1; // analog aan voor pot-meter

    // C lijn = LEDS */
    TRISC = 0;
    LATC = 0;
    }
/* End-of Init functions -------------------------------------------- */

//>>

/* Utility functions per module hereafter -- may be modularized later!*/

// ...

/* End-of utility functions per module -------------------------------*/

//>>

/* Interrupt Service Routine (ISR) hereafter -- keep with main module!*/

void __interrupt() isr();

/* End-of Interrupt Service Routine (ISR) ----------------------------*/

//>>
