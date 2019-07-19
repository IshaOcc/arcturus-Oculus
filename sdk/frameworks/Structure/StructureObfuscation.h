
#pragma once

// See global options at the end of this file.

#if !ARCTURUS
# error ARCTURUS is not defined. This most likely means your xcconfig file was not properly imported.
#endif

// Include this first to make sure ST_NEVER_OBFUSCATE gets set properly.
#include "StructureConfig.h"

#ifdef DEBUG
    #if !DEBUG
        #error DEBUG defined but zero. Many files will fail at handling this.
    #endif
#else
    // This is defined by BridgePartners since they always use a debug build.
    #if !defined(ST_NEVER_OBFUSCATE)
    # define ST_OBFUSCATE 1
    #endif
#endif

#if ST_OBFUSCATE

#   define OBFUSCATE_PROJECTIVE_TEXTURING_SHADERS 1

// Obfuscation and debug are not really compatible. In debug mode, we want the right symbols.
#   if DEBUG
#       error Obfuscation cannot be used in debug mode.
#   endif

#   define NDEBUG   1

#   define oc_exception_message(...) "Exception"
#   define iAP2LogDebug(...)
#   define iAP2LogError(...)
#   define TC_ELAPSED_MSECS(...)
#   define stream_printf(...)

// Uncomment this to simulate an SDK build without running the script, and disable obfuscation
// to analyse the list of exported symbols.
// #define  MANUALLY_ANALYZING_STRUCTURE_SDK_BUILD_SYMBOLS 1
#if MANUALLY_ANALYZING_STRUCTURE_SDK_BUILD_SYMBOLS
    #define STRUCTURE_SDK_BUILD 1
#else
    // Include the generated obfuscation directives.
    #include "StructureObfuscationGenerated.h"
#endif

#endif

