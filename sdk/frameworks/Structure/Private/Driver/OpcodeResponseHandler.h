//
//  OpcodeResponseHandler.h
//  Structure
//
//  Created by Forrest Heller on 2/18/14.
//  Copyright (c) 2015 Occipital, Inc. All rights reserved.
//

#pragma once

#import <Foundation/Foundation.h>

/* replyCode - contains the first two bytes of the payload, if any could be found.  If there is less than two bytes in the payload, 0xffff will be passed to replyCode.
 payload - the entire payload sent back, _including_ the replyCode.
 payloadSize - the size of the payload in bytes.
 error - if any error was encountered while receiving a response. (Right now the only thing that can happen is that the sensor disconnects */
typedef void (^OpcodeResponseHandlerBlock)(uint16_t replyCode, const uint16_t* payload, size_t payloadSize, NSError* error);

/* These objects are used by SensorCommunicationController.  They are created when a PS1080 opcode is sent from 
 SensorCommunicationController (sendOpcode:...).  When the opcode with .opcode and .seq is finished executing on the PS1080, the
 PS1080 sends back a response.  If an OpcodeResponseHandler exists for that .seq and .opcode, its .responseHandler is executed. */
@interface OpcodeResponseHandler : NSObject
@property (nonatomic, copy) OpcodeResponseHandlerBlock responseHandler;
@property (nonatomic) uint16_t opcode;
@property (nonatomic) uint8_t seq; // why 8 bit sequence numbers? See PS_ENCODING_SEQ
@end
