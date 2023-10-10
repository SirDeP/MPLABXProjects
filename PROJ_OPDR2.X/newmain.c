///*
// * File:   newmain.c
// * Author: SirDeP
// *
// * Created on September 22, 2023, 1:40 PM
// */
//

// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ      500000  // Used by the XC8 delay_ms(x) macro
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

#define REB PORTBbits.RB5
#define REA PORTBbits.RB4

#define N 4

#define DEADZONE 20
#define TOTAL 1023

#include <xc.h>

typedef struct _rot
    {
    int oldstate;
    int state;
    int counter;
    int oddcounter;
    } rot;

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void rotary_read(rot *rotary);

void main(void)
    {
    pic_init();


    // Do all initialisation here
    pic_init();
    int ADR[N] = {0};
    ADR[0] = TOTAL / (N + 1); //12800 25600 38400 51200

    for (int i = 1; i < N; i++)
        ADR[i] = ADR[0] * (i + 1);

    while (1)
        {
        __delay_us(5); //wait for ADC charging cap to settle
        GO = 1; //ADCON0.GO
        while (GO)continue; //wait voor conversion to be finished
        int ADRES = ADRESL + (ADRESH * 256);
        for (int i = 0; i < N; i++)
            {
            if (ADRES > (ADR[i] - DEADZONE))
                PORTA &= ~(1 << i);
            else if (ADRES <= (ADR[i] - DEADZONE))
                PORTA |= (1 << i);
            }
        }
    }

void pic_init(void)
    {
    init_osc();
    init_gpio();
    PORTA = 0b1111; // Turn LEDS off
    }

void
init_gpio(void)
    {
    TRISA = 0;
    TRISEbits.TRISE2 = 1;
    ANSEL = 0; // analog 0 to 7 off
    ANSELbits.ANS7 = 1;
    ANSELH = 0; // Analog 8 to 13 off
    ADCON0 = 0b00011111;
    ADCON1bits.ADFM = 1; // right justified
    }

void
init_osc(void)
    {
    // System Clock Select (SCS)
    //  OSCCON = 0b00111110;
    OSCCONbits.SCS = 0b1; // Internal Clock selected
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b100; // 1Mhz lock speed
    OSCCONbits.OSTS = 0b1; // OSC startup time
    while (OSCCONbits.HTS != 0b1); // Wait with booting until clock is stable
    // OSCCON = 0b00111000;
    }
