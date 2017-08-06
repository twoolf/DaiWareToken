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
/*! \file  CvMutex.h
    \brief C++ class providing portable Mutex functionality.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 23, 2012, 3:15 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 C++ class providing portable Mutex functionality.
 Context-sensitive Mutex Lock is also provided.

*/

#ifndef CVMUTEX_H
#define	CVMUTEX_H

#if defined(_WIN32)

	#include "windows.h"
	#define MUTEX_HANDLE	HANDLE

#elif defined(__linux__) || defined(__MACH__)

	#include <pthread.h>
	#define MUTEX_HANDLE	pthread_mutex_t

#else

    #error "Unsupported OS"

#endif

#include <stdint.h>

#include <string>

#include "CvTime.h"

namespace CvShared
{
	
class CvMutex
{
public:
	static const TimeValue_t TIMEOUT_INFINITE = -1;
			
	CvMutex( const char* apName  = "" );
	virtual ~CvMutex();
	
	bool				Create();
	
	MUTEX_HANDLE&		GetHandle()		{ return m_hMutex; }
	
	bool				Lock( const Millisecs& aTimeout = TIMEOUT_INFINITE );
	bool				LockNoWait()	{ return Lock(0); }
	
	bool				Unlock();
	
protected:
	CvMutex(const CvMutex& orig)	{}
	
	MUTEX_HANDLE	m_hMutex;
	
	String			m_name;
	bool			m_bValid;
};

class CvMutexLock
{
public:
	inline CvMutexLock( CvMutex& aMutex, const Millisecs& aTimeout = CvMutex::TIMEOUT_INFINITE );
	inline CvMutexLock(const CvMutexLock& orig);
	
	inline ~CvMutexLock();
	
	inline bool		Unlock();
	
	bool			IsLocked() const	{ return m_bLocked; }
	
private:
	
	CvMutex&		m_mutex;
	bool			m_bLocked;
};

CvMutexLock::CvMutexLock( CvMutex& aMutex, const Millisecs& aTimeout ) :
	m_mutex(aMutex)
{
	m_bLocked = m_mutex.Lock( aTimeout );
}

CvMutexLock::CvMutexLock( const CvMutexLock& orig ) :
	m_mutex(orig.m_mutex)
{
	m_bLocked = m_mutex.Lock();
}

CvMutexLock::~CvMutexLock()
{
	if ( m_bLocked )
		m_mutex.Unlock();
}

bool CvMutexLock::Unlock()
{
	if ( m_bLocked )
	{
		if ( !m_mutex.Unlock() )
			return false;
		
		m_bLocked = false;
	}
	
	return true;
}

}

#endif	/* CVMUTEX_H */

