/***********************************************************************
 * 
 * PIC   : 16F1829
 * Date  : A long time ago, in a galaxy far away (20-11-'23)
 * Author: SirDeP
 * 
 **********************************************************************/

#include <xc.h>                 // PIC hardware mapping
#define _XTAL_FREQ      4000000  // Used by the XC8 delay_ms(x) macro (4 MHz)

// For acceptable values use both datasheet and this file:
// /opt/microchip/xc8/v<compiler_version>/docs/pic_chipinfo.html

typedef struct __encoder
{
    int LastState;
    int Steps;
    int Input;
} _encoder;
_encoder encoder = {0, 0, 0};

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

#define LED LATCbits.LATC6
#define AANTAL_LEDS 4
#define ENC_B PORTBbits.RB5
#define ENC_A PORTBbits.RB4
#define ROTARY_MAX   4 //Max value to be reached with rotary encoder
#define ROTARY_MIN   1 //Min value to be reached with rotary encoder
#define ROTARY_STEPS 2 //Amount of "clicks" before the rotary encoder changes the value

void pic_init(void);
void init_gpio(void);
void init_osc(void);

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
}

void init_osc(void)
{
    // System Clock Select (SCS)
    OSCCONbits.SCS = 0b00; // Clk determined by FOSC in Conf-1
    // Internal Resistor-Capacitor Frequency select (IRCF)
    OSCCONbits.IRCF = 0b1101; // 4 MHz clock speed
    // Software Phase-Locked Loop ENable (SPLLEN)
    OSCCONbits.SPLLEN = 0b0; // 4 x Phase-Locked Loop disabled
}

void init_gpio(void)
{
    ANSELC = 0;
    ANSELB = 0;
    ANSELA = 0;
    TRISC = 0;
    encoder.LastState = 1;
    encoder.Input = 1;
    TRISBbits.TRISB4 = 1; // Rot Enc Pins == Input
    TRISBbits.TRISB5 = 1; // 
    IOCBPbits.IOCBP4 = 1; // RB4 IOC
    IOCBPbits.IOCBP5 = 1; // RB5 IOC

    GIE = 1;
    IOCIE = 1;
}

void encoder_lezer()
{
    // Only check for a change in the CLK pin to identify "full click"
    int a = ENC_A;

    // If state changed, a movement is registered
    if (a != encoder.LastState) {
        int b = ENC_B;

        if (b != a)
            encoder.Steps++; // Clockwise
        else
            encoder.Steps--; // Anti-clockwise

        // Only change counter if correct amount of clicks have been registered
        if (encoder.Steps > (ROTARY_STEPS * 2)) {
            encoder.Input++;
            encoder.Steps = 0;
        }
        else if (encoder.Steps < (-(ROTARY_STEPS * 2))) {
            encoder.Input--;
            encoder.Steps = 0;
        }

        // Adjust counter for boundaries
        if (encoder.Input < ROTARY_MIN)
            encoder.Input = ROTARY_MAX;
        else if (encoder.Input > ROTARY_MAX)
            encoder.Input = ROTARY_MIN;

        encoder.LastState = a;
    }
    PORTC = (unsigned char) (~(1 << (encoder.Input - 1)));
}

void __interrupt() isr(void)
{
    if (INTCONbits.IOCIF == 1) {

        encoder_lezer();
        INTCONbits.IOCIF = 0;
    }
}

/* End-of Interrupt Service Routine (ISR) -------------------------------------------*/