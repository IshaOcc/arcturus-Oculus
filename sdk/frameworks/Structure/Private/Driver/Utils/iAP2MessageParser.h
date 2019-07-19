#pragma once

#if defined(ZSP) && ZSP
#include "Framework\iAP2Commands.h"
#else
#include "iAP2Commands.h"
#include <stdbool.h>
#endif


typedef enum {
	iAP2MessageParserState_StartOfMessage,
	iAP2MessageParserState_MessageLength,
	iAP2MessageParserState_MessageID,
	iAP2MessageParserState_MessageParameterLength,
	iAP2MessageParserState_MessageParameterID,
	iAP2MessageParserState_MessageParameterPayload
} iAP2MessageParserState;

struct iAP2MessageParser;
typedef void (*iAP2MessageHeaderReceivedFunc)(struct iAP2MessageParser* __restrict parser);
typedef void (*iAP2MessageParameterHeaderReceivedFunc)(struct iAP2MessageParser* __restrict parser);
typedef void (*iAP2MessageParameterPayloadByteReceivedFunc)(struct iAP2MessageParser* __restrict parser, uint8_t newByte);
typedef void (*iAP2MessageEndedFunc)(struct iAP2MessageParser* __restrict parser);

struct iAP2MessageParser {
	/* Contains the parsed header */
	struct iAP2MessageHeader header;
	/* go from having to use 6 parser states to 3 states by keeping track of whether or
     not the current state is looking at the most significant byte of the current field */
	bool mostSignificantByte;
	iAP2MessageParserState state;
	struct iAP2MessageParameterHeader paramHeader;
	uint16_t remainingMessagePayload;
	uint16_t remainingMessageParameterPayload;
	iAP2MessageHeaderReceivedFunc headerReceived;
	iAP2MessageParameterHeaderReceivedFunc parameterHeaderReceived;
	iAP2MessageParameterPayloadByteReceivedFunc payloadByteReceived;
    iAP2MessageEndedFunc messageEnded;
    void* tag;
    
};
/* Resets the parser to the default state and gets it ready to receive bytes with iAP2MessageParserPut */
void iAP2MessageParserInit(struct iAP2MessageParser* parser,
                           iAP2MessageHeaderReceivedFunc headerReceived,
                           iAP2MessageParameterHeaderReceivedFunc parameterHeaderReceived,
                           iAP2MessageParameterPayloadByteReceivedFunc payloadByteReceived,
                           iAP2MessageEndedFunc messageEnded);
/* Feed a byte into the parser.  It will digest the byte and call one of the callbacks below when appropriate */
void iAP2MessageParserPut(struct iAP2MessageParser* __restrict parser, uint8_t newByte);
