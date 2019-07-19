//
//  FixedParamsData.h
//  Structure SDK
//
//  Created by Kai on 4/28/14.
//  Copyright (c) 2015 Occipital, Inc. All rights reserved.
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
    
    struct FixedParamsData {
        
        FixedParamsData() {
            cmosAndEmitterDistance = 0;
            referencePlaneDistance = 0;
            planePixelSize = 0;
        }
        
        // Note: we don't parse every data from the FixedParams yet
        float cmosAndEmitterDistance;
        float referencePlaneDistance;
        float planePixelSize;
        
    };
    
    
    typedef enum {
        FixedParamDownloadError_CannotStartDownload,
        FixedParamDownloadError_NoSensorAttached,
        IFixedParamDownloadError_AlreadyDownloading
    } FixedParamDownloadError;
    
   
#ifdef __cplusplus
}
#endif