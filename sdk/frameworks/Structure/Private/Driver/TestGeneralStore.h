//
//  TestGeneralStore.h
//  Structure
//
//  Created by Pat O'Keefe on 8/8/14.
//  Copyright (c) 2015 Occipital, Inc. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>

#if DEBUG
@interface TestGeneralStore : NSObject

- (void)runAllTests;
- (void)testiOSDeviceEntry;
- (void)testStructureSensorIntrinsicsEntry;
- (void)testiOSBuiltInCameraIntrinsicsEntry;
- (void)testCalibrationFileEntry;

- (void)testUploadCalibrationFile;
- (void)testUnrecognizeStream;
- (void)testFileSizeLimit;
- (void)testNewCalibrationFile;
- (void)testRemoveCurrentCalibration;

// testUpdateExistingCalibrationFiles, when sensor already has a old calibration file with
// 6 other devices, and update the current one
- (void)testUpdateExistingCalibrationFiles;

- (void)testPreserveOrOverwriteUnknownValueForBuiltInCameraCalibrationEntry;
- (void)testPreserveOrOverwriteUnknownValueForStructureSensorIntrinsicsEntry;
- (void)testPreserveOrOverwriteUnknownValueForiOSDeviceEntry;
- (void)testPreserveUnknownEntry;
- (void)testOverwrittenStructureIntrinsics;
- (void)testCorruptedFile;

// Not actually a test, more like a utility
- (void)debugExistingCorruptedFile;

@end

#endif
