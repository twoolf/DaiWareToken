/* 
 * File:   CvCondVar.cpp
 * Author: mony
 * 
 * Created on October 1, 2012, 4:10 PM
 */

#include "CvCondVar.h"

#include "CvString.h"
#include "CvMutex.h"

#include "CvLogger.h"

namespace CvShared
{

CvCondVar::CvCondVar( const char* apName ) :
	m_name(apName), m_bValid(false),
	m_mutex((CvString("cond-var-") + apName).c_str()),
	m_value(0)
{
}

CvCondVar::~CvCondVar( )
{
	if ( m_bValid )
		pthread_cond_destroy( &m_hCondVar );
}

bool CvCondVar::Create( int aInitialValue )
{
	m_value = aInitialValue;
	
	if ( !m_mutex.Create() )
	{
		LogMessage( enLogLevel_Error, "ERROR while creating mutex for conditional variable [%s]", m_name.c_str() );
		return false;
	}
	
	int rc = pthread_cond_init( &m_hCondVar, NULL );
	
	if ( rc != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while creating conditional variable [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
		return false;
	}
	
	m_bValid = true;
	return true;
}

bool CvCondVar::Wait( int aValue, const Millisecs& aTimeout )
{
	int rc = 0;
	
	CvMutexLock lock(m_mutex);
	
	switch ( aTimeout.Value() )
	{
		case TIMEOUT_INFINITE:
		{
			while ( m_value != aValue && rc == 0 )
			{
				rc = pthread_cond_wait( &m_hCondVar, &m_mutex.GetHandle() );
			}
				
			if ( rc != 0 )
			{
				LogMessage( enLogLevel_Error, "ERROR while waiting on conditional variable [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
			}
					
			break;
		}	
		
		default:	// Timed wait
		{
			TimeSpec ts;
			GetCurrentTime( ts );

			ts += aTimeout.ToTimeSpec();

			int rc = 0;
			
			while ( m_value != aValue && rc == 0 )
			{
				rc = pthread_cond_timedwait( &m_hCondVar, &m_mutex.GetHandle(), &ts );
			}

			if ( rc != 0 && rc != ETIMEDOUT )
			{
				LogMessage( enLogLevel_Error, "ERROR while waiting on conditional variable [%s] with timeout: %s (%d)", m_name.c_str(), strerror(rc), rc );				
			}
			
			break;	
		}
	}

	return ( rc == 0 );
}

bool CvCondVar::Signal( int aValue )
{
	CvMutexLock lock(m_mutex);
	
	m_value = aValue;
	
	int rc = pthread_cond_signal( &m_hCondVar );
	
	if ( rc != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while signaling conditional variable [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
		return false;
	}
	
	return true;
}

bool CvCondVar::Broadcast( int aValue )
{
	CvMutexLock lock(m_mutex);
	
	m_value = aValue;
	
	int rc = pthread_cond_broadcast( &m_hCondVar );
	
	if ( rc != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while broadcasting conditional variable [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
		return false;
	}
	
	return true;
}

void CvCondVar::Set( int aValue )
{
	CvMutexLock lock(m_mutex);

	m_value = aValue;
}
	
}	// namespace CvShared