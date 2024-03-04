#include "spi.h"

void spi_init(sspmode mst_slv_oscdiv, rxsmpmoment sdi_rx_smp, clkpol clock_pol, txclkedge sdo_tx_edge)
{
    SSPCON1BITS_SSPM = mst_slv_oscdiv; // Set `SSPM' datafield (SPI master / slave mode and prescaler selection)
    SDO_DDIR = 0; // Set SDO_DDIR for SDO to behave as an output

    if (mst_slv_oscdiv >= SLV_SS_EN && mst_slv_oscdiv <= SLV_SS_DIS) { // SLAVEMODE
        SSPSTATBITS_SMP = 0; // The SMP bit MUST remain clear (see datasheet)
        SCK_DDIR = 1; // Set SCK_DDIR for SCK to behave like an input          
        SSPCON1BITS_CKP = SCK_PIN; //  The data line `idle state' (CKP bit) must match the master's clock idle state.
        GIE = 1; // Interrupts Slave
        PEIE = 1;
        SSPIE_ENABLE = 1;
    }
    else if (mst_slv_oscdiv >= MST_OSC_DIV_04 && mst_slv_oscdiv <= MST_OSC_DIV_T2) { // MASTERMODE
        SSPSTATBITS_SMP = sdi_rx_smp; //The SMP bit needs to be set according to argument
        SCK_DDIR = 0; // Set SCK_DDIR for SCK to behave like an output
        SSPCON1BITS_CKP = clock_pol; // The data line `idle state' (CKP bit) needs to be set according to argument `clock_pol'
    }
    if (clock_pol == 0) {
        sdo_tx_edge ^= 1;
    }
        SSPSTATBITS_CKE = sdo_tx_edge; // Set the CKE bit according to the `sdo_tx_edge'
    
    SSPSTATBITS_CKE = sdo_tx_edge; // Set the CKE bit according to the `sdo_tx_edge'
    SSPCON1BITS_SSPEN = 1; // Enable the SPI module (by setting SSPEN bit).
}

void spi_transmit(char c)
{
    SSPBUFFER = c; // FILL BUFFER
}