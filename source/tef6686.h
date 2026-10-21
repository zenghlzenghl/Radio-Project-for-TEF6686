#ifndef TEF6686_H
#define TEF6686_H

#include "stc8h.h"

#define TEF6686_I2C_ADDR    0xC8

#define TRUE  1
#define FALSE 0
#define BOOL  uint8

enum TEF668x_MODULE {
    MODULE_FM    = 0x20,
    MODULE_AM    = 0x21,
    MODULE_AUDIO = 0x40,
    MODULE_APPL  = 0x50
};

enum TEF668X_RADIO_WRITE_CMD {
    CMD_FM_AM_TUNE_TO                   = 1,
    CMD_FM_SET_TUNE_OPTIONS             = 2,
    CMD_FM_AM_SET_BANDWIDTH             = 10,
    CMD_FM_AM_SET_RFAGC                 = 11,
    CMD_AM_SET_ANTENNA                  = 12,
    CMD_AM_SET_COCHANNELDET             = 14,
    CMD_FM_SET_MPH_SUPPRESSION          = 20,
    CMD_FM_SET_CHANNEL_EQUALIZER        = 22,
    CMD_FM_AM_SET_NOISE_BLANKER         = 23,
    CMD_AM_SET_AUDIO_NOISE_BLANKER      = 24,
    CMD_FM_AM_SET_DIGITAL_RADIO         = 30,
    CMD_FM_SET_DEEMPHASIS               = 31,
    CMD_FM_SET_STEREO_IMPROVEMENT		= 32,
    CMD_FM_AM_SET_HIGHCUT_FIX           = 33,
    CMD_FM_AM_SET_LOWCUT_FIX            = 34,
    CMD_FM_AM_SET_LEVEL_STEP            = 38,
    CMD_FM_AM_SET_LEVEL_OFFSET          = 39,
    CMD_FM_AM_SET_SOFTMUTE_TIME         = 40,
    CMD_AM_SET_SOFTMUTE_MOD             = 41,
    CMD_FM_AM_SET_SOFTMUTE_LEVEL        = 42,
    CMD_FM_SET_SOFTMUTE_NOISE           = 43,
    CMD_FM_SET_SOFTMUTE_MPH             = 44,
    CMD_FM_AM_SET_SOFTMUTE_MAX          = 45,
    CMD_FM_AM_SET_HIGHCUT_TIME          = 50,
    CMD_FM_AM_SET_HIGHCUT_MOD           = 51,
    CMD_FM_AM_SET_HIGHCUT_LEVEL         = 52,
    CMD_FM_SET_HIGHCUT_NOISE            = 53,
    CMD_FM_SET_HIGHCUT_MPH              = 54,
    CMD_FM_AM_SET_HIGHCUT_MAX           = 55,
    CMD_FM_AM_SET_HIGHCUT_MIN           = 56,
    CMD_FM_AM_SET_LOWCUT_MAX            = 57,
    CMD_FM_AM_SET_LOWCUT_MIN            = 58,
    CMD_FM_SET_STEREO_TIME              = 60,
    CMD_FM_SET_STEREO_MOD               = 61,
    CMD_FM_SET_STEREO_LEVEL             = 62,
    CMD_FM_SET_STEREO_NOISE             = 63,
    CMD_FM_SET_STEREO_MPH               = 64,
    CMD_FM_SET_STEREO_MAX               = 65,
    CMD_FM_SET_STEREO_MIN               = 66,
    CMD_FM_SET_STHIBLEND_TIME           = 70,
    CMD_FM_SET_STHIBLEND_MOD            = 71,
    CMD_FM_SET_STHIBLEND_LEVEL          = 72,
    CMD_FM_SET_STHIBLEND_NOISE          = 73,
    CMD_FM_SET_STHIBLEND_MPH            = 74,
    CMD_FM_SET_STHIBLEND_MAX            = 75,
    CMD_FM_SET_STHIBLEND_MIN            = 76,
    CMD_FM_SET_SCALER                   = 80,
    CMD_FM_SET_RDS                      = 81,
    CMD_FM_SET_QUALITY_STATUS           = 82,
    CMD_FM_SET_DR_BLEND                 = 83,
    CMD_FM_SET_DR_OPTIONS               = 84,
    CMD_FM_SET_SPECIALS                 = 85,
    CMD_FM_SET_BANDWIDTH_OPTIONS		= 86,
    CMD_FM_SET_SET_BANDBLEND_TIME 		= 90,
    CMD_FM_SET_SET_BANDBLEND_GAIN 		= 91,
    CMD_FM_SET_SET_BANDBLEND_BIAS		= 92
};

enum TEF668X_RADIO_READ_CMD {
    CMD_FM_AM_GET_QUALITY_STATUS        = 128,
    CMD_FM_GET_QUALITY_DATA             = 129,
    CMD_FM_GET_RDS_STATUS               = 130,
    CMD_FM_GET_RDS_DATA                 = 131,
    CMD_FM_AM_GET_AGC                   = 132,
    CMD_FM_AM_GET_SIGNAL_STATUS         = 133,
    CMD_FM_AM_GET_PROCESSING_STATUS     = 134,
    CMD_FM_AM_GET_INTERFACE_STATUS      = 135
};

enum TEF668X_AUDIO_WRITE_CMD {
    CMD_AUDIO_SET_VOLUME                = 10,
    CMD_AUDIO_SET_MUTE                  = 11,
    CMD_AUDIO_SET_INPUT                 = 12,
    CMD_AUDIO_SET_OUTPUT_SOURCE         = 13,
    CMD_AUDIO_SET_ANA_OUT               = 21,
    CMD_AUDIO_SET_DIG_IO                = 22,
    CMD_AUDIO_SET_INPUT_SCALER          = 23,
    CMD_AUDIO_SET_WAVEGEN               = 24
};

enum TEF668X_APPL_WRITE_CMD {
    CMD_APPL_SET_OPERATION_MODE         = 1,
    CMD_APPL_SET_GPIO                   = 3,
    CMD_APPL_SET_REFERENCE_CLOCK        = 4,
    CMD_APPL_ACTIVATE                   = 5
};

enum TEF668X_APPL_READ_CMD {
    CMD_APPL_GET_OPERATION_STATUS       = 128,
    CMD_APPL_GET_GPIO_STATUS            = 129,
    CMD_APPL_GET_IDENTIFICATION         = 130,
    CMD_APPL_GET_LAST_WRITE             = 131
};

#define READ_REG_QUAL_STATUS_H          0
#define READ_REG_QUAL_STATUS_L          1
#define READ_REG_LEVEL_H                2
#define READ_REG_LEVEL_L                3
#define READ_REG_USN_H                  4
#define READ_REG_USN_L                  5
#define READ_REG_WAM_H                  6
#define READ_REG_WAM_L                  7
#define READ_REG_OFFSET_H               8
#define READ_REG_OFFSET_L               9
#define READ_REG_BANDWIDTH_H            10
#define READ_REG_BANDWIDTH_L            11
#define READ_REG_MODULATION_H           12
#define READ_REG_MODULATION_L           13
#define MAX_READ_REG_QUAL_NUM           14

extern uint8 TunerReadBuff_Quality[MAX_READ_REG_QUAL_NUM];

void TEF6686_Init(void);
BOOL TEF6686_TuneToFreq(uint16 freq);
void TEF6686_SetVolume(uint8 volume);
void TEF6686_Mute(uint8 mute);
uint16 TEF6686_GetFrequency(void);
uint8 TEF6686_GetLevel(void);
uint8 TEF6686_GetUSN(void);
uint8 TEF6686_GetWAM(void);
int16 TEF6686_GetOffset(void);
void TEF6686_SeekStart(uint8 direction);
uint8 TEF6686_SeekStatus(void);
void TEF6686_PowerOn(void);
void TEF6686_PowerOff(void);

#endif