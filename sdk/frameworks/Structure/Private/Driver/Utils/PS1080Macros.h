//
//  PS1080Macros.h
//  Driver_iOS
//
//  Created by Forrest Heller on 4/30/13.
//  Copyright (c) 2013 Occipital. All rights reserved.
//

#pragma once

/* A lot of times we want to use a snippet of PS1080 code or header definitions.  The PS1080
   has an extensive set of macros.  If they are shared by at least two pieces of arcturus, it's 
   usually a good idea to put them here */
#define HIGH_BYTE(x) (x >> 8)
#define LOW_BYTE(x) (x & 0xff)

#define MAKE_WORD(high, low) (((high << 8) & 0xff00) | (low & 0xff))

//dev: what about the case of 0xFFFF
#define MASK(NumOfBits, Offset) ( ( (1UL<<(NumOfBits))-1 )<<Offset )

#define BITMASK(high, low) (MASK(high - low + 1UL,low))
/* Let's say we register FOOSTATUS has a counter in bits 11-3.  We can do the following to get the value of the counter:
 uint16_t counter = BITS(FOOSTATUS, 11, 3)
	The value of the counter is now in bits 8-0 of the counter variable */
#define BITS(value, high, low) ((BITMASK(high,low) & value) >> low)

