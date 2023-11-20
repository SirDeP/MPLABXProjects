/***********************************************************************
 * 
 * PIC   : 16F1829
 * Date  : A long time ago, in a galaxy far away (20-11-'23)
 * Author: SirDeP
 * 
 **********************************************************************/

// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ      4000000 // Used by the XC8 delay_ms(x) macro
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
#include "spi.h"

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void init_timers(void);

char spi_buffer = 0;
int on = 0;

void main(void)
{
    pic_init();
    SSPBUF = 0b11110000;
    while (1) {

    }
}

void pic_init(void)
{
    init_osc();
    init_gpio();
    init_timers();
    spi_init(SLV_SS_DIS, SMP0_SDI_MIDDLE, CKP0_CPOL0_IDLE_LOW, CKE0_CPHA1_TRSMIT_IDL2ACT);
}

void init_gpio(void)
{
    TRISAbits.TRISA0 = 0;
    //TRISBbits.TRISB0 = 1;
    ANSEL = 0;
    ANSELH = 0;
    LED = 1;
}

void init_osc(void)
{
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b1; // Internal Clock selected
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b110; // 4MHz clock speed
    OSCCONbits.OSTS = 0b1; // OSC startup time
    while (OSCCONbits.HTS != 0b1); // Wait with booting until clock is stable
}

void init_timers(void)
{

}

void __interrupt() isr(void)
{
    if(SSPIF)
    {
        spi_buffer = spi_read();
        if (spi_buffer & 0b01010101)
        {
            LED = 0;
            __delay_ms(500);
            LED = 1;
            spi_transmit(0b11110000);
        }
        SSPIF = 0;
    }
}