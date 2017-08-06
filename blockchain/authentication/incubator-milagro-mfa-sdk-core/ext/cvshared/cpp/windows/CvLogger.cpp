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
•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.																						   *	
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.			   *	
•	Neither the name of CertiVox UK Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.								   *
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,																		   *
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS																	   *
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE																	   *	
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,														   *
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.																		   *	
																																																						   *
***************************************************************************************************************************************************************************************************************************/
/*! \file  CvLogger.cpp
    \brief Portable logging facilities implementation.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 23, 2012, 5:01 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 Simple API for providing portable logging facilities

*/

#include "CvLogger.h"

#include "CvMutex.h"
#include "CvTime.h"

#include <string>
#include <map>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#define ROT_SIZE		10*1024*1024
#define MAX_ROTATIONS	10

#define EOL				"\r\n"

namespace CvShared
{
	
static bool				bInitialized = false;
static string			fileName;
static enLogLevel_t		logLevelLimit = enLogLevel_None;
ofstream				logFile;
CvMutex					mutex("logger");

string	GetTimestamp();
bool	FileExists( const string& filePath );
void	Rotate( int rot = 0 );
string	LevelStr( enLogLevel_t aLevel );

bool InitLogger( const char* apFileName, enLogLevel_t aLogLevelLimit )
{
	if ( bInitialized )
		return true;

	mutex.Create();

	CvMutexLock lock(mutex);

	fileName = apFileName;

	bool bFail = false;

	logFile.open( fileName, ios::out|ios::app|ios::binary );

	if ( logFile.is_open() )
	{
		try
		{
			logFile << "=== Log started: " << GetTimestamp() << " ===" << EOL;
			logFile.flush();
		}
		catch ( exception& e )
		{
			bFail = true;
			cout << "Exception in InitLogger(): " + string(e.what()) << EOL;
		}
	}
	else
		bFail = true;

	if (bFail)
	{
		cout << "ERROR writing to log file: " << fileName << EOL;
	}

	logLevelLimit = aLogLevelLimit;

	if ( logLevelLimit == enLogLevel_None )
		logFile.close();

	bInitialized = true;

	return true;
}

void Rotate( int rot )
{
	logFile.close();

	if ( rot == MAX_ROTATIONS )
	{
		remove( fileName.c_str() );
	}
	else 
	{
		char rotbuf[8] = {'\0'};

		_itoa_s( rot + 1, rotbuf, 8, 10 );

		if ( rot < MAX_ROTATIONS-1 && FileExists( fileName + "." + rotbuf ) ) 
			Rotate( rot+1 );
		
		_itoa_s( rot, rotbuf, 8, 10 );
		string old = fileName;
		if ( rot > 0 )
		{
			old += '.';
			old += rotbuf;
		}

		_itoa_s( rot + 1, rotbuf, 8, 10 );
		string neew = fileName + '.' + string(rotbuf);

		remove( neew.c_str() );
		rename( old.c_str(), neew.c_str() );
	}

	if (rot == 0)
		logFile.open( fileName, ios::out|ios::app|ios::binary );
}

void LogMessage( enLogLevel_t aLogLevel, const char* apFormat, ... )
{
	CvMutexLock lock(mutex);

	if ( !bInitialized )
	{
		printf( "WARNING: Logger not initialized yet\n" );
		
		va_list args;
		va_start( args, apFormat );
		
		vprintf( apFormat, args );
		printf( "\n" );
		
		va_end( args );
		return;
	}
	
	if ( aLogLevel == enLogLevel_None || aLogLevel > logLevelLimit )
		return;
	
	if ( !logFile.is_open() )
		return;

	logFile.seekp( 0, ios::end );

	if ( logFile.tellp() >= ROT_SIZE )
		Rotate();

	char thrId[64] = {'\0'};
	_i64toa_s( (long long)GetCurrentThreadId(), thrId, sizeof(thrId)-1, 10 );

	char buf[1024] = {'\0'};

	va_list args;
	va_start( args, apFormat );

	vsnprintf_s( buf, sizeof(buf)-1, _TRUNCATE, apFormat, args );

	va_end( args );

	logFile << GetTimestamp() << " [" << thrId << "] " << LevelStr(aLogLevel) << " " << buf << EOL;
	logFile.flush();
}

void SetLogLevelLimit( enLogLevel_t aLogLevelLimit )
{
	CvMutexLock lock(mutex);

	if ( logLevelLimit == enLogLevel_None && aLogLevelLimit != enLogLevel_None )
	{
		logFile.open( fileName, ios::out|ios::app|ios::binary );
	}
	else
	if ( logLevelLimit != enLogLevel_None && aLogLevelLimit == enLogLevel_None )
	{
		logFile.close();
	}

	logLevelLimit = aLogLevelLimit;
}

string LevelStr( enLogLevel_t aLevel )
{
	switch( aLevel )
	{
		case enLogLevel_Debug3 :	return "DEV  ";
		case enLogLevel_Debug2 :	return "TRACE";
		case enLogLevel_Debug1 :	return "DEBUG";
		case enLogLevel_Info :		return "INFO ";
		case enLogLevel_Notice :	return "NOTIC";
		case enLogLevel_Warning  :	return "WARN ";
		case enLogLevel_Error :		return "ERROR";
		case enLogLevel_Fatal :		return "FATAL";
	}

	return "?????";
}

string GetTimestamp()
{
	DateTime dateTime;
	GetCurrentDateTime( dateTime );

	char timebuf[64] = {'\0'};

	dateTime.Format( timebuf, sizeof(timebuf)-1, "%Y-%m-%dT%H:%M:%S" );

    return timebuf;
}

bool FileExists( const string& filePath )
{
	struct __stat64 buf;
	return ( _stat64( filePath.c_str(), &buf ) == 0 );
}

}	//namespace