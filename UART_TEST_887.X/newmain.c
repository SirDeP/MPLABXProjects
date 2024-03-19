/*
 * UART Example with PIC16F887 microcontroller.
 * C Code for MPLAB XC8 compiler.
 * Internal oscillator used @ 8MHz.
 * This is a free software with NO WARRANTY.
 * https://simple-circuit.com/
*/ 
 
// set configuration words
#pragma config CONFIG1 = 0x2CD4
#pragma config CONFIG2 = 0x0700
 
 
#include <xc.h>
#define _XTAL_FREQ 8000000
//#include <stdint.h>        // include stdint header
 
 
/********************** UART functions **************************/
void UART_Init(const uint32_t baud_rate)
{
  int16_t n = ( _XTAL_FREQ / (16 * baud_rate) ) - 1;
  
  if (n < 0)
    n = 0;
 
  if (n > 255)  // low speed
  {
    n = ( _XTAL_FREQ / (64 * baud_rate) ) - 1;
    if (n > 255)
      n = 255;
    SPBRG = n;
    TXSTA = 0x20;  // transmit enabled, low speed mode
  }
 
  else   // high speed
  {
    SPBRG = n;
    TXSTA = 0x24;  // transmit enabled, high speed mode
  }
 
  RCSTA = 0x90;  // serial port enabled, continues receive enabled
 
}
 
__bit UART_Data_Ready()
{
  return RCIF;  // return RCIF bit (register PIR1, bit 5)
}
 
uint8_t UART_GetC()
{
  while (RCIF == 0) ;  // wait for data receive
  if (OERR)  // if there is overrun error
  {  // clear overrun error bit
    CREN = 0;
    CREN = 1;
  }
  return RCREG;        // read from EUSART receive data register
}
 
void UART_PutC(const char data)
{
  while (TRMT == 0);  // wait for transmit shift register to be empty
  TXREG = data;       // update EUSART transmit data register
}
 
void UART_Print(const char *data)
{
  uint8_t i = 0;
  while (data[i] != '\0')
    UART_PutC (data[i++]);
}
/********************** end UART functions **************************/
 
const char message[] = "PIC16F887 microcontroller UART example" ;
int function_triggered = 0;
int low_duration = 0;
// main function
void main(void)
{
  OSCCON = 0x70;    // set internal oscillator to 8MHz
       TRISDbits.TRISD1 = 1;
    TRISDbits.TRISD2 = 0;
  UART_Init(9600);  // initialize UART module with 9600 baud
 
  __delay_ms(2000);  // wait 2 seconds
 
  UART_Print("Hello world!\r\n");  // UART print
 
  __delay_ms(1000);  // wait 1 second
 
  UART_Print("PIC16F887 microcontroller UART example");  // UART print message
 
  __delay_ms(1000);  // wait 1 second
 
  UART_Print("\r\n");  // start new line
 

  
  
      while (1) {
        if (PORTDbits.RD1 == 1) {
            UART_Print("High\r\n");
        } else
            UART_Print("Low\r\n");
                   
 
        // Delay for 1 microsecond
        
    }
 
}
// end of code.