#ifndef  __I2C_H__
#define  __I2C_H__

#include "stm32f10x.h"

void i2c_init(void);
void delay1(unsigned int n);

void SCL_Output( uint16_t val );
void SDA_Output( uint16_t val );

void I2CStart(void);
void I2CStop(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
unsigned char I2CWaitAck(void);

void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);

#endif
