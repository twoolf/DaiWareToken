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
/*! \file  CvXcode.cpp
    \brief Base64 and Hex Encoding/Decoding facilities.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 30, 2012, 4:43 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 Base64 and Hex Encoding/Decoding facilities.

*/

#include "CvXcode.h"

using namespace std;

namespace CvShared
{

static const string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool IsBase64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

void CvBase64::Encode( const uint8_t* apBytesToEncode, int aLen, OUT std::string& aTarget )
{
	int i = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	aTarget.clear();
	
	while( aLen-- )
	{
		char_array_3[i++] = *(apBytesToEncode++);
		if( i == 3 )
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for( i = 0; i < 4; i++ )
				aTarget += base64_chars[char_array_4[i]];
			
			i = 0;
		}
	}

	if( i )
	{
		int j = 0;
		
		for( j = i; j < 3; j++ )
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for( j = 0; j < i + 1; j++ )
			aTarget += base64_chars[char_array_4[j]];

		while( i++ < 3 )
			aTarget += '=';
	}
}

void CvBase64::Decode( const string& aSource, OUT string& aTarget )
{
	int in_len = (int)aSource.size();
	int i = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];

	aTarget.clear();
	
	while( in_len-- && aSource[in_] != '=' && IsBase64(aSource[in_]) )
	{
		char_array_4[i++] = aSource[in_];
		in_++;
		if( i == 4 )
		{
			for( i = 0; i < 4; i++ )
				char_array_4[i] = (unsigned char)base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for( i = 0; i < 3; i++ )
				aTarget += char_array_3[i];
			
			i = 0;
		}
	}

	if( i )
	{
		int j = 0;
		
		for( j = i; j < 4; j++ )
			char_array_4[j] = 0;

		for( j = 0; j < 4; j++ )
			char_array_4[j] = (unsigned char)base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for( j = 0; j < i - 1; j++ )
			aTarget += char_array_3[j];
	}
}

static const char* hex_chars = "0123456789abcdef";

void CvHex::Encode( const uint8_t* apBytesToEncode, int aLen, OUT string& aTarget )
{
	aTarget.clear();

	for ( int i = 0; i < aLen; i++ )
	{
		char c1 = hex_chars[ apBytesToEncode[i] >> 4 ];		// High nibble
		char c2 = hex_chars[ apBytesToEncode[i] & 0x0F ];	// Low nibble	

		aTarget += c1;
		aTarget += c2;		
	}
}

void CvHex::Decode( const string& aSource, OUT string& aTarget )
{
	aTarget.resize( aSource.length()/2 );
	
	for ( size_t i = 0; i < aSource.length(); ++i )
	{
		// High nibble		
		char c = tolower( aSource[i] );
		uint8_t nibble = 0;
		
		switch( c )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nibble = c - '0';
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				nibble = c - 'a' + 0xA;
				break;
		}

		if ( i%2 == 0 )
			aTarget[i/2] = ( nibble << 4 );
		else
			aTarget[i/2] |= nibble;
	}
}

}
