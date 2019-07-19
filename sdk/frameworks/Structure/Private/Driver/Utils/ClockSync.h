//
//  AccessoryTimestamp.h
//  Driver
//
//  Created by hellerf on 8/9/12.
//  Copyright (c) 2012 Occipital, Inc. All rights reserved.
//

#pragma once

#include "../ExperimentalFeatures.h"

struct TimestampPair {
    int64_t accessoryTimestamp;
    double hostTime;
    uint16_t seq;
};


struct RTTInfo {
    
    // We'll only act on a RTT response if the identifiers match
    uint16_t lastSentIdentifier;
   

    // For latency testing. Remove before release (or better
    // yet, re-design) so it doesn't require the RTT hack.
    uint16_t lastSentIdentifierForBottomLEDToggle;


    double lastSentTimestamp;

    // Since the rttAdjustment happens at a much lower frequency than it is used,
    // we'll store RTT/2 just to save on a few clock cycles
    double rttOver2;
};

#define CLOCK_SYNC_WINDOW_SIZE 512
struct ClockSync {
    struct TimestampPair timestamps[CLOCK_SYNC_WINDOW_SIZE];
    size_t timestampI;
    bool timestampsFilled;
    int64_t rollovers;
    
    struct RTTInfo rttInfo;
};

#ifdef __cplusplus
extern "C" {
#endif
    
    //void closeAccTimestampFiles();
    
void ClockSyncInit(struct ClockSync* clockSync);
void ClockSyncReset(struct ClockSync* clockSync);
void ClockSyncAddMeasurement(struct ClockSync* clockSync, uint32_t accessoryTimestamp, double hostTime, uint16_t seq);
double ClockSyncHostTimeFromAccessoryTimestamp(const struct ClockSync* clockSync, uint32_t accessoryTimestamp);
void ClockSyncHandleRTTRequest(struct ClockSync* clockSync, uint16_t identifier, double requestTime);
void ClockSyncHandleRTTResponse(struct ClockSync* clockSync, uint16_t identifier, double receivedTime);
void ClockSyncCorrectMeasurement(struct ClockSync* clockSync, uint16_t seq, uint32_t correction);
    
void ClockSyncRTTReceivedGlobalCallbackAfterBottomLEDToggle(double RTTInSeconds);

    
//    void closeAccTimestampFiles();
//    void openTimestampFiles2();
    
#ifdef __cplusplus
}
#endif
