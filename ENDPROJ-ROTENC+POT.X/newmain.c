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
#define MAX_AANTAL_LEDS 4

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
    int laatste_verandering_status;
    int status;
    int teller;
    int oneven_teller;
} encoder;

/* (Object) Potentiometer */
typedef struct _potmeter
{
    int VU[MAX_AANTAL_LEDS + 1];
    int die_berekende_adres;
} potmeter;

potmeter pm_1;

encoder enc = {0};

/* Functies */
void VU_berekenaar(potmeter *pm_1);
void led_regelaar(potmeter *pm_1);

void encoder_lezer(encoder *enc); // Leest encoder uit
void bitshifter(encoder *enc); // Shift de bits afhankelijk van enc

void pic_init(void); // PIC initialisatie
void init_gpio(void); // GPIO initialisatie
void init_osc(void); // OSCILLATOR initialisatie

void main(void)
{
    pic_init();
    /* Maak objecten aan: */



    /* Initialiseren van variabelen: */
    enc.laatste_verandering_status = 0;
    enc.status = 0;
    enc.teller = 0;
    enc.oneven_teller = 0;
    enc.laatste_verandering_status = ENC_A;

    /* ADC configuratie */
    ADCON0 = 0b00011111; // stel ADC ON0 in
    ADCON1bits.ADFM = 1; // Right justify
    while (1)
    {
        //        switch (MODI_SCHAKELAAR)
        //        {
        //            case 0:
        ////                encoder_lezer(&enc);
        ////                bitshifter(&enc);
        //                break;
        //            case 1:
        VU_berekenaar(&pm_1);
        __delay_us(5); // delay voor debounce enzovoort
        GO = 1; // ADC0C0
        while (GO) continue;
        led_regelaar(&pm_1);
        //                break;
        //        }
    }
}

void led_regelaar(potmeter *pm_1)
{
    for (int i = 0; i < MAX_AANTAL_LEDS; i++)
    {
        if (pm_1->die_berekende_adres > (pm_1->VU[i] + DEADZONE))
            PORTA = PORTA & ~(1 << i);
        else if (pm_1->die_berekende_adres < (pm_1->VU[i] - DEADZONE))
            PORTA = (unsigned char) (PORTA | (1 << i));
    }
}

void VU_berekenaar(potmeter *pm_1)
{
    pm_1->die_berekende_adres = ADRESL + (ADRESH * 256); // UPDATE DE TOTALE ADRES L EN H in een VAR
    pm_1->VU[0] = MAX_POT_METER / (MAX_AANTAL_LEDS + 1); // = MAX-POT-METER-RANGE/5 ~~204
    /* Bereken die grenswaarden */
    for (int i = 1; i <= MAX_AANTAL_LEDS; i++)
    {
        pm_1->VU[i] = pm_1->VU[0] * (i + 1); // VU[1] = (1+1)x204; VU[2] = (2+1)x204; VU[3] = (3+1)x204; VU[4] = (4+1)x204;
    }
}

void bitshifter(encoder *enc)
{
    /* Bitshifting van de LEDs: */
    PORTA = PORTA & ~(1 << (enc->oneven_teller)); // Bitshift & ~ zorgt voor dat de LEDs uitgaan, maar omdat de PORTA geinvert is, gaan ze juist aan
    __delay_ms(1);
    PORTA = PORTA | (unsigned char) (1 << (enc->oneven_teller)); // Bitshift & ~ zorgt voor dat de LEDs aan gaan, maar omdat de PORTA geinvert is, gaan ze juist uit
    __delay_ms(1);
}

void encoder_lezer(encoder *enc)
{
    enc->status = ENC_A; // Lees de huidige status af door deze gelijk te stellen aan output A (ENC_A)
    if (enc->status != enc->laatste_verandering_status)
    {
        if (ENC_B != enc->status) // Als ENC_B veranderd ten opzichte van ENC_A, dan betekent het dat hij aan het draaien is
        {
            enc->teller++; // Als er gedraaid wordt, doe dan teller plus 1
            if (enc->teller % 2 == 1) // Check of teller oneven is
            {
                enc->oneven_teller++; // Oneven-teller plus 1
            }
        }
        else if (ENC_A == enc->status) // Check of nu de andere kant op wordt gedraaid, zo ja:
        {
            enc->teller--; // Doe teller min 1
            if (enc->teller % 2 == 1) // Check of teller oneven is
            {
                enc->oneven_teller--; // Oneven-teller min 1
            }
        }
        /* Check of oneven_teller niet meer mag zijn dan het aantal ingestelde LEDs: */
        if (enc->oneven_teller == -1)
        {
            enc->oneven_teller = MAX_AANTAL_LEDS - 1;
        }
        if (enc->oneven_teller > MAX_AANTAL_LEDS - 1)
        {
            enc->oneven_teller = 0; // Als het groter wordt, reset hem dan naar 0
        }
    }
    enc->laatste_verandering_status = enc->status;
}

void pic_init(void)
{
    init_osc();
    init_gpio();
    PORTA = 0b1111; // Initialiseer PORTA zodat alle LEDs uitstaan!
}

void init_gpio(void)
{
    TRISA = 0; // Alle LEDs moeten op output ingesteld worden (je kan ook per bit instellen)
    TRISEbits.TRISE2 = 1; // register E (POT) == input
    TRISBbits.TRISB4 = 1; // Pin van encoder moet als input
    TRISBbits.TRISB5 = 1; // Pin van encoder moet als input
    ANSEL = 0b1000000; // Analog uit: ANS0 T/M ANS6, ANS7 moet AAN (POT)
    ANSELH = 0; // Analog uit voor hoge register voor de encoder

    IOCBbits.IOCB4 = 1;
    IOCBbits.IOCB5 = 1;
   
    WPUB = 0;
    INTCONbits.GIE = 1;
    //INTCONbits.PEIE = 1;
    //INTCONbits.INTE = 1;
    INTCONbits.RBIE = 1;

    INTCONbits.RBIF = 0;
}

void init_osc(void)
{
    // Do it like this, it's easily compared to the datasheet:
    // System Clock Select (SCS)
    //  OSCCON = 0b00111110;
    OSCCONbits.SCS = 0b1; // Clk determined by FOSC in Conf-1

    /* 
     * bit 6-4 Internal Oscillator Frequency Select bits
    111 = 8 MHz
    110 = 4 MHz (default)
    101 = 2 MHz
    100 = 1 MHz
    011 = 500 kHz
    010 = 250 kHz
    001 = 125 kHz
    000 = 31 kHz (LFINTOSC)
     */
    OSCCONbits.IRCF = 0b111; // 8 MHz clock speed | Internal Resistor-Capacitor Frequency select (IRCF)
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.OSTS = 0b1; // OSC startup time
    //    OSCCONbits.HTS = 0b1;
    //    OSCCONbits.LTS = 0b1;

    //OSSCONbits.LTS = 0b1;
    // XOR (Don't do this, it's unmaintainable)
    // OSCCON = 0b00111000;
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
            bitshifter(&enc);
            
        }
        INTCONbits.RBIF = 0;
    }
}