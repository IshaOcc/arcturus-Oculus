#pragma once

#ifndef __cplusplus
#include <stdint.h>
#include <stddef.h>
#else
#include <cstdint>
#include <cstddef>
#endif
/* An easy, cross-platform interface to write individual bytes to memory.  This exists because ZSP
 does not have 8-bit addressable memory.  Like, unsigned char on ZSP is 2 bytes.
 unsigned char storage[2] allocates 4 bytes. */

struct ByteStream {
	uint8_t* current;
	uint8_t* base;
};

static inline void bsInit(struct ByteStream* __restrict stream, void* newBase) {
	stream->base = (uint8_t*) newBase;
	stream->current = (uint8_t*)newBase;
}

/* The core of the ByteStream functions */
#define bsWrite(stream, newByte) *(stream)->current++ = (newByte)
#define bsRead(stream) (*(stream)->current++)
#define bsSetOffset(stream, newOffset) (stream)->current = (stream)->base + ((uintptr_t)newOffset)
#define bsGetOffset(stream) (((uintptr_t)(stream)->current) - ((uintptr_t)(stream)->base))
#define bsGetBase(stream) (stream)->base
#define bsSkip(stream, amount) (stream)->current += amount
#define bsRewind(stream, amount) (stream)->current -= amount


#ifdef __cplusplus
extern "C" {
#endif

void bsCopy(struct ByteStream* destinationStream, struct ByteStream* sourceStream, size_t bytesCount);
void bsMemcpy(struct ByteStream* destinationStream, const void* sourceData, size_t byteCount);
void bsMemRead(void* destinationData, struct ByteStream* sourceStream, size_t bytesCount);

/* helper functions to help you write commonly used sequences of data*/
void bsWriteBEU16(struct ByteStream* stream, uint16_t beu16);
uint16_t bsReadBEU16(struct ByteStream* stream);

void bsWriteLEU16(struct ByteStream* stream, uint16_t leu16);
uint16_t bsReadLEU16(struct ByteStream* stream);

void bsWriteLEU32(struct ByteStream* stream, uint32_t leu32);
uint32_t bsReadLEU32(struct ByteStream* stream);
                 
/* When you just want to write/read to an 8-bit offset */
inline void bwrite(void* target, size_t offset, uint8_t value) {
	uint8_t* target8 = (uint8_t*)target;
	target8[offset] = value;
}

inline uint8_t bread(void* target, size_t offset) {
	uint8_t* target8 = (uint8_t*)target;
	return target8[offset];
}
    
    
#ifdef __cplusplus
}
#endif
