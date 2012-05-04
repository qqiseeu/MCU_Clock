#ifndef _I2CCOM_H
#define _I2CCOM_H

/*for using the I2C bus to write to AT24C02 EEPROM*/

extern void I2C_writetime(UCHAR time[]);
extern void I2C_readtime(UCHAR time[]);
extern void I2C_init(void);

#endif
