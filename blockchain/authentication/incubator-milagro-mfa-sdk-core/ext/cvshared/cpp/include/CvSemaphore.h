/* 
 * File:   CvSemaphore.h
 * Author: mony
 *
 * Created on September 26, 2012, 4:28 PM
 */

#ifndef CVSEMAPHORE_H
#define	CVSEMAPHORE_H

#include <semaphore.h>
#include <stdint.h>

#include <string>

#include "CvTime.h"

namespace CvShared
{
	
class CvSemaphore
{
public:
	static const TimeValue_t TIMEOUT_INFINITE = -1;
			
	CvSemaphore( const char* apName  = "" );
	virtual ~CvSemaphore();
	
	bool	Create( u_int aInitialCount );
	
	sem_t&	GetHandle()		{ return m_hSemaphore; }
	
	bool	Pend( const Millisecs& aTimeout = TIMEOUT_INFINITE );
	bool	PendNoWait()	{ return Pend(0); }
	
	bool	Post();
	
protected:
	CvSemaphore(const CvSemaphore& orig)	{}
	
	sem_t		m_hSemaphore;
	
	String		m_name;
	bool		m_bValid;
};

class CvSemaphoreLock
{
public:
	inline CvSemaphoreLock( CvSemaphore& aSemaphore, const Millisecs& aTimeout = CvSemaphore::TIMEOUT_INFINITE );
	inline ~CvSemaphoreLock();
	
	inline bool		Unlock();
	
	bool			IsLocked() const	{ return m_bLocked; }
	
private:
	CvSemaphoreLock(const CvSemaphoreLock& orig) : m_semaphore(orig.m_semaphore)
	{}
	
	CvSemaphore&	m_semaphore;
	bool			m_bLocked;
};

CvSemaphoreLock::CvSemaphoreLock( CvSemaphore& aSemaphore, const Millisecs& aTimeout ) :
	m_semaphore(aSemaphore)
{
	m_bLocked = m_semaphore.Pend( aTimeout );
}

CvSemaphoreLock::~CvSemaphoreLock()
{
	if ( m_bLocked )
		m_semaphore.Post();
}

bool CvSemaphoreLock::Unlock()
{
	if ( m_bLocked )
	{
		if ( !m_semaphore.Post() )
			return false;
		
		m_bLocked = false;
	}
	
	return true;
}

}	// namespace CvShared

#endif	/* CVSEMAPHORE_H */

