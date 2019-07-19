//
//  HealthCheckFailure.h
//  Structure
//
//  Created by Forrest Heller on 8/27/14.
//  Copyright (c) 2015 Occipital, Inc. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>

typedef enum {
    HealthCheckFailureType_NoSensorConnected,
    HealthCheckFailureType_SensorInSystemRestore,
    HealthCheckFailureType_SensorMissingSystemRestoreFirmware
} HealthCheckFailureType;

/**
 * Represents a failure of a health check.  Right now a sensor health check is done by looking at the PS1080 flash file system
 * and checking for a few things that are possibly awry.
 */

@interface HealthCheckFailure : NSObject
@property (nonatomic) HealthCheckFailureType type;
@property (nonatomic, retain) NSString* shortName;
@property (nonatomic, retain) NSString* message; // friendly-ish message for Occipital internal
@end
