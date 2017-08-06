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
/*! \file  CvString.cpp
    \brief C++ class which extends the std::string with useful functionality.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 27, 2012, 10:49 AM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : STL std::string

 C++ class which extends the std::string with useful functionality.
 The CvString doesn't add any members to the std::string base, so it
 is binary compatible with it.

*/

#include "CvString.h"

#include <stdarg.h>

#ifdef _WIN32
	#include <windows.h>
#endif

CvString& CvString::Format( const char* apFormat, ... )
{
#ifdef _WIN32

	va_list args;
	va_start( args, apFormat );

	int len = _vscprintf( apFormat, args );

	reserve(len+1);
	resize(len);

	vsnprintf_s( (char*)data(), len+1, _TRUNCATE, apFormat, args );

	va_end( args );

#elif defined (__MACH__) || defined(ANDROID)
    
    char* pFormattedStr = NULL;
    
	va_list args;
	va_start( args, apFormat );

    vasprintf( &pFormattedStr, apFormat, args );
    
    va_end( args );
    
	*this = pFormattedStr;
    
    free( pFormattedStr );
    
#else	//linux

	char* pFormattedStr = NULL;
	size_t pFormattedSize = 0;

	FILE* pFormattedFd = open_memstream( &pFormattedStr, &pFormattedSize );
	if ( pFormattedFd != NULL )
	{
		va_list args;
		va_start( args, apFormat );

		vfprintf( pFormattedFd, apFormat, args );

		va_end( args );

		fclose( pFormattedFd );

		*this = pFormattedStr;
	}

#endif

	return *this;
}

CvString& CvString::TrimLeft( const String& aChars )
{
	size_t found = find_first_not_of( aChars );

	if ( found != npos )
		erase(0,found);
	else
		clear();
    
    return *this;
}

CvString& CvString::TrimRight( const String& aChars )
{
	size_t found = find_last_not_of( aChars );

	if ( found != npos )
		erase(found+1);
	else
		clear();
    
    return *this;
}

int CvString::ReplaceAll( const String& aPattern, const String& aReplacement )
{
	if ( aPattern.empty() )
		return 0;

	size_t patternLen = aPattern.length();
	size_t replaceLen = aReplacement.length();
	int count = 0;
	
	size_t pos = find( aPattern );

	while ( pos != npos )
	{
		replace( pos, patternLen, aReplacement );
		++count;
		pos = find( aPattern, pos + replaceLen );
	}

	return count;
}

void CvString::Tokenize( const String& aDelimiters, OUT CStringVector& aTokens ) const
{
	aTokens.clear();

	size_t posStart = 0;
	size_t posEnd = find_first_of( aDelimiters );

	while ( posEnd != npos )
	{
		aTokens.push_back( substr( posStart, posEnd - posStart ) );
		
		posStart = find_first_not_of( aDelimiters, posEnd );
		
		if ( posStart != npos )
			posEnd = find_first_of( aDelimiters, posStart );
		else
			posEnd = npos;
	}
	
	if ( posStart != npos )
	{
		aTokens.push_back( substr( posStart ) );		
	}
}

using namespace std;

wstring StringToWstring( const string& str )
{
#ifdef _WIN32
	int n = (int)str.size();
	wstring wstr( n+1, L'\0' );
	MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, (LPWSTR)wstr.data(), n+1 );
	wstr.resize( n );
	return wstr;
#else
	return wstring( str.begin(), str.end() );
#endif
}

string WstringToString( const wstring& wstr )
{
#ifdef _WIN32	
	int n = (int)wstr.size();
	string str( n+1, '\0' );
	WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, (LPSTR)str.data(), n+1, NULL, NULL );
	str.resize( n );
	return str;
#else
	return string( wstr.begin(), wstr.end() );	
#endif
}

