//
//  StructureInfo.h
//  Structure
//
//  Created by Forrest Heller on 12/4/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

#include "../Updates/FirmwareSignature.h"

#ifdef __cplusplus
#   include <cstring>
#endif

typedef enum {
    MainPCB_SP2 = 1,
    MainPCB_3_3,
    MainPCB_NewerThanKnown, // it's not unknown, but is newer than anything we know about when this was compiled
    MainPCB_Unknown = 0xf // corresponds to uninitialized EEPROM
} MainPCB;

typedef enum {
    DaughterPCB_SP2 = 1,
    DaughterPCB_3_3,
    DaughterPCB_3_4,
    DaughterPCB_NewerThanKnown, // it's not exactly unknown, but is newer than what we know about when this was compiled
    DaughterPCB_Unknown = 0xf // corresponds to uninitialized EEPROM
} DaughterPCB;

typedef enum {
    ChassisColor_Silver = 1,
    ChassisColor_IceBlue,
    ChassisColor_DarkGray,
    ChassisColor_NewerThanKnown, // it's not exactly unkonwn, but is newer than anything we know about when this was compiled
    ChassisColor_Unknown = 0xf // corresponds to uninitialized EEPROM
} ChassisColor;

#define RECORD_VERSION_UNKNOWN 0xf

struct HardwareInfo {
    int recordVersion; // 0 = too low for reality, 0xf = unknown
    MainPCB mainPCB;
    DaughterPCB daughterPCB;
    ChassisColor chassisColor;
};

inline bool hardwareInfoIsValid (const HardwareInfo* hwinfo)
{
    return hwinfo->recordVersion != RECORD_VERSION_UNKNOWN
        && hwinfo->mainPCB       != MainPCB_Unknown
        && hwinfo->daughterPCB   != DaughterPCB_Unknown
        && hwinfo->chassisColor  != ChassisColor_Unknown
        ;
}

#ifdef __cplusplus
inline bool operator == (const HardwareInfo& first, const HardwareInfo& second) {
    return first.recordVersion == second.recordVersion &&
    first.mainPCB == second.mainPCB &&
    first.daughterPCB == second.daughterPCB &&
    first.chassisColor == second.chassisColor;
}

inline bool operator != (const HardwareInfo& first, const HardwareInfo& second) {
    return !(first == second);
}

inline void clear (HardwareInfo& hardwareInfo) {
    hardwareInfo.recordVersion = 0;
    hardwareInfo.daughterPCB = DaughterPCB_Unknown;
    hardwareInfo.mainPCB = MainPCB_Unknown;
    hardwareInfo.chassisColor = ChassisColor_Unknown;
}
#endif

/* Holds the output of reading the XMEGA_REG_DEVICE_ID I2C register */
struct XmegaDeviceID {
    uint8_t structureHardwareVersion; /* 4 = SP3, 3 = SP2 */
    /* Hardware registers on the xmega signalling chip version */
    uint8_t xmegaMCUDEVID1;
    uint8_t xmegaMCUDEVID2;
    uint8_t xmegaMCUREVID;
};

#ifdef __cplusplus
inline void clear (XmegaDeviceID& xmegaDeviceId)
{
    xmegaDeviceId.structureHardwareVersion = 0;
    xmegaDeviceId.xmegaMCUDEVID1 = 0;
    xmegaDeviceId.xmegaMCUDEVID2 = 0;
    xmegaDeviceId.xmegaMCUREVID = 0;
}

inline bool operator == (const XmegaDeviceID& first, const XmegaDeviceID& second)
{
    return true
        && first.structureHardwareVersion == second.structureHardwareVersion
        && first.xmegaMCUDEVID1           == second.xmegaMCUDEVID1
        && first.xmegaMCUDEVID2           == second.xmegaMCUDEVID2
        && first.xmegaMCUREVID            == second.xmegaMCUREVID
        ;
}
#endif

struct StructureInfo {
    /* This is the output of OPCODE_GET_VERSION, which is included in the OPCODE_GET_STRUCTURE_INFO command */
    uint16_t psFirmwareVersion;
    uint16_t psBuildVersion;
    uint32_t PS1080Version;
    uint16_t psHardwareType; /* 4 = RD1081, 5 = RD1082, etc. */
    uint16_t psSystemVersion;
    /* This is information not included the OPCODE_GET_VERSION command, but only in the OPCODE_GET_STRUCTURE_INFO command */
    struct XmegaDeviceID xmegaDeviceID;
    struct FirmwareSignature firmwareSignature;
    /* Guarunteed to be 21 characters at most, including null terminator, see firmware for comments */

    enum { NumBuildTimeBytes = 21 };

    char ps1080FirmwareBuildTime[NumBuildTimeBytes];
    char xmegaFirmwareBuildTime[NumBuildTimeBytes];
    struct HardwareInfo hardwareInfo;
    bool containsHardwareInfo;
    uint8_t hardwareInfoEEPROM[2];
    uint16_t getPS1080MajorSequenceNumber()
    {
        return (firmwareSignature.ps1080.sequence & 0xff00) >> 8 ;
    }
};

#ifdef __cplusplus
inline void clear (StructureInfo& structureInfo)
{
    structureInfo.psFirmwareVersion = 0;
    structureInfo.psBuildVersion = 0;
    structureInfo.PS1080Version = 0;
    structureInfo.psHardwareType = 0;
    structureInfo.psSystemVersion = 0;
    structureInfo.containsHardwareInfo = false;
    std::memset(structureInfo.hardwareInfoEEPROM, 0, sizeof(structureInfo.hardwareInfoEEPROM));
    clear(structureInfo.xmegaDeviceID);
    clear(structureInfo.firmwareSignature);
    clear(structureInfo.hardwareInfo);
}

inline bool operator == (const StructureInfo& first, const StructureInfo& second)
{
    return true
        && first.psFirmwareVersion       == second.psFirmwareVersion
        && first.psBuildVersion          == second.psBuildVersion
        && first.PS1080Version           == second.PS1080Version
        && first.psHardwareType          == second.psHardwareType
        && first.psSystemVersion         == second.psSystemVersion
        && first.xmegaDeviceID           == second.xmegaDeviceID
        && first.firmwareSignature       == second.firmwareSignature
        && 0 == std::memcmp(first.ps1080FirmwareBuildTime, second.ps1080FirmwareBuildTime, StructureInfo::NumBuildTimeBytes)
        && 0 == std::memcmp(first.xmegaFirmwareBuildTime , second.xmegaFirmwareBuildTime , StructureInfo::NumBuildTimeBytes)
        && first.hardwareInfo == second.hardwareInfo
        && first.containsHardwareInfo == second.containsHardwareInfo
        ;
}
#endif

/* The firmware signature is a uint32_t that holds the following information:
 -major version number (1 byte)
 -minor version number (1 byte)
 -hardware version (4 bits)
 -chip version (4 bits)
 -tag, currently unused (1 byte)
 */

/* All the different chips our firmware could be compiled for */
#define CHIP_XMEGA128A4U 0
#define CHIP_XMEGA64A4U 1

#define HARDWARE_SP2 3
#define HARDWARE_SP3 4

#define FIRMWARE_SIGNATURE_SEQUENCE(signature) ((signature & 0xffff0000UL) >> 16UL)
#define FIRMWARE_SIGNATURE_HARDWARE(signature) ((signature & 0xf000) >> 12)
#define FIRMWARE_SIGNATURE_CHIP(signature) ((signature & 0xf00) >> 8)
#define FIRMWARE_SIGNATURE_IS_XMEGA_UPDATER(signature) ((signature & 0x80) ? true : false)
#define FIRMWARE_SIGNATURE_IS_ISENSE(signature) ((signature & 0x40) ? true : false)
#define FIRMWARE_SIGNATURE_TAG(signature) (signature & 0x3f)


/**
 * oc_NSDbg's the contents structureInfo in a nice, readable way
 */
void structureInfoLog(const struct StructureInfo* structureInfo);

/**
 * Parse out the signature into a nice signatureStruct
 */
void XmegaFirmwareSignatureFill(uint32_t signature, struct XmegaFirmwareSignature* signatureStruct);

/* Objective-C is not available on Windows */
#if defined(__OBJC__) && __OBJC__

/**
 * Returns a human-readable string for the hardware of an xmega firmware signature.  Don't release the returned string.
 */
NSString* XmegaFirmwareSignatureHardwareString(uint8_t hardware);

/**
 * Returns a human-readable string for the chip of an xmega firmware signature.  Don't release the returned string.
 */
NSString* XmegaFirmwareSignatureChipString(uint8_t chip);

/**
 * returns a new string containing a human readable representation of structureInof
 */
NSString* structureInfoCreateDescription(const struct StructureInfo* structureInfo);

/**
 * returns a new string containing a human readable representation of hardwareInfo
 */
NSString* hardwareInfoCreateDescription(const struct HardwareInfo* hardwareInfo);

NSString* hardwareInfoDaughterPCBDescription(DaughterPCB daughterPCB);

NSString* hardwareInfoChassisColorDescription(ChassisColor chassisColor);

NSString* hardwareInfoMainPCBDescription(MainPCB mainPCB);

#endif
