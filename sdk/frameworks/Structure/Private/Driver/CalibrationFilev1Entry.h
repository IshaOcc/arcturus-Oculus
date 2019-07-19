//
//  CalibrationFilev1iOSDeviceEntry.h
//  Structure
//
//  Created by Pat O'Keefe on 8/7/14.
//  Copyright (c) 2015 Occipital, Inc. All rights reserved.
//

#pragma once

#include <Core/Core-Prefix.pch>

#import "Utils/ByteStream.h"
#import <Core/GeneralStoreTypes.h>
#import <CommonCrypto/CommonDigest.h>
#import <Foundation/Foundation.h>

//------------------------------------------------------------------------------

enum class CalibrationValueType : uint8_t // DO NOT CHANGE, THIS MUST BE 8 BITS
{
    DeviceIdentifier        = 1,
    CalibratorAppIdentifier = 2,
    CalibrationTimeUTC      = 3,
    VendorID                = 4, // Reserve for now
    PlatformString          = 5,
    DeviceNameSha1          = 6,
    DeviceName              = 7,

    // For the color camera, this always store intrinsics for focus at 1.0 to ensure compatibility with old apps
    intrinsicsAtFocus1_0    = 8,
    iOSExtrinsics_R3x3_T1x3 = 9,

    // This is the actual focus position of the color camera used during calibration.
    FocusPositionDuringCalibration = 10,

    // This factor allows to transform the fx intrinsics stored for Focus1_0 to the actual
    // fx intrinsics at the focus position used during calibration. Keeping this information
    // enable apps to know where the intrinsics are most accurate. If this factor is not
    // specified, we assume it is 1.0 but this should not happen in practice.
    FactorToApplyTo1_0FocalLengthXToMatchSpecifiedFocus = 11,

    AVIntrinsicsDuringWVLCalib = 12,

    UsedVaryingAVIntrinsicsDuringCalib = 13,

    FakeHeaderForTesting = 253
};

//------------------------------------------------------------------------------

#pragma pack(push, 1)
struct CalibratorEntryHeader {
    GeneralStoreVersion2EntryType type;
    uint16_t entrySizeInBytes;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CalibratorValueHeader {
    CalibrationValueType type;
    uint16_t valueSizeInBytes;
};
#pragma pack(pop)

#pragma mark - CalibrationFileSerializer

@protocol CalibrationFileSerializer

@required
// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, readwrite, getter=serializedData, setter=setSerializedData:) uint8_t* serializedData;
@property (nonatomic, readwrite, getter=serializedDataSizeBytes, setter=setSerializedDataSizeBytes:) size_t serializedDataSizeBytes;

// Populate the fields based on data from a file
// It's assumed that the ByteStream points just past the Entry Header
- (void)readFromByteStream:(struct ByteStream*)sourceStream entrySizeInBytes:(uint16_t)entrySizeInBytes;

// Serialize the entry to a buffer. The header will be written first as well.
- (void)serialize;

//For tests only:
#ifdef DEBUG
@optional
- (void)serializeDataWithExtraUnknownValue;
#endif

@end

//------------------------------------------------------------------------------

#pragma mark - UnknownValue

@interface UnknownValue : NSObject <CalibrationFileSerializer>

+ (UnknownValue*)currentUnknownValueWithHeaderAndStream:(CalibratorValueHeader)header stream:(struct ByteStream*)sourceStream;

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1iOSDeviceEntry

@interface CalibrationFilev1iOSDeviceEntry : NSObject <CalibrationFileSerializer>

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, readwrite, getter=deviceIdentifier, setter=setDeviceIdentifier:) uint8_t deviceIdentifier;
//@property (nonatomic, retain) NSString* vendorIDString;
@property (nonatomic, retain, getter=platformString, setter=setPlatformString:) NSString* platformString;
@property (nonatomic, retain, getter=deviceName    , setter=setDeviceName:    ) NSString* deviceName;
@property (nonatomic, retain, getter=deviceNameSha1, setter=setDeviceNameSha1:) NSData*   deviceNameSha1;

// Returns a CalibrationFilev1iOSDeviceEntry with fields populated for the the current device
// The index is needed so that we don't actually create one of these and forget to set it.
+ (CalibrationFilev1iOSDeviceEntry*)currentDeviceWithIdentifier:(int)identifier;

// Only exposing for the tests...
+ (NSData*)sha1:(NSString*)input;

// Compare two device entries.
- (BOOL)isSameDeviceAs:(CalibrationFilev1iOSDeviceEntry*)otherDevice;

- (void)printStatus;

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1StructureIntrinsicsEntry

@interface CalibrationFilev1StructureSensorIntrinsicsEntry : NSObject <CalibrationFileSerializer>

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, readwrite, getter=calibratorAppIdentifier, setter=setCalibratorAppIdentifier:) uint16_t calibratorAppIdentifier;
@property (nonatomic, retain, getter=calibrationTimeUTC, setter=setCalibrationTimeUTC:) NSDate* calibrationTimeUTC;

- (oc::Intrinsics) getIntrinsics640x488;

- (void)printStatus;

- (bool) isIntrinsicsTheSame:(const oc::Intrinsics &)irIntrinsics640x488;

- (void) updateCalibration:(const oc::Intrinsics &)irIntrinsics640x488;

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1iOSBuiltInCameraCalibrationEntry

@interface CalibrationFilev1iOSBuiltInCameraCalibrationEntry : NSObject <CalibrationFileSerializer>

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, readwrite) GeneralStoreCameraType cameraType;
@property (nonatomic, readwrite, getter=deviceIdentifier, setter=setDeviceIdentifier:) uint8_t deviceIdentifier;
@property (nonatomic, readwrite, getter=calibratorAppIdentifier, setter=setCalibratorAppIdentifier:) uint16_t calibratorAppIdentifier;
@property (nonatomic, retain, getter=calibrationTimeUTC, setter=setCalibrationTimeUTC:) NSDate* calibrationTimeUTC;

// Returns a CalibrationFilev1iOSBuiltInCameraCalibrationEntry with fields populated for the the current device
// The index is needed so that we don't actually create one of these and forget to set it.
+ (id)currentDeviceCalibrationWithIdentifier:(int)identifier;

- (oc::Intrinsics) getIntrinsics640x480AtFocusPositionDuringCalibration;
- (OCIsometry3f) getExtrinsics;
- (float) getFocusPositionDuringCalibration;
- (OCVector4f) getAvIntrinsicsDuringCalib;

-(void)updateCalibrationWithCalibrationData:(GeneralStoreCalibrationData) calData;

-(void)updateCalibrationWithIntrinsics:(const oc::Intrinsics &)rgbIntrinsics640x480
                  intrinsicsAtFocus1_0:(const oc::Intrinsics &)rgbIntrinsics640x480AtFocus1_0
                            extrinsics:(const OCIsometry3f &)rbt
                         focusPosition:(const float)focusPosition;

- (void)printStatus;

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1iOSCameraWithWideAngleLensCalibrationEntry

@interface CalibrationFilev1iOSCameraWithWideAngleLensCalibrationEntry : CalibrationFilev1iOSBuiltInCameraCalibrationEntry

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1iOSCameraWithFrontFacingLensCalibrationEntry

@interface CalibrationFilev1iOSCameraWithFrontFacingLensCalibrationEntry : CalibrationFilev1iOSBuiltInCameraCalibrationEntry

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1UnknownDeviceSpecificEntry

@interface CalibrationFilev1UnknownDeviceSpecificEntry : NSObject <CalibrationFileSerializer>

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, readwrite, getter=deviceIdentifier, setter=setDeviceIdentifier:) uint8_t deviceIdentifier;
@property (nonatomic, retain, getter=calibrationTimeUTC, setter=setCalibrationTimeUTC:) NSDate* calibrationTimeUTC;

// Returns a CalibrationFilev1iOSBuiltInCameraCalibrationEntry with fields populated for the the current device
// The index is needed so that we don't actually create one of these and forget to set it.
+ (CalibrationFilev1UnknownDeviceSpecificEntry*)currentUnknownDeviceSpecificEntryWithHeader:(CalibratorEntryHeader)header;

@end

//------------------------------------------------------------------------------

#pragma mark - CalibrationFilev1UnknownEntry

@interface CalibrationFilev1UnknownEntry : NSObject <CalibrationFileSerializer>

+ (CalibrationFilev1UnknownEntry*)currentUnknownEntryWithHeader:(CalibratorEntryHeader)header;

@end

//------------------------------------------------------------------------------
#pragma mark - CalibrationFilev1

@interface CalibrationFilev1 : NSObject <CalibrationFileSerializer>

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, readwrite) uint8_t fileVersion;
@property (nonatomic, readwrite, getter=calibratorAppIdentifier, setter=setCalibratorAppIdentifier:) uint16_t calibratorAppIdentifier;

- (void)printStatus;

- (id) initWithVersion:(uint8_t)fileVersion calibratorAppIdentifier:(uint16_t)calibratorAppIdentifier;

// resetAll: if Structure intrinsics changes, remove the rest files
- (void) updateStructureIntrinsics:(const oc::Intrinsics &)irIntrinsics640x488;

- (void) updateCalibrationOnGeneralStore:(GeneralStoreCalibrationData&) calData;

- (void) updateIntrinsicsAndExtrinsicsForceDeviceEntry:(CalibrationFilev1iOSDeviceEntry*)deviceEntry
                                         iOSIntrinsics:(const oc::Intrinsics&)rgbIntrinsics640x480
                               iOSIntrinsicsAt1_0Focus:(const oc::Intrinsics &)rgbIntrinsics640x480At1_0Focus
                                            extrinsics:(const OCIsometry3f &)rbt
                                         focusPosition:(const float)focusPosition;

- (bool) hasStructureIntrinsics:(oc::Intrinsics &)irIntrinsics640x488Output;

- (bool) hasCurrentiOSDeviceCalibration:(GeneralStoreCalibrationData&) calData
                          forCameraType:(GeneralStoreCameraType) cameraType;

- (void) removeCalibrationEntryForCurrentDevice;

//For tests only:
#ifdef DEBUG
- (void)serializePlusTwoUnknownEntries;
#endif

@end
