//
//  IDParamsData.h
//  Structure SDK
//
//  Created by Pat O'Keefe on 4/18/14.
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
    
    struct IDParamsData {
        
        IDParamsData() {
            vendorID = 0;
            productID = 0;
            serialNumber[0] = '\0';
            deviceName[0] = '\0';
            vendorName[0] = '\0';
            containsExtendedFields = false;
            oldSNString[0] = '\0';
            credentialString[0] = '\0';
        }
        
        uint32_t vendorID;
        uint32_t productID;
        char serialNumber[32];
        char deviceName[128];
        char vendorName[128];
        
        bool containsExtendedFields;
        char oldSNString[32];
        char credentialString[32];
        
    };
    
    
    typedef enum {
        IDParamDownloadError_CannotStartDownload,
        IDParamDownloadError_NoSensorAttached,
        IDParamDownloadError_AlreadyDownloading
    } IDParamDownloadError;
    
   
#ifdef __cplusplus
}
#endif