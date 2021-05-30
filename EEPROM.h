//Patrick Reedy; May 24, 2021

#ifndef EEPROM_H_
#define EEPROM_H_

#define I2CPORT         P1          //Port to be used with I2C protocol
#define I2CSDA          BIT6        //Serial Data pin
#define I2CSCL          BIT7        //Clock pin

#define LOWADDMASK      0xFF        //Mask to save the lower byte of info

#define A0L_A1L_A2H     0x54        //Value associated with A0-A2 pins on EEPROM
#define A0L_A1H_A2L     0x52
#define A0H_A1L_A2L     0x51

#define FIVEMS          15000       //5ms / (1/3MHz) = 15000 3MHz clock cycles for 5ms

#endif /* EEPROM_H_ */
