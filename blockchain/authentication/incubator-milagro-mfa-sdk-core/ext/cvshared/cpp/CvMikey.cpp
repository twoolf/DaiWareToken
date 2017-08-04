/***************************************************************************************************************************************************************************************************************************
																																																						   *
This file is part of CertiVox M-Pin Client and Server Libraries.																																						   *
The CertiVox M-Pin Client and Server Libraries provide developers with an extensive and efficient set of strong authentication and cryptographic functions.																   *
For further information about its features and functionalities please refer to http://www.certivox.com																													   *
The CertiVox M-Pin Client and Server Libraries are free software: you can redistribute it and/or modify it under the terms of the BSD 3-Clause License http://opensource.org/licenses/BSD-3-Clause as stated below.		   *
The CertiVox M-Pin Client and Server Libraries are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
Note that CertiVox Ltd issues a patent grant for use of this software under specific terms and conditions, which you can find here: http://certivox.com/about-certivox/patents/											   * 	
Copyright (c) 2013, CertiVox UK Ltd																																														   *	
All rights reserved.																																																	   *
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:																			   *
�	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.																						   *	
�	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.			   *	
�	Neither the name of CertiVox UK Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.								   *
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,																		   *
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS																	   *
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE																	   *	
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,														   *
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.																		   *	
																																																						   *
***************************************************************************************************************************************************************************************************************************/
/*! \file  CvMikey.cpp
    \brief C++ class implementing the MIKEY standard for key transport

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 31, 2013, 10:32 AM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

 C++ class implementing the MIKEY standard for key transport.
 The class API is adapted to the existing SkyKey soltuion.

*/

#include "CvMikey.h"

#include "CvSakke.h"
#include "CvEccsi.h"

#include "CvXcode.h"

#include "CvMiraclDefs.h"

#define MIKEY_VERSION			1
#define MIKEY_IDR_PAYLOAD		14
#define MIKEY_SAKKE_PAYLOAD		254
#define MIKEY_SIGN_PAYLOAD		4
#define MIKEY_LAST_PAYLOAD		0
#define MIKEY_IDR_TYPE			2
#define MIKEY_IDR_SENDER		1
#define MIKEY_IDR_RECEIVER		2
#define MIKEY_IDR_KMS			3
#define MIKEY_IDR_SENDER_KMS	6
#define MIKEY_IDR_RECEIVER_KMS	7
#define MIKEY_SAKKE_PARAMSCHEME	2
#define MIKEY_ECCSI_SIG_TYPE	2

using namespace CvShared;

CvMikey::CvMikey( csprng* apRng ) :
	m_pRng(apRng)
{
}

CvMikey::~CvMikey()
{
}

int CvMikey::GetSize( const String& aSenderId, const String& aReceiverId, const String& aSenderKms, const String& aReceiverKms ) const
{
	int length = 0;
	
	/* Compute Header. */
	length += 2;
	
	/* Compute identities of sender and receiver. */
	length += ( 5 + (int)aSenderId.length() );
	length += ( 5 + (int)aReceiverId.length() );
	
	/* Compute identities of the KMS's. */
	if ( aSenderKms == aReceiverKms )
	{
		length += ( 5 + (int)aSenderKms.length() );
	}
	else
	{
		length += ( 5 + (int)aSenderKms.length() );
		length += ( 5 + (int)aReceiverKms.length() );
	}
	
	/* Compute output of SAKKE. */
	length += 5;
	length += AS;
	length += ( 4*AS + 1 );
	
	/* Compute length of signature. */
	length += 3;
	length += ( 8*AS + 1 );

	/* Compute size in base64 and append final null byte. */
	return ( ( length + 2 - ((length + 2) % 3) ) / 3 * 4 ) + 1;
}
	
bool CvMikey::CreateMessage( const String& aKey, const String& aSenderId, const String& aReceiverId, const String& aSenderKms, const String& aReceiverKms,
							const String& aSakkeParams, const String& aSakkeKmsPublicKey, const String& aEccsiSecret, const String& aEccsiPrivateKey,
							const String& aEccsiKmsPublicKey, OUT String& aPacket )
{
	/* Allocate maximum size. */
	String buf;
	buf.reserve( 2 + 10 + aSenderId.length() + aReceiverId.length() + 10 + aSenderKms.length() + aReceiverKms.length() + 5 + 5*AS + 1 + 2 + AES_SECURITY + 1 );

	aPacket.clear();
	
	/* First, add version and the next payload type (IDR). */
	buf += (char)MIKEY_VERSION;
	buf += (char)MIKEY_IDR_PAYLOAD;
	
	/* Also skip the timestamp T and the RAND, because we won't do further key derivation. */
	
	/* Add the sender and receiver with IDR type. */
	buf += (char)MIKEY_IDR_PAYLOAD;
	buf += (char)MIKEY_IDR_SENDER;
	buf += (char)MIKEY_IDR_TYPE;
	buf += (char)( aSenderId.length() >> 8 );
	buf += (char)( aSenderId.length() & 0xFF );

	buf += aSenderId;

	buf += (char)MIKEY_IDR_PAYLOAD;
	buf += (char)MIKEY_IDR_RECEIVER;
	buf += (char)MIKEY_IDR_TYPE;
	buf += (char)( aReceiverId.length() >> 8 );
	buf += (char)( aReceiverId.length() & 0xFF );

	buf += aReceiverId;

	/* Add the KMS's for the sender or receiver with the IDR type, depending if they are equal or not. */
	if ( aSenderKms == aReceiverKms )
	{
		buf += (char)MIKEY_SAKKE_PAYLOAD;
		buf += (char)MIKEY_IDR_KMS;
		buf += (char)MIKEY_IDR_TYPE;
		buf += (char)( aSenderKms.length() >> 8 );
		buf += (char)( aSenderKms.length() & 0xFF );

		buf += aSenderKms;
	}
	else
	{
		buf += (char)MIKEY_IDR_PAYLOAD;
		buf += (char)MIKEY_IDR_SENDER_KMS;
		buf += (char)MIKEY_IDR_TYPE;
		buf += (char)( aSenderKms.length() >> 8 );
		buf += (char)( aSenderKms.length() & 0xFF );

		buf += aSenderKms;
		
		buf += (char)MIKEY_SAKKE_PAYLOAD;
		buf += (char)MIKEY_IDR_RECEIVER_KMS;
		buf += (char)MIKEY_IDR_TYPE;
		buf += (char)( aReceiverKms.length() >> 8 );
		buf += (char)( aReceiverKms.length() & 0xFF );

		buf += aReceiverKms;
	}

	/* Encapsulate symmetric key with SAKKE. */
	String encapsulatedData;
	
	if ( !CvSakke(m_pRng).Encapsulate( aKey, aSakkeKmsPublicKey, aReceiverId, encapsulatedData, aSakkeParams ) )
		return false;
	
	size_t pos = encapsulatedData.find(",");
	if ( pos == String::npos )
		return false;

	String sakkeHint;
	CvBase64::Decode( encapsulatedData.substr(0,pos), sakkeHint );

	String sakkePayload;
	CvBase64::Decode( encapsulatedData.substr(pos+1), sakkePayload );		

	buf += (char)MIKEY_SIGN_PAYLOAD;
	buf += (char)MIKEY_SAKKE_PARAMSCHEME;
	buf += (char)MIKEY_SAKKE_PARAMSCHEME;
	buf += (char)( ( sakkeHint.size() + sakkePayload.size() ) >> 8 );
	buf += (char)( ( sakkeHint.size() + sakkePayload.size() ) & 0xFF );

	buf.append( sakkeHint.data(), sakkeHint.size() );

	buf.append( sakkePayload.data(), sakkePayload.size() );

	/* Sign the packet with ECCSI. */
	String msg;
	CvBase64::Encode( (uint8_t*)buf.data(), (int)buf.length(), msg );

	String signature;

	if ( !CvEccsi(m_pRng).Sign( msg.c_str(), (int)msg.length(), aSenderId, aEccsiKmsPublicKey, aEccsiSecret, aEccsiPrivateKey, signature ) ||
			signature.empty() )
		return false;
	
	String decodedSignature;
	CvBase64::Decode( signature, decodedSignature );

	buf += (char)MIKEY_LAST_PAYLOAD;
	buf += (char)( (MIKEY_ECCSI_SIG_TYPE << 4) + (decodedSignature.size() >> 8) );
	buf += (char)( decodedSignature.size() & 0xFF );

	buf.append( decodedSignature.data(), decodedSignature.size() );					

	CvBase64::Encode( (uint8_t*)buf.data(), (int)buf.length(), aPacket );

	return true;
}
	
bool CvMikey::ProcessMessage( const String& aPacket, const String& aUserId, const String& aUserKms, const String& aSakkePrivateKey, const String& aSakkeParams,
							  const String& aSakkeKmsPublicKey, const String& aEccsiKmsPublicKey, OUT String& aKey )
{
	String senderId;
	String senderKms;
	String receiverId;
	String receiverKms;
	
	int lenSakke, lenEccsi;
	
	String sakkeHint;
	String sakkePayload;
	String eccsiSignature;
	
	bool bDone = false;
	
	String decodedPacket;
	CvBase64::Decode( aPacket, decodedPacket );
	
	size_t packetSize = decodedPacket.size();
	
	if ( packetSize <= 1 )
		return false;
	
	size_t pos = 0;
	
	if ( (uint8_t)decodedPacket[pos++] != MIKEY_VERSION )
		return false;

	uint8_t next = (uint8_t)decodedPacket[pos++];
	
	while( !bDone )
	{
		switch (next)
		{
			case MIKEY_IDR_PAYLOAD:
				
				if ( pos + 5 > packetSize )
					return false;

				next = (uint8_t)decodedPacket[pos++];
				
				switch ( (uint8_t)decodedPacket[pos++] )
				{
					case MIKEY_IDR_SENDER:
					{
						if ( (uint8_t)decodedPacket[pos++] != MIKEY_IDR_TYPE )
							return false;

						uint8_t lenHi = (uint8_t)decodedPacket[pos++];
						uint8_t lenLow = (uint8_t)decodedPacket[pos++];

						int lenSender = ( (int)lenHi << 8 ) + lenLow;

						if ( pos + lenSender > packetSize )
							return false;

						senderId.assign( decodedPacket, pos, lenSender );
						pos += lenSender;

						break;
					}
				
					case MIKEY_IDR_RECEIVER:
					{
						if ( (uint8_t)decodedPacket[pos++] != MIKEY_IDR_TYPE )
							return false;

						uint8_t lenHi = (uint8_t)decodedPacket[pos++];
						uint8_t lenLow = (uint8_t)decodedPacket[pos++];

						int lenReceiver = ( (int)lenHi << 8 ) + lenLow;

						if ( pos + lenReceiver > packetSize )
							return false;

						receiverId.assign( decodedPacket, pos, lenReceiver );
						pos += lenReceiver;

						break;
					}
				
					case MIKEY_IDR_KMS:
					{
						if ( (uint8_t)decodedPacket[pos++] != MIKEY_IDR_TYPE )
							return false;

						uint8_t lenHi = (uint8_t)decodedPacket[pos++];
						uint8_t lenLow = (uint8_t)decodedPacket[pos++];

						int lenKms = ( (int)lenHi << 8 ) + lenLow;

						if ( pos + lenKms > packetSize )
							return false;

						receiverKms = senderKms.assign( decodedPacket, pos, lenKms );
						pos += lenKms;

						break;
					}
				
					case MIKEY_IDR_SENDER_KMS:
					{
						if ( (uint8_t)decodedPacket[pos++] != MIKEY_IDR_TYPE )
							return false;

						uint8_t lenHi = (uint8_t)decodedPacket[pos++];
						uint8_t lenLow = (uint8_t)decodedPacket[pos++];

						int lenSenderKms = ( (int)lenHi << 8 ) + lenLow;

						if ( pos + lenSenderKms > packetSize )
							return false;

						senderKms.assign( decodedPacket, pos, lenSenderKms );
						pos += lenSenderKms;

						break;
					}
				
					case MIKEY_IDR_RECEIVER_KMS:
					{
						if ( (uint8_t)decodedPacket[pos++] != MIKEY_IDR_TYPE )
							return false;

						uint8_t lenHi = (uint8_t)decodedPacket[pos++];
						uint8_t lenLow = (uint8_t)decodedPacket[pos++];

						int lenReceiverKms = ( (int)lenHi << 8 ) + lenLow;

						if ( pos + lenReceiverKms > packetSize )
							return false;

						receiverKms.assign( decodedPacket, pos, lenReceiverKms );
						pos += lenReceiverKms;

						break;
					}
				}
				
				break;
				
			case MIKEY_SAKKE_PAYLOAD:
			{	
				if ( pos + 5 > packetSize )
					return false;

				next = (uint8_t)decodedPacket[pos++];
				
				if ( (uint8_t)decodedPacket[pos++] != MIKEY_SAKKE_PARAMSCHEME ||
					(uint8_t)decodedPacket[pos++] != MIKEY_SAKKE_PARAMSCHEME )
					return false;
				
				uint8_t lenHi = (uint8_t)decodedPacket[pos++];
				uint8_t lenLow = (uint8_t)decodedPacket[pos++];

				lenSakke = ( (int)lenHi << 8 ) + lenLow;

				if ( pos + lenSakke > packetSize )
					return false;
				
				sakkeHint.assign( decodedPacket, pos, AS );
				sakkePayload.assign( decodedPacket, pos + AS, lenSakke - AS );

				pos += lenSakke;
				break;
			}

			case MIKEY_SIGN_PAYLOAD:
			{
				if ( pos + 3 > packetSize )
					return false;
				
				size_t posTmp = pos;
				
				if ( (uint8_t)decodedPacket[posTmp++] != MIKEY_LAST_PAYLOAD )
					return false;
				
				bDone = true;
				
				int type = ( decodedPacket[posTmp] >> 4 );
				
				if ( type != MIKEY_ECCSI_SIG_TYPE )
					return false;
				
				uint8_t lenHi = (uint8_t)decodedPacket[posTmp++] & 0x0F;
				uint8_t lenLow = (uint8_t)decodedPacket[posTmp++];

				lenEccsi = ( (int)lenHi << 8 ) + lenLow;

				if ( posTmp + lenEccsi > packetSize )
					return false;
				
				eccsiSignature.assign( decodedPacket, posTmp, lenEccsi );

				break;
			}	
			default:
				return false;
		}
	}
	
	if ( aUserId != receiverId || aUserKms != receiverKms )
		return false;
	
	String encodedSignature;
	CvBase64::Encode( (const uint8_t*)eccsiSignature.data(), (int)eccsiSignature.size(), encodedSignature );

	String encodedMessage;
	CvBase64::Encode( (const uint8_t*)decodedPacket.data(), (int)pos, encodedMessage );

	if ( !CvEccsi(m_pRng).Verify( encodedMessage.c_str(), (int)encodedMessage.length(), senderId, aEccsiKmsPublicKey, encodedSignature ) )
		return false;
	
	String encodedHint;
	CvBase64::Encode( (const uint8_t*)sakkeHint.data(), (int)sakkeHint.size(), encodedHint );

	String encodedPayload;
	CvBase64::Encode( (const uint8_t*)sakkePayload.data(), (int)sakkePayload.size(), encodedPayload );

	if ( !CvSakke(m_pRng).Decapsulate( encodedHint + "," + encodedPayload, aSakkeKmsPublicKey, aUserId, aSakkePrivateKey, aKey, aSakkeParams ) )
		return false;

	return true;
}


