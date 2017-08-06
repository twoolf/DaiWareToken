/* 
 * File:   CvTimer.h
 * Author: mony
 *
 * Created on November 6, 2012, 11:00 AM
 */

#ifndef CVTIMER_H
#define	CVTIMER_H

#include "CvTime.h"

#include <string>

#include <time.h>
#include <signal.h>

namespace CvShared
{

class CvTimer
{
public:
	
	class CEventListener
	{
	public:
		virtual void OnTimerExpired( const CvTimer* apTimer ) = 0;
	};
	
	CvTimer( const String& aName  = "" );
	virtual ~CvTimer();
	
	bool	Start( const Millisecs& aExpirationTime, CEventListener* apListener, bool abRecurrent = false );
	bool	Stop();
	
	const String&	GetName() const	{ return m_name; }
	
protected:
	CvTimer(const CvTimer& orig)	{}
	
	static void		_CallbackExpired( union sigval aSigval );
	
	String			m_name;
	
	timer_t			m_hTimer;
	
	bool			m_bStarted;
	CEventListener*	m_pListener;
	
	struct sigevent	m_sigevent;
	pthread_attr_t	m_threadAttr;	
};

}	//namespace CvShared

#endif	/* CVTIMER_H */

