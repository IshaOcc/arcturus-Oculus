//
//  OXRingBuffer.h
//  AACPlayer
//
//  Created by hellerf on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
//#define RINGBUFFER_TRACE /* Turn this on to track Grab/Release/Ungrab sequences and see where they go wrong */

/**
 * This here is your basic lock-free ring buffer (Single producer/consumer only!)
 * This is NOT a ring of pointers to buffers--this is a single, large, contiguous 
 * chunk of memory containing @capacity buffers of @bufferSize
 * I could not find any quick-and-easy drop-ins
 */
/** Tutorial **
 Producer side: 
 OXRBProducerGrab(rb, &bufferToRead); OR OXRBProducerPeek(rb, &bufferToWrite); (see end of comment)
 strcpy(bufferToWrite->data, "some data");
 OXRBProducerRelease(rb);
 
 Consumer side:
 OXRBConsumerGrab(rb, &bufferToRead); OR OXRBConsumerPeek(rb, &bufferToRead); (see end of comment)
 printf("buffer is %s\n", bufferToRead->data);
 OXRBConsumerRelease(rb);
 
 OXRBConsumerGrab/OXRBProducerGrab will safely allow the producer to overwrite old buffers
 OXRBConsumerPeek/OXRBProducerPeek will wait for the consumer to consume buffers before overwriting them
 */
struct OXRBState {
    /* Points to the next producer offset in bytes*/
    long producerBufferOffset;
    /* Points to the next consumer offset in bytes */
    long consumerBufferOffset;
    /**
     * The capacity, in bytes, of the entire allocated memory (including overheads for buffer headers and such)
     */
    long allocatedBytes;
    long bufferSize;
    long nextConsumerUsage;
    long nextProducerUsage;
    long bufferCount;
};

#define USAGE_UNFILLED 0
#define USAGE_FILLING -1
#define USAGE_UNFILLING -2

/** This is a variable-size struct -- data simply holds the
 *  pointer to the actual buffer
 */
struct OXRingBuffer {
    struct OXRBState state;
    uint8_t data[1];
};

union tag {
    uint64_t tag_u64;
    uint32_t tag_u32;
    int32_t tag_i32;
    int64_t tag_i64;
    double tag_float64;
    float tag_float32;
    void* tag_pointer;
};

struct OXBufferContiguousState {
    long capacity;
    long usage;
    long oldUsage;
    union tag tag;
};

struct OXBufferContiguous {
    struct OXBufferContiguousState state;
    uint8_t data[1];
};


/**
 * Creates a ring buffer.  Will allocate about bufferCount * bufferSize worth of data--be careful
 */
struct OXRingBuffer* OXRBCreate(long bufferCount, long bufferSize);

/**
 * Gets an exclusive lock on this consumer buffer
 * The lock is release with ConsumerRelease
 * @return 0 for success, anything else for failure
 */
#ifdef RINGBUFFER_TRACE
#define OXRBConsumerGrab(rb, buf) ({ printf("\nOXRBConsumerGrab at %s:%d\n", __FUNCTION__, __LINE__); int _res = OXRBConsumerGrab_real(rb, buf); (_res == _res) ? _res : _res; })
int OXRBConsumerGrab_real(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict bufferToRead);
#else
int OXRBConsumerGrab(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict bufferToRead);
#endif

/**
 * Releases the ring buffer to the next consumer buffer
 * @return 0 on success
 */
#ifdef RINGBUFFER_TRACE
#define OXRBConsumerRelease(rb) ({ printf("\nOXRBConsumerRelease at %s:%d\n", __FUNCTION__, __LINE__); int _res = OXRBConsumerRelease_real(rb); (_res == _res) ? _res : _res; })
int OXRBConsumerRelease_real(struct OXRingBuffer* __restrict rb);
#else
int OXRBConsumerRelease(struct OXRingBuffer* __restrict rb);
#endif

/**
 * Gives the buffer back to the consumer to be read again later
 * @return 0 on success
 */
#ifdef RINGBUFFER_TRACE
#define OXRBConsumerUngrab(rb) ({ printf("\nOXRBConsumerUngrab at %s:%d\n", __FUNCTION__, __LINE__); int _res = OXRBConsumerUngrab_real(rb); (_res == _res) ? _res : _res; })
int OXRBConsumerUngrab_real(struct OXRingBuffer* __restrict rb);
#else
int OXRBConsumerUngrab(struct OXRingBuffer* __restrict rb);
#endif

/**
 * Mirrors ConsumerRelease, but for producer buffers 
 */
/**
 * Gets an exclusive lock on this producer buffer
 * The lock is release with producerRelease
 * @return 0 for success, anything else for failure
 */
#ifdef RINGBUFFER_TRACE
#define OXRBProducerGrab(rb, bufferToFill) ({ printf("\nOXRBProducerGrab at %s:%d\n", __FUNCTION__, __LINE__); int _res = OXRBProducerGrab_real(rb, bufferToFill); (_res == _res) ? _res : _res; })
int OXRBProducerGrab_real(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict bufferToFill);
#else
int OXRBProducerGrab(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict bufferToFill);
#endif

/**
 * When you ProducerGrab a buffer and then fill it with trash (something that you don't want to be consumed),
 * call this function before Ungrab.
 * @return 0 for success
 */
int OXRBProducerMarkTrashed(struct OXRingBuffer* __restrict rb);

/**
 * Release the current grabbed buffer (if any)
 * and move on to the next producer buffer
 * @return 0 on success, failure otherwise
 */
#ifdef RINGBUFFER_TRACE
#define OXRBProducerRelease(rb) ({ printf("\nOXRBProducerRelease at %s:%d\n", __FUNCTION__, __LINE__); int _res = OXRBProducerRelease_real(rb); (_res == _res) ? _res : _res; })
int OXRBProducerRelease_real(struct OXRingBuffer* __restrict rb);
#else
int OXRBProducerRelease(struct OXRingBuffer* __restrict rb);
#endif
/**
 * Gives the buffer back to the producer
 * If the grabbed buffer was previously filled, then 
 * let that buffer be consumed again.  *** THAT MEANS IF YOU FILL THE BUFFER WITH TRASH, MARK IT AS TRASHED ***
 * @return 0 on success
 */
#ifdef RINGBUFFER_TRACE
#define OXRBProducerUngrab(rb) ({ printf("\nOXRBProducerUngrab at %s:%d\n", __FUNCTION__, __LINE__); int _res = OXRBProducerUngrab_real(rb); (_res == _res) ? _res : _res; })
int OXRBProducerUngrab_real(struct OXRingBuffer* __restrict rb);
#else
int OXRBProducerUngrab(struct OXRingBuffer* __restrict rb);
#endif


/**
 * Grab the last produced buffer
 * @return 0 on success
 */
int OXRBPeekLastProduced(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict lastProduced);

/**
 * Resets the ring buffer to its initial state: all buffers unfilled
 */
void OXRBReset(struct OXRingBuffer* __restrict rb);

/** 
 * Destroy is not thread safe
 * Frees all memory associated with this buffer ring
 */
void OXRBDestroy(struct OXRingBuffer* __restrict rb);
    
/****** DANGEROUS / DEPRECATED ******/

/**
 * "Peek" at the currently consumer buffer.  It is NOT safe. If this function returns 0,
 * then you are free to use use the buffer in any manner you please.
 * Called Peek because it follows semantics of peeking (not changing the underlying stream position)
 * @return 0 on success, -EAGAIN if the buffer has not been consumer
 */
int OXRBConsumerPeek(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict bufferToFill);

/**
 * Mirrors producerPeek, but for producer buffers 
 */
int OXRBProducerPeek(struct OXRingBuffer* __restrict rb, struct OXBufferContiguous** __restrict bufferConsumer);

/**
 * Prints some info about the ring buffer to stdout
 */
void OXRBPrintStatus(struct OXRingBuffer* __restrict rb);


int OXRBConsumerPeekAhead(struct OXRingBuffer* __restrict rb, long ahead, struct OXBufferContiguous** __restrict bufferToFill);
int OXRBProducerPeekAhead(struct OXRingBuffer* __restrict rb, long ahead, struct OXBufferContiguous** __restrict bufferToFill);


#ifdef __cplusplus
}
#endif
