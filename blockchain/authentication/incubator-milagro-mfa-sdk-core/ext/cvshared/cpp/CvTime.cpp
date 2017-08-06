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
/*! \file  CvTime.cpp
    \brief Set of classes and functions providing portable time convertion
	       and handling functionality.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 24, 2012, 9:39 AM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 Set of classes and functions providing portable time convertion
 and handling functionality.

*/

#include "CvTime.h"

#ifdef __MACH__
    #include <mach/clock.h>
    #include <mach/mach.h>
#endif

namespace CvShared
{
	
int DateTime::GetDaysInYear( int aYear )
{
	DateTime dateTime;
	
	dateTime.m_day = 0;
	dateTime.m_month = 1;
	dateTime.m_year = aYear+1;
	
	struct tm tmDateTime;
	dateTime.ToTm( tmDateTime );
	
	return tmDateTime.tm_yday+1;
}

int DateTime::GetDaysInMonth( int aMonth, int aYear )
{
	DateTime dateTime;
	
	dateTime.m_day = 0;
	dateTime.m_month = aMonth + 1;
	dateTime.m_year = aYear;
	
	if ( dateTime.m_month > 12 )
	{
		dateTime.m_month = 1;
		++dateTime.m_year;
	}
	
	struct tm tmDateTime;
	dateTime.ToTm( tmDateTime );
	
	return tmDateTime.tm_mday;
}

#if defined(_WIN32)

int clock_gettime( int aDummy, TimeSpec* apTimespec )
{
    static LARGE_INTEGER offset;
    static bool bInitialized = false;
 
	if ( !bInitialized )
	{
		SYSTEMTIME s;
		s.wYear = 1970;
		s.wMonth = 1;
		s.wDay = 1;
		s.wHour = 0;
		s.wMinute = 0;
		s.wSecond = 0;
		s.wMilliseconds = 0;

		FILETIME f;
		SystemTimeToFileTime( &s, &f );

		LARGE_INTEGER offset;
		offset.QuadPart = f.dwHighDateTime;
		offset.QuadPart = (offset.QuadPart << 32) | f.dwLowDateTime;

		bInitialized = true;
	}

    FILETIME f;
	GetSystemTimeAsFileTime( &f );

    LARGE_INTEGER t;
	t.QuadPart = f.dwHighDateTime;
	t.QuadPart = (t.QuadPart << 32) | f.dwLowDateTime;
    t.QuadPart -= offset.QuadPart;

	// t.QuadPart is in 100-nanosecond intervals
	*apTimespec = Microsecs(t.QuadPart/10).ToTimeSpec();

	return 0;
}

#elif defined (__MACH__)
    
int clock_gettime( int aDummy, TimeSpec* apTimespec )
{
    clock_serv_t cclock;
    mach_timespec_t mts;
    
    host_get_clock_service( mach_host_self(), CALENDAR_CLOCK, &cclock );
    
    clock_get_time(cclock, &mts);
    
    mach_port_deallocate(mach_task_self(), cclock);
    
    apTimespec->tv_sec = mts.tv_sec;
    apTimespec->tv_nsec = mts.tv_nsec;
    
    return 0;
}
    
#endif
	
}