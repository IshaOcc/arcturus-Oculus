#pragma once

/* General iAP2 serialization / deserialization functions.

  It's helpful to think of iAP2 as a mini-TCP+IP stack.
  iAP2Packet* stuff is the flow control and routing layer (TCP congestion + TCP reliability + IP)
  iAP2Message* + iAP2Parameter* stuff sits on top of iAP2Packet and provides iAP2 session
  level communication.  iAP2Messages have iAP2MessageParameters.
  iAP2Packets sometimes don't wrap things in messages or parameters (like the link synchronization stuff)
*/
#if defined(ZSP) && ZSP
#include "Framework\iAP2Platform.h"
#include "Framework\iAP2Error.h"
#else
#include "iAP2Platform.h"
#include "iAP2Error.h"
#endif

/* bits for the control byte */
#define IAP2_CONTROL_BYTE_SYN (1<<7) /* link synchronization is present */
#define IAP2_CONTROL_BYTE_ACK (1<<6) /* packet ACK number is valid, and iAP2 Session Payload may be present */
#define IAP2_CONTROL_BYTE_EAK (1<<5) /* extended ack payload is present */
#define IAP2_CONTROL_BYTE_RST (1<<4) /* link reset */
#define IAP2_CONTROL_BYTE_SLP (1<<3) /* device sleep */

static const uint8_t iAP2SessionControl = 0;
static const uint8_t iAP2SessionIDControl = 1;
static const uint8_t iAP2SessionEA = 2; //EA = external accessory
#define iAP2PacketHeaderSize 9
#define iAP2PacketPayloadChecksumSize 1
#define iAP2StartOfMessage 0x4040
#define iAP2MessageParameterHeaderSize  4
#define iAP2MessageHeaderSize 6
#define iAP2MessageIDRequestCertificate 0xaa00
#define iAP2MessageIDCertificate 0xaa01
#define iAP2MessageParameterIDCertificate 0
#define iAP2MessageIDRequestChallengeResponse 0xaa02
#define iAP2MessageIDChallengeResponse 0xaa03
#define iAP2MessageParameterIDResponse 0
#define iAP2MessageIDAuthenticationFailed 0xaa04
#define iAP2MessageIDAuthenticationSucceeded 0xaa05
#define iAP2MessageIDStartIdentification 0x1d00
#define iAP2MessageIDIdentificationInformation 0x1d01
#define iAP2MessageIDIdentificationAccepted 0x1d02
#define iAP2MessageIDIdentificationRejected  0x1d03
#define iAP2MessageIDStartPowerUpdates 0xae00
#define iAP2MessageIDPowerUpdate 0xae01
#define iAP2MessageIDStopPowerUpdates 0xae02
#define iAP2MessageIDPowerSourceUpdate 0xae03
#define iAP2MessageIDStartExternalAccessoryProtocolSession 0xea00
#define iAP2MessageIDStopExternalAccessoryProtocolSession 0xea01
#define iAP2ChallengeSizeMax 128

struct iAP2PacketHeader {
	uint16_t packetLength;
	uint8_t controlByte;
	uint8_t seqNumber;
	uint8_t ackNumber;
	uint8_t sessionID;
};

struct iAP2MessageHeader {
	uint16_t messageLength;
	uint16_t messageID;
};

struct iAP2MessageParameterHeader {
	uint16_t parameterID;
	uint16_t parameterLength;
};


#ifdef __cplusplus
extern "C" {
#endif

int8_t iAP2ChecksumGet(struct ByteStream* buffer8, size_t packetSize);

void iAP2PacketHeaderWrite(struct ByteStream* buffer, uint16_t payloadLength, uint8_t controlByte, uint8_t seqNumber, uint8_t ackNumber, uint8_t sessionID);
void iAP2PacketHeaderWriteStruct(struct ByteStream* buffer, const struct iAP2PacketHeader* header);
/* 
The ByteStream.base must point to the start of the packet header (SoP MSB) and ByteStream.offset must 
   == the last payload byte written.
   There's a lot of routines the fill up a ByteStream with a packet + payload and the Bytebuffer points
   to the end of the packet.  Make it so that those routines can just pass in their ByteStream to write the payload checksum.
   Saves code space.  */
void iAP2PacketWritePayloadChecksum(struct ByteStream* streamAtEndOfPayload);

iAP2Error iAP2PacketHeaderRead(struct ByteStream* buffer, struct iAP2PacketHeader* header);
size_t iAP2PacketHeaderGetPayloadSize(const struct iAP2PacketHeader* header);

void iAP2MessageHeaderWrite(struct ByteStream* buffer, uint16_t messageID, uint16_t parametersSize);
iAP2Error iAP2MessageHeaderRead(struct ByteStream* buffer, struct iAP2MessageHeader* header, size_t bytesLeftInBuffer);
size_t iAP2MessageHeaderGetParametersSize(const struct iAP2MessageHeader* header);

void iAP2MessageParameterHeaderWrite(struct ByteStream* buffer, uint16_t parameterID, uint16_t parameterDataSize);
iAP2Error iAP2MessageParameterHeaderRead(struct ByteStream* buffer, struct iAP2MessageParameterHeader* header, size_t bytesLeftInBuffer);
size_t iAP2MessageParameterHeaderGetDataSize(const struct iAP2MessageParameterHeader* header);
    
    
#ifdef __cplusplus
}
#endif
