/* 
 * File:   CvSemaphore.cpp
 * Author: mony
 * 
 * Created on September 26, 2012, 4:28 PM
 */

#include "CvSemaphore.h"

#include "CvLogger.h"

#include <string.h>
#include <errno.h>

namespace CvShared
{
	
CvSemaphore::CvSemaphore( const char* apName ) :
	m_name(apName), m_bValid(false)
{
}

CvSemaphore::~CvSemaphore()
{
	if ( m_bValid )
		sem_destroy( &m_hSemaphore );
}

bool CvSemaphore::Create( u_int aInitialCount )
{
	if ( sem_init( &m_hSemaphore, 0, aInitialCount ) != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR while creating semaphore [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
		return false;
	}
	
	m_bValid = true;
	return true;
}
	
bool CvSemaphore::Pend( const Millisecs& aTimeout )
{
	int bOk = true;
	
	switch ( aTimeout.Value() )
	{
		case 0:	// No Wait lock
			
			if ( sem_trywait( &m_hSemaphore ) != 0 )
			{
				if ( errno != EAGAIN )
					LogMessage( enLogLevel_Error, "ERROR while trying to pend on semaphore [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
				
				bOk = false;
			}
			
			break;

		case TIMEOUT_INFINITE:
		{
			int rc = 0;

			do
			{
				rc = sem_wait( &m_hSemaphore );
				
				if ( rc != 0 && errno != EINTR )
				{
					LogMessage( enLogLevel_Error, "ERROR while pending on semaphore [%s]: %s (%d)", m_name.c_str(), strerror(errno), errno );
					bOk = false;
				}
			}
			while( rc != 0 && errno == EINTR );
					
			break;
		}	
		
		default:	// Timed lock
		{
			TimeSpec ts;
			GetCurrentTime( ts );

			ts += aTimeout.ToTimeSpec();

			int rc = 0;
			
			do
			{
				rc = sem_timedwait( &m_hSemaphore, &ts );

				if ( rc != 0 && errno != EINTR && errno != ETIMEDOUT )
				{
					LogMessage( enLogLevel_Error, "ERROR pending on semaphore [%s] with timeout: %s (%d)\n", m_name.c_str(), strerror(errno), errno );
					bOk = false;
				}
			}
			while( rc != 0 && errno == EINTR );
			
			if ( rc != 0 && errno == ETIMEDOUT )
				bOk = false;
			
			break;	
		}
	}

	return bOk;	
}

bool CvSemaphore::Post()
{
	if ( sem_post( &m_hSemaphore ) == 0 )
		return true;
	
	LogMessage( enLogLevel_Error, "ERROR while posting semaphore [%s]: %s (%d)\n", m_name.c_str(), strerror(errno), errno );
	
	return false;
}

}	// namespace CvShared
