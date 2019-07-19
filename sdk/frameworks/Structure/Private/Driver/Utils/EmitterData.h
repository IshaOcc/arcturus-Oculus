//
//  EmitterData.h
//  Structure SDK
//
//  Created by Kai on 05/2/14.
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

struct EmitterData {
    bool m_State; //idle, calibrating
    uint16_t m_SetPointVoltage; //Current set point
    uint16_t m_SetPointClocks; //target cross duty cycle in system clocks
    uint16_t m_PD_Reading; //current cross duty cycle in system clocks(high time)
    uint16_t m_EmitterSet; //duty cycle on emitter set in system clocks (high time).
    bool  m_EmitterSettingLogic; //TRUE = positive logic, FALSE = negative logic
    bool m_LightMeasureLogic; //TRUE - positive logic, FALSE - negative logic
    bool m_IsEnabled;
    uint16_t m_EmitterSetStepSize;//in milliVolts
    uint16_t m_AccTolerance; //deprecated
    uint16_t m_SubClocking; //in system clocks.
    uint16_t m_Precision;
};

#ifdef __cplusplus

inline void clear (EmitterData& emitterData) {
    emitterData.m_State = false;
    emitterData.m_SetPointVoltage = 0;
    emitterData.m_SetPointClocks = 0;
    emitterData.m_PD_Reading = 0;
    emitterData.m_EmitterSet = 0;
    emitterData.m_EmitterSettingLogic = false;
    emitterData.m_LightMeasureLogic = false;
    emitterData.m_IsEnabled = false;
    emitterData.m_EmitterSetStepSize = 0;
    emitterData.m_AccTolerance = 0;
    emitterData.m_SubClocking = 0;
    emitterData.m_Precision = 0;
}

}
#endif
