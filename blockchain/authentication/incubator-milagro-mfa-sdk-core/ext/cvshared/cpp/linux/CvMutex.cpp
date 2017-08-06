/* 
 * File:   CvMutex.cpp
 * Author: mony
 * 
 * Created on August 23, 2012, 3:15 PM
 */

#include "CvMutex.h"

#include "CvLogger.h"

#include <string.h>
#include <errno.h>

namespace CvShared
{
	
CvMutex::CvMutex( const char* apName ) :
	m_name(apName), m_bValid(false)
{
}

CvMutex::~CvMutex()
{
	if ( m_bValid )
		pthread_mutex_destroy( &m_hMutex );
}

bool CvMutex::Create()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
	
	int rc = pthread_mutex_init( &m_hMutex, &attr );
	
	if ( rc != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while creating mutex [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
		return false;
	}
	
	m_bValid = true;
	return true;
}
	
bool CvMutex::Lock( const Millisecs& aTimeout )
{
	int rc = 0;

	switch ( aTimeout.Value() )
	{
		case 0:	// No Wait lock
			
			rc = pthread_mutex_trylock( &m_hMutex );
			
			if ( rc != 0 && rc != EBUSY )
			{
				LogMessage( enLogLevel_Error, "ERROR while trying to locking mutex [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );				
			}
			
			break;

		case TIMEOUT_INFINITE:
			
			rc = pthread_mutex_lock( &m_hMutex );
			
			if ( rc != 0 )
			{
				LogMessage( enLogLevel_Error, "ERROR while locking mutex [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );				
			}

			break;

		default:	// Timed lock
		{
            
#ifdef __MACH__
            
			LogMessage( enLogLevel_Warning, "WARNING MacOS doesn't support locking mutex with timeout. Locking [%s] indefinately...", m_name.c_str(), strerror(rc), rc );
            
            rc = pthread_mutex_lock( &m_hMutex );
			
			if ( rc != 0 )
			{
				LogMessage( enLogLevel_Error, "ERROR while locking mutex [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
			}
            
#else
            
			TimeSpec ts;
			GetCurrentTime( ts );

			ts += aTimeout.ToTimeSpec();

			rc = pthread_mutex_timedlock( &m_hMutex, &ts );
			
			if ( rc != 0 && rc != ETIMEDOUT )
			{
				LogMessage( enLogLevel_Error, "ERROR locking mutex [%s] with timeout: %s (%d)", m_name.c_str(), strerror(rc), rc );				
			}
            
#endif
			
			break;	
		}
	}

	return ( rc == 0 );	
}

bool CvMutex::Unlock()
{
	int rc = pthread_mutex_unlock( &m_hMutex );
	
	if ( rc == 0 )
		return true;
	
	LogMessage( enLogLevel_Error, "ERROR while unlocking mutex [%s]: %s (%d)", m_name.c_str(), strerror(rc), rc );
	
	return false;
}

}
