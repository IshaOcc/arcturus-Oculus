//
//  FrameSync.h
//  Structure SDK
//
//  Created by Pat O'Keefe on 11/14/13.
//  Copyright (c) 2013-2016 Occipital. All rights reserved.
//

#pragma once

// Always include that file first. This makes sure we have all the definitions we need without absolutely requiring precompiled headers.
// Note that on iOS/OSX we typically include it implicitly as a precompiled header.
#include <Structure/StructurePrefix.pch>

#include <Core/FramePairing.h>
#include <Core/Platform.h>
#include <functional>
#include <mutex>

#if __APPLE__
#   import "STSensorDriver.h"
#   import <Core/StructureTypesPrivate.h>
#   import <Foundation/Foundation.h>
#   import <CoreMedia/CMSampleBuffer.h>
#endif

//------------------------------------------------------------------------------

namespace oc {

struct ShutterSyncState : public FramePairingShutterDeltaDelegate
{
    virtual void onNewIOSandSensorShutterDelta (double iosColorFrameTimestamp,
                                                double sensorFrameTimestamp) override;

    using SetShutterDelay = std::function<void (double)>;
    void makeShutterDelayDecision (SetShutterDelay setShutterDelay);

#if __APPLE__
    void makeShutterDelayDecision (id<STSensorDriver> driver);
#endif
    
    double latestIOSColorTimestamp = NAN;
    double latestSensorTimestamp = NAN;
    double hostTimeWhenStateUpdated = NAN;
    
    std::mutex shutterSyncMutex;
};

} // oc namespace

//------------------------------------------------------------------------------

#if SYS_IOS // FIXME: The internal implementation unconditionally dispatches on the non-file sensor controller delegate, which precludes macOS usage.

@interface FrameSync : NSObject <STSensorDriverFrameDelegate>

- (void)onStartStreaming;
- (void)onStopStreaming;

- (BOOL) iOSColorFrameAvailable:(oc::CameraColorFramePtr)colorFrame;
- (BOOL) sensorDepthFrameAvailable:(oc::SensorDepthFramePtr)depthFrame;
- (BOOL) sensorInfraredFrameAvailable:(oc::SensorInfraredFramePtr)infraredFrame;
- (BOOL) sensorVisibleFrameAvailable:(oc::CameraColorFramePtr)visibleFrame;

@property (nonatomic) std::vector<oc::FramePairingCameraType> camerasToSync;
@property (nonatomic) BOOL frameSyncEnabled;

@property (nonatomic) NSTimeInterval maxDeltaTimestampBetweenIOSCameraAndSensor;
@property (nonatomic) BOOL veryHighLatencyModeEnabled;

/** Configurable queue on which callbacks are invoked.
 *
 * @note Defaults to the main queue.
 */
@property (nonatomic, retain) dispatch_queue_t callbackQueue;

@end

#endif
