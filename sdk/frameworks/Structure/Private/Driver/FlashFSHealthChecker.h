//
//  FlashFSHealthChecker.h
//  Structure
//
//  Created by Forrest Heller on 8/26/14.
//  Copyright (c) 2015 Occipital, Inc. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>

/**
 * This class looks at a Structure Sensor PS1080 flash file table.  Then it can tell you things about the sensor,
 * such as whether or not the sensor has just been through a system restore (where the file table gets reset to hard-coded default locations and sizes)
 * and whether the PS1080 system restore firmware exists
 */
@interface FlashFSHealthChecker : NSObject

/**
 * Returns an array of HealthCheckFailure objects
 * @param fileList is an array file FlashFSFile objects
 */
-(NSArray*) healthCheckFailuresOnFlashFS:(NSArray*) fileList;

/**
 * This method tells you if the sensor is in system restore mode.  System restore mode means that a new, default file table has been
 * installed on the system.  The default file table has assumed file offsets and lengths, meaning that files are lossed and
 * report an incorrect size.
 * @param fileList is an array file FlashFSFile objects
 */
-(BOOL) isSensorInSystemRestore:(NSArray*) fileList;

/**
 * Does the sensor contain a PS1080 firmware at the system restore location? This is important
 * because if the sensor goes into system restore, it will -only- use this firmware.
 * @param fileList is an array file FlashFSFile objects
 */
-(BOOL) sensorContainsSystemRestoreFirmware:(NSArray*) fileList;

@end
