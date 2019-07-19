//
//  PresetMaker.h
//  Structure
//
//  Created by Pat O'Keefe on 7/3/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#include "ByteStream.h"

#define PRESET_MAKER_NUM_REGISTRATION_PARAMS 29
#define PRESET_MAKER_NUM_PADDING_PARAMS 3

typedef enum {
    PRESET_MAKER_FPS_30 = 30,
    PRESET_MAKER_FPS_60 = 60
} PresetMakerFPS;

typedef enum {
    PRESET_MAKER_RESOLUTION_QVGA = 0,
    PRESET_MAKER_RESOLUTION_VGA = 1,
    PRESET_MAKER_RESOLUTION_QQVGA = 4
} PresetMakerResolution;

#ifdef __cplusplus
extern "C" {
#endif
    
    
    void bsInitRegistrationPreset(struct ByteStream* destinationStream);

    void bsAddRegistrationPreset(struct ByteStream* destinationStream,
                                 PresetMakerResolution res,
                                 PresetMakerFPS fps,
                                 uint32_t regParams[PRESET_MAKER_NUM_REGISTRATION_PARAMS],
                                 uint16_t paddingParams[PRESET_MAKER_NUM_PADDING_PARAMS]);
    
    // Preset_Find in the FW traverses the flash looking for headers with a specific signature.
    // The termination condition of the loop is when the first 4 bytes of the next header are
    // equal to MAX_UINT32. So to be safe here, we write a final 0xFFFFFFFF.
    void bsFinishPreset(struct ByteStream* destinationStream);
    
    
#ifdef __cplusplus
}
#endif
