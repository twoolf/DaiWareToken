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
/*! \file  CvThread.cpp
    \brief C++ class providing portable Thread facility.

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 23, 2012, 2:19 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : 

 C++ class providing portable Thread facility.
 The CvThread is an abstract class - it should be inheritted and its
 Body() method should be implemented.

*/

#include <string.h>
#include <stdio.h>

#include "CvThread.h"
#include "CvLogger.h"

namespace CvShared
{
	
CvThread::CvThread( const char* apName ) :
	m_name(apName), m_hThread(0)
{
}

bool CvThread::Create( void* apArgs )
{
	m_pArgs = apArgs;
	
	m_hThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)_Body, this, 0, NULL );

	if ( m_hThread == NULL )  // if ( m_hThread != NULL ) is wrong, m_hThread is the thread handle and is of Importante :)
	{
		LogMessage( enLogLevel_Error, "ERROR while creating thread [%s]: %d", m_name.c_str(), GetLastError() );
		return false;
	}
	
	return true;
}

CvThread::~CvThread()
{
	LogMessage( enLogLevel_Debug2, "Destroying thread [%s]", m_name.c_str() );
}

DWORD CvThread::_Body( LPVOID apThis )
{
	CvThread* pThis = (CvThread*)apThis;
	
	String& name = pThis->m_name;

	LogMessage( enLogLevel_Debug1, "Starting thread [%s]", name.c_str() );
	
	pThis->m_id = GetCurrentThreadId();

	int rc = pThis->Body( pThis->m_pArgs ); //???
	
	LogMessage( enLogLevel_Debug1, "Exiting thread [%s] with code [%d]", name.c_str(), rc );
	
	return (DWORD)rc;
}

THREAD_ID CvThreadCurrent::GetId()
{
	return GetCurrentThreadId();
}

}