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
/*! \file  CvAesGcm.cpp
    \brief C++ wrapper for the MIRACL AES GCM functionality

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 23, 2013, 3:08 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

 C++ wrapper for the MIRACK/SkyKeyXT AES GCM functionality.
 The class API is adapted to the existing SkyKey soltuion.

*/

#include "include/CvAesGcm.h"
#include "CvXcode.h"

#include <string.h>

using namespace CvShared;

CvAesGcm::CvAesGcm( csprng* apRng ) :
	m_pRng(apRng)
{}
	
void CvAesGcm::RandomiseBuffer( char* apBuffer, int aLength )
{
	if ( m_pRng == NULL )
		return;
	
    for( int i = 0; i < aLength; ++i )
    {
        apBuffer[i] = strong_rng( m_pRng );
    }
}

bool CvAesGcm::GenerateKey( OUT String& aKey, int aLen )
{
	if ( m_pRng == NULL )
		return false;

	String key( aLen, 0 );
	RandomiseBuffer( (char*)key.data(), aLen );

	CvBase64::Encode( (const uint8_t*)key.data(), aLen, aKey );

	return true;
}

bool CvAesGcm::Decrypt( const String& aKey, const String& aCipher, String& aPlainData, int aLengthIV, int aLengthHeader )
{
    String keyDecoded;
	CvBase64::Decode( aKey, keyDecoded );

    String cipherDecoded;
	CvBase64::Decode( aCipher, cipherDecoded );	

    String header( cipherDecoded.c_str(), aLengthHeader );

	String iv( cipherDecoded.c_str() + aLengthHeader, aLengthIV );

    int cipherLength = (int)cipherDecoded.size() - ( aLengthHeader + aLengthIV + 16 );
	String cipher( cipherDecoded.c_str() + aLengthHeader + aLengthIV, cipherLength );
	
	String tag( cipherDecoded.c_str() + aLengthHeader + aLengthIV + cipherLength, 16 );

	aPlainData.assign( cipherLength, 0 );

    //Decrypt
    gcm g;
    gcm_init( &g, (int)keyDecoded.size(), (char*)keyDecoded.c_str(), aLengthIV, (char*)iv.c_str() );
	
    if ( !gcm_add_header( &g, (char*)header.c_str(), aLengthHeader ) )
		return false;
	
    if ( !gcm_add_cipher( &g, GCM_DECRYPTING, (char*)aPlainData.data(), cipherLength, (char*)cipher.c_str() ) )
		return false;
	
    gcm_finish( &g, (char*)tag.c_str() );

    return true;
}

//static void printBuffer( const char* apPrefix, const char* apBuffer, int aLength )
//{
//	printf( "%s [", apPrefix );
//	for ( int i = 0; i < aLength; ++i )
//	{
//		if ( i+1 < aLength )
//			printf( "%hhu,", apBuffer[i] );
//		else
//			printf( "%hhu", apBuffer[i] );			
//	}
//	printf( "]\n" );
//}

bool CvAesGcm::Encrypt( const String& aKey, const String& aPlainData, String& aCipher, int aLengthIV, int aLengthHeader )
{
	if ( m_pRng == NULL )
		return false;
	
    String keyDecoded;
	CvBase64::Decode( aKey, keyDecoded );

	String iv( aLengthIV, 0 );
	RandomiseBuffer( (char*)iv.c_str(), aLengthIV );
//	for ( int i = 0; i < aLengthIV; ++i )
//		iv[i] = i;
		
	String header( aLengthHeader, 0 );
	RandomiseBuffer( (char*)header.c_str(), aLengthHeader );
//	for ( int i = 0; i < aLengthHeader; ++i )
//		header[i] = i;

    //Prepare out cipher and tag output buffers here
	String cipher( aPlainData.length(), 0 );
    String tag( 16, 0 );

//	printBuffer( "Key", keyDecoded.data(), keyDecoded.size() );
//	printBuffer( "IV", iv.data(), aLengthIV );
	
	//Encrypt
    gcm g;
    gcm_init( &g, (int)keyDecoded.size(), (char*)keyDecoded.c_str(), aLengthIV, (char*)iv.c_str() );
	
//	printBuffer( "Header", header.data(), aLengthHeader );
	
    if ( !gcm_add_header( &g, (char*)header.c_str(), aLengthHeader ) )
		return false;
	
//	printBuffer( "Plain", aPlainData.data(), aPlainData.size() );
	
    if ( !gcm_add_cipher( &g, GCM_ENCRYPTING, (char*)aPlainData.data(), (int)aPlainData.size(), (char*)cipher.data() ) )
		return false;
	
//	printBuffer( "Cipher", cipher.data(), cipher.size() );
	
    gcm_finish( &g, (char*)tag.data() );

//	printBuffer( "Tag", tag.data(), tag.size() );
	
    int totalLength = aLengthHeader + aLengthIV + (int)cipher.length() + 16;
	
	String result;
	result.reserve( totalLength );

	result.append( header.data(), aLengthHeader );
	result.append( iv.data(), aLengthIV );
	result.append( cipher.data(), cipher.size() );
	result.append( tag.data(), 16 );	
	
	CvBase64::Encode( (const uint8_t*)result.data(), (int)result.size(), aCipher );

    return true;
}
