//
//  StructureConfig.h
//  Structure
//
//  Created by Nicolas Burrus on 14/08/15.
//  Copyright © 2015 Occipital. All rights reserved.
//

#pragma once

// Global options of things we can enable. FIXME: these should probably go in a separate header
// to avoid confusion.

/*

 Documentation of these defines.
 
 STRUCTURE_SDK_BUILD is ONLY set when building a Structure SDK binary.
 It is actually set in the build_SDK.sh script, so it is NOT set when building
 an app embedding arcturus. We mainly use it to make sure code used only in apps
 don't end up in the SDK binary to keep the size small and avoid obfuscating code
 that does not need to be shipped yet. When building the Bridge Engine SDK, it
 is NOT set either.
 
 BRIDGE_ENGINE_SDK_BUILD is ONLY set when building a BridgeEnginePrivate binary
 targetted for bridge-system. It is set in the build-partners-package.sh script.
 We use it to avoid including stuff that won't be used the a BridgeEngine release.
 When building BEDemo or other apps in arcturus, it will NOT be set. Only the script
 sets it.

 STRUCTURE_PARTNERS_BUILD is ONLY set when building a StructurePartners binary, and
 it is very similar in use to BRIDGE_ENGINE_SDK_BUILD.

 RELEASE_PUBLIC is useful to detect public Release builds, which includes SDK builds
 of Structure SDK / Bridge Engine, as well as publicly-shipped apps. Public releases use
 the ReleasePublic Xcode build configuration and include obfuscation and have many features
 disabled for security. Note that Release builds without RELEASE_PUBLIC are also obfuscated.
 
 STRUCTURE_SDK_INTERNAL_BUILD is useful to experiment with new Structure SDK features
 without making it public yet (e.g. to build an experimental version of the SDK to
 share with selected partners). Setting this when building an SDK binary
 will enable these features (option --internal-build in structure-sdk/build_SDK.sh).
 NOTE: this also gets set automatically when NOT building an SDK (i.e. building an app), 
 since our internal apps may want to have access the private API. Please follow the best
 practices described below before using it though, and avoid using STRUCTURE_SDK_INTERNAL_BUILD
 directly in code.

 RELEASE_WITH_LOGS is useful to build a release with oc_dbg, oc_info, oc_warn, and oc_error, and 
 all public logs enabled. This macro does not have any effect when building in debug. It only triggers
 the appropriate feature macros when we are in release. 
 NOTE: There are certain debugs we never want exposed even when building a release with logs. Those 
 statements can be compiled out in release by checking the macro ARCTURUS_ENABLE_L1_LOGS_IN_RELEASE
 around your debug statement. That macro is set to true only if !DEBUG && RELEASE_WITH_LOGS is true.
 If we were to only use RELEASE_WTIH_LOGS to check those conditions there is the possibility that 
 someone will build in debug and accidentaly leave RELEASE_WITH_LOGS on, effectively compiling out 
 logs that they want to see when debugging. This follows the best practices paradigm listed below. 

 BEST PRACTICES: if you want to conditionally build some chunk of code:

     1. Add a new ARCTURUS_ENABLE_XXXXX define for it

     2. In this StructureConfig.h file, define it or not depending on the variables
     above like STRUCTURE_SDK_BUILD, etc.

     This is better than directly using variables like STRUCTURE_SDK_BUILD
     in the code, since it enables a finer control. We still have lots of
     STRUCTURE_SDK_BUILD occurences in the code, but this is for historical
     purposes, initially that was the only case we supported.
 
*/

// Uncomment this to temporarily make a test SDK build.
// #define STRUCTURE_SDK_BUILD 1
// #define BRIDGE_ENGINE_SDK_BUILD 1

#if __APPLE__
# include <TargetConditionals.h>
#endif

// IMPORTANT: check the documentation at the top of the header before messing up with these.

// Uncomment these to test an SDK build.
// #define STRUCTURE_SDK_BUILD 1
// #define RELEASE_PUBLIC 1
// #define STRUCTURE_SDK_INTERNAL_BUILD 0

// Uncomment this or define it through the command line to get sparse timings in Release.
// #define ARCTURUS_PROFILE_HTC_TRACKER_SPARSE_IN_RELEASE 1

#if ARCTURUS_PROFILE_HTC_TRACKER_SPARSE_IN_RELEASE
# define ST_NEVER_OBFUSCATE 1
# define OC_FORCE_AVERAGE_PROFILING 1
# define RELEASE_WITH_LOGS 1
#endif

#if DEBUG
// Enable logs if we are in debug
#   define ARCTURUS_ENABLE_DEBUG_LOGGING 1

#elif !DEBUG && RELEASE_WITH_LOGS
/* Enable logs in release if we explicitly ask for it
*  Note: See justification for using this condition to trigger
*        feature macros at the top of this file. 
*/
#   define ARCTURUS_ENABLE_L1_LOGS_IN_RELEASE 1

#else
// Otherwise, logging should be entirely disabled 
#   define ARCTURUS_ENABLE_DEBUG_LOGGING 0
#endif


// Structure App specific settings. Keep this enabled only in the structure-app release branches.
// #define STRUCTURE_APP_RELEASE_BUILD 1

// IMPORTANT: check the documentation at the top of the header before messing up with these.
#if (STRUCTURE_SDK_BUILD) && !RELEASE_PUBLIC
#   error "We shouldn't be building an SDK and have RELEASE_PUBLIC set false. This will include unreleased features."
#endif

#if STRUCTURE_APP_RELEASE_BUILD && !RELEASE_PUBLIC
#   error "We shouldn't be building a public app and have RELEASE_PUBLIC set false. This will include unreleased features."
#endif

// IMPORTANT: check the documentation at the top of the header before messing up with these.

#pragma mark - High level flavors

#if STRUCTURE_SDK_BUILD
#   define ARCTURUS_ENABLE_MANIFOLDS 1 // Strictly for shift trackers that leverage manifolds
#   define ARCTURUS_ENABLE_OPENCV 1
// nothing extra here.

#elif BRIDGE_ENGINE_SDK_BUILD
#   define ARCTURUS_ENABLE_PARTNERS_API 1
#   define STRUCTURE_SDK_INTERNAL_BUILD 1 // always enable internal SDK features for apps
#   define ARCTURUS_ENABLE_LARGE_SCALE_SLAM 1
#   define ARCTURUS_ENABLE_SIMPLE_TRACKER_MAPPER_SLAM 1 // Bridge Engine needs it.
#   define ARCTURUS_ENABLE_RGBONLY_TRACKER 1
#   define ARCTURUS_ENABLE_CONTROLLER_TRACKER 1
#   define ARCTURUS_ENABLE_MANIFOLDS 1
#   define ARCTURUS_ENABLE_OPENCV 1
#   define ARCTURUS_ENABLE_METAL_MESH_DATA 1

#elif RELEASE_PUBLIC || STRUCTURE_PARTNERS_BUILD // settings for other apps in ReleasePublic and StructurePartners for Canvas
#   define ARCTURUS_ENABLE_PARTNERS_API 1
#   define STRUCTURE_SDK_INTERNAL_BUILD 1 // always enable internal SDK features for apps
#   define ARCTURUS_ENABLE_LARGE_SCALE_SLAM 1
#   define ARCTURUS_ENABLE_SIMPLE_TRACKER_MAPPER_SLAM 1 // Bridge Engine needs it.
#   define ARCTURUS_ENABLE_RGBONLY_TRACKER 1
#   define ARCTURUS_ENABLE_CONTROLLER_TRACKER 1
#   define ARCTURUS_ENABLE_MANIFOLDS 1
#   define ARCTURUS_ENABLE_PRIMITIVE_DETECTION 1
#   define ARCTURUS_ENABLE_OPENCV 1
// To be able to release Canvas + SparseTracker
#   ifndef ARCTURUS_ENABLE_SPARSE_TRACKER
#       define ARCTURUS_ENABLE_SPARSE_TRACKER 1
#   endif
#   if !DEBUG // Prevent from enabling the visual logger in release, if you really need it comment this block
#       define OC_VISUAL_LOGGER_ENABLED 0       // this will cause the compiler to fail if we enable it by mistake
#       define OC_FORCE_VISUAL_LOGGER_ENABLED 0 // same here.
#   endif

#else // settings for internal releases.
#   define ARCTURUS_ENABLE_UNRELEASED_FEATURES 1 // Catch-all for a variety of internal-only code
#   define OC_FORCE_VISUAL_LOGGER_ENABLED 1 // Force VISUAL_LOGGER on for internal builds
#   define ARCTURUS_ENABLE_PARTNERS_API 1
#   define STRUCTURE_SDK_INTERNAL_BUILD 1
// Enable this if you want to use the Leica Displacement Sensor
//#   define ARCTURUS_ENABLE_BLUETOOTH_ACCESSORIES 1
#   define ARCTURUS_ENABLE_OPENCV 1

#   define ARCTURUS_ENABLE_LARGE_SCALE_SLAM 1
#   define ARCTURUS_ENABLE_SIMPLE_TRACKER_MAPPER_SLAM 1 // Bridge Engine needs it.
#   define ARCTURUS_ENABLE_RGBONLY_TRACKER 1 // Now enabling color-only tracking by default, because the Bridge Engine exposes it.
#   define ARCTURUS_ENABLE_CONTROLLER_TRACKER 1
#   define ARCTURUS_ENABLE_MANIFOLDS 1
#   define ARCTURUS_ENABLE_PRIMITIVE_DETECTION 1

    // Sparse SLAM is not enabled in any released product. It also is only available in some platforms,
    // so we must honor what the build scripts may already have specified
#   ifndef ARCTURUS_ENABLE_SPARSE_TRACKER
#       define ARCTURUS_ENABLE_SPARSE_TRACKER 1
#   endif

    // LSD SLAM only enabled on macOS, unless specifically disabled.
#   if !defined(ARCTURUS_ENABLE_LSD_SLAM) && TARGET_OS_MAC && !TARGET_OS_IPHONE
#       define ARCTURUS_ENABLE_LSD_SLAM 0 // keeping it disabled by default to save on build times.
#   endif

    // DSO SLAM
#   if !defined(ARCTURUS_ENABLE_DSO) && TARGET_OS_MAC && !TARGET_OS_IPHONE
#       define ARCTURUS_ENABLE_DSO 0
#   endif
#endif

#pragma mark - Automatically deduced variables

#if ARCTURUS_ENABLE_LARGE_SCALE_SLAM
#   define ARCTURUS_ENABLE_MAPPER_SPARSE_32i 1
#   define ARCTURUS_ENABLE_RELOCALIZER 1 // Required by ARCTURUS_ENABLE_LARGE_SCALE_SLAM.
#   define ARCTURUS_ENABLE_MANIFOLDS 1
#endif

// Only enable Ceres if enabling the sparse tracker or if we're on macOS for pose alignment stuff.
#if ARCTURUS_ENABLE_SPARSE_TRACKER || (TARGET_OS_MAC && !TARGET_OS_IPHONE)
#   define ARCTURUS_ENABLE_CERES 1
#else
#   define ARCTURUS_ENABLE_CERES 0
#endif

// Only enable ORB-SLAM2 compilation if specifically requested. Infrequently used and slow
// to compile.
#ifndef ARCTURUS_ENABLE_ORB_SLAM2
#   define ARCTURUS_ENABLE_ORB_SLAM2 0
#endif

#if ARCTURUS_ENABLE_PRIMITIVE_DETECTION && ARCTURUS_ENABLE_SPARSE_TRACKER
# define ARCTURUS_ENABLE_ROOM_LAYOUT 1
#endif

// CMake builds will define this flag.
#ifndef ARCTURUS_ENABLE_LINK_ZMQ
#  if __APPLE__ || (_WIN32 && BUILD_OPENVR_SOURCES)
#    define ARCTURUS_ENABLE_LINK_ZMQ 1
#  endif
#endif

// Uncomment this to temporarily make release builds fail is there are some unsolved oc_fixme_before_release
// By default we just issue a build message, and release scripts should check their existence and validate them
// with the user.
// #define OC_FIXME_BEFORE_RELEASE_FORCE_BUILD_ERROR_IN_RELEASE 1

// IMPORTANT: check the documentation at the top of the header before messing up with these.
// Use this to make a symbol always available to partners.
#if ARCTURUS_ENABLE_PARTNERS_API
#   if _WIN32
        // FIXME: there is no support for a public partners API right now. We'd need to make sure that
        // Structure.dll re-exports the symbols built in Core, SLAM, etc.
#       define ST_PARTNERS_API
#   else
#      define ST_PARTNERS_API __attribute__((visibility("default")))
#   endif
#else
#   define ST_PARTNERS_API
#endif

// Note: we do NOT define OC_GLES on Windows nor macOS, since they don't use GLES.
#if ANDROID
#   define OC_GLES_MAJOR_VERSION 3  // We're using OpenGL ES 3.1 on Android by default.
#   define OC_GLES_MINOR_VERSION 2
#elif __APPLE__
#   include "TargetConditionals.h"
#   if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#         define OC_GLES_MAJOR_VERSION 2
#   elif TARGET_OS_MAC
#     define OC_GL_DESKTOP 1
#   endif
#else
#  define OC_GL_DESKTOP 1
#endif

// Force enabling profiling in Release mode
// #define OC_FORCE_AVERAGE_PROFILING 1

// Hack for OCCFileConverter.
// #define ARCTURUS_LOSSLESS_OCC_JPEG_CONVERSION 1

// LP64 usage here is okay, since it's for apple devices only
// and we dont have access to SYS_64bit here
#if !defined(OCALGEBRA_ENABLE_SIMD) && defined(__APPLE__) && __LP64__
# define OCALGEBRA_ENABLE_SIMD 1
#endif
