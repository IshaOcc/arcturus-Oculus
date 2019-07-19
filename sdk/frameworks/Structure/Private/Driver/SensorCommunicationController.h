//
//  SensorCommunicationController.h
//  Structure
//
//  Copyright (c) 2015 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "STSensorDriver.h"
#import "ExperimentalFeatures.h"
#include "Utils/PSConfigs.h"
#include "XMegaUpdater.h"
#include "Utils/IRLEDDriver.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#import "FrameSync.h"
#import "Utils/IDParamsData.h"
#import "Utils/FixedParamsData.h"
#import "Utils/StateOfChargeDebug.h"

#import "Updates/FlashFileManager.h"

#if SYSTEM_PROFILE
#include <Core/Profiling.h>
#endif
#import "Utils/StructureInfo.h"

#import <Core/SensorDepthFrame.h>
#import <Core/ProcessedDepthFrame.h>
#import <Core/OccProtocol.h>

// Uncomment this to pretend your sensor is an Apple submit unit
//   which means FrameSync will be hacked to pretend it's working,
//   but not actually synchronize frames.

// #define PRETEND_APPLE_SUBMIT_UNIT 1

@protocol SensorCommunicationControllerDelegate <NSObject>
@optional
- (void) fileUploadStatus:(NSString*) status;
- (void) fileUploadProgress:(float) percentage;
- (void) fileUploadComplete;
- (void) fileUploadFailed:(FlashFileManagerResult)error withCode:(int)code;
- (void) xmegaUpdateFailed:(NSString*) message PS1080ErrorCode:(uint16_t)errorCode xmegaErrorCode:(uint8_t)xmegaErrorCode;
- (void) xmegaUpdateProgress:(float) progress;
- (void) xmegaUpdateSucceeded;
- (void) fileDeleteComplete;
- (void) fileDeleteFailed:(FlashFileManagerResult)error withCode:(int)code;

/* called when the FlashFileManager has read the file list from the PS1080.  fileList contains FlashFSFile objects */
- (void) flashFileManagerFinishedReadingFileList:(NSArray*) fileList;
- (void) flashFileManagerFailedReadingFileList:(NSError*) error;
- (void) ps1080LogMessageReceived:(NSString*)logMessage;
- (void) PS1080UploadFileInProgressMadeStreamingFail;

@end

#define ST_ERROR_DOMAIN @"Structure Sensor"

typedef enum {
    STError_NoSensorAttached = 100, // start at 100 because we are already using other opcodes elsewhere in errors
    STError_FileTableChangeCounterNotSupported, // 101 This version of the firmware does not support the file table change counter
    STError_FileTableChangeCounterError, // 102 the firmware returned a bad response when asked for the FileTableChangeCounter
    STError_GetEmitterDataError, // 103
    STError_FirmwareReplyCodeBad, // 104 the firmware returned a reply code indicating failure
    STError_FirmwareReponseTooShort, // 105 the firmware response was too short to be parsed or more short than expected.
    STError_FirmwareTooOld, // 106 the firmware does not have support for this action, because it is too old
    STError_UnrecognizedResponse, // 107 general error for an unrecognized response.  For example, we expect a status to be 1, 2, or 3, but the firwmare returns 4
    STError_CRCMismtach, // 108 A file upload or download operation did not have the CRC we expected
    STError_FileNotFound, // 109 The file was not found in the file table after a firmware upload
    STError_XmegaProgrammingCouldNotStart, // 110 a specific error which will contain the reason for the xmega programming failure
    STError_XmegaProgrammingFailed, // 111 there was a failure in the middle of programming the xmega
    STError_I2CReadResponseToShort, // 112 I2C read was issued to performI2CWriteRead, but too little read data was returned
    STError_XmegaProgrammingAlreadyStarted, //113 The programXmega: method was already called and hasn't finished
    STError_iOSChargingEnabledFailure, //114 We told the sensor to either disable/enable iOS charging but the sensor was unable to
    STError_EEPROMReadAddressMimstach, //115 We told the firmware that we wanted to read EEPROM address X, but it returned some other address
    STError_HardwareInfoReadbackMimstach, //116 We tried to set the sensor hardwareInfo, but after reading back hardwareInfo, it doesn't match what we wrote
    STError_EEPROMWriteAndVerifyDataMismatch, //117 We wrote values into the Xmega EEPROM, but upon readback the values did not match what we had written
    STError_HealthCheckFailurePreventedOperation, //118 We need to see 0 health check failures to do whatever you were trying to do
    STError_RequestAlreadyOutstanding,
} STError;

typedef enum {
    TestIRLEDResult_Pass = 1,
    TestIRLEDResult_Fail,
    TestIRLEDResult_TestDidNotComplete
} TestIRLEDResult;

typedef enum {
    XmegaFirmwareUpdateStatus_NotUpdating,
    XmegaFirmwareUpdateStatus_UpdatingApplication,
    XmegaFirmwareUpdateStatus_UpdatingUpdater
} XmegaFirmwareUpdateStatus;

typedef enum {
    XmegaProgrammingResult_Busy,
    XmegaProgrammingResult_Failed,
    XmegaProgrammingResult_Successful
} XmegaProgrammingResult;

typedef enum {
    XmegaRunningProgram_Application,
    XmegaRunningProgram_Updater
} XmegaRunningProgram;

struct XmegaProgrammingStatus {
    XmegaProgrammingResult result;
    size_t programmedByteCount;
    uint16_t ps1080ErrorCode;
    uint16_t xmegaErrorCode;
};

NSString* NSStringFromXmegaFirmwareUpdateStatus(XmegaFirmwareUpdateStatus status);

typedef enum {
    RegistrationParamsSource_Flash,
    RegistrationParamsSource_DRAM
} RegistrationParamsSource;

struct Registration {
    RegistrationParamsSource paramsSource;
    uint32_t params[29];
    RegistrationParamsSource paramsPaddingSource;
    uint16_t paramsPadding[3];
};

typedef enum {
    iOSChargingSetting_Disabled,
    iOSChargingSetting_Enabled,
    iOSChargingSetting_UnsetAndDisabled
} iOSChargingSetting;

// simple completion block that passes error=nil on success, error != nil on failure
typedef void (^CompletionBlock)(NSError* error);




@interface SensorCommunicationController : NSObject <STSensorDriver>

+ (SensorCommunicationController *)sharedController;

// When set to STSensorModel_None (the default value), this property has no effect.
// When set to either STSensorModel_StructureSensor or STSensorModel_ISense, it restricts streaming and general store operations to the specified model only.
// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, assign, getter=sensorModelRestriction, setter=setSensorModelRestriction:) STSensorModel sensorModelRestriction;

@property (nonatomic, readonly) STStreamConfig currentStreamConfig;

// EASession Info
- (STSensorModel) sensorModel;

// EASession Control
- (bool) closeSessions;

- (void) stopStreaming;

- (void) writeToConfigSession:(const void*)writeData size:(size_t)writeDataSize;

//! Updates the on-device General Store with a new calibration.
- (void)uploadCalibrationWithData:(GeneralStoreCalibrationData&) calData;

// sets new registration params, but does not latch them.  If you set params this way, you have to restart the stream for them to take effect
-(void) setRegistrationParams:(const struct Registration*)registration completionBlock:(CompletionBlock)completionBlock;
// Sets and latches registration params.  After calling this you should see depth frames that have this latchID. can be called mid-stream
- (void) setAndLatchRegistrationParams:(const struct Registration*)registration latchID:(uint16_t)latchID completionBlock:(CompletionBlock)completionBlock;
// sets and latches this RBT.  Computes registration params based on the newRbt
- (void) setAndLatchRegistrationRBT:(CompletionBlock)completionBlock;
// sets and latches based on the given custom rgb intrinsics. Useful for regression testing.
- (void) setAndLatchRegistrationRBTWithCustomVgaRgbIntrinsics:(oc::Intrinsics)rgbVgaIntrinsics completionBlock:(CompletionBlock)completionBlock;
- (bool) firmwareSupportsLatch;

@property(nonatomic, assign) id<SensorCommunicationControllerDelegate> fwDelegate;
// we will use this config id to encode the sequnence (upper 8 bits)
@property (nonatomic, readwrite) uint8_t sessionID;

/* THESE METHODS ARE OUTDATED. USE THE NEW METHODS like deleteFile:completionBlock:*/
// File Uploader methods
- (void) startUploadFile:(NSString*) filePath readOnly:(bool)readOnly;
- (void) resetCalibrationForCamera:(GeneralStoreCameraType) cam;
- (void) resetAllCalibrationFile;

// return 0 = success
-(int) startFileUploadFromMemory:(NSData*)fileContents fileType:(uint16_t) fileType fileVersion:(uint32_t) fileVersion readOnly:(bool)readOnly;
-(int) startFileUploadFromMemory:(NSData*)fileContents fileType:(uint16_t) fileType fileVersion:(uint32_t) fileVersion readOnly:(bool)readOnly fileOffset:(uint32_t) fileOffset;
- (void) fileUploadStatus:(NSString*) status;
- (void) fileUploadProgress:(float) percentage;
- (void) fileUploadComplete;
- (void) fileUploadFailed:(FlashFileManagerResult)error withCode:(int)code;
- (void) fileDeleteComplete;
- (void) fileDeleteFailed:(FlashFileManagerResult)error withCode:(int)code;

/* called from XMegaUpdater */
- (void) xmegaUpdateFailed:(NSString*) message PS1080ErrorCode:(uint16_t)errorCode xmegaErrorCode:(uint8_t)xmegaErrorCode;
- (void) xmegaUpdateProgress:(float) progress;
- (void) xmegaUpdateSucceeded;

- (void) setEmitterInput:(uint16_t)input;
- (void) setEnableCharging:(bool)enable;

- (void) setCmosGain:(int)gain;
- (void) setHoleFilterEnabled:(bool)enabled;

- (NSString*)getName;
- (NSString*)getFirmwareRevision;
- (NSString*)getHardwareRevision;
- (NSString*)getSerialNumber;
/* By default, use the PS1080 firmware sequence number as the canonical sequence number for a firmware. It will be updated last during a firmware update.
 However, there are features specific to the xmega OR that require xmega + PS1080 operation that necessitate the checking of xmega sequence numbers. */
- (uint16_t) getPS1080FirmwareSequenceMajorNumber;
- (uint16_t) getXmegaAppFirmwareSequenceMajorNumber;
- (uint16_t) getXmegaUpdaterFirmwareSequenceMajorNumber;

/* Is the serial number one of the Apple SP2 submission units? Early Nov 2012
 we submitted early prototype units with SP2 circuit boards and old firmware to Apple.
 We think that Apple might use these units to review their apps.  However the firmware is so
 old that it causes problems with many iOS driver assumptions and SFU so we have to special
 case things.  This routine tells you whether or not the current sensor one of these 
 ancient sensors by looking at the special serial number. */
+(BOOL) isSerialNumberAppleSubmitSP2:(NSString*) serialNumber;

- (void) getStructureInfo:(struct StructureInfo*) structureInfo;
- (oc::OccDepthSensorInfo) getOccDepthSensorInfo;

/* ONLY AVAIALBEAFTER CALLING asynchronousIDParamsDownloadWithCompletion */
- (NSString*) getProductionLineSerialNumber;
- (NSString*) getCredential;

- (bool) isConnected;
- (bool) isInitialized;
- (bool) isLowPower;

- (void) startBatteryRequest;
- (void) stopBatteryRequest;
- (float) getBatteryChargePercentage;
// is the battery % (returned by getBatteryChargePercentage) real?
- (bool) hasBatteryChargePercentage;

// deprecated: use programXmegaFirmware instead
- (void) startXMEGAProgramming:(XmegaUpdateType) updateType xmegaCRCOfFile:(uint16_t) crc16  xmegaFileSize:(int32_t) xmegaFileSize;
- (void) setResetPS1080;
- (void) setLowPowerMode; // deprecated: use enterLowPowerMode instead
- (void) enterLowPowerMode:(CompletionBlock)completionBlock;

/**
 * Provides very low level access to the IR LED driver
 **/
typedef void (^SetControlIRLEDDriverBlock)(NSError* error);
typedef void (^SetIRCmosTargetFramerateCompletionBlock)(NSError* error, uint16_t actualFramerateTimes1000);
// Sets the target framerate for the sensor's IR cmos.  Appears that we can go as low as 10.  The block returns the actual calculated framerate that the caller should receive
- (void) setIRCMOSTargetFramerate:(uint16_t)targetFPSTimes1000 completionBlock:(SetIRCmosTargetFramerateCompletionBlock) completionBlock;
- (void) setSyncUserIRLEDs:(bool)sync withFrequency:(uint32_t)frequency;

- (void) setIRLEDEnabled:(BOOL) enabled;

#if SYSTEM_PROFILE
- (oc::LatencyTimer*) getSendFrameToMainThreadLatencyTimer;
#endif


/* Data acquisition */
/**
 * Grab TEC data from Structure. When Structure sends back TEC data, STSensorController calls
 * [delegate TECDataAvailable:] with the TEC data from the host.
 */
- (void) startGetTECData;

typedef void (^EmitterDataReceivedBlock)(struct EmitterData emitterData, NSError* error);
- (void) getEmitterData:(EmitterDataReceivedBlock)emitterDataReceived;


/* General control */
/**
 * Enables/Disables the TEC.  Usually, the TEC is on during normal operation (PS1080 talking)
 **/
- (void) setTECEnabled:(bool) tecEnabled;
- (void) setPWM:(uint16_t) pwmId enabled:(bool) enabled frequencyInHz:(float)frequency dutyCycle100To0:(uint16_t) dutyCycle;

- (void) fileDownloadCompleteWithData:(uint8_t*)data length:(size_t)length;
- (void) fileDownloadFailed:(FlashFileManagerResult)error withCode:(int)code;
/* called when the FlashFileManager has read the file list from the PS1080.  fileList contains FlashFSFile objects */
 - (void) flashFileManagerFinishedReadingFileList:(NSArray*) fileList;

typedef void (^iOSChargeCurrentReceivedBlock)(int iOSChargeCurrentmA, NSError* error);
- (void) getiOSChargeCurrent:(iOSChargeCurrentReceivedBlock)iOSChargeCurrentReceived;
// these are available only in firmware 522+ (2.0+)
typedef void (^iOSChargingEnabledCompletionBlock)(bool isEnabled, NSError* error);
-(void) getiOSChargingEnabled:(iOSChargingEnabledCompletionBlock)completionBlock;
// this is similar to getiOSChargingEnabled, except it uses the firmware EEPROM backdoor and can determine whether or not the charging value was ever set
typedef void (^iOSChargingEnabledEEPROMSettingCompletionBlock)(iOSChargingSetting setting, uint8_t EEPROMContents, NSError* error);
-(void) getiOSChargingEnabledEEPROMSetting:(iOSChargingEnabledEEPROMSettingCompletionBlock)completionBlock;

// sets whether iOS charging is enabled -- available only in firmware 522 (2.0+)
-(void) setiOSChargingEnabled:(bool) enabled completionBlock:(CompletionBlock)completionBlock;

- (STStreamConfig)currentStreamConfig;

typedef void (^BatteryVoltageCurrentSOCReceivedBlock)(float voltage, float current, float socMilliampHours, float socPercent, bool hasACAdapterVoltage, float ACAdapterVoltage, NSError* error);
- (void) getBatteryVoltageCurrentSOC:(BatteryVoltageCurrentSOCReceivedBlock)batteryVoltageCurrentSOCReceivedBlock;

/* Uses the last battery voltage measurement to determine if a firmware update is safe */
- (BOOL) batteryVoltageIsHighEnoughForFirmwareUpdate;
- (BOOL) batteryVoltageIsHighEnoughForFirmwareUpdate:(float)measuredVoltage;

/* Gets the last measured battery voltage (may be ~10 seconds old) */
- (float) getBatteryVoltage;
/* Gets the last measured battery voltage (may be ~10 seconds old) in Amps */
- (float) getBatteryCurrent;

typedef void (^IDParamsDownloadCompletion)(struct IDParamsData *idParams, NSError* error);
/* Calls completionBlock on the *accessory thread* with sensor ID_PARAMS info Fills out getProductionLineSerialNumber and getCredential if available. */
- (void) asynchronousIDParamsDownloadWithCompletion:(IDParamsDownloadCompletion)completionBlock;
/* This works like the old asynchronousIDParamsDownloadWithCompletion, synchronously running the completionBlock on the main thread */
- (void) asynchronousIDParamsDownloadWithCompletionOnMainThread:(IDParamsDownloadCompletion)completionBlock;

/* Is the file table change counter supported by the firmware? */
-(bool) firmwareFileTableChangeCounterIsSupported;
typedef void (^FirmwareFileTableChangeCounterReceivedBlock)(uint16_t fileTableChangeCounter, NSError* error);
-(void) getFirmwareFileTableChangeCounter:(FirmwareFileTableChangeCounterReceivedBlock)completionBlock;

-(void) invalidateCachedFileList;

-(bool) stateOfChargeDebugInfoIsSupported;
typedef void (^StateOfChargeDebugInfoReceivedBlock)(const StateOfChargeDebugInfo* debugInfo, NSError* error);
-(void) getStateOfChargeDebugInfo:(StateOfChargeDebugInfoReceivedBlock)completionBlock;

typedef void (^TestIRLEDBlock)(TestIRLEDResult result, NSError* error);
- (void) testIRLEDs:(TestIRLEDBlock)completionBlock;

//typedef void (^FixedParamsDownloadCompletion)(struct FixedParamsData *fixedParams, NSError* error);
//- (void) asynchronousFixedParamsDownloadWithCompletion:(FixedParamsDownloadCompletion)completionBlock;

// This method is a workaround for a bug where the cmos gain doesn't actually properly get set without
// also hitting a certain codepath in firmware that resets all the registers on the CMOS. Weird bug.
// Should only be used for Calibrator.app (longer-term solution desired)
- (void)setSpecialForcingGainForInfraredStream:(int)gain;

// Performs a AHB_Write32Bits.  The AHB bus is the internal memory bus of the PS1080.  See the body function for an example.  AHB_Write32Bits looks like:
// 	UINT32 CurrentValue = Ahb_ReadDwordWA(Address);
//  CurrentValue = CurrentValue & ~Mask;
//  CurrentValue = CurrentValue | (Value & Mask);
//   Ahb_WriteDwordWA(Address, CurrentValue);
- (void) AHBWrite:(uint32_t)address value:(uint32_t)value mask:(uint32_t)mask completionBlock:(CompletionBlock)completionBlock;

// Performs a 32-bit read of the AHB bus. The AHB bus is the internal memory bus of the PS1080.  See the body function for an example
typedef void (^AHBReadCompletionBlock)(NSError* error, uint32_t value);
- (void) AHBRead:(uint32_t)address completionBlock:(AHBReadCompletionBlock)completionBlock;

- (void) pullGMCData;
- (void) setGMCModeEnabled:(BOOL)isEnabled completionBlock:(CompletionBlock)completionBlock;
- (void) setGMCDebugModeEnabled:(BOOL)isEnabled completionBlock:(CompletionBlock)completionBlock;
- (void) setGMCSmoothModeEnabled:(BOOL)isEnabled completionBlock:(CompletionBlock)completionBlock;

typedef void (^SetLowPowerModeTimeoutBlock)(NSError* error);
-(void) setLowPowerModeTimeout:(uint32_t) newTimeoutInMilliseconds completionBlock:(SetLowPowerModeTimeoutBlock)completionBlock;

typedef void (^XmegaRunningProgramReceivedBlock)(XmegaRunningProgram runningProgram, NSError* error);
-(void) getXmegaRunningProgram:(XmegaRunningProgramReceivedBlock)completionBlock;

// Starts PS1080 programming the xmega and waits for the programming to complete.  Internally it starts a timer and watches the
// xmega update status, giving you progress updates through the progressBlock, which can be nil if you don't care about
// progress updates.  This combines the work of startXmegaProgramming: and getXmegaProgrammingStatus: together.
typedef void (^ProgramXmegaFirmwareProgressBlock)(float progress, size_t programmedBytes, size_t totalFirmwareBytes);
-(void) programXmegaFirmware:(XmegaUpdateType) updateType
              xmegaCRCOfFile:(uint16_t) crc16
               progressBlock:(ProgramXmegaFirmwareProgressBlock)progressBlock
               xmegaFileSize:(size_t)xmegaFileSize
             completionBlock:(CompletionBlock)completionBlock;

// starts programming either the xmega updater or application
// if this returns an error in the completionBlock, use getXmegaProgrammingStatus to get
// the xmega and PS1080 error codes and diagnose the failure.
-(void) startXMEGAProgramming:(XmegaUpdateType) updateType xmegaCRCOfFile:(uint16_t) crc16  completionBlock:(CompletionBlock)completionBlock;

// returns the xmega programming status, how many bytes are written, whether it failed, succeeded, or is busy, etc.
typedef void (^XmegaProgrammingStatusReceivedBlock)(struct XmegaProgrammingStatus* status, NSError* error);
-(void) getXmegaProgrammingStatus:(XmegaProgrammingStatusReceivedBlock) completionBlock;

// This function is only supported on 1.8+ firmwares (even though the code appears to contain two paths).
// It allows you to get the internal atomic state that
// the sensor's xmega processor uses during a firmware update.  This state is set after entering a firmware
// update (but not right after startProgrammingXmega!  It takes a few more ms to start).  This state is set
// to not updating, if and only if the firmware update has been completed.
typedef void (^XmegaFirmwareUpdateStatusReceivedBlock)(XmegaFirmwareUpdateStatus status, NSError* error);
-(void) getXmegaFirmwareUpdateStatus:(XmegaFirmwareUpdateStatusReceivedBlock) completionBlock;

// hey you can only write 1 byte with this due to a firmware limitation!
typedef void (^I2CWriteReadCompleted)(NSData* readData, NSError* error);
-(void) performI2CWriteRead:(uint8_t) busId slaveAddress:(uint8_t) slaveAddress writeData:(NSData*) writeData readLengthInWords:(size_t) readLengthInWords completionBlock:(I2CWriteReadCompleted) completionBlock;
-(void) performI2CWriteRead:(uint8_t) busId slaveAddress:(uint8_t) slaveAddress writeWord:(uint16_t) writeWord readLengthInWords:(size_t) readLengthInWords completionBlock:(I2CWriteReadCompleted) completionBlock;

typedef void (^I2CWriteCompleted)(NSError* error);
-(void) performI2CWrite:(uint8_t) busId slaveAddress:(uint8_t) slaveAddress writeData:(NSData*) writeData completionBlock:(I2CWriteCompleted) completionBlock;
-(void) performI2CWrite:(uint8_t) busId slaveAddress:(uint8_t) slaveAddress writeData:(uint16_t*)writeData writeDataSizeInBytes:(size_t)writeDataSizeInBytes completionBlock:(I2CWriteCompleted) completionBlock;

// routines for reading and writing xmega EEPROM directly, available only in firmware 522 (2.0)
typedef void (^XmegaEEPROMReadCompleted)(NSData* readData, NSError* error);
-(void) performXmegaEEPROMRead:(uint16_t)EEPROMAddress readLength:(size_t)readLength completionBlock:(XmegaEEPROMReadCompleted)completionBlock;
-(void) performXmegaEEPROMWrite:(uint16_t)EEPROMAddress writeContents:(const uint8_t*)writeContents writeContentsLength:(size_t)writeContentsLength completionBlock:(CompletionBlock)completionBlock;
// this will write the EEPROM and then read it back, checking that the contents matches
-(void) performXmegaEEPROMWriteAndVerify:(uint16_t)EEPROMAddress writeContents:(const uint8_t*)writeContents writeContentsLength:(size_t)writeContentsLength completionBlock:(CompletionBlock)completionBlock;

/* Hardware info is some 16-bits that live the xmega EEPROM describing some useful properties of the hardware.
 Previously we just relied on the firmware signature for all the hardware info.  The trigger to introduce hardware info
 was needing to know DaughterPCB versions, which are not encoded anywhere besides hardwareInfo */
-(bool) hardwareInfoIsSupported; // only firmware 2.0-beta6+ supports hardware info
// Retrieves the hardwareInfo of the sensor
typedef void (^HardwareInfoReceived)(const struct HardwareInfo* hardwareInfo, const uint8_t* hardwareInfoRaw, size_t hardwareInfoRawLength, NSError* error);
-(void) getHardwareInfo:(HardwareInfoReceived)completionBlock;
// Sets the hardware info in the sensor, then reads the hardwareInfo back.  If the hardwareInfo read back doesn't match what was sent, then an error is returned on the completionBlock.  You shouldbe using hardwareInfo-> to make decisions, hardwareInfoRaw is intended for testing/verification purposes
-(void) setAndVerifyHardwareInfo:(const struct HardwareInfo*)hardwareInfo completionBlock:(CompletionBlock)completionBlock;

// Sets the Xmega running program! DOES NOT CHECK THE PROGRAM IS RUNNING! Use getXmegaRunningProgram to get the status of the running program
-(void) setXmegaRunningProgram:(XmegaRunningProgram)newRunningProgram completionBlock:(CompletionBlock)completionBlock;

typedef void (^DownloadFileListCompletionBlock)(NSArray* fileList, NSError* error);
// does a full download of the PS1080 file table.  Unsorted. Uses a cached file list if possible
-(void) downloadFileList:(DownloadFileListCompletionBlock) completionBlock;
// bypasses any file list cached by the driver.  Unsorted.  Does not update the cache
-(void) downloadFileListNoCache:(DownloadFileListCompletionBlock) completionBlock;
// returns the currently cached file list, an NSArray of FlashFSFile objects.  In general you don't want to use this.
// It's more of an optimization if you know your app doesn't even need to check the file list change counter.
-(NSArray*) getCachedFileList;

// this method will sort the array of FlashFSFiles returned in the downloadFileList completionBlock
+(NSArray*) sortedFileListByFileId:(NSArray*) unsortedFileList;

typedef void (^DownloadFileCompletionBock)(uint16_t type, uint32_t version, NSData* contents, NSError* error);
typedef void (^DownloadFileProgressBlock)(float progress, size_t downloadedWords, size_t fileSizeInWords, NSData* downloadedFileContents);
// This method downloads the file table and then downloads the file, *checking the crc* against
// the contents of the PS1080 file table.  You can avoid downloading the file table if you call downloadFileById:fileSizeInWords:filePS1080CRC:progressBlock:completionBlock:
- (void) downloadFileById:(uint16_t) fileId
            progressBlock:(DownloadFileProgressBlock)progressBlock
          completionBlock:(DownloadFileCompletionBock) completionBlock;
// this function is used by downloadFileById:completionBlock:.  It does not download the file table before downloading the file.
// Use this function if you already have the file table since it will be faster.
- (void) downloadFileById:(uint16_t) fileId
          fileSizeInWords:(size_t) fileSizeInWords
             filePS1080CRC:(uint16_t)filePS1080CRC
            progressBlock:(DownloadFileProgressBlock)progressBlock
          completionBlock:(DownloadFileCompletionBock) completionBlock;

typedef void (^DownloadFileFLACompletionBock)(NSData* flaFileContents, NSError* error);
// this function will give you the full/raw .fla file from the firmware. It's used by the downloadFileById methods. The firmware natively gives back .fla files and this function has the lowest overhead of any of the download functions
- (void) downloadFLAFileById:(uint16_t) fileId
fileSizeInWordsWithFLAHeader:(size_t) fileSizeInWordsWithFLAHeader
               filePS1080CRC:(uint16_t)filePS1080CRC
               progressBlock:(DownloadFileProgressBlock)progressBlock
             completionBlock:(DownloadFileFLACompletionBock) completionBlock;

// this method will give you the full/raw .fla file.  It picks out the latest file version of fileType from the fileList and downloads it.  If a file of that type cannot be found in the file list, then this method passes an error (STError_FileNotFound) to the completionBlock. 
- (void) downloadLatestFLAFile:(uint16_t)fileType
                      fileList:(NSArray*)fileList
                 progressBlock:(DownloadFileProgressBlock)progressBlock
               completionBlock:(DownloadFileFLACompletionBock)completionBlock;


// Sets file attributes on a flash file
-(void) setFileAttributes:(uint16_t)fileId attributes:(uint16_t)attributes completionBlock:(CompletionBlock) completionBlock;

// Deletes a file from the flash.  Does not work if you do not call setFileAttributes
-(void) deleteFile:(uint16_t) fileId completionBlock:(CompletionBlock) completionBlock;

// Clears file attributes and delete the file
-(void) clearAttributesAndDeleteFile:(uint16_t) fileId completionBlock:(CompletionBlock) completionBlock;

// Finds and deletes the general store file for you
- (void) deleteGeneralStoreFile:(CompletionBlock) completionBlock;

typedef void (^UploadFileProgressBlock)(float progress, size_t uploadedWords, size_t fileSizeInWords);
// Uploads a file.  The space on the flash file system must be empty. This method does not automatically delete files like FlashFileManager
-(void) uploadFile:(XFlashMap_FileType)fileType
          contents:(NSData*)contents offset:(uint32_t)offset
           version:(uint32_t)version attributes:(uint16_t)attributes
     progressBlock:(UploadFileProgressBlock)progressBlock
   completionBlock:(CompletionBlock)completionBlock;

// Uploads a file, knows about .fla file headers so they don't have to be broken out
-(void) uploadFileFLAContents:(NSData*)contentsWithFLAHeader
                       offset:(uint32_t)offset
                    attributes:(uint16_t)attributes
                progressBlock:(UploadFileProgressBlock)progressBlock
              completionBlock:(CompletionBlock)completionBlock;

// Hook for SFU to determine whether or not a file upload was interrupted by the app backgrounding or something
@property (nonatomic) bool uploadFileWasInterruptedBySensorCloseDueToAppResignActive;

// Cancel an in-progress file upload.  Only available in 1:12 and above.
-(void) cancelFileUpload:(CompletionBlock)completionBlock;

//Tells you whether or not the PS1080 is in the middle of a file upload
typedef void (^PS1080UploadFileInProgress)(bool uploadInProgress, uint32_t uploadInProgressExpectedOffset, NSError* error);
-(void) detectPS1080UploadFileInProgress:(PS1080UploadFileInProgress)completionBlock;

/* Setting this to YES will cause the upload routines to send some purposely wrong data to the PS1080 to check our upload routine CRC checking */
@property (nonatomic, getter=uploadCorruptMemoryTestCRCCheck, setter=setUploadCorruptMemoryTestCRCCheck:) BOOL uploadCorruptMemoryTestCRCCheck;
/**
 * Returns an NSArray containing HealthCheckFailures
 */
-(NSArray*) getHealthCheckFailures;
/**
 * Returns true if all the health checks passed
 */
-(bool) healthCheckPassed;

-(void) startReadingFileList;

- (FlashFileManager*) flashFileManager;

-(void) refreshStructureInfo: (CompletionBlock) completionBlock;

/* These two methods are for internal testing only! They set xmega eeprom state such that
 the xmega thinks that the sensor failed in the middle of programming the xmega application
 or programming the xmega updater */
-(void) setXmegaUpdaterProgrammingFailed: (CompletionBlock) completionBlock;
-(void) setXmegaApplicationProgrammingFailed: (CompletionBlock) completionBlock;
-(bool) firmwareSupportsStreamHeaderExtensions;

/* You shouldn't need to call this directly.  The SensorCommunicationController will regularly call this on a timer and provide you with logs */
typedef void (^PS1080LogMessagesReceivedBlock)(NSArray* newLogMessages, NSError* error);
-(void) getPS1080NewMessages:(PS1080LogMessagesReceivedBlock)completionBlock;

@property (getter=inMaintenanceMode, setter=setInMaintenanceMode:) BOOL inMaintenanceMode;

@property (atomic, readwrite, getter=isRunningIRLEDTest, setter=setIsRunningIRLEDTest:) bool isRunningIRLEDTest;

/* This only works on special, ATS-specific firmware! */
-(void) makeFirmwareSendAppLaunchMessageForATS:(CompletionBlock)completionBlock;
-(void) makeFirmwareSendStartStopPowerMessageForATS:(CompletionBlock)completionBlock;

// percentage is really 0.0 - 1.0 ... someday we should get consistent about percentage
-(void) setFakeBatteryPercentage:(float) percentage;
-(void) setUseFakeBatteryPercentage:(bool) shouldUse;

typedef void (^SendRttRequestWithToggleLedBlock)(double rttTime, NSError* error);
-(void) sendRttRequestWithToggleLed:(bool)toggleLed completionBlock:(SendRttRequestWithToggleLedBlock)completionBlock;


- (void)sendSensorStatus;

@end

bool hardwareInfoFromEEPROMBlock(struct HardwareInfo* hardwareInfo, const uint8_t EEPROMBlock[]);
