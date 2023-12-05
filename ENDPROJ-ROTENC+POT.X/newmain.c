/////*
//// * File:   newmain.c
//// * Author: SirDeP
//// *
//// * Created on September 22, 2023, 1:40 PM
//// */
////
//
//// PIC16F887 Configuration Bit Settings
//#define _XTAL_FREQ 8000000 // Used by the XC8 delay_ms(x) macro
//// 'C' source line config statements
//
//// CONFIG1
//#pragma config FOSC = INTRC_NOCLKOUT // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
//#pragma config WDTE = OFF            // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
//#pragma config PWRTE = ON            // Power-up Timer Enable bit (PWRT disabled)
//#pragma config MCLRE = OFF           // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
//#pragma config CP = OFF              // Code Protection bit (Program memory code protection is disabled)
//#pragma config CPD = OFF             // Data Code Protection bit (Data memory code protection is disabled)
//#pragma config BOREN = ON            // Brown Out Reset Selection bits (BOR enabled)
//#pragma config IESO = OFF            // Internal External Switchover bit (Internal/External Switchover mode is disabled)
//#pragma config FCMEN = OFF           // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
//#pragma config LVP = OFF             // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
//
//// CONFIG2
//#pragma config BOR4V = BOR40V // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
//#pragma config WRT = OFF      // Flash Program Memory Self Write Enable bits (Write protection off)
//
//// #pragma config statements should precede project file includes.
//// Use project enums instead of #define for ON and OFF.
//
//#define REB PORTBbits.RB5
//#define REA PORTBbits.RB4
//
//#define MAX 4
//
//#include <xc.h>
//
//void pic_init(void);
//void init_gpio(void);
//void init_osc(void);
//void encoder_read(void);
//
//#define DEADZONE 20
//#define N 4
//#define TOTAL 1023
//
//#define REB PORTBbits.RB5
//#define REA PORTBbits.RB4
//
//int ENC_State = 0;
//int counter = 0;
//int OddCounter = 0;
//int ENC_Last_State = REA;
//
//void main(void)
//{
//    ADCON0 = 0b00001111;
//    ADCON1bits.ADFM = 1; // right justified
//    pic_init();
//
//    int ADR[N] = {0};
//    ADR[0] = TOTAL / (N + 1); //12800 25600 38400 51200
//    for (int i = 1; i < N; i++) {
//        ADR[i] = ADR[0] * (i + 1);
//    }
//    while (1) {
//        __delay_us(5); //wait for ADC charging cap to settle
//        GO = 1; //ADCON0.GO
//
//        while (GO)continue; //wait voor conversion to be finished
//        for (int i = 0; i < N; i++) {
//            if (ADRES > (ADR[i] + DEADZONE)) {
//                LATC = (unsigned char) (LATC | (1 << i));
//            }
//            else if (ADRES <= (ADR[i] - DEADZONE)) {
//                LATC = (unsigned char) (LATC & ~(1 << i));
//            }
//        }
//    }
//}
//
//void pic_init(void)
//{
//    init_osc();
//    init_gpio();
//    PORTA = 0b1111; // Turn LEDS off
//}
//
//void encoder_read(void)
//{
//    ENC_State = REA; // Current State of A pin of RotaryEncoder
//    if (ENC_State != ENC_Last_State) // Compare current state of REA with last state of REA
//    {
//        // If the state of REB differs REA state, that means the encoder is rotating clockwise
//        if (REB != ENC_State) {
//            counter++;
//            if (counter % 2 != 0) // If counter is odd increase OddCounter variable
//                OddCounter++;
//        }
//        else if (REA == ENC_State) // Counterclockwise rotation
//        {
//            counter--;
//            if (counter % 2 != 0) // If counter is odd decrease OddCounter variable
//                OddCounter--;
//        }
//        if (OddCounter < 0) // If OddCounter is smaller than 0 jump OddCounter and Counter to their respective highest allowed values
//        {
//            OddCounter = MAX - 1;
//        }
//    }
//    ENC_Last_State = ENC_State; // Update last encoder state with current encoder state
//    PORTA &= ~(1 << (OddCounter)); // Turn LED on or off depending on OddCounter
//    PORTA |= (1 << (OddCounter)); // Inverted from normal, but PORTA is inverted so in this case correct
//    if (OddCounter > MAX - 1) // Reset Counter and OddCounter to their standard values
//    {
//        OddCounter = 0;
//        counter = 0;
//    }
//}
//
//init_gpio(void)
//{
//    TRISA = 0;
//    TRISBbits.TRISB4 = 1;
//    TRISBbits.TRISB5 = 1;
//
//    ANSEL = 0; // analog 0 to 7 off
//        ANSELbits.ANS7 = 1; // analog aan voor pot-meter
//    ANSELH = 0; // Analog 8 to 13 off
//    IOCBbits.IOCB4 = 1;
//    IOCBbits.IOCB5 = 1;
//    GIE = 1;
//    PEIE = 1;
//    INTCONbits.RBIE = 1;
//}
//
//void init_osc(void)
//{
//    // System Clock Select (SCS)
//    //  OSCCON = 0b00111110;
//    OSCCONbits.SCS = 0b1; // Internal Clock selected
//    // Internal Resistor-Capacitor Frequency select (IRCF)
//    OSCCONbits.IRCF = 0b111; // 8 MHz clock speed
//    OSCCONbits.OSTS = 0b1; // OSC startup time
//    while (OSCCONbits.HTS != 0b1); // Wait with booting until clock is stable
//    // OSCCON = 0b00111000;
//}
//
//void __interrupt() irs(void)
//{
//    if (INTCONbits.RBIF) {
//        PORTA = 0b1111;
//        for (int i = 0; i < 5000; i++) {
//            __delay_ms(1);
//        }
//        encoder_read();
//        INTCONbits.RBIF = 0;
//    }
//}
/*
 * File:   main.c
 * Author: broodjesemih
 * Created on September 21, 2023, 8:16 PM
 * 
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
//#include <proc/pic16f887.h>

/* LEDs */
#define MAX_LEDS 4

/* Encoder */
#define ENC_B PORTBbits.RB5
#define ENC_A PORTBbits.RB4

/* Potentiometer */
#define POT_METER PORTEbits.RE2
#define MAX_POT_METER 1024 // 2^10
#define DEADZONE    50     // Deadzone dat jittering voorkomt

/* ENCODER=0 / POTMETER=1 */
#define MODI_SCHAKELAAR 1

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
    int ADR[MAX_LEDS + 1];
    int ADRESS;
} potmeter;

potmeter pm_1;

encoder enc = {0};

/* Functies */
void ADC_berekenaar(potmeter *pm_1);
void ADC_LEDS(potmeter *pm_1);

void encoder_lezer(encoder *enc); // Leest encoder uit
void bitshifter(encoder *enc); // Shift de bits afhankelijk van enc

void pic_init(void); // PIC initialisatie
void init_gpio(void); // GPIO initialisatie
void init_osc(void); // OSCILLATOR initialisatie

void main(void)
{
    pic_init();

    enc.ENC_Last_State = 0;
    enc.ENC_State = 0;
    enc.Counter = 0;
    enc.OddCounter = 0;
    enc.ENC_Last_State = ENC_A;

    /* ADC configuratie */
    ADCON0 = 0b00011111; // stel ADC ON0 in
    ADCON1bits.ADFM = 1; // Right justify
    while (1)
    {
        ADC_berekenaar(&pm_1);
        __delay_us(5); // delay for Debounce
        GO = 1; // ADC0C0
        while (GO) continue;
        ADC_LEDS(&pm_1);
    }
}

void ADC_LEDS(potmeter *pm_1)
{
    for (int i = 0; i < MAX_LEDS; i++)
    {
        if (pm_1->ADRESS > (pm_1->ADR[i] + DEADZONE))
            PORTA = PORTA & ~(1 << i);
        else if (pm_1->ADRESS < (pm_1->ADR[i] - DEADZONE))
            PORTA = (unsigned char) (PORTA | (1 << i));
    }
}

void ADC_berekenaar(potmeter *pm_1)
{
    pm_1->ADRESS = (ADRESH << 8) + ADRESL; // Combine ADRESL and ADRESH
    pm_1->ADR[0] = MAX_POT_METER / (MAX_LEDS + 1); // = MAX-POT-METER-RANGE/5 ~~204
    /* Bereken die grenswaarden */
    for (int i = 1; i <= MAX_LEDS; i++)
    {
        pm_1->ADR[i] = pm_1->ADR[0] * (i + 1); // ADR[1] = (1+1)x204; ADR[2] = (2+1)x204; ADR[3] = (3+1)x204; ADR[4] = (4+1)x204;
    }
}

void encoder_lezer(encoder *enc)
{
    ENC_State = REA; // Current State of A pin of RotaryEncoder
//    if (ENC_State != ENC_Last_State) // Compare current state of REA with last state of REA
//    {
//        // If the state of REB differs REA state, that means the encoder is rotating clockwise
//        if (REB != ENC_State) {
//            counter++;
//            if (counter % 2 != 0) // If counter is odd increase OddCounter variable
//                OddCounter++;
//        }
//        else if (REA == ENC_State) // Counterclockwise rotation
//        {
//            counter--;
//            if (counter % 2 != 0) // If counter is odd decrease OddCounter variable
//                OddCounter--;
//        }
//        if (OddCounter < 0) // If OddCounter is smaller than 0 jump OddCounter and Counter to their respective highest allowed values
//        {
//            OddCounter = MAX - 1;
//        }
//    }
//    ENC_Last_State = ENC_State; // Update last encoder state with current encoder state
//    PORTA &= ~(1 << (OddCounter)); // Turn LED on or off depending on OddCounter
//    PORTA |= (1 << (OddCounter)); // Inverted from normal, but PORTA is inverted so in this case correct
//    if (OddCounter > MAX - 1) // Reset Counter and OddCounter to their standard values
//    {
//        OddCounter = 0;
//        counter = 0;
//    }
    enc->ENC_State = ENC_A; // Current State of A pin of RotaryEncoder
    if (enc->ENC_State != enc->ENC_Last_State)
    {
        if (ENC_B != enc->ENC_State) // Compare current state of REA with last state of REA
        {
            enc->Counter++;
            if (enc->Counter % 2 == 1) // If counter is odd increase OddCounter variable
            {
                enc->OddCounter++; 
            }
        }
        else if (ENC_A == enc->ENC_State) // Counterclockwise rotation
        {
            enc->Counter--; // Doe Counter min 1
            if (enc->Counter % 2 == 1) // If counter is odd decrease OddCounter variable
            {
                enc->OddCounter--;
            }
        }
        // If OddCounter is smaller than 0 jump OddCounter to the highest allowed value
        if (enc->OddCounter < 0)
        {
            enc->OddCounter = MAX_LEDS - 1;
        }
        // If OddCounter is greater than the highest allowed value jump OddCounter to 0
        if (enc->OddCounter > MAX_LEDS - 1)
        {
            enc->OddCounter = 0;
        }
    }
    enc->ENC_Last_State = enc->ENC_State;
        /* Bitshifting van de LEDs: */
    PORTA = PORTA & ~(1 << (enc->OddCounter)); // Bitshift & ~ Turn LEDs On
    __delay_ms(1);
    PORTA = PORTA | (unsigned char) (1 << (enc->OddCounter)); // Bitshift & ~ Turn LEDs Off
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
    TRISA = 0; // All LEDS as output
    TRISEbits.TRISE2 = 1; // Potmeter == input
    TRISBbits.TRISB4 = 1; // Rot Enc Pins == Input
    TRISBbits.TRISB5 = 1; // 
    ANSEL = 0b1000000; // Analog off: ANS0 T/M ANS6, ANS7 on (POT)
    ANSELH = 0; // Analog off: ANS8 T/M ANS13

    IOCBbits.IOCB4 = 1; // Enable Interrupt on change for Rotary Encoder
    IOCBbits.IOCB5 = 1;
   
    WPUB = 0; // Disable weak pull up resistors on B
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
            encoder_lezer(&enc);
        }
        INTCONbits.RBIF = 0;
    }
}