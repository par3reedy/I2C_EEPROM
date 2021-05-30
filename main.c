//Patrick Reedy; May 24, 2021

/*--------------------------------------------------------------------------------------------------------------------------------------
*  Library Declarations
--------------------------------------------------------------------------------------------------------------------------------------*/
#include "msp.h"
#include "DCO.h"
#include "EEPROM.h"
#include "LED.h"

/*--------------------------------------------------------------------------------------------------------------------------------------
*  Function Prototypes
--------------------------------------------------------------------------------------------------------------------------------------*/
void EEPROM_init(uint8_t address);
uint8_t EEPROM_read(uint16_t address);
void EEPROM_write(uint16_t address, uint8_t byte);

/*--------------------------------------------------------------------------------------------------------------------------------------
*  Main
--------------------------------------------------------------------------------------------------------------------------------------*/

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     //Stop watchdog timer
    LEDPORT->SEL0 &= ~LEDPINS;                      //Configure onboard LED for GPIO
    LEDPORT->SEL1 &= ~LEDPINS;
    LEDPORT->DIR = LEDPINS;                         //Set the LEDPINS for pull up resistors
    LEDPORT->OUT = RED;                             //Initialize the LED as RED
    uint16_t data;                                  //Initialize variable used for reading data
    EEPROM_init(A0L_A1H_A2L);

    while(1) {                                      //Always run
        EEPROM_write(0x0020, 0xAB);                 //Write 0xAB to the adress 0x0020
        __delay_cycles(FIVEMS);                     //Delay for the maximum 5ms write time
        data = EEPROM_read(0x0020);                 //Read from 0x0020 and save the data
        if (data == 0xAB){                          //Check if the read data is the correct data
            LEDPORT->OUT = GREEN;                   //If correct change LED to GREEN
        }
        else {
            LEDPORT->OUT = RED;                     //If wrong change LED to RED
        }
    }
}
