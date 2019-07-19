/*
 * iAP2Error.h
 *
 * Created: 2/8/2013 12:12:59 PM
 *  Author: Occipital
 */ 

#pragma once

typedef enum {
	iAP2Error_None = 0,
	iAP2Error_StartOfPacketNotPresent,
	iAP2Error_NotFullHeader,
	iAP2Error_NotFullPacket,
	iAP2Error_BufferTooSmall,
	iAP2Error_InvalidPacketLength,
	iAP2Error_StartOfMessageNotPresent,
	iAP2Error_MessageHeaderLengthInvalid,
	iAP2Error_MessageParameterHeaderLengthInvalid,
	iAP2Error_AlreadySendingMessage,
	iAP2Error_AckQueueFull,
	iAP2Error_AlreadySendingBytes,
	iAP2Error_CouldNotSend
} iAP2Error;
