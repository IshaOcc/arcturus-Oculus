//
//  XMegaUpdater.h
//  Driver_iOS
//
//  Created by Forrest Heller on 8/19/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>

#define MAKE_WORD(high, low) (((high << 8) & 0xff00) | (low & 0xff))

typedef enum {
    XmegaUpdateType_Application,
    XmegaUpdateType_Updater
} XmegaUpdateType;

//enum EFrameworkModules in firmware/Driver
#define MODULE_XMEGA 100

typedef enum {
	XMEGA_UPDATER_ERROR_INVALID_UPDATE_TYPE = MAKE_WORD(MODULE_XMEGA, 0),
	XMEGA_UPDATER_COULD_NOT_SWITCH_PROGRAMS,
	XMEGA_UPDATER_FAILED_INIT_XMEGA,
	XMEGA_UPDATE_WRITE_FLASH_PAGE_TO_XMEGA_FAILED,
	XMEGA_UPDATER_ERROR_FROM_PAGE_WRITE_ACK,
	XMEGA_UPDATER_COULD_NOT_SWITCH_BACK_TO_APP_AT_FINISH,
	XMEGA_UPDATER_CRC_DOES_NOT_MATCH,
	XMEGA_UPDATER_ALREADY_IN_PROGRESS,
	XMEGA_UPDATER_COULD_NOT_READ_FLASH,
	XMEGA_UPDATER_CRC_I2C_ERROR,
	XMEGA_UPDATER_CRC_INCORRECT
} EXmegaUpdater_ErrorCodes;

@class PSCommand;
/**
 *
 * The XMegaUpdater deals with telling the PS1080 to update the firmware of the Xmega and reports the status
 * back to sensorController (passed in to initWithSensorController)
 */
@interface XMegaUpdater : NSObject
/**
 * The sensorController is not retained
 */
-(id) init;
/* tell PS1080 to start programming the Xmega -- returns 0 on success */
- (void) startXMEGAProgramming:(XmegaUpdateType) updateType xmegaCRCOfFile:(uint16_t) crc16 xmegaFileSize:(int32_t) xmegaFileSize ;
/**
 * When a command is received from PS1080, call this method to tell XMegaUpdater about it
 */
-(void) commandReceived:(PSCommand*) command;
@end
