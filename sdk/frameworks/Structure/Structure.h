/*
    This file is part of the Structure SDK.
    Copyright © 2016 Occipital, Inc. All rights reserved.
    http://structure.io
*/

#pragma once

#include <stdint.h>
#include <stdlib.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <GLKit/GLKit.h>
#import <Core/StructureTypes.h>

#define ST_API __attribute__((visibility("default")))

# if !defined(__cplusplus) && !defined (HAS_LIBCXX)
#   error "Structure framework requires the C++ runtime. See Structure SDK Reference."
# endif

// Make sure the deployment target is higher or equal to iOS 8.
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && (__IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_8_0)
#   error This version of Structure SDK only supports iOS 8 or higher.
#endif

typedef struct {
    const int major;
    const int minor;
    const int patch;
    const char* version;
} SDKVersion;

/** Returns a string specifying the current SDK version */
SDKVersion currentSDKVersion(void);

//------------------------------------------------------------------------------
#pragma mark - Sensor Controller Constants

/** Indicates the initialization status of the Structure Sensor.
 
See also:

- [STSensorController initializeSensorConnection]
*/
typedef NS_ENUM(NSInteger, STSensorControllerInitStatus)
{
/// Indicates controller uninitialized because the sensor was not found.
    STSensorControllerInitStatusSensorNotFound      = 0,
/// Indicates controller initialization succeeded.
    STSensorControllerInitStatusSuccess             = 1,
/// Indicates controller was previously initialized.
    STSensorControllerInitStatusAlreadyInitialized  = 2,
/// Indicates controller uninitialized because sensor is waking up.
    STSensorControllerInitStatusSensorIsWakingUp    = 3,
/// Indicates controller uninitialized because of a failure to open the connection.
    STSensorControllerInitStatusOpenFailed          = 4,
/** Indicates controller is uninitialized and sensor is not opened because the application is running in the background.

At the moment we do not support initialiazing the sensor while in the background.
*/
    STSensorControllerInitStatusAppInBackground     = 5,
};

/** Streaming Interruption Reason
 
See also:
 
- [STSensorControllerDelegate sensorDidStopStreaming:]
*/
typedef NS_ENUM(NSInteger, STSensorControllerDidStopStreamingReason)
{
/** Sensor stops streaming because of iOS app will resign active.
This can occur when apps are sent to the background, during fast app switching, or when the notification/control center appears.
*/
    STSensorControllerDidStopStreamingReasonAppWillResignActive = 0
};

/** Frame Sync Configuration.

Constants indicating whether the driver should attempt to synchronize depth or infrared frames with color frames from AVFoundation.

When frame sync is active (i.e. **not** equal to `STFrameSyncOff`), one of the following methods is used in lieu of [STSensorControllerDelegate sensorDidOutputDepthFrame:], depending on the selected configuration:

- [STSensorControllerDelegate sensorDidOutputSynchronizedDepthFrame:colorFrame:]
- [STSensorControllerDelegate sensorDidOutputSynchronizedInfraredFrame:colorFrame:]

You must then repeatedly call frameSyncNewColorBuffer: from the AVFoundation video capture delegate methods. Otherwise, the sensor controller delegate methods will never deliver any frames. This is simply because synchronized frames cannot be delivered if there are no color frames to synchronize.

@note Frame sync of depth+infrared+RGB and 60 FPS depth are not currently supported.
@note For frame sync to be effective, the AVCaptureDevice must be configured to have a min and max frame rate of 30fps.
*/
typedef NS_ENUM(NSInteger, STFrameSyncConfig)
{
    /// Default mode, no synchronization.
    STFrameSyncOff __attribute__((deprecated)) = 0,
    
    /// Frame sync between AVFoundation video frame and depth frame.
    STFrameSyncDepthAndRgb __attribute__((deprecated)),
    
    /// Frame sync between AVFoundation video frame and infrared frame.
    STFrameSyncInfraredAndRgb __attribute__((deprecated)),
    
};

/** Sensor Calibration Type

Calibration types indicate whether a Structure Sensor + iOS device combination has no calibration, approximate calibration, or a device specific calibration from Calibrator.app.
 
See also:

- [STSensorController calibrationType]
 */
typedef NS_ENUM(NSInteger, STCalibrationType)
{
    /// There is no calibration for Structure Sensor + iOS device combination.
    STCalibrationTypeNone = 0,
    
    /// There exists an approximate calibration Structure Sensor + iOS device combination.
    STCalibrationTypeApproximate,
    
    /// There exists a device specific calibration from Calibrator.app of this Structure Sensor + iOS device combination.
    STCalibrationTypeDeviceSpecific,
};

//------------------------------------------------------------------------------

// Dictionary keys for `[STSensorController startStreamingWithOptions:error:]`.
extern NSString* const kSTStreamConfigKey;
extern NSString* const kSTFrameSyncEnabledKey;
extern NSString* const kSTFrameSyncConfigKey __attribute__ ((deprecated));
extern NSString* const kSTPreferInfraredSyncOverDepthKey;
extern NSString* const kSTHoleFilterEnabledKey;
extern NSString* const kSTHighGainEnabledKey;
extern NSString* const kSTColorCameraFixedLensPositionKey;
extern NSString* const kSTCoreInfraredStreamSourceKey;
extern NSString* const kSTColorCameraAVIntrinsicsEnabledIfAvailableKey;
extern NSString* const kSTStreamPresetKey;
extern NSString* const kSTSensorControllerManagedBySTCaptureSessionKey;

//------------------------------------------------------------------------------
#pragma mark - STSensorControllerDelegate

/** Sensor Controller Delegate
 
The interface that your application-specific class must implement in order to receive sensor controller callbacks.
 
@warning When creating a new application implementing a sensor controller delegate, the main `Info.plist` needs to contain an additional key "`Supported external accessory protocols`", with the following array of values:
 
 - `io.structure.control`
 - `io.structure.depth`
 - `io.structure.infrared`
 
Without this modification to the plist, the app will not be able to connect to the sensor. All sample apps have this key/value array.
 
See also <[STSensorController sharedController]> & <[STSensorController delegate]>.
 
Delegate Registration Example:
 
    [ STSensorController sharedController ].delegate = self;
*/
@protocol STSensorControllerDelegate <NSObject>

/// @name Connection Status

/// Notify the delegate that the controller established a successful connection to the sensor.
- (void)sensorDidConnect;

/// Notify the delegate that the sensor was disconnected from the controller.
- (void)sensorDidDisconnect;

/** Notify the delegate that the sensor stopped streaming frames to the controller.

@param reason The reason why the stream was stopped. See: STSensorControllerDidStopStreamingReason.
*/
- (void)sensorDidStopStreaming:(STSensorControllerDidStopStreamingReason)reason;

/// @name Power Management

/// Notify the delegate that the sensor has left low power mode.
- (void)sensorDidLeaveLowPowerMode;

/// Notify the delegate that the sensor needs charging.
- (void)sensorBatteryNeedsCharging;

@optional

/// Notify the delegate that the sensor has had an AC Adapter plugged in or unplugged
- (void)sensorChargerPluggedIn;
- (void)sensorChargerUnplugged;

/// Notify the delegate that the sensor has entered low power mode, this currently does nothing and is reserved for future usage.
- (void)sensorDidEnterLowPowerMode;

/// @name Colorless Frames

/** Notify the delegate that the sensor made a new depth frame available to the controller.

If the data is needed beyond the scope of this method, the depthFrame or its data must be copied by the receiver, e.g. using `[STDepthFrame copy]`.

@param depthFrame The new depth frame.
*/
- (void)sensorDidOutputDepthFrame:(STDepthFrame *)depthFrame;

/** Notify the delegate that the sensor made a new infrared frame available to the controller.

If the data is needed beyond the scope of this method, the irFrame and its data must be copied by the receiver.

@param irFrame The new infrared frame.
*/
- (void)sensorDidOutputInfraredFrame:(STInfraredFrame *)irFrame;

/// @name Color-synchronized Frames

/** Notify the delegate that the sensor made a new pair of depth and color frames available to the controller.

Frame sync methods will only be used if `kSTFrameSyncEnabledKey` has been enabled in [STSensorController startStreamingWithOptions:error:] to sync frames .
Also, data will only be delivered if frameSyncNewColorBuffer: is called every time a new sample buffer is available. The driver needs the CMSampleBuffers in order to return them through these methods.
If the CMSampleBuffers are needed beyond the scope of these functions, retain them with `CFRetain(sampleBuffer)`. Make sure to pair the retain with `CFRelease(sampleBuffer)` when the data is no longer needed. If the STFrame types are needed beyond the scope of these functions, the data must be copied by the receiver.
 
See also:

- <[STSensorController startStreamingWithOptions:error:]>
- <[STSensorController frameSyncNewColorBuffer:]>

@param depthFrame The new STDepthFrame depth frame.
@param colorFrame The new STColorFrame color buffer.
*/
- (void)sensorDidOutputSynchronizedDepthFrame:(STDepthFrame *)depthFrame
                                   colorFrame:(STColorFrame *)colorFrame;

// FIXME: document this, still private
- (void)sensorDidOutputSynchronizedSensorDepthFrame:(STDepthFrame *)depthFrame
                                sensorInfraredFrame:(STInfraredFrame *)irFrame
                                 sensorVisibleFrame:(STColorFrame *)visibleFrame
                                      iosColorFrame:(STColorFrame *)iosColorFrame;

// FIXME: document this, still private
- (void)sensorDidOutputSynchronizedSensorDepthFrame:(STDepthFrame *)depthFrame
                                sensorInfraredFrame:(STInfraredFrame *)irFrame
                                 sensorVisibleFrame:(STColorFrame *)visibleFrame
                                      iosColorFrame:(STColorFrame *)iosColorFrame
                             multiCameraColorFrames:(STMultiCameraColorFrame*) multiCameraColorFrames;

// FIXME: this is private too. Handles dropping.
- (void)sensorDidDropSynchronizedSensorDepthFrame:(STDepthFrame *)depthFrame
                              sensorInfraredFrame:(STInfraredFrame *)irFrame
                               sensorVisibleFrame:(STColorFrame *)visibleFrame
                                    iosColorFrame:(STColorFrame *)iosColorFrame;

/** Notify the delegate that the sensor made a new pair of synchronized infrared and color frames available to the controller.

Frame sync methods will only be used if `kSTFrameSyncEnabledKey` has been set in [STSensorController startStreamingWithOptions:error:] to sync frames .
Also, data will only be delivered if frameSyncNewColorBuffer: is called every time a new sample buffer is available. The driver needs the CMSampleBuffers in order to return them through these methods.
If the CMSampleBuffers are needed beyond the scope of these functions, retain them with `CFRetain(sampleBuffer)`. Make sure to pair the retain with `CFRelease(sampleBuffer)` when the data is no longer needed. If the STFrame types are needed beyond the scope of these functions, the data must be copied by the receiver.
 
See also:

- <[STSensorController startStreamingWithOptions:error:]>
- <[STSensorController frameSyncNewColorBuffer:]>

@param irFrame The new STInfraredFrame infrared frame.
@param colorFrame The STColorFrame new color buffer.
*/
- (void)sensorDidOutputSynchronizedInfraredFrame:(STInfraredFrame *)irFrame
                                      colorFrame:(STColorFrame *)colorFrame;

/// Notify the delegate that the driver had to drop frames because processing was too slow.
- (void)sensorDidDropDepthFrame:(STDepthFrame*)depthFrame infraredFrame:(STInfraredFrame*)irFrame colorFrame:(STColorFrame*)colorFrame;

/**
 * Notify the delegate of a new color frame as soon as it is received
 * (un-synchronized). This is the lowest latency notification of a new color
 * frame possible.
 *
 * @param colorFrame The new color frame.
 */
- (void)sensorDidOutputColorFrame:(STColorFrame*)colorFrame;

/**
 * Notify the delegate of a new visible frame. This delegate will only be called
 * if FrameSync is disabled.
 *
 * @param visibleFrame The new visible frame.
 */
- (void)sensorDidOutputVisibleFrame:(STColorFrame*)visibleFrame;

@end

//------------------------------------------------------------------------------
#pragma mark - Sensor Controller

/** The sensor controller is the central point that manages all the interactions between the sensor and your application-specific delegate class.

Its only instance is available through the sharedController method.

Your custom delegate object can be registered using its delegate property.

See also:

- <STSensorControllerDelegate>
*/
ST_API
@interface STSensorController : NSObject

/// @name Controller Setup

/**
The STSensorController singleton.

Use it to register your application-specific STSensorControllerDelegate delegate.
*/
+ (STSensorController *)sharedController;

/**
The STSensorControllerDelegate delegate.

It will receive all notifications from the sensor, as well as raw stream data.
*/
@property(nonatomic, assign) id<STSensorControllerDelegate> delegate;

/**
 * Customizable queue on which sensor callbacks are received.
 */
@property(nonatomic, retain, readwrite) dispatch_queue_t callbackQueue;

/**
 * Check whether the sensor charger is plugged in.
 */
@property(nonatomic) BOOL isChargerConnected;

/**
Attempt to connect to the Structure Sensor.

@return Connection has succeeded only if the STSensorControllerInitStatus return value is either one of:

- STSensorControllerInitStatusSuccess
- STSensorControllerInitStatusAlreadyInitialized

@note Many methods (including startStreamingWithOptions:error:) will have no effect until this method succeeds at initializing the sensor.
*/
- (STSensorControllerInitStatus)initializeSensorConnection;

/**
This will begin streaming data from the sensor and delivering data using the delegate methods.

Here is some sample code to get registered QVGA depth with depth/color frame sync:

    NSError* error;
    BOOL success = [sensorController
        startStreamingWithOptions:@{
            kSTStreamConfigKey:@(STStreamConfigRegisteredDepth320x240),
            kSTFrameSyncConfigKey:@(STFrameSyncDepthAndRgb)
        }
        error:&error
    ];

Here is some sample code to get VGA infrared images with high gain:

    NSError* error;
    BOOL success = [sensorController
        startStreamingWithOptions:@{
            kSTStreamConfigKey:@(STStreamConfigInfrared640x488),
            kSTHighGainEnabledKey:@YES
        }
        error:&error
    ];
 
@param options Specifies the sensor configuration to use. The valid dictionary keys are:

- `kSTStreamConfigKey`:
  - Specifies the desired streaming configuration.
  - `NSNumber` integral value equal to one of the STStreamConfig constants.
  - Required.
- `kSTFrameSyncEnabledKey`:
  - Specifies whether frame sync should be active.
  - `NSNumber` integral value equal to one of the STFrameSyncConfig constants.
  - Defaults to `NO`.
  - When active, depth frame will be synchronized with the color frames from AVFoundation.
- `kSTHoleFilterEnabledKey`:
  - Specifies whether to enable depth frame hole filtering.
  - `NSNumber` boolean value.
  - Defaults to `@YES` if the stream configuration includes depth.
  - When active, it enables a dilation filter over depth values that has the effect of filling holes.
  - Setting it to `@YES` if the stream configuration does not include depth is invalid.
- `kSTHighGainEnabledKey`:
  - Specifies whether to enable high gain mode.
  - `NSNumber` boolean value.
  - Defaults to `@NO`.
  - When set to `@YES` the sensor gain will be increased, causing better performance in dark or far away objects at the expense of some bright nearby objects.
  - See also: setHighGainEnabled:.
- `kSTColorCameraFixedLensPositionKey`:
  - Specifies a fixed lens position for the color camera.
  - `NSNumber` floating point value.
  - Required when using a registered depth mode.
  - The focus of the color camera has to remain fixed to this value when using a registered depth mode.
- `kSTColorCameraAVIntrinsicsEnabledIfAvailableKey`:
  - Set AV Intrinsics use for camera calibration. Does not replace calibration from Calibrator App
  - `BOOL` value, true by default.
 
@param error will contain detailed information if the provided options are incorrect.

@return `YES` if the streaming options combination is valid, `NO` otherwise, filling in `error` with a detailed message.
*/
- (BOOL)startStreamingWithOptions:(NSDictionary *)options error:(NSError * __autoreleasing *)error;

/** Stop streaming data from the sensor.

@warning After this method is called, pending frames may still be delivered.
*/
- (void)stopStreaming;

/** Give the driver a color frame that will be used to synchronize shutters between the iOS camera and the Structure Sensor camera.

When receiving the CMSampleBufferRef from AVFoundation, you should only call this one method and do no other processing.
When a synchronized frame is found, one of the optional synchronized STSensorController delegate methods will be called, at which point you can then process/render the sampleBuffer.
 
@param sampleBuffer The new color buffer. __Note__: the supported resolutions are 640x480, 2048x1536, 2592x1936, 3264x2448 and 4032x3024.
*/
- (void)frameSyncNewColorBuffer:(CMSampleBufferRef)sampleBuffer;

/// @name Sensor Status

/// Check whether the controlled sensor is connected.
- (BOOL)isConnected;

/// Check whether the controlled sensor is in low-power mode. Low-power mode is used when the sensor is in an idle state, and is entered when the Structure Sensor is out of battery.
- (BOOL)isLowPower;

/// Return an integer in 0..100 representing the battery charge.
- (int)getBatteryChargePercentage;

/// @name Sensor Information

/// Return the name of the controlled sensor, or nil if no sensor is connected.
- (NSString *)getName;

/// Return the serial number of the controlled sensor, or nil if no sensor is connected.
- (NSString *)getSerialNumber;

/// Return the firmware revision of the controlled sensor, or nil if no sensor is connected.
- (NSString *)getFirmwareRevision;

/// Return the hardware revision of the controlled sensor, or nil if no sensor is connected.
- (NSString *)getHardwareRevision;

/** Launch the Calibrator app or prompt the user to install it.

An iOS app using the Structure Sensor should present its users with an opportunity to call this method when the following conditions are simultaneously met:

 - The sensor doesn't have a `calibrationType` with value `CalibrationTypeDeviceSpecific`.
 - A registered depth stream is required by the application.
 - The iOS device is supported by the Calibrator app.

@warning For this method to function, your app bundle's info plist must contain the following entry:

    <key>LSApplicationQueriesSchemes</key>
    <array>
        <string>structure-sensor-calibrator</string>
    </array>

@note See the calibration overlay sample code for more details.

*/
+ (BOOL)launchCalibratorAppOrGoToAppStore;

/** Return a boolean indicating whether an (at least) approximate depth-color calibration will be available when the sensor is connected to the current device.

This can be used to decide whether it is relevant to show color-specific UI elements before a sensor is connected.
*/
+ (BOOL)approximateCalibrationGuaranteedForDevice;

/** Returns the type of the current depth-color calibration as a STCalibrationType enumeration value.

This value can change depending on the actual device-sensor combination in use.

@warning This method will throw an exception when called while no sensor is connected.
*/
- (STCalibrationType)calibrationType;

/// @name Advanced Setup

/** Enable or disable high sensor gain after the stream was started.

This method can dynamically overwrite the `kSTHighGainEnabledKey` option specified when startStreamingWithOptions:error: was called.
 
@param enabled When set to `YES`, the sensor gain will be increased, causing better performance in dark or far away objects at the expense of some bright nearby objects.
*/
- (void)setHighGainEnabled:(BOOL)enabled;

/**
Specify a new rigid body transformation (RBT) between the iOS camera and the Structure Sensor camera. This transformation will be used by the SDK to associate depth pixels to color pixels. When using a registered streaming mode, depth images will be pre-warped to match the iOS camera viewpoint using this transform too.

You should only use this method if you are using a custom bracket on an unsupported device. Otherwise we strongly recommend using the Calibrator program.

A stream stop and restart will be required for this to take effect on registered depth images. The new transform will be lost after the sensor did disconnect, so it needs to be called again if the sensor got unplugged or after switching applications.

The RBT represents the world motion of the RGB camera w.r.t. the Structure Sensor. The coordinate frame is right handed: X right, Y down, Z out. Equivalently, it can transform a 3D point expressed in the iOS color camera coordinate system to the Structure Sensor camera coordinate system, which is aligned with the infrared camera.

@param newMatrix4x4 This parameter is expected as a pointer to 16 floating point values in _column_ major order. This is the default ordering of GLKMatrix4.

@note The following is an example call of this method using the Eigen C++ library (not required).
Eigen is column major by default, so the `data()` function of an `Isometry3f` can be used directly.

    - (void)updateRegistration
    {
        [[STSensorController sharedController] stopStreaming];
        
        OCIsometry3f sampleIsometry = OCIsometry3f::Identity();
        Eigen::Vector3f translation = Eigen::Vector3f(0.034, 0, 0.017);

        sampleIsometry.translate(translation);
        sampleIsometry.rotate((Eigen::Matrix3f() << 0.99977, -0.0210634, -0.00412405,
                                                   0.0210795, 0.99977, 0.00391278,
                                                   0.00404069, -0.00399881, 0.999984).finished());

        [[STSensorController sharedController] setColorCameraPoseInSensorCoordinateFrame:sampleIsometry.data()];

        [[STSensorController sharedController] startStreamingWithOptions:@{kSTStreamConfigKey: @(STStreamConfigRegisteredDepth320x240)} 
                                                                   error:nil];
    }
*/
// Removed API: - (void)setColorCameraPoseInSensorCoordinateFrame:(float *)newMatrix4x4;

/** Retrieve the current 4x4 transformation in _column_ major order between the iOS color camera and the Structure Sensor camera.

@param matrix4x4 This output parameter should be a pointer to 16 floating point values in _column_ major order. This is the default ordering of GLKMatrix4.
*/
- (void)colorCameraPoseInSensorCoordinateFrame:(float *)matrix4x4;

+ (NSString*)sdkVersion __deprecated_msg("use [NSString stringWithUTF8String:currentSDKVersion(void).version] instead");

@end

//------------------------------------------------------------------------------
#pragma mark - STGLTextureShaderRGBA

/// STGLTextureShaderRGBA instances can render OpenGL ES 2D textures in RGBA format.
ST_API
@interface STGLTextureShaderRGBA : NSObject

/// Enable the underlying shader program.
- (void)useShaderProgram;

/** Render the texture on a fullscreen quad using the given GL_TEXTUREX unit.

@param textureUnit A given native OpenGL texture unit to render.
*/
- (void)renderTexture:(GLint)textureUnit;

/** Render texture with custom quad vertices using the given GL_TEXTUREX unit.
 @param textureUnit A given native OpenGL texture unit to render.
 @param vertexPositions Array of length 8 containing x,y positions of corners in NDC.
    default is (-1.0, 1.0,) (-1.0, -1.0), (1.0, 1.0), (1.0, -1.0)

    FIXME: private API.
 */
- (void)renderTexture:(GLint)textureUnit
         withVertices:(GLfloat*)vertexPositions;

@property (nonatomic) BOOL flipY;
@property (nonatomic) float alpha;

@end

//------------------------------------------------------------------------------
#pragma mark - STGLTextureShaderYCbCr

/// STGLTextureShaderYCbCr instances can render OpenGL ES 2D textures in YCbCr format.
ST_API
@interface STGLTextureShaderYCbCr : NSObject

/// Enable the underlying shader program.
- (void)useShaderProgram;

/// Enable the underlying shader program and undistort the YCbCr texture.
- (void) useShaderProgramWithUndistortion:(STIntrinsics*)stIntrinsics;

/** Render the texture on a fullscreen quad using the given GL_TEXTUREX unit.

@param lumaTextureUnit A given native OpenGL luminance texture unit to render.
@param chromaTextureUnit A given native OpenGL chroma texture unit to render.
*/
- (void)renderWithLumaTexture:(GLint)lumaTextureUnit chromaTexture:(GLint)chromaTextureUnit;

@end

//------------------------------------------------------------------------------
#pragma mark - STGLTextureShaderGray

/// STGLTextureShaderGray instances can render OpenGL ES 2D textures in single-channel luminance format.
ST_API
@interface STGLTextureShaderGray : NSObject

/// Enable the underlying shader program.
- (void)useShaderProgram;

/** Render the texture on a fullscreen quad using the given GL_TEXTUREX unit.

@param lumaTextureUnit A given native OpenGL texture unit to render.
*/
- (void)renderWithLumaTexture:(GLint)lumaTextureUnit;

@end

//------------------------------------------------------------------------------
#pragma mark - STDepthToRgba

/**
Constants specifying a depth to color conversion strategy.

See also:
 
- [STDepthToRgba initWithOptions:error:]
*/
typedef NS_ENUM(NSInteger, STDepthToRgbaStrategy)
{
    /// Linear mapping using a color gradient – pure red encodes the minimal depth, and pure blue encodes the farthest possible depth.
    STDepthToRgbaStrategyRedToBlueGradient = 0,
    
    /// Linear mapping from closest to farthest depth as a grayscale intensity.
    STDepthToRgbaStrategyGray,
};

// Dictionary keys for `[STDepthToRgba initWithOptions:]`.
extern NSString* const kSTDepthToRgbaStrategyKey;

// Default will be full range.
// FIXME: document this if we keep them.
extern NSString* const kSTDepthToRgbaMinDepthKey;
extern NSString* const kSTDepthToRgbaMaxDepthKey;

/// Helper class to convert float depth data to RGB values for better visualization.
ST_API
@interface STDepthToRgba : NSObject

/// Pointer to the RGBA values.
@property (nonatomic, readonly) uint8_t *rgbaBuffer;

/// Width of the output RGBA image.
@property (nonatomic, readonly) int width;

/// Height of the output RGBA image.
@property (nonatomic, readonly) int height;

/** Initialize with required parameters.

@param options The options dictionary. The valid keys are:
 
 - `kSTDepthToRgbaStrategyKey`:
   - Specifies the conversion strategy to use.
   - `NSNumber` integral value equal to one of the STDepthToRgbaStrategy constants.
   - Required.
*/
- (instancetype)initWithOptions:(NSDictionary *)options;

/** Convert the given depth frame to RGBA. The returned pointer is the same as the one accessed through `rgbaBuffer`.

@param frame The depth frame.
*/
- (uint8_t *)convertDepthFrameToRgba:(STDepthFrame *)frame;

@end

//------------------------------------------------------------------------------
#pragma mark - STWirelessLog

/** The STWirelessLog class is a wireless logging utility.

This class gives the ability to wirelessly send log messages to a remote console.

It is very useful when the sensor is occupying the lightning port.
*/
ST_API
@interface STWirelessLog : NSObject

/**  Redirect the standard and error outputs to a TCP connection.

Messages sent to the `stdout` and `stderr` (such as `NSLog`, `std::cout`, `std::cerr`, `printf`) will be sent to the given IPv4 address on the specified port.

In order to receive these messages on a remote machine, you can, for instance, use the *netcat* command-line utility (available by default on Mac OS X). Simply run in a terminal: `nc -lk <port>`

@param ipv4Address The IPv4 address of the remote console.
@param port The port of the remote console.
@param error will contain detailed information if the provided options are incorrect.

@note If the connection fails, the returned error will be non-`nil` and no output will be transmitted.
@note Only one connection can be active.
*/
+ (void)broadcastLogsToWirelessConsoleAtAddress:(NSString *)ipv4Address usingPort:(int)port error:(NSError **)error;

@end

//------------------------------------------------------------------------------
#pragma mark - STBackgroundTaskDelegate

@class STBackgroundTask;

/** STBackgroundTaskDelegate is a delegate protocol that your class can implement in order to receive STBackgroundTask callbacks.

See also:

- <[STBackgroundTask delegate]>
*/
ST_API
@protocol STBackgroundTaskDelegate <NSObject>
@optional

/** Report progress in the background task.

@param sender The STBackgroundTask that reports the progress.
@param progress is a floating-point value from 0.0 (Not Started) to 1.0 (Completed).
*/
- (void)backgroundTask:(STBackgroundTask*)sender didUpdateProgress:(double)progress;

@end

//------------------------------------------------------------------------------
#pragma mark - STBackgroundTask

/** STBackgroundTask instances enable control of tasks running asynchronously in a background queue.

See also:

- <[STMesh newDecimateTaskWithMesh:numFaces:completionHandler:]>
- <[STMesh newFillHolesTaskWithMesh:completionHandler:]>
- <[STColorizer newColorizeTaskWithMesh:scene:keyframes:completionHandler:options:error:]>
- STBackgroundTaskDelegate
*/
ST_API
@interface STBackgroundTask : NSObject

/// Start the execution of the task asynchronously, in a background queue.
- (void)start;

/** Cancel the background task if possible.
 
@note If the operation is already near completion, the completion handler may still be called successfully.
*/
- (void)cancel;

/// Synchronously wait until the task finishes its execution.
- (void)waitUntilCompletion;

/// Whether the task was canceled. You can check this in the completion handler to make sure the task was not canceled right after it finished.
@property(atomic, readonly) BOOL isCancelled;

/// By setting a STBackgroundTaskDelegate delegate to an STBackgroundTask, you can receive progress updates.
@property(atomic, assign) id<STBackgroundTaskDelegate> delegate;

@end

//------------------------------------------------------------------------------
#pragma mark - STErrorCode

/// String constant to identify the Structure Framework error domain.
extern NSString* const StructureSDKErrorDomain;

/** Error codes that specify an error.

These may appear in NSError objects returned by various Structure Framework methods.

`NSString* const StructureSDKErrorDomain;` is the constant to identify the Structure Framework error domain.
*/
typedef NS_ENUM(NSInteger, STErrorCode)
{
/// The operation could not be completed because parameters passed to the SDK method contains non-valid values.
    STErrorInvalidValue                            = 10,
    
/// STCameraPoseInitializer tried to initialize a camera pose using without a depth frame.
    STErrorCameraPoseInitializerDepthFrameMissing  = 20,

/// The operation could not be completed because file output path is invalid.
    STErrorFileWriteInvalidFileName                = 31,

/// `STTracker` is not initialized yet.
    STErrorTrackerNotInitialized                   = 41,
    
/// `STTracker` detected the color sample buffer exposure has changed.
    STErrorTrackerColorExposureTimeChanged         = 44,
    
/// `STTracker` doesn't have device motion and cannot continue tracking.
    STErrorTrackerDeviceMotionMissing              = 45,
    
/// The STMesh operation could not be completed because it contains an empty mesh.
    STErrorMeshEmpty                               = 60,
    
/// The STMesh operation could not be completed because it was cancelled.
    STErrorMeshTaskCancelled                       = 61,
    
/// The STMesh operation could not be completed because it contains an invalid texture format.
    STErrorMeshInvalidTextureFormat                = 62,
    
/// The colorize operation could not be completed because `STColorizer` doesn't have keyframes.
    STErrorColorizerNoKeyframes                    = 80,
    
/// The colorize operation could not be completed because `STColorizer` doesn't have a mesh.
    STErrorColorizerEmptyMesh                      = 81,
};
