//
//  TecData.h
//  Structure SDK
//
//  Created by Kai on 11/1/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once
#ifndef __cplusplus
#include <stdint.h>
#include <stddef.h>
#else
#include <cstdint>
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TECScanMode_Initial,
    TECScanMode_Crude,
    TECScanMode_Precise
} TECScanMode;

typedef enum {
    TECHeatMode_Idle,
    TECHeatMode_Heat,
    TECHeatMode_Cool
} TECHeatMode;

typedef enum {
    TECTempRange_Cool,
    TECTempRange_Room,
    TECTempRange_Warm
} TECTempRange;

struct TECData {
    float setPointTemperatureCelsius;
    float measuredTemperatureCelsius;
    int32_t proportionalError;
    int32_t integralError;
    int32_t derivativeError;
    TECScanMode scanMode;
    TECHeatMode heatMode;
    int16_t dutyCycle;
    TECTempRange temperatureRange;
};

#ifdef __cplusplus
}
#endif