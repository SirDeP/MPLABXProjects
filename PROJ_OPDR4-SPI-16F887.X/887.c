/***********************************************************************
 * 
 * PIC   : 16F1829
 * Date  : A long time ago, in a galaxy far away (20-11-'23)
 * Author: SirDeP
 * 
 **********************************************************************/

// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ      8000000 // Used by the XC8 delay_ms(x) macro
// 'C' source line config statements

// CONFIG1
#pragma config FOSC  = INTRC_NOCLKOUT // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE  = OFF            // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF             // Power-up Timer Enable bit (PWRT disabled)
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
//#define LED PORTAbits.RA0
#define NUMBER_OF_LEDS 8

#include <xc.h>
#include "spi.h"

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void init_timers(void);

int spi_buffer = 0;
int on = 0;
int i = 0;

void main(void)
{
    pic_init();
    spi_init(MST_OSC_DIV_04, SMP0_SDI_MIDDLE, CKP0_CPOL0_IDLE_LOW, CKE0_CPHA1_TRSMIT_IDL2ACT); // NODIG?
    while (1) {
        //        spi_transmit(spi_buffer);
    }
}

void pic_init(void)
{
    init_osc();
    init_timers();
    init_gpio();
    
}

void init_gpio(void)
{
    TRISAbits.TRISA0 = 0;
    //TRISBbits.TRISB0 = 1;
    ANSEL = 0;
    ANSELH = 0;
    GIE = 1;
    PEIE = 1;
    TMR1IE = 1;
    TMR1IF = 0;
    TRISCbits.TRISC2 = 0;
}

void init_osc(void)
{
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b1; // Internal Clock selected
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b111; // 8MHz clock speed
    OSCCONbits.OSTS = 0b1; // OSC startup time
    while (OSCCONbits.HTS != 0b1); // Wait with booting until clock is stable
}

void init_timers(void)
{
//Timer1 Registers Prescaler= 2 - TMR1 Preset = 65535 - Freq = 1000000.00 Hz - Period = 0.000001 seconds
T1CONbits.T1CKPS1 = 0;   // bits 5-4  Prescaler Rate Select bits
T1CONbits.T1CKPS0 = 1;   // bit 4
T1CONbits.T1OSCEN = 1;   // bit 3 Timer1 Oscillator Enable Control bit 1 = on
T1CONbits.T1SYNC = 1;    // bit 2 Timer1 External Clock Input Synchronization Control bit...1 = Do not synchronize external clock input
T1CONbits.TMR1CS = 0;    // bit 1 Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
T1CONbits.TMR1ON = 1;    // bit 0 enables timer
TMR1H = 255;             // preset for timer1 MSB register
TMR1L = 255;             // preset for timer1 LSB register
}

void __interrupt() isr(void)
{
    if (TMR1IF) {
        on ^= 1;
        spi_buffer = 1;
        spi_buffer = spi_buffer << i;
        if (on == 1) {
            PORTCbits.RC2 = 1;
        }
        else if (on == 0)
            PORTCbits.RC2 = 0;
        spi_transmit((char)spi_buffer);
        if (on == 1)
        {
            i++;
            if (i >= NUMBER_OF_LEDS)
                i = 0;
        }
        TMR1IF = 0; // interrupt must be cleared by software
        TMR1IE = 1; // reenable the interrupt
    }
}