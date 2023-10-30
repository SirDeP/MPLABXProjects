#include <xc.h>                 // PIC hardware mapping

#define _XTAL_FREQ      500000  // Used by the XC8 delay_ms(x) macro (500 kHz)

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

// CONFIG WORD 2 @ flash program memory location 0x8008
#pragma config WRT      = OFF
#pragma config PLLEN    = OFF
#pragma config STVREN   = OFF
#pragma config BORV     = LO
//#pragma config DEBUG    = OFF   // Pre-processor short-circuits !
#pragma config LVP      = OFF // Low voltage programming

/* LEDS: */
#define LED1   LATCbits.LATC0
#define LED2   LATCbits.LATC1
#define LED3   LATCbits.LATC2
#define LED4   LATCbits.LATC3

#define BUTTON PORTAbits.RA2 // de Button

int ButtonTrigger = 1;
int on = 0;
int counter = 0;

const int lookuptable[256] = {
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 81, 82, 83, 84, 85, 86, 87, 89, 90, 91, 92, 93, 95, 96, 97, 98, 100, 101, 102, 103, 105, 106, 107, 108, 110, 111, 112, 114, 115, 116, 118, 119, 121, 122, 123, 125, 126, 127, 129, 130, 132, 133, 135, 136, 138, 139, 141, 142, 144, 145, 147, 148, 150, 151, 153, 154, 156, 157, 159, 160, 162, 164, 165, 167, 169, 170, 172, 173, 175, 177, 178, 180, 182, 183, 185, 187, 189, 190, 192, 194, 196, 197, 199, 201, 203, 204, 206, 208, 210, 212, 213, 215, 217, 219, 221, 223, 225, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 255
};

int OnOfSwitch = 0; // Voor het switchen van on naar uit en andersom

void pic_init(void);
void init_gpio(void);
void init_osc(void);
void init_timer(void);

void main(void)
{
    pic_init();
    while (1)
    {
    }
}

void pic_init(void)
{

    init_osc();
    init_gpio();
    init_timer();
}

void init_osc(void)
{
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b00; // Clk determined by FOSC in Conf-1
    // Internal Resistor-Capacitor Frequency select (IRCF)sadasdasdasd
    OSCCONbits.IRCF = 0b0111; // 500 kHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
}

/* End-of Init functions -------------------------------------------- */
void init_gpio(void)
{
    /* Button en LED init */
    TRISAbits.TRISA2 = 1; // A2 (BUTTON) to input
    TRISC = 0; // LEDs
    ANSELAbits.ANSA2 = 0; // Button analog off
    LED4 = 0; // LED 4 is de process indicator_1 (de T_on en T_uit calculatie proces)
    LED3 = 0; // LED 3 is de process indicator_2 (de PRELOAD calculatie proces)
    LED2 = 0; // LED 2 is de stopwatch (geel)
    LED1 = 0; // LED 1 is de puls plotter 

    PR2 = 63;
    CCP2CONbits.DC2B = 0;
    CCP2CONbits.CCP2M = 0b1100; // PWM mode
    CCPR2L = 0;
}

void init_timer(void)
{
    //Timer2 Registers Prescaler= 16 - TMR2 PostScaler = 4 - PR2 = 244 - Freq = 8.00 Hz - Period = 0.124928 seconds
    T2CON |= 24; // bits 6-3 Post scaler 1:1 thru 1:16
    TMR2ON = 1; // bit 2 turn timer2 on;
    T2CKPS1 = 1; // bits 1-0  Prescaler Rate Select bits
    T2CKPS0 = 0;
    PR2 = 63; // PR2 (Timer2 Match value)

    TMR2IE = 1; // Enable TMR2
    GIE = 1;
    PEIE = 1;

    TMR2ON = 1;
}

/* Interrupt Service Routine (ISR) hereafter -- keep with main module!*/

void __interrupt() isr(void)
{
    if (on == 1 && OnOfSwitch == 0)
    {
        if (TMR2IF)
        {
            //  release_b2 = 1;
            TMR2IF = 0;
            //CCP2CONbits.DC2B = (counter );
            CCPR2L = (unsigned char) lookuptable[counter];

            counter++;

            if (counter <= 253 && counter > 252)
            {
                LED1 = 1;
            }
            if (counter >= 253)
            {
                on = 0;
                OnOfSwitch = 1;
            }
        }
    }
    if (on == 1 && OnOfSwitch == 1)
    {
        if (TMR2IF)
        {
            TMR2IF = 0;
            CCPR2L = (unsigned char) lookuptable[counter];

            counter--;

            if (counter < 253)
                LED1 = 0;
            if (counter < 5 && counter > 0)
            {
                LED1 = 1;
            }
            if (counter == 0)
            {
                on = 0;
                OnOfSwitch = 0;
            }
        }
    }
    if (INTCONbits.INTF) // Button event handler:
    {
        ButtonTrigger ^= 1;
        switch (ButtonTrigger)
        {
            case 0:
                on = 0;
                OPTION_REGbits.INTEDG = 0; //
                INTCONbits.INTF = 0;
                break;
            case 1:
                on = 1;
                OPTION_REGbits.INTEDG = 1; //
                INTCONbits.INTF = 0;
                break;
        }
    }
}
/* End-of Interrupt Service Routine (ISR) -------------------------------------------*/