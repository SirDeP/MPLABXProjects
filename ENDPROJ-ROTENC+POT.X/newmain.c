///*
// * File:   newmain.c
// * Author: SirDeP
// *
// * Created on September 22, 2023, 1:40 PM
// */
//

// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ 8000000 // Used by the XC8 delay_ms(x) macro
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF            // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON            // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF           // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF              // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF             // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON            // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF            // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF           // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF             // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF      // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define REB PORTBbits.RB5
#define REA PORTBbits.RB4

#define MAX 4

#include <xc.h>

void pic_init(void);
void init_gpio(void);
void init_osc(void);

int ENC_Last_State = 0;
int ENC_State = 0;
int counter = 0;
int OddCounter = 0;
int ENC_Last_State = REA;

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
    PORTA = 0b1111; // Turn LEDS off
}

void encoder_read(void)
{
    ENC_State = REA; // Current State of A pin of RotaryEncoder
    if (ENC_State != ENC_Last_State) // Compare current state of REA with last state of REA
    {
        // If the state of REB differs REA state, that means the encoder is rotating clockwise
        if (REB != ENC_State) {
            counter++;
            if (counter % 2 != 0) // If counter is odd increase OddCounter variable
                OddCounter++;
        }
        else if (REA == ENC_State) // Counterclockwise rotation
        {
            counter--;
            if (counter % 2 != 0) // If counter is odd decrease OddCounter variable
                OddCounter--;
        }
        if (OddCounter < 0) // If OddCounter is smaller than 0 jump OddCounter and Counter to their respective highest allowed values
        {
            OddCounter = MAX - 1;
        }
    }
    ENC_Last_State = ENC_State; // Update last encoder state with current encoder state
    PORTA &= ~(1 << (OddCounter)); // Turn LED on or off depending on OddCounter
    PORTA |= (1 << (OddCounter)); // Inverted from normal, but PORTA is inverted so in this case correct
    if (OddCounter > MAX - 1) // Reset Counter and OddCounter to their standard values
    {
        OddCounter = 0;
        counter = 0;
    }
}

init_gpio(void)
{
    TRISA = 0;
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;
    ANSEL = 0; // analog 0 to 7 off
    ANSELH = 0; // Analog 8 to 13 off
    IOCBbits.IOCB4 = 1;
    IOCBbits.IOCB5 = 1;
    GIE = 1;
    PEIE = 1;
    INTCONbits.RBIE = 1;
}

void init_osc(void)
{
    // System Clock Select (SCS)
    //  OSCCON = 0b00111110;
    OSCCONbits.SCS = 0b1; // Internal Clock selected
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b111; // 8 MHz clock speed
    OSCCONbits.OSTS = 0b1; // OSC startup time
    while (OSCCONbits.HTS != 0b1); // Wait with booting until clock is stable
    // OSCCON = 0b00111000;
}

void __interrupt() irs(void)
{
    if (INTCONbits.RBIF) {
        PORTA = 0b1111;
        for (int i = 0; i < 5000; i++) {
            __delay_ms(1);
        }
        encoder_read();
        INTCONbits.RBIF = 0;
    }
}