//Patrick Reedy; May 24, 2021

/*--------------------------------------------------------------------------------------------------------------------------------------
*  Library Declarations
--------------------------------------------------------------------------------------------------------------------------------------*/
#include "msp.h"
#include "EEPROM.h"
#include "DCO.h"

/*--------------------------------------------------------------------------------------------------------------------------------------
*  Function Prototypes
--------------------------------------------------------------------------------------------------------------------------------------*/
void set_DCO(uint32_t freq);

/*--------------------------------------------------------------------------------------------------------------------------------------
*  Function Definitions
--------------------------------------------------------------------------------------------------------------------------------------*/

//This function intializes I2C to work with the EEPROM
void EEPROM_init(uint8_t address){
    set_DCO(FREQ_3_MHZ);                                    //Set the DCO to 3MHz with SMCLK 3MHz
    I2CPORT->SEL0 |= (I2CSDA | I2CSCL);                     //Set I2C serial data and clock pins for EUSCIB0
    I2CPORT->SEL1 &= ~(I2CSDA | I2CSCL);
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;                 //Software reset enabled
    EUSCI_B0->CTLW0 = (EUSCI_B_CTLW0_SWRST                  //Set software reset
                     | EUSCI_B_CTLW0_SSEL__SMCLK            //Set EUSCIB0 to the SMCLK
                     | EUSCI_B_CTLW0_MODE_3                 //Set EUSCIB0 to I2C mode
                     | EUSCI_B_CTLW0_MST                    //Set EUSCIB0 to Controller mode to control EEPROM
                     | EUSCI_B_CTLW0_SYNC);                 //Synchronous mode enabled for rising clock edge
    EUSCI_B0->BRW = 0x08;                                   //Divide EUSCIB0 clock by 8 for Fast Mode (3MHz/8 = 375kHz)
    EUSCI_B0->I2CSA = address;                              //Set the address of I2C for the EEPROM config
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;                //Disable the software reset to allow usage
}

//This function reads data from the EEPROM
uint8_t EEPROM_read(uint16_t address){
    uint8_t data;                                           //Initialize variable used to return the read data from EEPROM
    uint8_t lower = (address & LOWADDMASK);                 //Take the input address and save the lower byte
    uint8_t upper = (address >> 8);                         //Take the input address and save the upper byte
    EUSCI_B0->CTLW0 |= (EUSCI_B_CTLW0_TR                    //Set mode to transmit
                      | EUSCI_B_CTLW0_TXSTT);               //Set start bit to 1 (to begin transmission)
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));            //Wait for transmit buffer to be empty
    EUSCI_B0->TXBUF = upper;                                //Send the upper address to the transmit buffer (Upper first b/c MSB first)
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = lower;                                //Send the lower address to the transmit buffer
    while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;                   //Set mode to receive
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;                 //Set start bit to 1 to begin receiving
    while((EUSCI_B0 -> CTLW0 & EUSCI_B_CTLW0_TXSTT));       //Wait until UCTXSTT flag is cleared (cleared when peripheral address sent)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;                 //Send stop bit to end Controller/Peripheral transmission
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));            //Wait for the receive buffer to be empty
    data = EUSCI_B0->RXBUF;                                 //Read the data from the recieve buffer
    return data;
}

//This function writes data to the EEPROM
void EEPROM_write(uint16_t address, uint8_t data){
    uint8_t lower = (address & LOWADDMASK);                 //Take the input address and save the lower byte
    uint8_t upper = (address >> 8);                         //Take the input address and save the upper byte
    EUSCI_B0->CTLW0 |= (EUSCI_B_CTLW0_TR                    //Set mode to transmit
                      | EUSCI_B_CTLW0_TXSTT);               //Set start bit to 1 (to begin transmission)
    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT);           //Wait until UCTXSTT flag is cleared (cleared when peripheral address sent)
    EUSCI_B0->TXBUF = upper;                                //Send the upper address to the transmit buffer (Upper first b/c MSB first)
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = lower;                                //Send the lower address to the transmit buffer
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->TXBUF = data;                                 //Send the data to be saved in the EEPROM
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;                 //Send stop bit to end Controller/Peripheral transmission
}
