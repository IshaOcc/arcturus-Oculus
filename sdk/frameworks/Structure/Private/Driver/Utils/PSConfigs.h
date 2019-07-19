//
//  PSCommandSequences.h
//  Driver_iOS
//
//  Created by Forrest Heller on 4/18/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SensorConfigSequence holds different sequences of config commands to send to a
 * PS1080 device to do things like start streaming depth or IR
 */

#pragma pack(push, 1) // Make sure the compiler aligns these with no padding
struct SensorSetting {
    uint16_t param;
    uint16_t value;
};
#pragma pack(pop)
    
struct SensorConfigSequence {
    struct SensorSetting* settings;
    size_t settingsCount;
};

extern struct SensorConfigSequence QVGADepthIRSequence;
extern struct SensorConfigSequence QVGADepthSequence;
extern struct SensorConfigSequence QVGADepth60FpsSequence;
extern struct SensorConfigSequence VGADepthSequence;
extern struct SensorConfigSequence QVGADepthRegisteredSequence;
extern struct SensorConfigSequence QVGAIRSequence;
extern struct SensorConfigSequence VGAIRSequence;
extern struct SensorConfigSequence SXGAIRSequence;
extern struct SensorConfigSequence VGADepthIRSequence;
extern struct SensorConfigSequence StopStreamSequence;
extern struct SensorConfigSequence VGADepthRegisteredSequence;
extern struct SensorConfigSequence VGADepthGMCSmoothSequence;
extern struct SensorConfigSequence QVGADepthGMCSmoothSequence;

typedef enum
{
    //General,
    PARAM_GENERAL_CURRENT_MODE = 0,
    PARAM_GENERAL_FRAME_SYNC,
    PARAM_GENERAL_REGISTRATION_ENABLE,
    PARAM_GENERAL_STREAM_PRIORITY,
    PARAM_GENERAL_TRIGGER_ACTION,
    PARAM_GENERAL_STREAM0_MODE,
    PARAM_GENERAL_STREAM1_MODE,
    PARAM_GENERAL_STREAM2_MODE,
    //Audio,
    PARAM_AUDIO_STEREO_MODE, //8
    PARAM_AUDIO_SAMPLE_RATE, //9
    PARAM_AUDIO_LEFT_CHANNEL_VOLUME_LEVEL, //10
    PARAM_AUDIO_RIGHT_CHANNEL_VOLUME_LEVEL, //11
    //Image,
    PARAM_IMAGE_FORMAT,
    PARAM_IMAGE_RESOLUTION,
    PARAM_IMAGE_FPS,
    PARAM_IMAGE_GAIN_CONTROL,
    PARAM_IMAGE_QUALITY,
    PARAM_IMAGE_FLICKER_DETECTION, //POWER_LINE_FREQUENCY
    //Depth,
    PARAM_DEPTH_FORMAT, // 18
    PARAM_DEPTH_RESOLUTION,
    PARAM_DEPTH_FPS,
    PARAM_DEPTH_GAIN_CONTROL,
    PARAM_DEPTH_HOLE_FILTER, //22
    PARAM_DEPTH_MIRROR,
    PARAM_DEPTH_DECIMATION,
    //IR,
    PARAM_IR_FORMAT,
    PARAM_IR_RESOLUTION,
    PARAM_IR_FPS,
    PARAM_IR_GAIN_CONTROL,
    PARAM_IR_QUALITY,
    //Misc,
    PARAM_MISC_LOG_FILTER,
    PARAM_MISC_PACKET_TIMEOUT,
    PARAM_MISC_PS_SESSION_TIMEOUT,
    PARAM_AUDIO_LEFT_CHANNEL_MUTE,
    PARAM_AUDIO_RIGHT_CHANNEL_MUTE,
    PARAM_AUDIO_MICROPHONE_IN,
    PARAM_DEPTH_GMC_MODE, //36
    PARAM_DEPTH_GMC_REF_OFFSET, //37
    PARAM_DEPTH_GMC_RICC, //38
    PARAM_DEPTH_GMC_DX_CORRECTION, //39
    PARAM_DEPTH_GMC_DX_CORRECTION_A, //40
    PARAM_DEPTH_GMC_DX_CORRECTION_B_HIGH, //41
    PARAM_DEPTH_GMC_DX_CORRECTION_B_LOW, //42
    PARAM_DEPTH_GMC_DX_CORRECTION_DB_HIGH, //43
    PARAM_DEPTH_GMC_DX_CORRECTION_DB_LOW, //44
    //Depth white balance
    PARAM_DEPTH_WHITE_BALANCE_ENABLE, //45
    
    //Image Crop
    PARAM_IMAGE_CROP_SIZE_X, //46
    PARAM_IMAGE_CROP_SIZE_Y,
    PARAM_IMAGE_CROP_OFFSET_X,
    PARAM_IMAGE_CROP_OFFSET_Y,
    PARAM_IMAGE_CROP_ENABLE, //50
    //Depth Crop
    PARAM_DEPTH_CROP_SIZE_X, //51
    PARAM_DEPTH_CROP_SIZE_Y,//52
    PARAM_DEPTH_CROP_OFFSET_X,//53
    PARAM_DEPTH_CROP_OFFSET_Y,//54
    PARAM_DEPTH_CROP_ENABLE,//55
    //IR Crop
    PARAM_IR_CROP_SIZE_X,//56
    PARAM_IR_CROP_SIZE_Y,//57
    PARAM_IR_CROP_OFFSET_X,//58
    PARAM_IR_CROP_OFFSET_Y,//59
    PARAM_IR_CROP_ENABLE,//60
    
    PARAM_DEPTH_GMC_DEBUG_MODE,//61
    PARAM_ACC_ENABLE,//62
    
    //Gain Control
    PARAM_GAIN_CONTROL_BIN0_LOW,//63
    PARAM_GAIN_CONTROL_BIN0_HIGN,//64
    PARAM_GAIN_CONTROL_BIN1_LOW,//65
    PARAM_GAIN_CONTROL_BIN1_HIGN,//66
    PARAM_GAIN_CONTROL_BIN2_LOW,//67
    PARAM_GAIN_CONTROL_BIN2_HIGN,//68
    PARAM_GAIN_CONTROL_BIN3_LOW,//69
    PARAM_GAIN_CONTROL_BIN3_HIGN,//70
    
    //Mirror
    PARAM_IMAGE_MIRROR,//71
    PARAM_IR_MIRROR,//72
    
    //Wavelength correction: - Deprecated
    //PARAM_WAVELENGTH_CORRECTION_ENABLE,//73
    //PARAM_WAVELENGTH_CORRECTION_DEBUG_ENABLE,//74
    //PARAM_GMC_CORRECTION_MODE,//75
    
    //Logitec Params
    PARAM_IMAGE_SHARPNESS = 76,//76
    PARAM_IMAGE_AUTO_WHITE_BALANCE_MODE,//77
    PARAM_IMAGE_COLOR_TEMPERATURE,//78
    PARAM_IMAGE_BACK_LIGHT_COMPENSATION,//79
    PARAM_IMAGE_AUTO_EXPOSURE_MODE,//80
    PARAM_IMAGE_EXPOSURE_BAR,//81
    PARAM_IMAGE_LOW_LIGHT_COMPENSATION_MODE,//82
    
    //Tec debug
    PARAM_TEC_ENABLE,//83
    
    //Close Range
    PARAM_DEPTH_CLOSE_RANGE,//84
    PARAM_GMC_SMOOTH_HINT,//85
    
    PARAM_NUM_OF_PARAMS
} EConfig_Params;
    
#ifdef __cplusplus
}
#endif
