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
/*! \file  CvHttpCommon.h
    \brief Common HTTP-related definitions and functionalities.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 28, 2012, 4:30 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 Common HTTP-related definitins and functionalities.

*/

#ifndef CVHTTPCOMMON_H
#define	CVHTTPCOMMON_H

#include "CvString.h"

#include <map>

enum enHttpMethod_t
{
	enHttpMethod_Unknown = -1,
	enHttpMethod_GET,
	enHttpMethod_PUT,
	enHttpMethod_POST,
	enHttpMethod_DEL,
	enHttpMethod_HEAD
};

inline std::string HttpMethodEnumToString( enHttpMethod_t aMethod )
{
	switch( aMethod )
	{
		case enHttpMethod_POST:
			return "POST";
		case enHttpMethod_GET:
			return "GET";
		case enHttpMethod_PUT:
			return "PUT";
		case enHttpMethod_DEL:
			return "DELETE";
		case enHttpMethod_HEAD:
			return "HEAD";
		default:
			break;
	}

	return "";
}

inline enHttpMethod_t HttpMethodStringToEnum( const std::string& aMethod )
{
	if ( aMethod == "POST" )
		return enHttpMethod_POST;
	else
	if ( aMethod == "GET" )
		return enHttpMethod_GET;	
	else
	if ( aMethod == "PUT" )
		return enHttpMethod_PUT;	
	else
	if ( aMethod == "DELETE" )
		return enHttpMethod_DEL;	
	else
	if ( aMethod == "HEAD" )
		return enHttpMethod_HEAD;	

	return enHttpMethod_Unknown;
}

typedef std::map<CvString,CvString>	CMapHttpHeaders;	// Maps Header name to value

#define HTTP_HEADER_CONTENT_TYPE	"Content-Type"
#define HTTP_HEADER_CONTENT_LENGTH	"Content-Length"

#endif	/* CVHTTPCOMMON_H */

