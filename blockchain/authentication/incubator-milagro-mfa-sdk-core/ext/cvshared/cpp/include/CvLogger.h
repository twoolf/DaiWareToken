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
/*! \file  CvLogger.h
    \brief Simple API for providing portable logging facilities

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 23, 2012, 5:01 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 Simple API for providing portable logging facilities

*/

#ifndef CVLOGGER_H
#define	CVLOGGER_H

#if defined(__linux__) || defined(__MACH__)
    #include <syslog.h>
#endif

namespace CvShared
{
	
enum enLogLevel_t
{
	enLogLevel_None = 0,
	enLogLevel_Fatal = 1,
	enLogLevel_Error,
	enLogLevel_Warning,
	enLogLevel_Notice,
	enLogLevel_Info,
	enLogLevel_Debug1,
	enLogLevel_Debug2,
	enLogLevel_Debug3
};

#if defined (_WIN32)

	bool InitLogger( const char* apFileName, enLogLevel_t aLogLevelLimit );

#elif defined (__linux__) || defined(__MACH__)

	enum enLogFacility_t
	{
		enLogFacility_None = 0,
		enLogFacility_User = LOG_USER,
		enLogFacility_Local0 = LOG_LOCAL0,
		enLogFacility_Local1 = LOG_LOCAL1,
		enLogFacility_Local2 = LOG_LOCAL2,
		enLogFacility_Local3 = LOG_LOCAL3,
		enLogFacility_Local4 = LOG_LOCAL4,
		enLogFacility_Local5 = LOG_LOCAL5,
		enLogFacility_Local6 = LOG_LOCAL6,
		enLogFacility_Local7 = LOG_LOCAL7	
	};

	bool InitLogger( const char* apProgramName, enLogLevel_t aLogLevelLimit, enLogFacility_t aFacility = enLogFacility_Local0 );

#endif

void LogMessage( enLogLevel_t aLogLevel, const char* apFormat, ... );
void SetLogLevelLimit( enLogLevel_t aLogLevelLimit );

}

#endif	/* CVLOGGER_H */

