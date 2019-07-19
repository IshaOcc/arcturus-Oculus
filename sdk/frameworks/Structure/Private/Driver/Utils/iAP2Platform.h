/*
 * iAP2Platform.h
 *
 * Created: 3/28/2013 12:23:45 PM
 *  Author: Occipital
 */ 

#pragma once

#if defined(AVR) && AVR /* defined by avr-gcc */

#include <stddef.h>
#include <stdint.h>
#include <string.h> // for memcpy and memcmp
#include "Time.h"
#define MEMORY_ALIGN32
#define sizeof_in_bytes(x) sizeof(x)
#include "ByteStream.h"

#include "Log.h"
#define iAP2LogDebug(...) LogDebug(__VA_ARGS__)
#define iAP2LogError(...) LogError(__VA_ARGS__)
#define iAP2LogErrorHere(errorCode) LogErrorHere(errorCode);

#ifndef MIN
#define MIN(a,b) (a <= b) ? a : b
#endif
#ifndef MAX
#define MAX(a,b) (a >= b) ? a : b
#endif
#define iAP2SendPacketBufferSize 256
#include "Macros.h"
#define PRINTF_FORMAT_UINT32 "%lu"
#define iAP2IdentificationInformationMessageSize 225

#elif defined(ZSP) && ZSP
/* ZSP does not have 8-bit types.  char is 16-bits */
#define sizeof_in_bytes(x) (sizeof(x)*2)
#include "Framework\ZSPStddefs.h"
#include "Framework\ByteStream.h"
/* Must include DriversDef.h because it may redefine printf to something else */
#include "Drivers\DriversDef.h"
#include "Framework\Log.h"

/* printfs on the ZSP take a long time, so disable them by default */
#define iAP2LogDebug(...)
#define iAP2LogError(...) printf(__VA_ARGS__)
#define iAP2LogErrorHere(errorCode) printf("error %d at line %d\n", errorCode, __LINE__);
#define OXAssert ASSERT
#define timeGetMilliseconds GetTickCount
/* TODO: account for rollover */
#define timeGetDifferenceMilliseconds(before, after) (after - before)
#define timeGetMicroseconds() GetMicroTickCount()
#define iAP2SendPacketBufferSize 256
#define PRINTF_FORMAT_UINT32 "%lu"
#define iAP2IdentificationInformationMessageSize 305

#else
/* For Windows and Mac OS X */
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#ifdef __APPLE__
#else
#include <Windows.h>
#endif
#define MEMORY_ALIGN32
#define timeGetMilliseconds GetTickCount
uint32_t timeGetDifferenceMilliseconds(uint32_t before, uint32_t after);
#define timeGetMicroseconds() (GetTickCount() * 1000)
#define sizeof_in_bytes(x) sizeof(x)
#include "ByteStream.h"

#define OXAssert assert

#ifndef iAP2LogDebug
#define iAP2LogDebug(...) printf(__VA_ARGS__)
#endif

#ifndef iAP2LogError
#define iAP2LogError(...) printf(__VA_ARGS__)
#endif

#define iAP2LogErrorHere(errorCode) iAP2LogError("error %d at line %d\n", errorCode, __LINE__)
#ifndef MIN
#define MIN(a,b) (a <= b) ? a : b
#endif
#ifndef MAX
#define MAX(a,b) (a >= b) ? a : b
#endif
#define iAP2SendPacketBufferSize 128
#define PRINTF_FORMAT_UINT32 "%u"
#define iAP2IdentificationInformationMessageSize 232
#endif

#ifndef IAP2_PLATFORM_INTERAL
extern uint16_t iAP2IdentificationInformationMessage[];
#endif

#define iAP2CertificateBufferSize 512
void iAP2PlatformInit(void);

#define SWAP_BYTES16(x) ((x << 8U) | (x >> 8U))
