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
/*! \file  CvString.h
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

#ifndef CVSTRING_H
#define	CVSTRING_H

#include "CvCommon.h"

#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
	#define STRCASECMP	_stricmp
	#define STRNCASECMP	_strnicmp
	#define SPRINTF		sprintf_s
#else
	#define STRCASECMP	strcasecmp
	#define STRNCASECMP	strncasecmp
	#define SPRINTF		sprintf
#endif

class CvString : public std::string
{
public:
	typedef std::string				String;
	typedef std::vector<CvString>	CStringVector;
	
	CvString()	{}
	CvString( const String& aString ) : String(aString)	{}
	CvString( const char* apString ) : String(apString)	{}
	CvString( const CvString& aString ) : String(aString)	{}
	CvString( const String& aString, size_t aPos, size_t aSize = npos ) : String(aString,aPos,aSize)	{}
	CvString( const char* apString, size_t aSize ) : String(apString,aSize)	{}
	CvString( size_t aSize, char aChar ) : String(aSize,aChar)	{}
	
	CvString( uint32_t aUint )	{ *this = aUint; }
	CvString( long aInt )	{ *this = aInt; }
	
	CvString&	Format( const char* apFormat, ... );
	CvString&	TrimLeft( const String& aChars = " \t\f\v\n\r" );
	CvString&	TrimRight( const String& aChars = " \t\f\v\n\r" );
	int			ReplaceAll( const String& aPattern, const String& aReplacement );
	void		Tokenize( const String& aDelimiters, OUT CStringVector& aTokens ) const;
	
	inline long		Long( int aBase = 10 ) const;
	inline uint32_t	Ulong( int aBase = 10 ) const;
	
	inline int		CompareNoCase( const String& aOther ) const;
	inline int		CompareNoCase( const String& aOther, size_t n ) const;
	
	inline CvString&	operator=( uint32_t aUint );
	inline CvString&	operator=( long aInt );
	
private:

};

long CvString::Long( int aBase ) const
{
	char* pEnd;
	return strtol( c_str(), &pEnd, aBase );
}

uint32_t CvString::Ulong( int aBase ) const
{
	char* pEnd;
	return (uint32_t)strtoul( c_str(), &pEnd, aBase );
}

int CvString::CompareNoCase( const String& aOther ) const
{
	return STRCASECMP( c_str(), aOther.c_str() );
}

int CvString::CompareNoCase( const String& aOther, size_t n ) const
{
	return STRNCASECMP( c_str(), aOther.c_str(), n );	
}
	
CvString& CvString::operator=( uint32_t aUint )
{
	char str[16];
	SPRINTF( str, "%u", aUint );
	*this = str;
    
    return *this;
}

CvString& CvString::operator=( long aInt )
{
	char str[16];
	SPRINTF( str, "%ld", aInt );
	*this = str;
    
    return *this;
}

std::wstring StringToWstring( const std::string& str );
std::string WstringToString( const std::wstring& str );

#endif	/* CVSTRING_H */

