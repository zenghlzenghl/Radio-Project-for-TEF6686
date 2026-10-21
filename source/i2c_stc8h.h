#ifndef I2C_STC8H_H
#define I2C_STC8H_H

#include "stc8h.h"

#define I2C_SCL  P1_1
#define I2C_SDA  P1_0

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(uint8);
uint8 I2C_RecvByte(void);
void I2C_SendAck(uint8);
uint8 I2C_RecvAck(void);
void I2C_Write(uint8, uint16);
void I2C_Read(uint8, uint16);

#endif