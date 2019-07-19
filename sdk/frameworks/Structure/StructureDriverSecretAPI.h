//
//  StructureDriverSecretAPI.h
//  Structure
//
//  Created by Nicolas Burrus on 19/11/15.
//  Copyright © 2015 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

#import <Structure/Structure.h>
// TODO(Brandon): GeneralStoreCameraType should be moved into another file...
#include <Structure/Private/Driver/CalibrationFilev1Entry.h>

// Whether to enable the Structure Core driver in STSensorController.
// Set this to NO before sending any message to the STSensorController class.
// Typically as early as possible during app startup.
// YES by default.
extern BOOL STSensorController_StructureCoreDriverEnabled;

typedef enum {
    STSensorModel_Invalid         = -1,
    STSensorModel_None            = 0,
    STSensorModel_StructureSensor = 1,
    STSensorModel_ISense          = 2,
    STSensorModel_StructureCore   = 3,
} STSensorModel;

typedef enum {
    STSensorAutoExposureMode_Disabled   = 0,
    STSensorAutoExposureMode_OneShot    = 1,
    STSensorAutoExposureMode_Continuous = 2,
} STSensorAutoExposureMode;

#if !STRUCTURE_SDK_BUILD
extern NSString* const kSTInfraredSensorAutoExposureModeKey;
extern NSString* const kSTInfraredSensorManualExposureKey;
extern NSString* const kSTInfraredSensorManualGainKey;
extern NSString* const kSTVisibleSensorAutoExposureModeKey;
extern NSString* const kSTVisibleSensorManualExposureKey;
extern NSString* const kSTVisibleSensorManualGainKey;
#endif // !STRUCTURE_SDK_BUILD

@protocol STSensorControllerProtocol <NSObject>
@property(nonatomic, unsafe_unretained) id<STSensorControllerDelegate> delegate;
@property(nonatomic, retain, readwrite) dispatch_queue_t callbackQueue;

@property (nonatomic, readonly) BOOL isStructureCore;

@property (nonatomic) STSensorAutoExposureMode visibleCameraAutoExposureMode;
@property (nonatomic) float visibleCameraExposure;
@property (nonatomic) float visibleCameraGain;
@property (nonatomic) STSensorAutoExposureMode infraredCameraAutoExposureMode;
@property (nonatomic) float infraredCameraExposure;
@property (nonatomic) float infraredCameraGain;

- (STSensorControllerInitStatus)initializeSensorConnection;
- (BOOL)startStreamingWithOptions:(NSDictionary*)options error:(NSError* __autoreleasing *)error;
- (void)stopStreaming;
- (void)frameSyncNewColorFrame:(STColorFrame*)sampleBuffer;
- (void)frameSyncNewColorBuffer:(CMSampleBufferRef)sampleBuffer;
- (void)setSensorModelRestriction:(STSensorModel)sensorModelRestriction;
- (BOOL)isConnected;
- (BOOL)isLowPower;
- (int)getBatteryChargePercentage;
- (NSString *)getName;
- (NSString *)getSerialNumber;
- (NSString *)getFirmwareRevision;
- (NSString *)getHardwareRevision;
+ (BOOL)launchCalibratorAppOrGoToAppStore;
+ (BOOL)approximateCalibrationGuaranteedForDevice;
- (STCalibrationType)calibrationType;
- (void)setHighGainEnabled:(BOOL)enabled;
- (BOOL)hasInfraredLED;
- (void)setIRLEDEnabled:(BOOL)isOn;
- (BOOL)hasVisibleCamera;
- (void)setProjectorPower:(float)percent;
- (void)setCurrentCameraType:(GeneralStoreCameraType)cameraType;
- (void)optimizeIROffsets;


// Hidden Methods
@optional
- (NSDictionary*) lastStreamingOptions;
- (NSTimeInterval)originalOccTimestamp:(NSTimeInterval)deliveredEventTimestamp;
@end


#pragma mark - Private (not for partners)
// Not for partners.
#import <Core/OccProtocol.h>
#pragma mark -

// simple completion block that passes error=nil on success, error != nil on failure
typedef void (^CompletionBlock)(NSError* error);

// Mark our public STSensorController as conforming to this protocol. I put it here to avoid
// showing anything in the public API.
@interface STSensorController () <STSensorControllerProtocol>

#pragma mark - Private (not for partners)

/// Return sensor calibration information, or nil if no sensor is connected
- (oc::OccDepthSensorInfo) getDepthSensorInfo;
- (void)downloadFixedParamsAndGeneralStoreFile;
- (void)uploadCalibrationWithData:(GeneralStoreCalibrationData&) calData;
- (int)writeGeneralStoreFile:(uint8_t*)generalStoreData
                    withSize:(uint32_t)generalStoreDataSize;
- (void) deleteGeneralStoreFile:(CompletionBlock) completionBlock;
- (void) resetConnectionState;

#pragma mark -

@end
