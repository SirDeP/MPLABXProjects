#ifndef SPI_H
#define	SPI_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef enum {
    MST_OSC_DIV_04,   // 0x0: 0000
    MST_OSC_DIV_16,   // 0x1: 0001
    MST_OSC_DIV_64,   // 0x2: 0010
    MST_OSC_DIV_T2,   // 0x3: 0011
    SLV_SS_EN,        // 0x4: 0100 - not used in this version of our lib ...
    SLV_SS_DIS        // 0x5: 0101
} sspmode;

// Datasheet SMP (Sample Timing) behavior in (SPI MASTER MODE ONLY):
// 1 = Input data sampled (received) at end of data output time
// 0 = Input data sampled (recieved) at middle of data output time
typedef enum { SMP0_SDI_MIDDLE,
    SMP1_SDI_END } rxsmpmoment;

// Datasheet CKP (Clock Polarity) select bit behavior (SPI mode):
// 1 = Idle state for clock is a high level
// 0 = Idle state for clock is a low level
typedef enum { CKP0_CPOL0_IDLE_LOW,
    CKP1_CPOL1_IDLE_HIGH } clkpol;

// Datasheet CKE (Clock Edge) select bit behavior (SPI mode):
// 1 = Transmit occurs on transition from active to Idle clock state
// 0 = Transmit occurs on transition from Idle to active clock state
// ! NOTE: Microchip CKE bit is always the inverse of Motorola CPHA bit!
typedef enum { CKE0_CPHA1_TRSMIT_IDL2ACT,
    CKE1_CPHA0_TRSMIT_ACT2IDL } txclkedge;

// Prototypes
void spi_init(sspmode mst_slv_oscdiv, rxsmpmoment sdi_rx_smp, clkpol clock_pol, txclkedge sdo_tx_edge);
void spi_transmit(char c);
char spi_read(void);

#define PIC16F1829MSSP1 // Select correct PIC
//#define PIC16F1829MSSP2
//#define PIC16F887

#endif	/* SPI_H */

