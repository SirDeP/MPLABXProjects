/***********************************************************************
 * Universal startup code structure for new and simple projects
 * 
 * PIC   : 16F1829
 * Board : PICkit 5 Low Pin Count Demo Board (PK3LPCDB) or breadboard
 * Date  : A long time ago, in a galaxy far away
 * 
 **********************************************************************/

#include <xc.h>                 // PIC hardware mapping

#define _XTAL_FREQ      125000  // Used by the XC8 delay_ms(x) macro (125 kHz)

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

#define F_osc 125000        // OSC frequency
#define PRESCALER_TMR1 8    // Prescaler of TIMER1, if changed also change T1CONbits.T1CKPS
#define MAX_TMR1_PRELOAD    65536 // MAX value of TMR1 Preload, more than this is overflow

#define UITPERCENTAGE 0.1 // (std: 10%)
#define PULSDELER 2 // halves T_on each time


#define NUM_ROUNDS 15 // Total amount of executable rounds

int timer_opname_100ms = 0;
int Release = 0;
int Release2 = 0;

int button_triggered = 1;

int OnOfSwitch = 0;

double Recorded_Time = 0; // Record Time
int CurrentRound = 0; // Current Round
// Arrays to calculate Time_On and Time_Off for every round
long double Time_On[NUM_ROUNDS + 1];
long double Time_Off[NUM_ROUNDS + 1];
double preload_calc_on[NUM_ROUNDS + 1];
double preload_calc_off[NUM_ROUNDS + 1];

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void timer_calc(void);

void main(void)
    {
    pic_init();
    while (1)
        {
        // Empty as the void
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
    /* Button and LED init */
    TRISAbits.TRISA2 = 1; // A2 (BUTTON) is Input
    TRISC = 0; // Reg C is output
    ANSELAbits.ANSA2 = 0; // Analog OFF for button
    LED_4 = 0; // LED 4 = Program calculation
    LED_2 = 1; // LED 2 = Stopwatch (500ms)
    LED_1 = 0; // LED 1 is de Stopwatch licht plotter
    
    /* TMR1 config and init */
    T1CONbits.TMR1CS = 0b00; // Clock source select bits (F_OSC)
    T1CONbits.T1OSCEN = 0b1;
    T1CONbits.nT1SYNC = 0b1;
    T1CONbits.TMR1ON = 0b1;
    T1CONbits.T1CKPS = 0b11; // Prescaler = 1:8
    
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
    }

//>>

/* Interrupt Service Routine (ISR) hereafter -- keep with main module!*/
void timer_calc(void)
    {
    LED_2 = 0; // Turn Indicator off
    LED_4 = 1; // Turn LED4 on while calculating to show delay
    for (int round = 0; round <= NUM_ROUNDS-1; round++)
        {
        if (round == 0)
            {
            Time_On[round] = Recorded_Time; // (1 << (round)
            Time_Off[round] = UITPERCENTAGE * Time_On[round];
            Time_On[round + 1] = Time_On[round] / PULSDELER;
            }
        else
            Time_On[round + 1] = Time_On[round] / PULSDELER; 
        Time_Off[round+1] = UITPERCENTAGE * Time_On[round+1];
        }
    for (int round = 0; round <= NUM_ROUNDS; round++)
        { //Precalculate the time on and off for compatability with the TMR register
        preload_calc_on[round] = (double)(-1 * (((F_osc / 4) / PRESCALER_TMR1) * Time_On[round] - MAX_TMR1_PRELOAD));
        preload_calc_off[round] = (double)(-1 * (((F_osc / 4) / PRESCALER_TMR1) * Time_Off[round] - MAX_TMR1_PRELOAD));
        }
    LED_4 = 0; // Turn LED4 off when finished
    }

void __interrupt() isr(void)
    {
    if (TMR2IF && button_triggered == 1) // If button triggered with rising edge:
        {
        TMR2IF = 0; // Run stopwatch using TIMER2
        LED_2 ^= 1; // half-second LED indicator
        }

    if (INTCONbits.INTF) // Button event handler:
        {
        button_triggered ^= 1;
        switch (button_triggered)
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
    // If TMR1Flag=1 and button is Pressed AND has not been triggered:
    if (TMR1IF && button_triggered == 1 && Release == 0)
        {
        Release2 = 1;
        TMR1IF = 0;
        Recorded_Time += 1;
        TMR1 = 61629;
        }
        // If button is NOT Pressed AND has been triggered:
    else if (button_triggered == 0 && Release == 0 && Release2 == 1)
        {
        timer_calc();
        Release = 1;
        }

    // As soon as Release becomes 1, begin playback
    if (OnOfSwitch == 0 && TMR1IF && button_triggered == 0 && Release == 1)
        {
        LED_1 = 1;
        TMR1IF = 0;
        TMR1 = (unsigned short) preload_calc_on[CurrentRound];

        OnOfSwitch = 1;
        }
    else if (OnOfSwitch == 1 && TMR1IF && button_triggered == 0 && Release == 1)
        {
        LED_1 = 0;
        TMR1IF = 0;
        TMR1 = (unsigned short) preload_calc_off[CurrentRound]; //load pre-calculated time converted to TMR register format

        CurrentRound += 1; // Goto next round
        if (CurrentRound >= NUM_ROUNDS)// if finished final round
            OnOfSwitch = -1;
        else
            OnOfSwitch = 0; // continue
        }
    if (OnOfSwitch == -1) // Reset variables to begin state
        {
        Release = 0;
        Release2 = 0;
        button_triggered = 0;
        OnOfSwitch = 0;
        Recorded_Time = 0;
        CurrentRound = 0;
        }
    }
/* End-of Interrupt Service Routine (ISR) -------------------------------------------*/

//>>