#include "spi.h"

#ifdef PIC16F887
// For the 16F887, only one SPI port:
#define SCK_PIN PORTCbits.RC3
#define SDO_DDIR TRISCbits.TRISC5   // DDIR = Data DIRection
#define SCK_DDIR TRISCbits.TRISC3
#endif
#ifdef PIC16F1829MSSP1
// For the 16F1829, MSSP1:
#define SCK_PIN PORTBbits.RB6
#define SDO_DDIR TRISCbits.TRISC7
#define SCK_DDIR TRISBbits.TRISB6
#endif
#ifdef PIC16F1829MSSP2
// For the 16F1829, MSSP2 (default alternate pin function assignment):
#define SCK_PIN PORTBbits.RB7
#define SDO_DDIR TRISCbits.TRISC1
#define SCK_DDIR TRISBbits.TRISB7
#endif

void spi_init(sspmode mst_slv_oscdiv, rxsmpmoment sdi_rx_smp, clkpol clock_pol, txclkedge sdo_tx_edge)
{
#ifdef PIC16F1829MSSP2 // Set `SSPM' datafield (SPI master / slave mode and prescaler selection)
    SSP2CON1bits.SSPM = mst_slv_oscdiv; //
#endif//
#ifdef PIC16F1829MSSP1
    SSP1CON1bits.SSPM = mst_slv_oscdiv;
#endif
#ifdef PIC16F887
    SSPCONbits.SSPM = mst_slv_oscdiv;
#endif

    SDO_DDIR = 0; // Set SDO_DDIR for SDO to behave as an output

    if (mst_slv_oscdiv >= SLV_SS_EN && mst_slv_oscdiv <= SLV_SS_DIS) { // SLAVEMODE
#ifdef PIC16F1829MSSP2 // The SMP bit MUST remain clear (see datasheet)
        SSP2STATbits.SMP = 0;
#endif
#ifdef PIC16F1829MSSP1
        SSP1STATbits.SMP = 0;
#endif
#ifdef PIC16F887
        SSPSTATbits.SMP = 0;
#endif
        SCK_DDIR = 1; // Set SCK_DDIR for SCK to behave like an input

#ifdef PIC16F1829MSSP2              //  The data line `idle state' (CKP bit) must match the
        SSP2CON1bits.CKP = SCK_PIN; //  master's clock idle state. To achieve this, set the
#endif                              //  CKP bit to whatever value can be read on the SCK_PIN.
#ifdef PIC16F1829MSSP1
        SSP1CON1bits.CKP = SCK_PIN;
#endif
#ifdef PIC16F887
        SSPCONbits.CKP = SCK_PIN;
#endif
        
        GIE = 1; // Interrupts Slave
        PEIE = 1;
#ifdef PIC16F1829MSSP2
        PIE4bits.SSP2IE = 1;
#endif
#ifdef PIC16F1829MSSP1
        PIE1bits.SSP1IE = 1;
#endif
#ifdef PIC16F887
        PIE1bits.SSPIE = 1;
#endif

    }
    else if (mst_slv_oscdiv >= MST_OSC_DIV_04 && mst_slv_oscdiv <= MST_OSC_DIV_T2) { // MASTERMODE

#ifdef PIC16F1829MSSP2 //The SMP bit needs to be set according to argument
        SSP2STATbits.SMP = sdi_rx_smp;
#endif
#ifdef PIC16F1829MSSP1
        SSP1STATbits.SMP = sdi_rx_smp;
#endif
#ifdef PIC16F887
        SSPSTATbits.SMP = sdi_rx_smp;
#endif

        SCK_DDIR = 0; // Set SCK_DDIR for SCK to behave like an output

#ifdef PIC16F1829MSSP2         // The data line `idle state' (CKP bit) needs to be set according to argument `clock_pol'
        SSP2CON1bits.CKP = clock_pol;
#endif
#ifdef PIC16F1829MSSP1
        SSP1CON1bits.CKP = clock_pol;
#endif
#ifdef PIC16F887
        SSPCONbits.CKP = clock_pol;
#endif
    }

#ifdef PIC16F1829MSSP2              //          Set the CKE bit according to the `sdo_tx_edge' and `clock_pol'
    SSP2STATbits.CKE = sdo_tx_edge; //		parameters (i.e. if `sdo_tx_edge' and `clock_pol' select
#endif                              //		mode 0 (CKE 1, CKP 0), then the CKE bit must be set, etc. -
#ifdef PIC16F1829MSSP1
    SSP1STATbits.CKE = sdo_tx_edge;
#endif
#ifdef PIC16F887
    SSPSTATbits.CKE = sdo_tx_edge;
#endif
#ifdef PIC16F1829MSSP2 // Enable the SPI module (by setting SSPEN bit).
    SSP2CON1bits.SSPEN = 1;
#endif
#ifdef PIC16F1829MSSP1
    SSP1CON1bits.SSPEN = 1;
#endif
#ifdef PIC16F887
    SSPCONbits.SSPEN = 1;
#endif
}

void spi_transmit(char c)
{
#ifdef PIC16F1829MSSP2   // FILL SPI BUFFER
    SSP2BUF = c;         //
#endif                   //
#ifdef PIC16F1829MSSP1   //
    SSP1BUF = c;         //
#endif                   //
#ifdef PIC16F887         //
    SSPBUF = c;          //
#endif                   //
}

char spi_read() // Read the received data
{
#ifdef PIC16F1829MSSP2
    while (!SSP2STATbits.BF); // Wait for Data Receive complete
    return (SSP2BUF);
#endif
#ifdef PIC16F1829MSSP1
    while (!SSP1STATbits.BF); // Wait for Data Receive complete
    return (SSP1BUF);
#endif
#ifdef PIC16F887
    while (!SSPSTATbits.BF); // Wait for Data Receive complete
    return (SSPBUF);
#endif
}