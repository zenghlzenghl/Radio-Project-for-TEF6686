#include "tef6686.h"
#include "i2c_stc8h.h"

uint8 TunerReadBuff_Quality[MAX_READ_REG_QUAL_NUM];
extern uint8 xdata I2C_Buf[];

static uint8 F_Radio_Init_Ok = 0;
static uint16 CurrentFreq = 8750;

#define FM_VOLUME_GAIN     60
#define AM_VOLUME_GAIN     90
#define MUTE_VOLUME_GAIN   235
#define DEFAULT_VOLUME_GAIN 70

static void WriteTunerRegister(uint8 nb)
{
    if (F_Radio_Init_Ok) {
        I2C_Write(TEF6686_I2C_ADDR, nb);
    }
}

static void ReadTunerRegister(uint8 module, uint8 Cmd, uint8 nb)
{
    uint8 i;

    if (F_Radio_Init_Ok || (module == MODULE_APPL)) {
        I2C_Buf[0] = module;
        I2C_Buf[1] = Cmd;
        I2C_Buf[2] = 1;

        I2C_Write(TEF6686_I2C_ADDR, 3);
        I2C_Read(TEF6686_I2C_ADDR, nb);

        for (i = 0; i < nb; i++) {
            TunerReadBuff_Quality[i] = I2C_Buf[i];
        }
    }
}

static void LoadPatch(void)
{
    I2C_Buf[0] = 0x1c;
    I2C_Buf[1] = 0x00;
    I2C_Buf[2] = 0x00;
    I2C_Write(TEF6686_I2C_ADDR, 3);

    I2C_Buf[0] = 0x1c;
    I2C_Buf[1] = 0x00;
    I2C_Buf[2] = 0x74;
    I2C_Write(TEF6686_I2C_ADDR, 3);

    I2C_Buf[0] = 0x1c;
    I2C_Buf[1] = 0x00;
    I2C_Buf[2] = 0x75;
    I2C_Write(TEF6686_I2C_ADDR, 3);
}

void TEF6686_PowerOn(void)
{
    I2C_Init();

    I2C_Buf[0] = 0x40;
    I2C_Buf[1] = 0x04;
    I2C_Buf[2] = 0x01;
    I2C_Buf[3] = 0x00;
    I2C_Buf[4] = 0x3d;
    I2C_Buf[5] = 0x09;
    I2C_Buf[6] = 0x00;
    I2C_Buf[7] = 0x00;
    I2C_Buf[8] = 0x00;
    I2C_Write(TEF6686_I2C_ADDR, 9);

    I2C_Buf[0] = 0x14;
    I2C_Buf[1] = 0x00;
    I2C_Buf[2] = 0x01;
    I2C_Write(TEF6686_I2C_ADDR, 3);

    I2C_Buf[0] = 0x40;
    I2C_Buf[1] = 0x05;
    I2C_Buf[2] = 0x01;
    I2C_Buf[3] = 0x00;
    I2C_Buf[4] = 0x01;
    I2C_Write(TEF6686_I2C_ADDR, 5);

    LoadPatch();

    F_Radio_Init_Ok = 1;
}

void TEF6686_PowerOff(void)
{
    F_Radio_Init_Ok = 0;

    I2C_Buf[0] = 0x40;
    I2C_Buf[1] = 0x05;
    I2C_Buf[2] = 0x01;
    I2C_Buf[3] = 0x00;
    I2C_Buf[4] = 0x00;
    I2C_Write(TEF6686_I2C_ADDR, 5);
}

void TEF6686_Init(void)
{
    TEF6686_PowerOn();
    TEF6686_TuneToFreq(8750);
}

BOOL TEF6686_TuneToFreq(uint16 freq)
{
    CurrentFreq = freq;

    if (!F_Radio_Init_Ok) {
        return FALSE;
    }

    if (freq > 2000) {
        I2C_Buf[0] = MODULE_FM;
        I2C_Buf[1] = CMD_FM_AM_TUNE_TO;
        I2C_Buf[2] = 0x01;
        I2C_Buf[3] = (freq >> 8) & 0xFF;
        I2C_Buf[4] = freq & 0xFF;
        I2C_Buf[5] = 0x00;
        I2C_Buf[6] = 0x00;

        WriteTunerRegister(7);
    } else {
        I2C_Buf[0] = MODULE_AM;
        I2C_Buf[1] = CMD_FM_AM_TUNE_TO;
        I2C_Buf[2] = 0x01;
        I2C_Buf[3] = (freq >> 8) & 0xFF;
        I2C_Buf[4] = freq & 0xFF;
        I2C_Buf[5] = 0x00;
        I2C_Buf[6] = 0x00;

        WriteTunerRegister(7);
    }

    return TRUE;
}

void TEF6686_SetVolume(uint8 volume)
{
    I2C_Buf[0] = MODULE_AUDIO;
    I2C_Buf[1] = CMD_AUDIO_SET_VOLUME;
    I2C_Buf[2] = 0x01;
    I2C_Buf[3] = 0x00;
    I2C_Buf[4] = volume;

    WriteTunerRegister(5);
}

void TEF6686_Mute(uint8 mute)
{
    uint8 volume;

    if (mute) {
        volume = MUTE_VOLUME_GAIN;
    } else {
        volume = DEFAULT_VOLUME_GAIN;
    }

    TEF6686_SetVolume(volume);
}

uint16 TEF6686_GetFrequency(void)
{
    return CurrentFreq;
}

uint8 TEF6686_GetLevel(void)
{
    uint16 level;

    ReadTunerRegister(MODULE_FM, CMD_FM_GET_QUALITY_DATA, MAX_READ_REG_QUAL_NUM);

    level = ((uint16)TunerReadBuff_Quality[READ_REG_LEVEL_H] << 8) |
             TunerReadBuff_Quality[READ_REG_LEVEL_L];

    return (uint8)(level / 10);
}

uint8 TEF6686_GetUSN(void)
{
    uint16 usn;

    ReadTunerRegister(MODULE_FM, CMD_FM_GET_QUALITY_DATA, MAX_READ_REG_QUAL_NUM);

    usn = ((uint16)TunerReadBuff_Quality[READ_REG_USN_H] << 8) |
           TunerReadBuff_Quality[READ_REG_USN_L];

    return (uint8)(usn / 10);
}

uint8 TEF6686_GetWAM(void)
{
    uint16 wam;

    ReadTunerRegister(MODULE_FM, CMD_FM_GET_QUALITY_DATA, MAX_READ_REG_QUAL_NUM);

    wam = ((uint16)TunerReadBuff_Quality[READ_REG_WAM_H] << 8) |
           TunerReadBuff_Quality[READ_REG_WAM_L];

    return (uint8)(wam / 10);
}

int16 TEF6686_GetOffset(void)
{
    int16 offset;

    ReadTunerRegister(MODULE_FM, CMD_FM_GET_QUALITY_DATA, MAX_READ_REG_QUAL_NUM);

    offset = ((int16)TunerReadBuff_Quality[READ_REG_OFFSET_H] << 8) |
              TunerReadBuff_Quality[READ_REG_OFFSET_L];

    if (offset & 0x8000) {
        offset = (((~offset) + 1) & 0xFFFF);
        offset = -offset;
    }

    return offset;
}

void TEF6686_SeekStart(uint8 direction)
{
    if (!F_Radio_Init_Ok) {
        return;
    }

    if (CurrentFreq > 2000) {
        I2C_Buf[0] = MODULE_FM;
        I2C_Buf[1] = CMD_FM_AM_TUNE_TO;
        I2C_Buf[2] = 0x01;
        I2C_Buf[3] = direction ? 0xFF : 0xFE;
        I2C_Buf[4] = 0xFF;
        I2C_Buf[5] = 0x00;
        I2C_Buf[6] = 0x01;
        I2C_Buf[7] = 0x00;

        WriteTunerRegister(8);
    } else {
        I2C_Buf[0] = MODULE_AM;
        I2C_Buf[1] = CMD_FM_AM_TUNE_TO;
        I2C_Buf[2] = 0x01;
        I2C_Buf[3] = direction ? 0xFF : 0xFE;
        I2C_Buf[4] = 0xFF;
        I2C_Buf[5] = 0x00;
        I2C_Buf[6] = 0x01;
        I2C_Buf[7] = 0x00;

        WriteTunerRegister(8);
    }
}

uint8 TEF6686_SeekStatus(void)
{
    uint16 status;

    ReadTunerRegister(MODULE_FM, CMD_FM_AM_GET_QUALITY_STATUS, 2);

    status = ((uint16)TunerReadBuff_Quality[READ_REG_QUAL_STATUS_H] << 8) |
              TunerReadBuff_Quality[READ_REG_QUAL_STATUS_L];

    if (status & 0x0100) {
        return 1;
    } else if (status & 0x0200) {
        return 2;
    } else {
        return 0;
    }
}