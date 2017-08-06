/* 
 * File:   CvLogger.cpp
 * Author: mony
 * 
 * Created on August 23, 2012, 5:01 PM
 */

#include "CvLogger.h"

#include <string>
#include <map>

#include <stdio.h>
#include <stdarg.h>

using namespace std;

namespace CvShared
{
	
static bool				bInitialized = false;
static string			programName;
static enLogLevel_t		logLevelLimit = enLogLevel_None;

map<enLogLevel_t, int>	mapLogLevelToSyslogLevel;

bool InitLogger( const char* apProgramName, enLogLevel_t aLogLevelLimit, enLogFacility_t aFacility )
{
	programName = apProgramName;
	logLevelLimit = aLogLevelLimit;
	
	mapLogLevelToSyslogLevel[enLogLevel_Fatal] = LOG_CRIT;
	mapLogLevelToSyslogLevel[enLogLevel_Error] = LOG_ERR;
	mapLogLevelToSyslogLevel[enLogLevel_Warning] = LOG_WARNING;
	mapLogLevelToSyslogLevel[enLogLevel_Notice] = LOG_NOTICE;
	mapLogLevelToSyslogLevel[enLogLevel_Info] = LOG_INFO;
	mapLogLevelToSyslogLevel[enLogLevel_Debug1] = LOG_DEBUG;
	mapLogLevelToSyslogLevel[enLogLevel_Debug2] = LOG_DEBUG;
	mapLogLevelToSyslogLevel[enLogLevel_Debug3] = LOG_DEBUG;	
	
	openlog( programName.c_str(), LOG_NDELAY|LOG_PID, aFacility );
	
	bInitialized = true;

	return true;
}

void LogMessage( enLogLevel_t aLogLevel, const char* apFormat, ... )
{
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
	
	if ( mapLogLevelToSyslogLevel.count( aLogLevel ) < 1 )
	{
		printf( "ERROR: LogMessage() called with invalid level [%d]\n", aLogLevel );
		return;
	}
	
	va_list args;
	va_start( args, apFormat );

	vsyslog( mapLogLevelToSyslogLevel[aLogLevel], apFormat, args );
	
	va_end( args );
}

void SetLogLevelLimit( enLogLevel_t aLogLevelLimit )
{
	logLevelLimit = aLogLevelLimit;
}
	
}