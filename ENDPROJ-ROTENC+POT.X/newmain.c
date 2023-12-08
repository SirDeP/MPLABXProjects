/*
 * File:   newmain.c
 * Author: SirDeP
 *
 * Created on September 22, 2023, 1:40 PM
 */



// PIC16F887 Configuration Bit Settings
#define _XTAL_FREQ      8000000  // Used by the XC8 delay_ms(x) macro
// 'C' source line config statements

// CONFIG_1_PIC16F887
#pragma config FOSC  = INTRC_NOCLKOUT // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE  = OFF            // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON            // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF            // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP    = OFF            // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD   = OFF            // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON             // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO  = OFF            // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF            // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP   = OFF            // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG_2_PIC16F887
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>

/* LEDs */
#define POTMETER_STAGES 4

/* Encoder */
#define ENC_B PORTBbits.RB5
#define ENC_A PORTBbits.RB4

/* Potentiometer */
#define POT_METER PORTEbits.RE2
#define MAX_POT_METER 1023 // 2^10
#define DEADZONE    50     // Deadzone config

/* (Object) Encoder */
typedef struct _encoder
{
    int ENC_Last_State;
    int ENC_State;
    int Counter;
    int OddCounter;
} encoder;

/* (Object) Potentiometer */
typedef struct _potmeter
{
    int ADR[POTMETER_STAGES+1];
    int ADRESS;
} potmeter;

potmeter pm_1;

encoder enc = {0,0,0,0};

/* Functies */
void ADC_Calc();
void ADC_LEDS();

void encoder_lezer(); // Leest encoder uit

void pic_init(void);
void init_gpio(void);
void init_osc(void);

void main(void)
{
    pic_init();

    enc.ENC_Last_State = ENC_A;

    ADCON0 = 0b00011111; 
    ADCON1bits.ADFM = 1; // Right justify
    ADC_Calc();
    while (1)
    {
        pm_1.ADRESS = (ADRESH << 8) + ADRESL; // Combine ADRESL and ADRESH
        __delay_us(5); // delay for Debounce
        GO = 1; // ADC0C0
        while (GO) continue;
        ADC_LEDS();
    }
}

void ADC_LEDS()
{
    for (int i = 0; i < POTMETER_STAGES; i++)
    {
        if (pm_1.ADRESS > (pm_1.ADR[i] + DEADZONE))
            PORTA = PORTA & ~(1 << i);
        else if (pm_1.ADRESS < (pm_1.ADR[i] - DEADZONE))
            PORTA = (unsigned char) (PORTA | (1 << i));
    }
}

void ADC_Calc()
{
    for (int i; i < POTMETER_STAGES; i++)
        pm_1.ADR[i] = 0;
    pm_1.ADR[0] = MAX_POT_METER / (POTMETER_STAGES + 1); // = MAX-POT-METER-RANGE/5 ~~204
    for (int i = 1; i <= POTMETER_STAGES; i++)
    {
        pm_1.ADR[i] = pm_1.ADR[0] * (i + 1); // ADR[1] = (1+1)x204; ADR[2] = (2+1)x204; ADR[3] = (3+1)x204; ADR[4] = (4+1)x204;
    }
}

void encoder_lezer()
{
    enc.ENC_State = ENC_A; // Current State of A pin of RotaryEncoder
    if (enc.ENC_State != enc.ENC_Last_State)
    {
        if (ENC_B != enc.ENC_State) // Compare current state of REA with last state of REA
        {
            enc.Counter++;
            if (enc.Counter % 2 == 1) // If counter is odd increase OddCounter variable
            {
                enc.OddCounter++; 
            }
        }
        else if (ENC_A == enc.ENC_State) // Counterclockwise rotation
        {
            enc.Counter--; // Doe Counter min 1
            if (enc.Counter % 2 == 1) // If counter is odd decrease OddCounter variable
            {
                enc.OddCounter--;
            }
        }
        // If OddCounter is smaller than 0 jump OddCounter to the highest allowed value
        if (enc.OddCounter < 0)
        {
            enc.OddCounter = POTMETER_STAGES - 1;
        }
        // If OddCounter is greater than the highest allowed value jump OddCounter to 0
        if (enc.OddCounter > POTMETER_STAGES - 1)
        {
            enc.OddCounter = 0;
        }
    }
    enc.ENC_Last_State = enc.ENC_State;
    PORTA = PORTA & ~(1 << (enc.OddCounter)); // Bitshift & ~ Turn LEDs On
    __delay_ms(1);
    PORTA = PORTA | (unsigned char) (1 << (enc.OddCounter)); // Bitshift & ~ Turn LEDs Off
    __delay_ms(1);
}

void pic_init(void)
{
    init_osc();
    init_gpio();
    PORTA = 0b1111; // Turn LEDs OFF
}

void init_gpio(void)
{
    TRISA = 0; // All LEDS == output
    TRISEbits.TRISE2 = 1; // Potmeter == input
    TRISBbits.TRISB4 = 1; // Rot Enc Pins == Input
    TRISBbits.TRISB5 = 1; // 
    ANSEL = 0b1000000; // Analog off: ANS0 T/M ANS6, ANS7 on (POT)
    ANSELH = 0; // Analog off: ANS8 T/M ANS13

    IOCBbits.IOCB4 = 1; // Enable Interrupt on change for Rotary Encoder
    IOCBbits.IOCB5 = 1;
    
    INTCONbits.GIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
}

void init_osc(void)
{
    OSCCONbits.SCS = 0b1; // Clk determined by FOSC in Conf-1
    OSCCONbits.IRCF = 0b111; // 8 MHz clock speed | Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.OSTS = 0b1; // OSC startup time
    while (OSCCONbits.HTS != 0b1);
}

void __interrupt() isr(void)
{
    if (INTCONbits.RBIF == 1)
    {
        PORTA = 0b1111;
        for (int i = 0; i < 2500; i++)
        {
            __delay_us(1);
            encoder_lezer();
        }
        INTCONbits.RBIF = 0;
    }
}