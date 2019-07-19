//
//  SensorLowPowerWaker.h
//  Structure
//
//  Created by Forrest Heller on 10/27/15.
//  Copyright © 2015 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>
#import <ExternalAccessory/ExternalAccessory.h>

@class EAAccessory;

typedef enum {
    WakeResult_Error,
    WakeResult_TimedOut,
    WakeResult_NeedsChargeBattery,
    WakeResult_DisconnectedWillReconnect
} WakeResult;

typedef void (^WakeCompleteBlock)(WakeResult result);

/*
 Structure Sensor can go into a low power mode.  In order to stream depth it must be woken from low power mode.
 This class takes care of sending the wake up message and calls the block you provide with a result.  You can 
change the timeout by changing the GIVE_UP_TIME_INTERVAL #define. Before this class some of the methods in
 SensorCommunicationController ran on the main thread sometimes and other times on the accessoryThread and
 that was kind of difficult to keep track of so Forrest split the stuff that runs on the main thread out
 into this class. */
@interface SensorLowPowerWaker : NSObject<NSStreamDelegate, EAAccessoryDelegate>
-(void) writeWakeMessageToAccessoryOnMainThread:(EAAccessory*)accessory completionBlockOnMainThread:(WakeCompleteBlock)completionBlock;
@end