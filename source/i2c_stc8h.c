#include "i2c_stc8h.h"

void I2C_Init(void)
{
    I2C_SCL = 1;
    I2C_SDA = 1;
}

void I2C_Delay(void)
{
    uint8 i;
    for (i = 0; i < 10; i++) {
        NOP4();
    }
}

void I2C_Start(void)
{
    I2C_SDA = 1;
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SDA = 0;
    I2C_Delay();
    I2C_SCL = 0;
    I2C_Delay();
}

void I2C_Stop(void)
{
    I2C_SDA = 0;
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SDA = 1;
    I2C_Delay();
}

void I2C_SendByte(uint8 dat)
{
    uint8 i;
    
    for (i = 0; i < 8; i++) {
        I2C_SDA = (dat & 0x80) ? 1 : 0;
        dat <<= 1;
        I2C_SCL = 1;
        I2C_Delay();
        I2C_SCL = 0;
        I2C_Delay();
    }
    
    I2C_SDA = 1;
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SCL = 0;
    I2C_Delay();
}

uint8 I2C_RecvByte(void)
{
    uint8 i, dat = 0;
    
    I2C_SDA = 1;
    
    for (i = 0; i < 8; i++) {
        I2C_SCL = 1;
        I2C_Delay();
        dat <<= 1;
        if (I2C_SDA) {
            dat |= 0x01;
        }
        I2C_SCL = 0;
        I2C_Delay();
    }
    
    return dat;
}

void I2C_SendAck(uint8 ack)
{
    if (ack) {
        I2C_SDA = 0;
    } else {
        I2C_SDA = 1;
    }
    I2C_SCL = 1;
    I2C_Delay();
    I2C_SCL = 0;
    I2C_Delay();
    I2C_SDA = 1;
}

uint8 I2C_RecvAck(void)
{
    uint8 ack;
    
    I2C_SDA = 1;
    I2C_SCL = 1;
    I2C_Delay();
    ack = I2C_SDA;
    I2C_SCL = 0;
    I2C_Delay();
    
    return ack;
}

uint8 xdata I2C_Buf[256];

void I2C_Write(uint8 device_addr, uint16 len)
{
    uint16 i;

    I2C_Start();
    I2C_SendByte(device_addr);
    if (I2C_RecvAck()) {
        I2C_Stop();
        return;
    }

    for (i = 0; i < len; i++) {
        I2C_SendByte(I2C_Buf[i]);
        if (I2C_RecvAck()) {
            I2C_Stop();
            return;
        }
    }

    I2C_Stop();
}

void I2C_Read(uint8 device_addr, uint16 len)
{
    uint16 i;

    I2C_Start();
    I2C_SendByte(device_addr | 0x01);
    if (I2C_RecvAck()) {
        I2C_Stop();
        return;
    }

    for (i = 0; i < len; i++) {
        I2C_Buf[i] = I2C_RecvByte();
        if (i == len - 1) {
            I2C_SendAck(1);
        } else {
            I2C_SendAck(0);
        }
    }
    
    I2C_Stop();
}