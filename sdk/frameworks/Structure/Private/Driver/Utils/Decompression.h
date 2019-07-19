//
//  Decompression.h
//  Driver
//
//  Created by hellerf on 7/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <stdbool.h>


#define IR_PACKED_10_QVGA_SIZE (320.0f * 248.0f * 10.0f / 8.0f)
#define IR_PACKED_10_VGA_SIZE (640.0f * 488.0f * 10.0f / 8.0f)
#define IR_PACKED_10_SXGA_SIZE (1280.0f * 1024.0f * 10.0f / 8.0f)
#define IR_UNPACKED_QVGA_SIZE (320 * 248 * sizeof(uint16_t))
#define IR_UNPACKED_VGA_SIZE (640 * 488 * sizeof(uint16_t))
#define IR_UNPACKED_SXGA_SIZE (1280 * 1024 * sizeof(uint16_t))

#define DEPTH_UNCOMPRESSED_QVGA_SIZE (320 * 240 * sizeof(uint16_t))
#define DEPTH_UNCOMPRESSED_VGA_SIZE (640 * 480 * sizeof(uint16_t))



#ifdef __cplusplus
extern "C" {
#endif

int uncompressDepthPS(const uint8_t* pInput, const uint32_t nInputSize,
                      uint16_t* pOutput, uint32_t* pnOutputSize,
                      uint32_t* pnActualRead, bool bLastPart);

void unpackBitStreamTo16(const uint8_t* __restrict packedStream, size_t packedStreamSize, int bitsPerPixel, uint16_t* __restrict unpackedStream, size_t* __restrict unpackedStreamSize);


#ifdef __cplusplus
}
#endif
