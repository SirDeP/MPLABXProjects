/***********************************************************************
 * Universal startup code structure for new and simple projects
 * 
 * PIC   : 16F1829
 * Board : PICkit 5 Low Pin Count Demo Board (PK3LPCDB) or breadboard
 * Date  : A long time ago, in a galaxy far away
 * 
 **********************************************************************/

#include <xc.h>                 // PIC hardware mapping
// This auto-determines (using sub-includes, #pragma's, and IDE-
// controlled data sources) that 
// /opt/microchip/xc8/v<compiler_version>/pic/include/proc/pic16f1829.h
// should be included ...
// As for Windoze, `/opt' is probably somewhere in the user's AppData 
// folder ...

#define _XTAL_FREQ      125000  // Used by the XC8 delay_ms(x) macro (125 kHz)

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
// #pragma config CONFIG1 = 0b (00)00 1111 1010 0100
// i.e. (don't do this)
// #pragma config CONFIG1 = 0x0FA4

// CONFIG WORD 2 @ flash program memory location 0x8008
#pragma config WRT      = OFF
#pragma config PLLEN    = OFF
#pragma config STVREN   = OFF
#pragma config BORV     = LO
//#pragma config DEBUG    = OFF   // Pre-processor short-circuits !
#pragma config LVP      = OFF

/* LEDS: */
#define LED_1   LATCbits.LATC0
#define LED_2   LATCbits.LATC1
#define LED_4   LATCbits.LATC3

#define BUTTON_1 PORTAbits.RA2 // de Button

#define F_osc 125000        // FREQUENTIE van osccilator
#define PRESCALER_TMR1 8    // Deze staat hier op 8, maar als je dze zou weizigen moet je hem ook daadwerkelijk verandere bij de T1CONbits.T1CKPS
#define MAX_TMR1_PRELOAD    65536 // Maximale waarde van de TMR1 PRELOAD, voorbij dit getal geeft overflow

#define UITPERCENTAGE 0.1 // (std: 10%)
#define PULSDELER 2 // (std: 10%)


#define NUM_ROUNDS 15 // Het aantal 'rondes' dat we willen uitvoeren

int timer_opname_100ms = 0;
int release = 0;
int release_2 = 0;

int button_1_getriggerd = 1;

int aan_uit_schakelaar = 0;

double tijd_opname = 0; // tijd_opname, aanname van 100 eenheden voor dit voorbeelddouble tijd_opname = (timer_opname_100ms / 50);
int index_counter = 0;
// Arrays om de aan-tijd en uit-tijd voor elke ronde op te slaan
long double tijd_aan[NUM_ROUNDS + 1];
long double tijd_uit[NUM_ROUNDS + 1];
double preload_calc_on[NUM_ROUNDS + 1];
double preload_calc_off[NUM_ROUNDS + 1];
/* Hardware / state naming scheme ------------------------------------*/

// ...
//lol
/* End-of hardware / state naming scheme -----------------------------*/

//>>

/* Forward function declarations + main function hereafter -----------*/

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void timer_calc(void);

// ... more init function declarations here, one per each subsystem !

// ... and even more function declarations here, for module behaviors !

void main(void)
    {
    // Do all initialisation here
    pic_init();
    while (1)
        {
        // Leeg :P
        }
    }

void pic_init(void)
    {
    init_osc();
    init_gpio();
    }

void init_osc(void)
    {
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b00; // Clk determined by FOSC in Conf-1
    // Internal Resistor-Capacitor Frequency select (IRCF)sadasdasdasd
    OSCCONbits.IRCF = 0b0101; // 125 kHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
    // XOR (Don't do this, it's unmaintainable)
    //OSCCON = 0b00101000; //0b00111000;
    }

/* End-of Init functions -------------------------------------------- */
void init_gpio(void)
    {
    /* Button en LED init */
    TRISAbits.TRISA2 = 1; // A2 (BUTTON) moet op input ingesteld worden
    TRISC = 0; // Gooi de hele C register op output, want daar zitten de LED's
    ANSELAbits.ANSA2 = 0; // Analog UIT voor button
    LED_4 = 0; // LED 4 is de PROCES indicator
    LED_1 = 0; // LED 1 is de Stopwatch licht plotter
    LED_2 = 1; // LED 2 is de "om de halve seconde lichtje" indicator

//    /* TMR1 config/init */
//    T1CON = 0x10;
//    T1GCON = 0x00;
//    T1CONbits.T1CKPS = 0b11; // Prescaler staat op 8 (3 in bin voor config)
/* TMR1 config/init */
    T1CONbits.TMR1CS = 0b00; // DE clock source select bits (F_osc)
    T1CONbits.T1OSCEN = 0b1;
    T1CONbits.nT1SYNC = 0b1;
    T1CONbits.TMR1ON = 0b1;
    T1CONbits.T1CKPS = 0b11; // Prescaler staat op 8 (3 in bin voor config)
    
    T1GCON = 0b00000000;

    //Timer2 Registers Prescaler= 16 - TMR2 PostScaler = 1 - PR2 = 156 - Freq = 50.08 Hz - Period = 0.019968 seconds
    T2CON |= 56; // bits 6-3 Post scaler 1:1 thru 1:16
    TMR2ON = 1; // bit 2 turn timer2 on;
    T2CKPS1 = 1; // bits 1-0  Prescaler Rate Select bits
    T2CKPS0 = 0;
    PR2 = 122; // PR2 (Timer2 Match value)


    TMR1IE = 1;
    TMR2IE = 1;
    GIE = 1;
    PEIE = 1;
    
    TMR2ON = 1;
    TMR1 = 61629; //TMR1 = 65047; //1sec=64560;
    //TMR2 = 122;
    }

//>>

/* Interrupt Service Routine (ISR) hereafter -- keep with main module!*/

void timer_calc(void)
    {
    LED_2 = 0; // Zet de indicator LED uit
    LED_4 = 1; // Laat zien met LED_4 hoelang hij aan het calculeren is (AAN)
    for (int round = 0; round <= NUM_ROUNDS-1; round++)
        {
        if (round == 0)
            {
            tijd_aan[round] = tijd_opname; // (1 << (round)
            tijd_uit[round] = UITPERCENTAGE * tijd_aan[round];
            tijd_aan[round + 1] = tijd_aan[round] / PULSDELER;
            }
        else
            tijd_aan[round + 1] = tijd_aan[round] / PULSDELER; //  tijd_aan[round] = tijd_opname / (powf(round,PULSDELER)); // (1 << (round)
        tijd_uit[round+1] = UITPERCENTAGE * tijd_aan[round+1];
        }
    for (int round = 0; round <= NUM_ROUNDS; round++)
        {
        preload_calc_on[round] = (double)(-1 * (((F_osc / 4) / PRESCALER_TMR1) * tijd_aan[round] - MAX_TMR1_PRELOAD));
        preload_calc_off[round] = (double)(-1 * (((F_osc / 4) / PRESCALER_TMR1) * tijd_uit[round] - MAX_TMR1_PRELOAD));
        }
    LED_4 = 0; // Laat zien met LED_4 hoelang hij aan het calculeren is (UIT)
    }

void __interrupt() isr(void)
    {
    if (TMR2IF && button_1_getriggerd == 1) // Als knop getriggerd wordt met rising edge:
        {
        TMR2IF = 0; // Hiervoor gebruik ik TMR2
        LED_2 ^= 1; // Halve seconde LED indicator
        }

    if (INTCONbits.INTF) // Button event handelaar:
        {
        button_1_getriggerd ^= 1;
        switch (button_1_getriggerd)
            {
            case 0:
                OPTION_REGbits.INTEDG = 0; //
                INTCONbits.INTF = 0;
                break;
            case 1:
                OPTION_REGbits.INTEDG = 1; //
                INTCONbits.INTF = 0;
                break;
            }
        }
    // Als TMR1Flag=1 en button is ingedrukt EN hij is nog niet los gelaten doe:
    if (TMR1IF && button_1_getriggerd == 1 && release == 0)
        {
        release_2 = 1;
        TMR1IF = 0;
        timer_opname_100ms += 1;
        tijd_opname = (timer_opname_100ms); // /50
        TMR1 = 61629;
        }
        // Als button is NIET ingedrukt EN hij is los gelaten doe:
    else if (button_1_getriggerd == 0 && release == 0 && release_2 == 1)
        {
        timer_calc();
        release = 1;
        }

    // Zodra release weer 1 wordt, begin het afspelen van de stopwatch
    if (aan_uit_schakelaar == 0 && TMR1IF && button_1_getriggerd == 0 && release == 1)
        {
        LED_1 = 1;
        TMR1IF = 0;
        TMR1 = (unsigned short) preload_calc_on[index_counter];

        aan_uit_schakelaar = 1;
        }
    else if (aan_uit_schakelaar == 1 && TMR1IF && button_1_getriggerd == 0 && release == 1)
        {
        LED_1 = 0;
        TMR1IF = 0;
        TMR1 = (unsigned short) preload_calc_off[index_counter];

        index_counter += 1;
        if (index_counter >= NUM_ROUNDS)
            aan_uit_schakelaar = -1;
        else
            aan_uit_schakelaar = 0;
        }
    if (aan_uit_schakelaar == -1)
        {
        timer_opname_100ms = 0;
        release = 0;
        release_2 = 0;
        button_1_getriggerd = 0;
        aan_uit_schakelaar = 0;
        tijd_opname = 0; // tijd_opname, aanname van 100 eenheden voor dit voorbeelddouble tijd_opname = (timer_opname_100ms / 50);
        index_counter = 0;
        }
    }
/* End-of Interrupt Service Routine (ISR) -------------------------------------------*/

//>>