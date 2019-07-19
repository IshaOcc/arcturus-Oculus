//
//  FirmwareUpdater.h
//  Driver_iOS
//
//  Created by Forrest Heller on 8/27/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>
#import "SensorCommunicationController.h"

//------------------------------------------------------------------------------

@class PS1080File;
@class STSensorController;

//------------------------------------------------------------------------------

typedef NS_ENUM(NSInteger, STFirmwareSensorType) {
	STFirmwareSensorType_StructureSensor	= 0,
	STFirmwareSensorType_iSense             = 1,
	STFirmwareSensorType_AllowAny           = 2,
};

typedef enum {
	SUCCESS = 0,
	CRC_CORRECT,
	CRC_INCORRECT,
	UNKNOWN_OPCODE,
	UNEXPECTED_DATA_SIZE,
	XMEGA_SWITCH_RETRY_TIMEOUT,
	SIZE_CRC_RETRY_TIMEOUT,
	DATA_CRC_RETRY_TIMEOUT,
	XMEGA_PAGE_WRITE_TIMEOUT,
	FIRMWARE_UPDATE_CANNOT_SWITCH_UPDATER_UPDATE_IN_PROGRESS,
	FIRMWARE_UPDATE_CANNOT_SWITCH_APP_UPDATE_IN_PROGRESS,
	COULD_NOT_UPDATE_FIRMWARE_UPDATE_STATUS,
	FIRMWARE_UPDATE_PAGE_BUFFER_NOT_WRITTEN_YET,
	FIRMWARE_UPDATE_I2C_DATA_TOO_LONG,
	FIRMWARE_UPDATE_SIZE_TOO_BIG,
	FIRMWARE_UPDATE_SIZE_NEGATIVE,
	FIRMWARE_UPDATE_INVALID_PAYLOAD_SIZE,
	FIRMWARE_UPDATE_CANT_RESET_TOTAL_SIZE,
	FIRMWARE_UPDATE_NO_DATA,
	FIRMWARE_UPDATE_INCORRECT_STATE,
	FIRMWARE_UPDATE_NO_PAGE_GIVEN,
	FIRMWARE_UPDATE_DOESNT_HAVE_ALL_DATA,
	FIRMWARE_UPDATE_UNEXPECTED_BYTE_OFFSET,
	FIRMWARE_UPDATE_CRC_NOT_FINISHED,
	FIRMWARE_UPDATE_CRC_INCORRECT,
	FIRMWARE_UPDATE_CRC_CORRECT,
	FIRMWARE_UPDATE_CANT_SET_STATUS,
	FIRMWARE_UPDATE_NOT_A_PROGRAM,
	TWI_OPERATION_DOES_NOT_EXIST,
	FIRMWARE_UPDATE_NEEDS_CHARGE_BATTERY,
	APP_IS_RUNNING = 0xAA,
	UPDATER_IS_RUNNING = 0xBB,
} FirmwareUpdateReturnCodes;

typedef enum {
    iOSFileDownloadState_Downloading,
    iOSFileDownloadState_Downloaded,
    iOSFileDownloadState_Error,
    iOSFileDownloadState_ErrorWrongSize,
    iOSFileDownloadState_NotDownloaded
} iOSFileDownloadState;

//------------------------------------------------------------------------------

@interface iOSFile : NSObject

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, copy) NSString* fileName;
@property (nonatomic, copy, getter=localFilePath, setter=setLocalFilePath:) NSString* localFilePath;
@property (nonatomic, copy, getter=lastModified, setter=setLastModified:) NSString* lastModified;
@property (nonatomic, getter=crc16, setter=setCrc16:) uint16_t crc16;
@property (nonatomic, getter=crc16CCITTXmega, setter=setCrc16CCITTXmega:) uint16_t crc16CCITTXmega;
@property (nonatomic, getter=downloadState, setter=setDownloadState:) iOSFileDownloadState downloadState;
@property (nonatomic, getter=fileType, setter=setFileType:) uint16_t fileType;
@property (nonatomic) uint32_t version;

@end

//------------------------------------------------------------------------------

@protocol FirmwareUpdateDelegate<NSObject>

-(void) FirmwareUpdateStatus:(NSString*)status;
-(void) FirmwareUpdateProgress:(float)ratio;
-(void) FirmwareUpdateComplete;
-(void) FirmwareUpdateFailed:(NSString*)msg;

@end

//------------------------------------------------------------------------------

@interface OCFirmwareUpdatePackage : NSObject

-(id) initFromFirmwareUpdateContents:(const struct FirmwareUpdateContents*) firmwareUpdateContents;

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, retain, getter=ps1080Application, setter=setPs1080Application:) PS1080File* ps1080Application;
@property (nonatomic, retain, getter=xmegaApplication , setter=setXmegaApplication: ) PS1080File* xmegaApplication;
@property (nonatomic, retain, getter=xmegaUpdater     , setter=setXmegaUpdater:     ) PS1080File* xmegaUpdater;

@end

//------------------------------------------------------------------------------

@interface FirmwareUpdater : NSObject <SensorCommunicationControllerDelegate>

-(void) startUpdate:(NSMutableArray*)fileList;
-(void) startUpdateWithFirmwareUpdatePackage:(OCFirmwareUpdatePackage*) firmwareUpdatePackage;
/* Called from whoever is listening to STSensorController -- lets FirmwareUpdater proceed */

@property (nonatomic, assign) id<FirmwareUpdateDelegate> delegate;

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, getter=replaceSystemRestoreFirmware, setter=setReplaceSystemRestoreFirmware:) BOOL replaceSystemRestoreFirmware;

@end
