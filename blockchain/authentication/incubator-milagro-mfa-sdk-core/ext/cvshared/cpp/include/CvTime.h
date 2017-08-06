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
/*! \file  CvTime.h
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

#ifndef CVTIME_H
#define	CVTIME_H

#include "CvCommon.h"

#include <time.h>

#if defined(_WIN32)

	#include "windows.h"

	#define CLOCK_REALTIME	0

	namespace CvShared
	{
		class TimeSpec;
	}

	int clock_gettime( int aDummy, CvShared::TimeSpec* apTimespec );

	#define suseconds_t		long

	#undef GetCurrentTime

#elif defined(__linux__)

	#include <sys/time.h>
	#include <errno.h>
	#include <unistd.h>

#elif defined (__MACH__)

    #include <sys/time.h>
    #include <errno.h>
	#include <unistd.h>

	#define CLOCK_REALTIME	0

    namespace CvShared
    {
        class TimeSpec;
    }

    int clock_gettime( int aDummy, CvShared::TimeSpec* apTimespec );

#else

    #error "Unsupported OS"

#endif

#include <string>

namespace CvShared
{

typedef time_t	TimeValue_t;

#define _e_1	(10)
#define _e_3	(_e_1 * _e_1 * _e_1)
#define _e_6	(_e_3 * _e_3)
#define _e_9	(_e_3 * _e_3 * _e_3)

class Millisecs;
class Microsecs;
class Seconds;
class Minutes;
class Hours;
class Days;
class TimeSpec;
class TimeVal;

class Nanosecs
{
public:
	inline Nanosecs();
	inline Nanosecs( TimeValue_t aValue );
	inline Nanosecs( const Microsecs& aMicrosecs );
	inline Nanosecs( const Millisecs& aMillisecs );
	inline Nanosecs( const Seconds& aSeconds );
	inline Nanosecs( const Minutes& aMinutes );
	inline Nanosecs( const Hours& aHours );
	inline Nanosecs( const Days& aDays );
	inline Nanosecs( const TimeSpec& aTimespec );
	inline Nanosecs( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;
	
	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Nanosecs& aOther ) const;
	inline bool	operator!=( const Nanosecs& aOther ) const;	
	inline bool	operator<( const Nanosecs& aOther ) const;
	inline bool	operator>( const Nanosecs& aOther ) const;
	
private:
	TimeValue_t	m_value;
};

class Microsecs
{
public:
	inline Microsecs();
	inline Microsecs( TimeValue_t aValue );
	inline Microsecs( const Nanosecs& aNanosecs );
	inline Microsecs( const Millisecs& aMillisecs );
	inline Microsecs( const Seconds& aSeconds );
	inline Microsecs( const Minutes& aMinutes );
	inline Microsecs( const Hours& aHours );
	inline Microsecs( const Days& aDays );
	inline Microsecs( const TimeSpec& aTimespec );
	inline Microsecs( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;

	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Microsecs& aOther ) const;
	inline bool	operator!=( const Microsecs& aOther ) const;
	inline bool	operator<( const Microsecs& aOther ) const;
	inline bool	operator>( const Microsecs& aOther ) const;
	
private:
	TimeValue_t	m_value;
};
		
class Millisecs
{
public:
	inline Millisecs();
	inline Millisecs( TimeValue_t aValue );
	inline Millisecs( const Nanosecs& aNanosecs );
	inline Millisecs( const Microsecs& aMicrosecs );
	inline Millisecs( const Seconds& aSeconds );
	inline Millisecs( const Minutes& aMinutes );
	inline Millisecs( const Hours& aHours );
	inline Millisecs( const Days& aDays );
	inline Millisecs( const TimeSpec& aTimespec );
	inline Millisecs( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;

	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Millisecs& aOther ) const;
	inline bool	operator!=( const Millisecs& aOther ) const;
	inline bool	operator<( const Millisecs& aOther ) const;
	inline bool	operator>( const Millisecs& aOther ) const;
	
private:
	TimeValue_t	m_value;
};

class Seconds
{
public:
	inline Seconds();
	inline Seconds( TimeValue_t aValue );
	inline Seconds( const Nanosecs& aNanosecs );
	inline Seconds( const Microsecs& aMicrosecs );
	inline Seconds( const Millisecs& aMillisecs );
	inline Seconds( const Minutes& aMinutes );
	inline Seconds( const Hours& aHours );
	inline Seconds( const Days& aDays );
	inline Seconds( const TimeSpec& aTimespec );
	inline Seconds( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;

	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Seconds& aOther ) const;
	inline bool	operator!=( const Seconds& aOther ) const;
	inline bool	operator<( const Seconds& aOther ) const;
	inline bool	operator>( const Seconds& aOther ) const;
	
private:
	TimeValue_t	m_value;
};

class Minutes
{
public:
	inline Minutes();
	inline Minutes( TimeValue_t aValue );
	inline Minutes( const Nanosecs& aNanosecs );
	inline Minutes( const Microsecs& aMicrosecs );
	inline Minutes( const Millisecs& aMillisecs );
	inline Minutes( const Seconds& aSeconds );
	inline Minutes( const Hours& aHours );
	inline Minutes( const Days& aDays );
	inline Minutes( const TimeSpec& aTimespec );
	inline Minutes( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;

	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Minutes& aOther ) const;
	inline bool	operator!=( const Minutes& aOther ) const;
	inline bool	operator<( const Minutes& aOther ) const;
	inline bool	operator>( const Minutes& aOther ) const;
	
private:
	TimeValue_t	m_value;
};

class Hours
{
public:	
	inline Hours();
	inline Hours( TimeValue_t aValue );
	inline Hours( const Nanosecs& aNanosecs );
	inline Hours( const Microsecs& aMicrosecs );
	inline Hours( const Millisecs& aMillisecs );
	inline Hours( const Seconds& aSeconds );
	inline Hours( const Minutes& aMinutes );
	inline Hours( const Days& aDays );
	inline Hours( const TimeSpec& aTimespec );
	inline Hours( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToDays() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;

	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Hours& aOther ) const;
	inline bool	operator!=( const Hours& aOther ) const;	
	inline bool	operator<( const Hours& aOther ) const;
	inline bool	operator>( const Hours& aOther ) const;
	
private:
	TimeValue_t	m_value;
};

class Days
{
public:
	inline Days();
	inline Days( TimeValue_t aValue );
	inline Days( const Nanosecs& aNanosecs );
	inline Days( const Microsecs& aMicrosecs );
	inline Days( const Millisecs& aMillisecs );
	inline Days( const Seconds& aSeconds );
	inline Days( const Minutes& aMinutes );
	inline Days( const Hours& aHours );
	inline Days( const TimeSpec& aTimespec );
	inline Days( const TimeVal& aTimeval );	
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeSpec		ToTimeSpec() const;
	inline TimeVal		ToTimeVal() const;

	TimeValue_t Value() const		{ return m_value; }
	
	inline bool	operator==( const Days& aOther ) const;
	inline bool	operator!=( const Days& aOther ) const;
	inline bool	operator<( const Days& aOther ) const;
	inline bool	operator>( const Days& aOther ) const;
	
private:
	TimeValue_t	m_value;
};

class TimeSpec : public timespec
{
public:
	inline TimeSpec();
	inline TimeSpec( const struct timespec& aTimespec );
	inline TimeSpec( const Nanosecs& aNanosecs );
	inline TimeSpec( const Microsecs& aMicrosecs );
	inline TimeSpec( const Millisecs& aMillisecs );
	inline TimeSpec( const Seconds& aSeconds );
	inline TimeSpec( const Minutes& aMinutes );
	inline TimeSpec( const Hours& aHours );
	inline TimeSpec( const Days& aDays );
	inline TimeSpec( const TimeVal& aTimeval );
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;	
	inline TimeVal		ToTimeVal() const;
	
	inline bool			operator==( const TimeSpec& aOther ) const;
	inline bool			operator!=( const TimeSpec& aOther ) const;	
	inline bool			operator<( const TimeSpec& aOther ) const;
	inline bool			operator>( const TimeSpec& aOther ) const;
	inline TimeSpec		operator+( const TimeSpec& aOther ) const;
	inline TimeSpec&	operator+=( const TimeSpec& aOther );	
	inline TimeSpec		operator-( const TimeSpec& aOther ) const;
	inline TimeSpec&	operator-=( const TimeSpec& aOther );
};

class TimeVal : public timeval
{
public:
	inline TimeVal();
	inline TimeVal( const struct timeval& aTimeval );	
	inline TimeVal( const Nanosecs& aNanosecs )	;
	inline TimeVal( const Microsecs& aMicrosecs );
	inline TimeVal( const Millisecs& aMillisecs );
	inline TimeVal( const Seconds& aSeconds );
	inline TimeVal( const Minutes& aMinutes );
	inline TimeVal( const Hours& aHours );
	inline TimeVal( const Days& aDays )	;
	inline TimeVal( const TimeSpec& aTimespec );
	
	inline TimeValue_t	ToNanosecs() const;
	inline TimeValue_t	ToMicrosecs() const;
	inline TimeValue_t	ToMillisecs() const;
	inline TimeValue_t	ToSeconds() const;
	inline TimeValue_t	ToMinutes() const;
	inline TimeValue_t	ToHours() const;
	inline TimeValue_t	ToDays() const;	
	inline TimeSpec		ToTimeSpec() const;

	inline bool			operator==( const TimeVal& aOther ) const;
	inline bool			operator!=( const TimeVal& aOther ) const;	
	inline bool			operator<( const TimeVal& aOther ) const;
	inline bool			operator>( const TimeVal& aOther ) const;
	inline TimeVal		operator+( const TimeVal& aOther ) const;
	inline TimeVal&		operator+=( const TimeVal& aOther );	
	inline TimeVal		operator-( const TimeVal& aOther ) const;
	inline TimeVal&		operator-=( const TimeVal& aOther );		
	
};

Nanosecs::Nanosecs()								{ m_value = 0; }
Nanosecs::Nanosecs( TimeValue_t aValue )			{ m_value = aValue; }
Nanosecs::Nanosecs( const Microsecs& aMicrosecs )	{ m_value = aMicrosecs.Value() * _e_3; }
Nanosecs::Nanosecs( const Millisecs& aMillisecs )	{ m_value = aMillisecs.Value() * _e_6; }
Nanosecs::Nanosecs( const Seconds& aSeconds )		{ m_value = aSeconds.Value() * _e_9; }
Nanosecs::Nanosecs( const Minutes& aMinutes )		{ m_value = aMinutes.Value() * _e_9 * 60; }	
Nanosecs::Nanosecs( const Hours& aHours )			{ m_value = aHours.Value() * _e_9 * 60 * 60; }
Nanosecs::Nanosecs( const Days& aDays )				{ m_value = aDays.Value() * _e_9 * 60 * 60 * 24; }
Nanosecs::Nanosecs( const TimeSpec& aTimespec )		{ m_value = aTimespec.ToNanosecs(); }
Nanosecs::Nanosecs( const TimeVal& aTimeval )		{ m_value = aTimeval.ToNanosecs(); }

TimeValue_t Nanosecs::ToMicrosecs() const			{ return Microsecs(*this).Value(); }	
TimeValue_t	Nanosecs::ToMillisecs() const			{ return Millisecs(*this).Value(); }	
TimeValue_t	Nanosecs::ToSeconds() const				{ return Seconds(*this).Value(); }	
TimeValue_t	Nanosecs::ToMinutes() const				{ return Minutes(*this).Value(); }
TimeValue_t	Nanosecs::ToHours() const				{ return Hours(*this).Value(); }
TimeValue_t	Nanosecs::ToDays() const				{ return Days(*this).Value(); }
TimeSpec Nanosecs::ToTimeSpec() const				{ return TimeSpec(*this); }
TimeVal Nanosecs::ToTimeVal() const					{ return TimeVal(*this); }

bool Nanosecs::operator==( const Nanosecs& aOther ) const	{ return m_value == aOther.m_value; }
bool Nanosecs::operator!=( const Nanosecs& aOther ) const	{ return m_value != aOther.m_value; }
bool Nanosecs::operator<( const Nanosecs& aOther ) const	{ return m_value < aOther.m_value; }
bool Nanosecs::operator>( const Nanosecs& aOther ) const	{ return m_value > aOther.m_value; }

Microsecs::Microsecs()								{ m_value = 0; }
Microsecs::Microsecs( TimeValue_t aValue )			{ m_value = aValue; }
Microsecs::Microsecs( const Nanosecs& aNanosecs )	{ m_value = aNanosecs.Value() / _e_3; }
Microsecs::Microsecs( const Millisecs& aMillisecs )	{ m_value = aMillisecs.Value() * _e_3; }
Microsecs::Microsecs( const Seconds& aSeconds )		{ m_value = aSeconds.Value() * _e_6; }
Microsecs::Microsecs( const Minutes& aMinutes )		{ m_value = aMinutes.Value() * _e_6 * 60; }	
Microsecs::Microsecs( const Hours& aHours )			{ m_value = aHours.Value() * _e_6 * 60 * 60; }
Microsecs::Microsecs( const Days& aDays )			{ m_value = aDays.Value() * _e_6 * 60 * 60 * 24; }
Microsecs::Microsecs( const TimeSpec& aTimespec )	{ m_value = aTimespec.ToMicrosecs(); }
Microsecs::Microsecs( const TimeVal& aTimeval )		{ m_value = aTimeval.ToMicrosecs(); }

TimeValue_t Microsecs::ToNanosecs() const			{ return Nanosecs(*this).Value(); }	
TimeValue_t	Microsecs::ToMillisecs() const			{ return Millisecs(*this).Value(); }	
TimeValue_t	Microsecs::ToSeconds() const			{ return Seconds(*this).Value(); }	
TimeValue_t	Microsecs::ToMinutes() const			{ return Minutes(*this).Value(); }
TimeValue_t	Microsecs::ToHours() const				{ return Hours(*this).Value(); }
TimeValue_t	Microsecs::ToDays() const				{ return Days(*this).Value(); }
TimeSpec Microsecs::ToTimeSpec() const				{ return TimeSpec(*this); }
TimeVal Microsecs::ToTimeVal() const				{ return TimeVal(*this); }

bool Microsecs::operator==( const Microsecs& aOther ) const	{ return m_value == aOther.m_value; }
bool Microsecs::operator!=( const Microsecs& aOther ) const	{ return m_value != aOther.m_value; }
bool Microsecs::operator<( const Microsecs& aOther ) const	{ return m_value < aOther.m_value; }
bool Microsecs::operator>( const Microsecs& aOther ) const	{ return m_value > aOther.m_value; }

Millisecs::Millisecs()								{ m_value = 0; }
Millisecs::Millisecs( TimeValue_t aValue )			{ m_value = aValue; }
Millisecs::Millisecs( const Nanosecs& aNanosecs )	{ m_value = aNanosecs.Value() / _e_6; }
Millisecs::Millisecs( const Microsecs& aMicrosecs )	{ m_value = aMicrosecs.Value() / _e_3; }
Millisecs::Millisecs( const Seconds& aSeconds )		{ m_value = aSeconds.Value() * _e_3; }
Millisecs::Millisecs( const Minutes& aMinutes )		{ m_value = aMinutes.Value() * _e_3 * 60; }	
Millisecs::Millisecs( const Hours& aHours )			{ m_value = aHours.Value() * _e_3 * 60 * 60; }
Millisecs::Millisecs( const Days& aDays )			{ m_value = aDays.Value() * _e_3 * 60 * 60 * 24; }
Millisecs::Millisecs( const TimeSpec& aTimespec )	{ m_value = aTimespec.ToMillisecs(); }
Millisecs::Millisecs( const TimeVal& aTimeval )		{ m_value = aTimeval.ToMillisecs(); }

TimeValue_t	Millisecs::ToNanosecs() const			{ return Nanosecs(*this).Value(); }	
TimeValue_t Millisecs::ToMicrosecs() const			{ return Microsecs(*this).Value(); }	
TimeValue_t	Millisecs::ToSeconds() const			{ return Seconds(*this).Value(); }	
TimeValue_t	Millisecs::ToMinutes() const			{ return Minutes(*this).Value(); }
TimeValue_t	Millisecs::ToHours() const				{ return Hours(*this).Value(); }
TimeValue_t	Millisecs::ToDays() const				{ return Days(*this).Value(); }
TimeSpec Millisecs::ToTimeSpec() const				{ return TimeSpec(*this); }
TimeVal Millisecs::ToTimeVal() const				{ return TimeVal(*this); }

bool Millisecs::operator==( const Millisecs& aOther ) const	{ return m_value == aOther.m_value; }
bool Millisecs::operator!=( const Millisecs& aOther ) const	{ return m_value != aOther.m_value; }
bool Millisecs::operator<( const Millisecs& aOther ) const	{ return m_value < aOther.m_value; }
bool Millisecs::operator>( const Millisecs& aOther ) const	{ return m_value > aOther.m_value; }

Seconds::Seconds()									{ m_value = 0; }
Seconds::Seconds( TimeValue_t aValue )				{ m_value = aValue; }
Seconds::Seconds( const Nanosecs& aNanosecs )		{ m_value = aNanosecs.Value() / _e_9; }
Seconds::Seconds( const Microsecs& aMicrosecs )		{ m_value = aMicrosecs.Value() / _e_6; }
Seconds::Seconds( const Millisecs& aMillisecs )		{ m_value = aMillisecs.Value() / _e_3; }
Seconds::Seconds( const Minutes& aMinutes )			{ m_value = aMinutes.Value() * 60; }	
Seconds::Seconds( const Hours& aHours )				{ m_value = aHours.Value() * 60 * 60; }
Seconds::Seconds( const Days& aDays )				{ m_value = aDays.Value() * 60 * 60 * 24; }
Seconds::Seconds( const TimeSpec& aTimespec )		{ m_value = aTimespec.ToSeconds(); }
Seconds::Seconds( const TimeVal& aTimeval )			{ m_value = aTimeval.ToSeconds(); }

TimeValue_t	Seconds::ToNanosecs() const				{ return Nanosecs(*this).Value(); }
TimeValue_t Seconds::ToMicrosecs() const			{ return Microsecs(*this).Value(); }	
TimeValue_t	Seconds::ToMillisecs() const			{ return Millisecs(*this).Value(); }	
TimeValue_t	Seconds::ToMinutes() const				{ return Minutes(*this).Value(); }
TimeValue_t	Seconds::ToHours() const				{ return Hours(*this).Value(); }
TimeValue_t	Seconds::ToDays() const					{ return Days(*this).Value(); }
TimeSpec Seconds::ToTimeSpec() const				{ return TimeSpec(*this); }
TimeVal Seconds::ToTimeVal() const					{ return TimeVal(*this); }

bool Seconds::operator==( const Seconds& aOther ) const	{ return m_value == aOther.m_value; }
bool Seconds::operator!=( const Seconds& aOther ) const	{ return m_value != aOther.m_value; }
bool Seconds::operator<( const Seconds& aOther ) const	{ return m_value < aOther.m_value; }
bool Seconds::operator>( const Seconds& aOther ) const	{ return m_value > aOther.m_value; }

Minutes::Minutes()									{ m_value = 0; }
Minutes::Minutes( TimeValue_t aValue )				{ m_value = aValue; }
Minutes::Minutes( const Nanosecs& aNanosecs )		{ m_value = 0; }
Minutes::Minutes( const Microsecs& aMicrosecs )		{ m_value = aMicrosecs.Value() / (_e_6 * 60); }
Minutes::Minutes( const Millisecs& aMillisecs )		{ m_value = aMillisecs.Value() / (_e_3 * 60); }
Minutes::Minutes( const Seconds& aSeconds )			{ m_value = aSeconds.Value() / 60; }	
Minutes::Minutes( const Hours& aHours )				{ m_value = aHours.Value() * 60; }
Minutes::Minutes( const Days& aDays )				{ m_value = aDays.Value() * 60 * 24; }
Minutes::Minutes( const TimeSpec& aTimespec )		{ m_value = aTimespec.ToMinutes(); }
Minutes::Minutes( const TimeVal& aTimeval )			{ m_value = aTimeval.ToMinutes(); }

TimeValue_t	Minutes::ToNanosecs() const				{ return Nanosecs(*this).Value(); }
TimeValue_t Minutes::ToMicrosecs() const			{ return Microsecs(*this).Value(); }	
TimeValue_t	Minutes::ToMillisecs() const			{ return Millisecs(*this).Value(); }	
TimeValue_t	Minutes::ToSeconds() const				{ return Seconds(*this).Value(); }	
TimeValue_t	Minutes::ToHours() const				{ return Hours(*this).Value(); }
TimeValue_t	Minutes::ToDays() const					{ return Days(*this).Value(); }
TimeSpec Minutes::ToTimeSpec() const				{ return TimeSpec(*this); }
TimeVal Minutes::ToTimeVal() const					{ return TimeVal(*this); }

bool Minutes::operator==( const Minutes& aOther ) const	{ return m_value == aOther.m_value; }
bool Minutes::operator!=( const Minutes& aOther ) const	{ return m_value != aOther.m_value; }
bool Minutes::operator<( const Minutes& aOther ) const	{ return m_value < aOther.m_value; }
bool Minutes::operator>( const Minutes& aOther ) const	{ return m_value > aOther.m_value; }

Hours::Hours()										{ m_value = 0; }
Hours::Hours( TimeValue_t aValue )					{ m_value = aValue; }
Hours::Hours( const Nanosecs& aNanosecs )			{ m_value = 0; }
Hours::Hours( const Microsecs& aMicrosecs )			{ m_value = 0; }
Hours::Hours( const Millisecs& aMillisecs )			{ m_value = aMillisecs.Value() / (_e_3 * 60 * 60); }
Hours::Hours( const Seconds& aSeconds )				{ m_value = aSeconds.Value() / (60 * 60); }	
Hours::Hours( const Minutes& aMinutes )				{ m_value = aMinutes.Value() / 60; }
Hours::Hours( const Days& aDays )					{ m_value = aDays.Value() * 24; }
Hours::Hours( const TimeSpec& aTimespec )			{ m_value = aTimespec.ToHours(); }
Hours::Hours( const TimeVal& aTimeval )				{ m_value = aTimeval.ToHours(); }

TimeValue_t	Hours::ToNanosecs() const				{ return Nanosecs(*this).Value(); }
TimeValue_t Hours::ToMicrosecs() const				{ return Microsecs(*this).Value(); }	
TimeValue_t	Hours::ToMillisecs() const				{ return Millisecs(*this).Value(); }	
TimeValue_t	Hours::ToSeconds() const				{ return Seconds(*this).Value(); }	
TimeValue_t	Hours::ToMinutes() const				{ return Minutes(*this).Value(); }
TimeValue_t	Hours::ToDays() const					{ return Days(*this).Value(); }
TimeSpec Hours::ToTimeSpec() const					{ return TimeSpec(*this); }
TimeVal Hours::ToTimeVal() const					{ return TimeVal(*this); }

bool Hours::operator==( const Hours& aOther ) const	{ return m_value == aOther.m_value; }
bool Hours::operator!=( const Hours& aOther ) const	{ return m_value != aOther.m_value; }
bool Hours::operator<( const Hours& aOther ) const	{ return m_value < aOther.m_value; }
bool Hours::operator>( const Hours& aOther ) const	{ return m_value > aOther.m_value; }

Days::Days()										{ m_value = 0; }
Days::Days( TimeValue_t aValue )					{ m_value = aValue; }
Days::Days( const Nanosecs& aNanosecs )				{ m_value = 0; }
Days::Days( const Microsecs& aMicrosecs )			{ m_value = 0; }
Days::Days( const Millisecs& aMillisecs )			{ m_value = aMillisecs.Value() / (_e_3 * 60 * 60 * 24); }
Days::Days( const Seconds& aSeconds )				{ m_value = aSeconds.Value() / (60 * 60 * 24); }	
Days::Days( const Minutes& aMinutes )				{ m_value = aMinutes.Value() / (60 * 24); }
Days::Days( const Hours& aHours )					{ m_value = aHours.Value() / 24; }
Days::Days( const TimeSpec& aTimespec )				{ m_value = aTimespec.ToDays(); }
Days::Days( const TimeVal& aTimeval )				{ m_value = aTimeval.ToDays(); }

TimeValue_t	Days::ToNanosecs() const				{ return Nanosecs(*this).Value(); }
TimeValue_t Days::ToMicrosecs() const				{ return Microsecs(*this).Value(); }	
TimeValue_t	Days::ToMillisecs() const				{ return Millisecs(*this).Value(); }	
TimeValue_t	Days::ToSeconds() const					{ return Seconds(*this).Value(); }	
TimeValue_t	Days::ToMinutes() const					{ return Minutes(*this).Value(); }
TimeValue_t	Days::ToHours() const					{ return Hours(*this).Value(); }
TimeSpec Days::ToTimeSpec() const					{ return TimeSpec(*this); }
TimeVal Days::ToTimeVal() const						{ return TimeVal(*this); }

bool Days::operator==( const Days& aOther ) const	{ return m_value == aOther.m_value; }
bool Days::operator!=( const Days& aOther ) const	{ return m_value != aOther.m_value; }
bool Days::operator<( const Days& aOther ) const	{ return m_value < aOther.m_value; }
bool Days::operator>( const Days& aOther ) const	{ return m_value > aOther.m_value; }

TimeSpec::TimeSpec()								{ tv_sec = 0; tv_nsec = 0; }
TimeSpec::TimeSpec( const struct timespec& aTimespec )	{ tv_sec = aTimespec.tv_sec; tv_nsec = aTimespec.tv_nsec; }
TimeSpec::TimeSpec( const Nanosecs& aNanosecs )		{ tv_sec = aNanosecs.ToSeconds(); tv_nsec = (long) aNanosecs.Value() % Seconds(1).ToNanosecs(); }
TimeSpec::TimeSpec( const Microsecs& aMicrosecs )	{ tv_sec = aMicrosecs.ToSeconds(); tv_nsec = (long) Microsecs( aMicrosecs.Value() % Seconds(1).ToMicrosecs() ).ToNanosecs(); }
TimeSpec::TimeSpec( const Millisecs& aMillisecs )	{ tv_sec = aMillisecs.ToSeconds(); tv_nsec = (long) Millisecs( aMillisecs.Value() % Seconds(1).ToMillisecs() ).ToNanosecs(); }
TimeSpec::TimeSpec( const Seconds& aSeconds )		{ tv_sec = aSeconds.Value(); tv_nsec = 0; }
TimeSpec::TimeSpec( const Minutes& aMinutes )		{ tv_sec = aMinutes.ToSeconds(); tv_nsec = 0; }
TimeSpec::TimeSpec( const Hours& aHours )			{ tv_sec = aHours.ToSeconds(); tv_nsec = 0; }
TimeSpec::TimeSpec( const Days& aDays )				{ tv_sec = aDays.ToSeconds(); tv_nsec = 0; }
TimeSpec::TimeSpec( const TimeVal& aTimeval )		{ tv_sec = aTimeval.tv_sec; tv_nsec = (long) Microsecs( aTimeval.tv_usec ).ToNanosecs(); }	

TimeValue_t TimeSpec::ToNanosecs() const			{ return Seconds(tv_sec).ToNanosecs() + Nanosecs(tv_nsec).Value(); }
TimeValue_t TimeSpec::ToMicrosecs() const			{ return Seconds(tv_sec).ToMicrosecs() + Nanosecs(tv_nsec).ToMicrosecs(); }	
TimeValue_t	TimeSpec::ToMillisecs() const			{ return Seconds(tv_sec).ToMillisecs() + Nanosecs(tv_nsec).ToMillisecs(); }	
TimeValue_t	TimeSpec::ToSeconds() const				{ return Seconds(tv_sec).Value() + Nanosecs(tv_nsec).ToSeconds(); }	
TimeValue_t	TimeSpec::ToMinutes() const				{ return Seconds(tv_sec).ToMinutes() + Nanosecs(tv_nsec).ToMinutes(); }
TimeValue_t	TimeSpec::ToHours() const				{ return Seconds(tv_sec).ToHours() + Nanosecs(tv_nsec).ToHours(); }
TimeValue_t	TimeSpec::ToDays() const				{ return Seconds(tv_sec).ToDays() + Nanosecs(tv_nsec).ToDays(); }
TimeVal TimeSpec::ToTimeVal() const					{ return TimeVal(*this); }

bool TimeSpec::operator==( const TimeSpec& aOther ) const	{ return tv_sec == aOther.tv_sec && tv_nsec == aOther.tv_nsec; }
bool TimeSpec::operator!=( const TimeSpec& aOther ) const	{ return tv_sec != aOther.tv_sec || tv_nsec != aOther.tv_nsec; }
bool TimeSpec::operator<( const TimeSpec& aOther ) const	{ return ( tv_sec < aOther.tv_sec || ( tv_sec == aOther.tv_sec && tv_nsec < aOther.tv_nsec) ); }
bool TimeSpec::operator>( const TimeSpec& aOther ) const	{ return ( tv_sec > aOther.tv_sec || ( tv_sec == aOther.tv_sec && tv_nsec > aOther.tv_nsec) ); }

TimeSpec TimeSpec::operator+( const TimeSpec& aOther ) const	{ return ( TimeSpec(*this) += aOther ); }
TimeSpec TimeSpec::operator-( const TimeSpec& aOther ) const	{ return ( TimeSpec(*this) -= aOther ); }

TimeSpec& TimeSpec::operator+=( const TimeSpec& aOther )
{
	tv_sec += aOther.tv_sec;
	tv_nsec += aOther.tv_nsec;
	if ( tv_nsec > Seconds(1).ToNanosecs() )
	{
		++tv_sec;
		tv_nsec %= Seconds(1).ToNanosecs();
	}
	return *this;
}

TimeSpec& TimeSpec::operator-=( const TimeSpec& aOther )
{
	if ( *this > aOther )
	{
		tv_sec -= aOther.tv_sec;
		tv_nsec -= aOther.tv_nsec;
		if ( tv_nsec < 0 )
		{
			--tv_sec;
			tv_nsec += (long) Seconds(1).ToNanosecs();
		}
	}
	else
	{
		tv_sec = 0;
		tv_nsec = 0;
	}	
	return *this;
}

TimeVal::TimeVal()									{ tv_sec = 0; tv_usec = 0; }
TimeVal::TimeVal( const struct timeval& aTimeval )	{ tv_sec = aTimeval.tv_sec; tv_usec = aTimeval.tv_usec; }
TimeVal::TimeVal( const Nanosecs& aNanosecs )		{ tv_sec = (long)aNanosecs.ToSeconds(); tv_usec = (suseconds_t)Nanosecs( aNanosecs.Value() % Seconds(1).ToNanosecs() ).ToMicrosecs(); }
TimeVal::TimeVal( const Microsecs& aMicrosecs )		{ tv_sec = (long)aMicrosecs.ToSeconds(); tv_usec = (suseconds_t)aMicrosecs.Value() % Seconds(1).ToMicrosecs(); }
TimeVal::TimeVal( const Millisecs& aMillisecs )		{ tv_sec = (long)aMillisecs.ToSeconds(); tv_usec = (suseconds_t)Millisecs( aMillisecs.Value() % Seconds(1).ToMillisecs() ).ToMicrosecs(); }
TimeVal::TimeVal( const Seconds& aSeconds )			{ tv_sec = (long)aSeconds.Value(); tv_usec = 0; }
TimeVal::TimeVal( const Minutes& aMinutes )			{ tv_sec = (long)aMinutes.ToSeconds(); tv_usec = 0; }
TimeVal::TimeVal( const Hours& aHours )				{ tv_sec = (long)aHours.ToSeconds(); tv_usec = 0; }
TimeVal::TimeVal( const Days& aDays )				{ tv_sec = (long)aDays.ToSeconds(); tv_usec = 0; }
TimeVal::TimeVal( const TimeSpec& aTimespec )		{ tv_sec = (long)aTimespec.tv_sec; tv_usec = (suseconds_t)Nanosecs( aTimespec.tv_nsec ).ToMicrosecs(); }	

TimeValue_t TimeVal::ToNanosecs() const				{ return Seconds(tv_sec).ToNanosecs() + Microsecs(tv_usec).ToNanosecs(); }
TimeValue_t TimeVal::ToMicrosecs() const			{ return Seconds(tv_sec).ToMicrosecs() + Microsecs(tv_usec).Value(); }
TimeValue_t	TimeVal::ToMillisecs() const			{ return Seconds(tv_sec).ToMillisecs() + Microsecs(tv_usec).ToMillisecs(); }
TimeValue_t	TimeVal::ToSeconds() const				{ return Seconds(tv_sec).Value() + Microsecs(tv_usec).ToSeconds(); }
TimeValue_t	TimeVal::ToMinutes() const				{ return Seconds(tv_sec).ToMinutes() + Microsecs(tv_usec).ToMinutes(); }
TimeValue_t	TimeVal::ToHours() const				{ return Seconds(tv_sec).ToHours() + Microsecs(tv_usec).ToHours(); }
TimeValue_t	TimeVal::ToDays() const					{ return Seconds(tv_sec).ToDays() + Microsecs(tv_usec).ToDays(); }
TimeSpec TimeVal::ToTimeSpec() const				{ return TimeSpec(*this); }

bool TimeVal::operator==( const TimeVal& aOther ) const	{ return tv_sec == aOther.tv_sec && tv_usec == aOther.tv_usec; }
bool TimeVal::operator!=( const TimeVal& aOther ) const	{ return tv_sec != aOther.tv_sec || tv_usec != aOther.tv_usec; }
bool TimeVal::operator<( const TimeVal& aOther ) const	{ return ( tv_sec < aOther.tv_sec || ( tv_sec == aOther.tv_sec && tv_usec < aOther.tv_usec) ); }
bool TimeVal::operator>( const TimeVal& aOther ) const	{ return ( tv_sec > aOther.tv_sec || ( tv_sec == aOther.tv_sec && tv_usec > aOther.tv_usec) ); }

TimeVal TimeVal::operator+( const TimeVal& aOther ) const	{ return ( TimeVal(*this) += aOther ); }
TimeVal TimeVal::operator-( const TimeVal& aOther ) const	{ return ( TimeVal(*this) -= aOther ); }

TimeVal& TimeVal::operator+=( const TimeVal& aOther )
{
	tv_sec += aOther.tv_sec;
	tv_usec += aOther.tv_usec;
	if ( tv_usec > Seconds(1).ToMicrosecs() )
	{
		++tv_sec;
		tv_usec %= Seconds(1).ToMicrosecs();
	}
	return *this;
}

TimeVal& TimeVal::operator-=( const TimeVal& aOther )
{
	if ( *this > aOther )
	{
		tv_sec -= aOther.tv_sec;
		tv_usec -= aOther.tv_usec;
		if ( tv_usec < 0 )
		{
			--tv_sec;
			tv_usec += (long)Seconds(1).ToMicrosecs();
		}
	}
	else
	{
		tv_sec = 0;
		tv_usec = 0;
	}
	return *this;
}

inline void GetCurrentTime( OUT TimeSpec& aTimespec )
{
    clock_gettime( CLOCK_REALTIME, &aTimespec );
}

class DateTime
{
public:
	inline DateTime();	
	inline DateTime( const struct tm& aTm );
	inline DateTime( const Seconds& aSeconds );
	
	int		GetHour() const		{ return m_hour; }
	int		GetMinute() const	{ return m_minute; }
	int		GetSecond() const	{ return m_second; }
	int		GetDay() const		{ return m_day; }
	int		GetMonth() const	{ return m_month; }
	int		GetYear() const		{ return m_year; }

	void	SetHour( int aHour )		{ m_hour = aHour; }
	void	SetMinute( int aMinute )	{ m_minute = aMinute; }
	void	SetSecond( int aSecond )	{ m_second = aSecond; }
	void	SetDay( int aDay )			{ m_day = aDay; }
	void	SetMonth( int aMonth )		{ m_month = aMonth; }
	void	SetYear( int aYear )		{ m_year = aYear; }

	bool	IsDstOn() const		{ return m_bDstOn; }
	
	inline String	GetMonthName( bool abAbbreviated = true ) const;
	inline int		GetDayOfWeek() const;
	inline String	GetDayOfWeekName( bool abAbbreviated = true ) const;
	inline int		GetDayOfYear() const;	// 0 - 365
	inline int		GetDaysInYear() const;
	inline int		GetDaysInMonth() const;	

	inline void		ToTm( OUT struct tm& aTm ) const;
	inline TimeValue_t	ToSeconds() const;
	
	inline bool		Set( const Seconds& aSeconds, bool abGmt = false );

	inline DateTime& operator=( const struct tm& aTm );
	inline DateTime& operator=( const Seconds& aSeconds );
	
	inline TimeSpec operator-( const DateTime& aDateTime ) const;

	inline size_t	Format( OUT char* apBuffer, size_t aMaxLen, const char* apFormat ) const;
	
	static int	GetDaysInYear( int aYear );
	static int	GetDaysInMonth( int aMonth, int aYear );	
	
private:
	int		m_hour;			// 0..23
	int		m_minute;		// 0..59
	int		m_second;		// 0..59
	
	int		m_day;			// 1..31
	int		m_month;		// 1..12
	int		m_year;			// Full year, with thousands
	
	bool	m_bDstOn;
};

DateTime::DateTime() : m_day(0), m_month(0), m_year(0), m_hour(0), m_minute(0), m_second(0), m_bDstOn(false)	{}
DateTime::DateTime( const struct tm& aTm )		{ *this = aTm; }
DateTime::DateTime( const Seconds& aSeconds )	{ *this = aSeconds; }

String DateTime::GetMonthName( bool abAbbreviated ) const
{
	struct tm _tm;
	ToTm(_tm);
	char name[64] = {'\0'};
	strftime( name, sizeof(name)-1, abAbbreviated ? "%b" : "%B", &_tm );
	return name;
}

int DateTime::GetDayOfWeek() const
{
	struct tm _tm;
	ToTm(_tm);
	return _tm.tm_wday + 1;
}

String DateTime::GetDayOfWeekName( bool abAbbreviated ) const
{
	struct tm _tm;
	ToTm(_tm);
	char name[64] = {'\0'};
	strftime( name, sizeof(name)-1, abAbbreviated ? "%a" : "%A", &_tm );
	return name;	
}

int DateTime::GetDayOfYear() const
{
	struct tm _tm;
	ToTm(_tm);
	return _tm.tm_yday;
}

int DateTime::GetDaysInYear() const
{
	return GetDaysInYear( m_year );
}

int DateTime::GetDaysInMonth() const
{
	return GetDaysInMonth( m_month, m_year );
}

void DateTime::ToTm( OUT struct tm& aTm ) const
{
	aTm.tm_hour = m_hour;
	aTm.tm_min = m_minute;
	aTm.tm_sec = m_second;
	aTm.tm_mday = m_day;
	aTm.tm_mon = m_month - 1;
	aTm.tm_year = m_year - 1900;
	aTm.tm_isdst = m_bDstOn ? 1 : 0;
	mktime(&aTm);
}
	
TimeValue_t	DateTime::ToSeconds() const
{
	return Days( m_year*GetDaysInYear() ).ToSeconds() +
			Days( GetDayOfYear() ).ToSeconds() +
			Hours( GetHour() ).ToSeconds() +
			Minutes( GetMinute() ).ToSeconds() +
			Seconds( GetSecond() ).Value();
}

bool DateTime::Set( const Seconds& aSeconds, bool abGmt )
{
	time_t _time = aSeconds.Value();
	struct tm _tm;
	bool result;

#if defined(_WIN32)
	result = ( ( abGmt ? gmtime_s( &_tm, &_time ) : localtime_s( &_tm, &_time ) ) == 0 );
#elif defined(__linux__) || defined(__MACH__)
	result = ( ( abGmt ? gmtime_r( &_time, &_tm ) : localtime_r( &_time, &_tm ) ) != NULL );
#endif

	*this = _tm;

	return result;
}

DateTime& DateTime::operator=( const struct tm& aTm )
{
	m_hour = aTm.tm_hour;
	m_minute = aTm.tm_min;
	m_second = aTm.tm_sec;
	m_day = aTm.tm_mday;
	m_month = aTm.tm_mon + 1;
	m_year = aTm.tm_year + 1900;
	m_bDstOn = ( aTm.tm_isdst > 0 );

	return *this;
}

DateTime& DateTime::operator=( const Seconds& aSeconds )
{
	Set( aSeconds );
	return *this;
}

TimeSpec DateTime::operator-( const DateTime& aDateTime ) const
{
	return TimeSpec( Seconds( ToSeconds() - aDateTime.ToSeconds() ) );
}
	
size_t DateTime::Format( OUT char* apBuffer, size_t aMaxLen, const char* apFormat ) const
{
	struct tm _tm;
	ToTm(_tm);

	return strftime( apBuffer, aMaxLen, apFormat, &_tm );
}

inline bool GetCurrentDateTime( OUT DateTime& aDateTime, bool abGmt = false )
{
	return aDateTime.Set( Seconds(time(NULL)), abGmt );
}

inline bool SleepFor( const Millisecs& aTime )
{
	int rc = 0;

#if defined (_WIN32)

	Sleep( (DWORD)aTime.Value() );

#elif defined (__linux__) || defined(__MACH__)
    
	TimeSpec ts = aTime.ToTimeSpec();
	
	do
	{
		rc = nanosleep( &ts, &ts );
	}
	while ( rc != 0 && errno == EINTR );

#endif

	return ( rc == 0 );
}

inline bool SleepUntil( const DateTime& aDateTime )
{
	DateTime currDateTime;
	GetCurrentDateTime( currDateTime );
	
	TimeValue_t secondsToSleep = (aDateTime - currDateTime).ToSeconds();
	
	if ( secondsToSleep < 0 )
		return false;
			
#if defined(_WIN32)

	Sleep( (DWORD)Seconds(secondsToSleep).ToMillisecs() );

#elif defined(__linux__) || defined(__MACH__)

	do
	{
		secondsToSleep = (TimeValue_t)sleep( (u_int)secondsToSleep );
	}
	while ( secondsToSleep != 0 );

#endif

	return true;
}

}

#endif	/* CVTIME_H */

