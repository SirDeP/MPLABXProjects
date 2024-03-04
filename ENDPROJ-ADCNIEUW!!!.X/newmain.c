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



/* (Object) Encoder */
typedef struct _RotEnc_ {
    int rotA;
    int rotB; 
    int rotAlastState;
    int currentSteps;
    uint8_t counter;
} RotEnc;


/* (Object) Potentiometer */
typedef struct _potmeter
{
    int ADR[POTMETER_STAGES+1];
    int ADRESS;
    int ADCCount;
} potmeter;

potmeter pm_1;
RotEnc Rot;

/* Potentiometer */
#define POT_METER PORTEbits.RE2
#define MAX_POT_METER 1023 // 2^10
#define DEADZONE    50     // Deadzone config
#define POTMETER_STAGES 4

/* Rotary Encoder */
#define ROT_A  PORTBbits.RB4
#define ROT_B   PORTBbits.RB5
#define ROTARY_MAX     4  //Max value to be reached with rotary encoder
#define ROTARY_MIN     1  //Min value to be reached with rotary encoder
#define ROTARY_STEPS    2  //Amount of "clicks" before the rotary encoder changes the value


//Set starting conditions for a Rotary encoder struct

char counter = 0;
char state[8] = {0};
/* Functies */
void ADC_Init();
void ADC_Read();
void initRotEncoder(RotEnc *rot);
void checkRotEncoder(RotEnc *rot);
void pic_init(void);
void init_gpio(void);
void init_osc(void);

void main(void)
{
    pic_init();
    initRotEncoder(&Rot);
    ADCON0 = 0b00011111; 
    ADCON1bits.ADFM = 1; // Right justify
    ADC_Init();
    while (1)
    {
        pm_1.ADRESS = (ADRESH << 8) + ADRESL; // Combine ADRESL and ADRESH
        __delay_us(5); // delay for Debounce
        GO = 1; // ADC0C0
        while (GO) continue;
        ADC_Read();
        PORTA = ~pm_1.ADCCount;
        
    }
}

void ADC_Read()
{
    pm_1.ADRESS = (ADRESH << 8) + ADRESL; // Combine ADRESL and ADRESH
    __delay_us(5); // delay for Debounce
    GO = 1; // ADC0C0
    while (GO) continue; // no continue?
    
    for (int i = 0; i < POTMETER_STAGES; i++) {
        if (pm_1.ADRESS > (pm_1.ADR[i]) + DEADZONE)
            state[i] = 1;
        else if (pm_1.ADRESS < (pm_1.ADR[i] - DEADZONE))
            state[i] = 0;
    }
    for (char i = 0; i < 8; i++)
    {
        counter = counter + state[i];
    }
    pm_1.ADCCount = counter;
    counter = 0;
}

void ADC_Init()
{
    for (int i; i < POTMETER_STAGES; i++)
        pm_1.ADR[i] = 0;
    pm_1.ADR[0] = MAX_POT_METER / (POTMETER_STAGES + 1); // = MAX-POT-METER-RANGE/5 ~~204
    for (int i = 1; i <= POTMETER_STAGES; i++)
    {
        pm_1.ADR[i] = pm_1.ADR[0] * (i + 1); // ADR[1] = (1+1)x204; ADR[2] = (2+1)x204; ADR[3] = (3+1)x204; ADR[4] = (4+1)x204;
    }
}

void initRotEncoder(RotEnc *rot){
    rot->currentSteps = 0;
    rot->rotAlastState = 1;
    rot->rotA = 1;
    rot->rotB = 1;
    rot->counter = ROTARY_MIN;
}

void checkRotEncoder(RotEnc *rot){
    //Only check for a change in the CLK pin to identify "full click"
    rot->rotA = ROT_A;
        
    //If state changed, a movement is registerd
    if(rot->rotA != rot->rotAlastState){
        rot->rotB = ROT_B;
        if(rot->rotB != rot->rotA){ //Clockwise
            rot->currentSteps++;
        }else{ //Anti-clockwise
            rot->currentSteps--;
        }

        //Only change counter if correct amount of clicks have been reigstered
        if(rot->currentSteps > (ROTARY_STEPS*2)){
            rot->counter++;
            rot->currentSteps = 0;
        }else if(rot->currentSteps < (-(ROTARY_STEPS*2))){
            rot->counter --;
            rot->currentSteps = 0;
        }

        //Adjust counter for boundaries
        if(rot->counter < ROTARY_MIN){
            rot->counter = ROTARY_MAX;
        }else if(rot->counter > ROTARY_MAX){
            rot->counter = ROTARY_MIN;
        }
        rot->rotAlastState = rot->rotA;
    }
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
            checkRotEncoder(&Rot);
            PORTA = ~Rot.counter;
        }
        INTCONbits.RBIF = 0;
    }
}