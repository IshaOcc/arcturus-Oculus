//
//  STSensorDriver.h
//  Structure
//
//  Created by Matt Quick on 5/16/16.
//  Copyright © 2016 Occipital. All rights reserved.
//

#pragma once

#include <Core/Core-Prefix.pch>

#include <Core/Intrinsics.h>
#include <Core/SensorDepthFrame.h>
#include <Core/SensorInfraredFrame.h>
#include <Core/CameraColorFrame.h>

#if __OBJC__
#   include <Structure/StructureDriverSecretAPI.h>
// TODO(Brandon): GeneralStoreCameraType should be moved into another file...
#   include <Structure/Private/Driver/CalibrationFilev1Entry.h>
#endif

typedef enum {
    VisibleFrameFormat_RGB565,
    VisibleFrameFormat_Monochrome16
} VisibleFrameFormat;

/**
 * This is a visible frame from StructureCore
 */
struct VisibleFrame
{
    VisibleFrameFormat format;
    const void* contents; // FIXME: We likely should hold a copy of the buffer data here. This looks dangerous.
    size_t contentsLength;
    double timestampHost;
    oc::Intrinsics intrinsics;
};

// FIXME: All the cross-platform definitions above should be moved to a pure C++ header.

//------------------------------------------------------------------------------

#if __OBJC__

#   import <Foundation/Foundation.h>
#   import <Core/StructureTypesPrivate.h>
#   import <Core/OccProtocol.h>
#   import "../../Structure.h"

@protocol STSensorDriver;
@class EAAccessory;

@protocol STSensorDriverFrameDelegate <NSObject>
    @property(nonatomic, assign) id<STSensorDriver> driver;

    - (void) resetState;

    - (void) sensorDepthFrameAvailable:(oc::SensorDepthFramePtr)depthFrame;
    - (void) sensorInfraredFrameAvailable:(oc::SensorInfraredFramePtr)infraredFrame;

    // Called when a visible frame is available.
    // Only called from Structure Core sensors, not Structure Sensors.
    - (void) sensorVisibleFrameAvailable:(oc::CameraColorFramePtr)visibleFrame;

    /* These will be refactored later.  The logic that depends on them exists in SensorCommunicationController */
    - (BOOL)frameSyncEnabled;
    - (void)setMaxDeltaTimestampBetweenIOSCameraAndSensor:(NSTimeInterval)thresh;
@end

/**
 *
 */
@protocol STSensorDriverDelegate <NSObject>
    @required
    - (void) deviceConnected :(STSensorModel)model driver:(id<STSensorDriver>)driver;
    - (void) deviceWakingUp :(STSensorModel)model driver:(id<STSensorDriver>)driver;
    - (void) deviceDisconnected;
@end

/**
 *
 */

// simple completion block that passes error=nil on success, error != nil on failure
typedef void (^CompletionBlock)(NSError* error);

namespace oc {
    
    struct ShutterSyncState;

};

@protocol STSensorDriver <NSObject>
    @property(nonatomic, assign) id<STSensorDriverDelegate> delegate;
    @property(nonatomic, assign) id<STSensorDriverFrameDelegate> frameDelegate;

    @property(nonatomic, readonly) NSString *name;
    @property(nonatomic, readonly) NSString *serialNumber;
    @property(nonatomic, readonly) NSString *firmwareRevision;
    @property(nonatomic, readonly) NSString *hardwareRevision;
    @property(nonatomic, readonly) NSArray<NSString*> *eaProtocols;
    @property(nonatomic, readonly) STSensorModel sensorModel;

    @required
    @property(nonatomic, readonly) oc::ShutterSyncState* shutterSyncState;

    @property (nonatomic) STSensorAutoExposureMode visibleCameraAutoExposureMode;
    @property (nonatomic) float visibleCameraExposure;
    @property (nonatomic) float visibleCameraGain;

    @property (nonatomic) STSensorAutoExposureMode infraredCameraAutoExposureMode;
    @property (nonatomic) float infraredCameraExposure;
    @property (nonatomic) float infraredCameraGain;

    - (BOOL) canUseEAAccessory:(EAAccessory*)theAccessory;

    // Used in calibrator; we need to be able to register iSenses.
    - (void) setSensorModelRestriction:(STSensorModel)sensorModelRestriction;
    - (void) startListeningForDevices;
    - (void) stopListeningForDevices;
    - (STSensorControllerInitStatus)wakeAndConnect;
    - (BOOL) startStreaming:(STStreamConfig)streamCfg colorLensPositionIfLatchIDNotSupported:(float)colorLensPosition;
    - (BOOL) stopStreaming;
    - (BOOL) isConnected;
    - (BOOL) isLowPower;
    - (float) getBatteryChargePercentage;
    - (void) setHoleFilterEnabled:(bool)enabled;
    - (void) setCmosGain:(int)gain;
    - (BOOL) hasInfraredLED;
    - (void) setIRLEDEnabled:(bool)enabled;
    - (void) sendShutterDelay:(double)diffTime;
    // Needed for some serialization tasks
    - (oc::OccDepthSensorInfo) getDepthSensorInfo;
    // General Store access
    - (void)downloadFixedParamsAndGeneralStoreFile;
    - (void)uploadCalibrationWithData:(GeneralStoreCalibrationData&) calData;
    - (int) writeGeneralStoreFile:(uint8_t*) generalStoreData withSize:(uint32_t)generalStoreDataSize;
    - (void) deleteGeneralStoreFile:(CompletionBlock) completionBlock;
    - (void) resetConnectionState;
    - (BOOL) hasVisibleCamera;
    - (void) optimizeIROffsets;

    - (void) setCoreInfraredStreamSource:(STCoreInfraredStreamSource) source;
    - (void) setProjectorPower:(float)percent;

    - (void)sendSensorStatus;
    // FIXME: The web request code should be the same for both drivers.
    // Only the part retrieveing the data from the sensor flash should differ.

    @optional
    - (void) supportedStreamConfigs:(const STStreamConfig**)configs configsLength:(size_t*)length; //DTMP
    - (void) setStreamPreset:(STStreamPreset) preset;
@end

#endif
