//
//  iAP2Message.h
//  Driver_iOS
//
//  Created by Forrest Heller on 4/23/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

/** 
  * The iAP2MessageReader:
  * You put some bytes in (void*) and you get nice message objects out (NSArray of iAP2Message).
  * (it was made for an 8-bit embedded processor -- feel free to extend it in this repo)
  */
 
#import <Foundation/Foundation.h>

//------------------------------------------------------------------------------

@interface iAP2MessageParameter : NSObject

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, retain) NSData* payload;
@property (nonatomic, getter=parameterID, setter=setParameterID:) uint16_t parameterID;

@end

//------------------------------------------------------------------------------

@interface iAP2Message : NSObject

// NOTE: Obfuscated properties are potentially unsafe unless their getters are explicitly specified.
@property (nonatomic, getter=messageID, setter=setMessageID:) uint16_t messageID;
@property (nonatomic, readonly) NSMutableDictionary* parameters;

@end

//------------------------------------------------------------------------------

/* WARNING: this class is not thread safe */
@interface iAP2MessageReader : NSObject

-(NSArray*) newMessagesFromData:(const void*)data size:(size_t)dataSize;

@end
