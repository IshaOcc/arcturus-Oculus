//
//  SensorGeneralStoreManager.h
//  Structure/Private/Driver
//
//  Created by Tsung-Kai Lin on 8/7/14.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#import "CalibrationFilev1Entry.h"
#import "Utils/ByteStream.h"
#import <Core/Intrinsics.h>
#import <Eigen/Eigen>
#import <Foundation/Foundation.h>

#define GENERAL_STORE_FILE_CURRENT_VERSION 2
#define CALIBRATION_FILE_CURRENT_VERSION 1 // CalibrationFilev1Entry.h
#define CALIBRATION_FILE_CORE_VERSION 2    // Core Calibration (also Filev1 for now)

#pragma pack(push, 1)
struct GeneralStoreFileHeader
{
    uint8_t type;
    uint8_t encodingVersion;
    uint16_t typeSpecificInfo;
    uint16_t entrySizeInWords;
};
#pragma pack(pop)

enum GeneralStoreDataType {
    GENERAL_STORE_DATA_TYPE_VERSION = 0,
    // _INTRINSICS and _EXTRINSICS are deprecated as of GSv2
    GENERAL_STORE_DATA_TYPE_INTRINSICS,       // 1
    GENERAL_STORE_DATA_TYPE_EXTRINSICS,       // 2
    // GSv2 Hierarchy format starts here
    GENERAL_STORE_DATA_TYPE_CALIBRATION_FILE, // 3
    GENERAL_STORE_NUMBER_OF_DATA_TYPES        // 4
};

typedef enum {
    STGeneralManagerError_NewCalibrationFileDetected = 1,
    STGeneralManagerError_MaximumFileSizeExceeded,
    STGeneralManagerError_FileCorrupted,
    STGeneralManagerError_UnknownErrorWhileSerializing
} STGeneralManagerError;

enum GeneralStoreAppIdentifier {
    // Calibrator: Use new AppId for each release. Keep within the 0x0001-0x01ff range.
    GeneralStoreAppIdentifier_Calibrator_1_0     = 0x0001,
    //     1.0.1 - Bugfix version?
    GeneralStoreAppIdentifier_Calibrator_1_0_1   = 0x0002,
    //     1.0.2 - Addded iPad Air 2 support. And mini 3?
    GeneralStoreAppIdentifier_Calibrator_1_0_2   = 0x0003,
    //     1.1   - Added iPhone support and custom brackets
    GeneralStoreAppIdentifier_Calibrator_1_1     = 0x0004,
    //     1.2   - Added WVL support (internal) and iPad Pro bracket
    GeneralStoreAppIdentifier_Calibrator_1_2     = 0x0005,
    //     2.0   - Added WVL intrinsics calibration, preview screen for registration,
    //             vertical baseline calibration. Added 10/14/16.
    GeneralStoreAppIdentifier_Calibrator_2_0     = 0x0006,
    //     2.0.1 - Added support for iSenses
    GeneralStoreAppIdentifier_Calibrator_2_0_1   = 0x0007,
    //     2.0.2 - better iSense support, WVL check during Intrinsics cal, Preservation
    //             of partial calibration (Lens, but not Bracket cal)
    GeneralStoreAppIdentifier_Calibrator_2_0_2   = 0x0008,
    //     2.0.3 - Fixed nasty arm7 bug w.r.t. Eigen alignment, clarifications between
    //             Standard/WVL calibrations. Added 11/23/16.
    GeneralStoreAppIdentifier_Calibrator_2_0_3   = 0x0009,
    //     2.1.0 - Bridge Headset accessible in Release, General Store warnings back in
    //             effect, Store links to bridge and WVL, added WVL intro page,
    //             preservation of Lens Cal works in all cases. Added 12/07/16.
    GeneralStoreAppIdentifier_Calibrator_2_1_0   = 0x0010,
    //     2.2.0 - Wide Vision Lens Auto-detector added, Welcome Screen layout changed
    //             entirely. Number of WVL grids dropped from 12 to 5.
    //             WVL Intro screen added. Friendlier interface, UI changes across stages.
    GeneralStoreAppIdentifier_Calibrator_2_2_0   = 0x0011,
    //     2.2.1 - Improvements to the Wide Vision Lens Auto-detector
    GeneralStoreAppIdentifier_Calibrator_2_2_1   = 0x0012,
    //     2.2.1 - Adding iPhone 8 compatibility
    GeneralStoreAppIdentifier_Calibrator_2_2_2   = 0x0013,
    //     2.3.0 - Adding Apple Intrinsics Calibration compensation
    GeneralStoreAppIdentifier_Calibrator_2_3_0   = 0x0014,
    //     2.3.1 - OIS compensation still imperfect, but WVL + Apple Intrinsics are handled
    //             properly throughout the app.
    //           - Easier conditions for indoor calibration mode
    //           - UI improvements for iPhoneX
    GeneralStoreAppIdentifier_Calibrator_2_3_1   = 0x0015,
    //     2.3.2 - OIS compensation refined; see Jupyter document here:
    //             https://drive.google.com/open?id=1Yv_uFEw_xripTI4e8ewuKnY2OMlIzRvg
    //           - CalibrationState struct now used throughout the App
    //           - UI improvements throughout the app
    //           - IR Normalization is now adaptive between inside and outside mode
    GeneralStoreAppIdentifier_Calibrator_2_3_2   = 0x0016,
    //     2.3.3 - Added support for iPad 6th Gen
    GeneralStoreAppIdentifier_Calibrator_2_3_3   = 0x0017,
    //     2.3.4 - Corrected OIS status of iPad 12.9 (2nd Gen)
    GeneralStoreAppIdentifier_Calibrator_2_3_4   = 0x0018,
    //     2.3.5 - Fixed a major bug that occurred when flashing the General Store
    GeneralStoreAppIdentifier_Calibrator_2_3_5   = 0x0019,
    //     2.3.6 - Fixed a bug on arm7 architectures
    GeneralStoreAppIdentifier_Calibrator_2_3_6   = 0x001A, /// c-c-c-combo breaker
    //     2.3.7 - Added iPhone X* capabilities, as well as minor bug fixes
    GeneralStoreAppIdentifier_Calibrator_2_3_7   = 0x001B,
    //     2.3.8 - Added iPhone XS Bridge Bracket Selection
    GeneralStoreAppIdentifier_Calibrator_2_3_8   = 0x001C,
    
    GeneralStoreAppIdentifier_Most_Recent_Calibrator = GeneralStoreAppIdentifier_Calibrator_2_3_8,
    
    // ExpertCalibrator: Keep within the 0x0200-0x2ff range.
    GeneralStoreAppIdentifier_ExpertCalibrator = 0x0200,
    GeneralStoreAppIdentifier_Invalid          = 0xFFFF
};

#ifdef __cplusplus
extern "C" {
#endif
// Writes 6 very important bytes so that the firmware knows where to plop the file
void bsInitGeneralStorePreset(struct ByteStream* destinationStream);
#ifdef __cplusplus
}
#endif

@interface SensorGeneralStoreManager : NSObject

+ (SensorGeneralStoreManager*)sharedManager;

+ (GeneralStoreAppIdentifier) appIdentifier;  // different for each version of Calibrator & ExpertCalibrator
+ (GeneralStoreAppIdentifier) appIdentifierForFileHeader;  // Same between Calibrator & ExpertCalibrator
+ (void)checkFileCorrupted:(bool)cond line:(int)line;
- (void)setCalibrationFileVersion:(int)fileVersion;
- (int)getCalibrationFileVersion;
- (void)cleanup;
- (void)cleanupCalibrationFiles;
- (void)parseByteStream:(struct ByteStream&)dataStream length:(size_t)length;

- (void)updateCalibrationWithData:(GeneralStoreCalibrationData&) calData;

- (bool)hasStructureIntrinsicsForVersion:(uint8_t)fileVersion
                              intrinsics:(oc::Intrinsics&)intrinsics;

- (void)removeCalibrationEntryForCurrentDevice;
- (void)deleteCalibrationFileForCamera:(GeneralStoreCameraType) cam;
// Accessors for Calibrator, mostly
- (bool)getNewerVersionCalibrationFilesDetected;
- (bool)getGeneralStoreFilesSizeLimitDetected;
- (bool)getGeneralStoreFilesCorruptedDetected;
- (void)setGeneralStoreFilesHaveBeenRead:(bool)haveBeenRead;
- (bool)generalStoreFilesHaveBeenRead;
- (int) uploadGeneralStoreFLAToSensor:(std::string) filePath
                          errorOutput:(NSError **)error
                          forceUpload:(bool)force;

- (void)printStatus;
- (bool)startUpload:(NSError**)error forceUpload:(bool)force;
- (bool)writeGeneralStoreFileToPath:(NSString*)filePath error:(NSError**)error;

// Debug Only Function
- (void)addCalibrationFile:(CalibrationFilev1*)calibrationFile;

- (bool)getCurrentDeviceCalibration:(GeneralStoreCalibrationData&) calData
                      forCameraType:(GeneralStoreCameraType)cameraType;

@end
