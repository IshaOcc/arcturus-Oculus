//
//  SensorCommunicationController.h
//  Structure
//
//  Copyright (c) 2015 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/NSObject.h>

@interface SensorCommunicationController : NSObject

+ (SensorCommunicationController *)sharedController;

@property BOOL useWAL;

@end
