//
//  OCRegistrationManager.h
//  Structure
//
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>

#include "PSConfigs.h"

#include <Core/Intrinsics.h>

// defined in SensorCommunicationController.h
struct Registration;

typedef struct {
    float xCoeffs[10];
    float yCoeffs[10];
    float betaX[2];
    float betaY[2];
} RegistrationPolynomialCoefficients;

@interface OCRegistrationManager : NSObject


// In default non-latch call that will upload the registration parameter to ZSP
// We will need to set the latch map with key 0 to update to the latest one
// if not forcing the forceZeroLatchID, we need to insert the latest one to a new latchID.
// this function will return the latchID passing to the firmware, and this is the same latch ID inside the intrinsics map of AVfoundationCameraCalibrationManager
//
// For now we need to provide a fixed camera color lens position if using firmware 1.1, since it does
// not support latch ID.
- (uint16_t) computeRegistrationParamsForConfig:(const struct SensorConfigSequence*)sequence
                             registrationParams:(struct Registration*)registration
                               forceZeroLatchID:(bool)forceZeroLatchID
         colorLensPositionIfLatchIDNotSupported:(float)colorLensPosition;

- (void) computeAndSendRegistrationParamsForConfig:(const struct SensorConfigSequence*)sequence
            colorLensPositionIfLatchIDNotSupported:(float)colorLensPosition;

- (uint16_t) computeRegistrationParamsForVgaRgbIntrinsics:(oc::Intrinsics)rgbVGAIntrinsics
                                                   config:(const struct SensorConfigSequence*)sequence
                                       registrationParams:(struct Registration*)registrationParams
                                         forceZeroLatchID:(bool)forceZeroLatchID;

//- (void) createAndUploadGeneralStore:(bool)includeIntrinsics;


//- (RegistrationPolynomialCoefficients) getPolynomialCoefficients;

@end
