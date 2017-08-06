/* 
 * File:   CvCondVar.h
 * Author: mony
 *
 * Created on October 1, 2012, 4:10 PM
 */

#ifndef CVCONDVAR_H
#define	CVCONDVAR_H

#include <pthread.h>
#include <stdint.h>

#include <string>

#include "CvMutex.h"
#include "CvTime.h"

namespace CvShared
{

class CvCondVar
{
public:
	CvCondVar( const char* apName  = "" );
	virtual ~CvCondVar();
	
	static const TimeValue_t TIMEOUT_INFINITE = -1;
			
	bool			Create( int aInitialValue );
	
	pthread_cond_t&	GetHandle()		{ return m_hCondVar; }
	
	bool			Wait( int aValue, const Millisecs& aTimeout = TIMEOUT_INFINITE );
	bool			Signal( int aValue );
	bool			Broadcast( int aValue );
	
	void			Set( int aValue );
	
protected:
	CvCondVar(const CvCondVar& orig)	{}
	
	pthread_cond_t	m_hCondVar;
	
	String			m_name;
	bool			m_bValid;

	CvMutex			m_mutex;
	int				m_value;
};

}	// namespace CvShared

#endif	/* CVCONDVAR_H */

