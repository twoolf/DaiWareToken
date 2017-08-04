/* 
 * File:   CvThread.cpp
 * Author: mony
 * 
 * Created on August 23, 2012, 2:19 PM
 */

#include "CvThread.h"
#include "CvLogger.h"

#include <string.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifdef __linux__
    #include <sys/prctl.h>
#endif

namespace CvShared
{
	
CvThread::CvThread( const char* apName ) :
	m_name(apName), m_hThread(0)
{
}

bool CvThread::Create( void* apArgs )
{
	m_pArgs = apArgs;
	
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	
	int rc = pthread_create( &m_hThread, &attr, _Body, this );
	
	pthread_attr_destroy( &attr );
		
	if ( rc != 0 )
	{
		m_hThread = 0;
		LogMessage( enLogLevel_Error, "ERROR while creating thread [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
		return false;
	}
	
	return true;
}

CvThread::~CvThread()
{
	LogMessage( enLogLevel_Debug2, "Destroying thread [%s]", m_name.c_str() );
}

void* CvThread::_Body( void* apThis )
{
	CvThread* pThis = (CvThread*)apThis;
	
	String name = pThis->m_name;
    
#ifdef __linux__
	prctl( PR_SET_NAME, name.c_str() );
#endif
	
	LogMessage( enLogLevel_Debug1, "Starting thread [%s]", name.c_str() );
	
	pThis->m_id = (pid_t)syscall(SYS_gettid);

	long rc = pThis->Body( pThis->m_pArgs );
	
	LogMessage( enLogLevel_Debug1, "Exiting thread [%s] with code [%d]", name.c_str(), rc );
	
	return (void*)rc;
}

THREAD_ID CvThreadCurrent::GetId()
{
	return (pid_t)syscall(SYS_gettid);
}

}