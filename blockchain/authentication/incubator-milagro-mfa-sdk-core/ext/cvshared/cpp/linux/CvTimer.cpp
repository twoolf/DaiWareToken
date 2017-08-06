/* 
 * File:   CvTimer.cpp
 * Author: mony
 * 
 * Created on November 6, 2012, 11:00 AM
 */

#include "CvTimer.h"

#include "CvLogger.h"

#include <pthread.h>
#include <string.h>
#include <errno.h>

namespace CvShared
{

CvTimer::CvTimer( const String& aName ) :
	m_name(aName), m_hTimer(NULL), m_bStarted(false),
	m_pListener(NULL)
{
	pthread_attr_init( &m_threadAttr );
	pthread_attr_setdetachstate( &m_threadAttr, PTHREAD_CREATE_DETACHED );
	
	memset( &m_sigevent, 0, sizeof(m_sigevent) );
}

CvTimer::~CvTimer( )
{
	Stop();
}

bool CvTimer::Start( const Millisecs& aExpirationTime, CEventListener* apListener, bool abRecurrent )
{
	m_pListener = apListener;
	
	m_sigevent.sigev_notify = SIGEV_THREAD;
	m_sigevent.sigev_notify_function = _CallbackExpired;
	m_sigevent.sigev_value.sival_ptr = this;
	m_sigevent.sigev_notify_attributes = &m_threadAttr;
	
	if ( timer_create( CLOCK_REALTIME, &m_sigevent, &m_hTimer ) != 0 )
	{
		m_hTimer = NULL;
		LogMessage( enLogLevel_Error, "ERROR while creating timer [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
		return false;
	}

	struct itimerspec timerSpec;
	memset( &timerSpec, 0, sizeof(timerSpec) );
	
	timerSpec.it_value = TimeSpec( aExpirationTime );
	
	if ( abRecurrent )
		timerSpec.it_interval = timerSpec.it_value;
	
	if ( timer_settime( m_hTimer, 0, &timerSpec, NULL ) != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while starting timer [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
		return false;
	}
	
	m_bStarted = true;
	
	return true;
}

bool CvTimer::Stop()
{
	if ( m_hTimer == NULL || !m_bStarted )
		return false;
	
	struct itimerspec timerSpec;
	memset( &timerSpec, 0, sizeof(timerSpec) );
	
	if ( timer_settime( m_hTimer, 0, &timerSpec, NULL ) != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while deleting timer [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
		return false;
	}
	
	if ( timer_delete( m_hTimer ) != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while deleting timer [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
		return false;
	}
	
	m_bStarted = false;
	m_hTimer = NULL;
	
	return true;
}

void CvTimer::_CallbackExpired( union sigval aSigval )
{
	CvTimer* pThis = (CvTimer*)aSigval.sival_ptr;
	
	if ( pThis->m_pListener != NULL )
		pThis->m_pListener->OnTimerExpired( pThis );
}
	
}	//namespace CvShared