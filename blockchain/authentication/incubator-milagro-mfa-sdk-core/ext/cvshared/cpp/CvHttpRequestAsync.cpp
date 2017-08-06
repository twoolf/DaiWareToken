/* 
 * File:   CvHttpRequestAsync.cpp
 * Author: mony
 * 
 * Created on October 10, 2012, 2:24 PM
 */

#include <set>
#include <sys/types.h>

#include "CvHttpRequestAsync.h"
#include "CvLogger.h"

int									CvHttpRequestAsync::m_maxThreads = 0;
CvHttpRequestAsync::CSetThreads		CvHttpRequestAsync::m_threads;
CvHttpRequestAsync::CvMutex			CvHttpRequestAsync::m_mutexThreads;
int									CvHttpRequestAsync::m_countIdleThreads = 0;
CvHttpRequestAsync::CQueueExecute	CvHttpRequestAsync::m_queueExecute;

using namespace CvShared;

CvHttpRequestAsync::CvHttpRequestAsync( enHttpMethod_t aMethod ) :
	CvHttpRequest(aMethod), m_pListener(NULL), m_bDone(true)
{
}

CvHttpRequestAsync::~CvHttpRequestAsync( )
{
	m_pListener = NULL;
	
	while( !m_bDone )
	{
		LogMessage( enLogLevel_Warning, "WARNING: Asynchronous http request [%s] is not done yet. Canceling...", m_req.url.c_str() );
		SetCancel();
		SleepFor( Millisecs(200) );
	}
}

void CvHttpRequestAsync::Init( int aMaxConcurrentRequests )
{
	m_maxThreads = aMaxConcurrentRequests;
	
	m_mutexThreads.Create();

	CreateThread();
}

bool CvHttpRequestAsync::CreateThread()
{
	CvMutexLock lock( m_mutexThreads );
	
	if ( m_threads.size() >= m_maxThreads )
	{
		LogMessage( enLogLevel_Warning, "Max concurrency of async http requests reached: [%d]", m_maxThreads );
		return false;
	}
	
	CThreadExecute* pThread = new CThreadExecute( m_threads.size()+1 );
	
	m_threads.insert( pThread );
	
	return pThread->Create(NULL);
}
	
void CvHttpRequestAsync::Clear()
{
	CvHttpRequest::Clear();
	m_pListener = NULL;
	m_bDone = true;
}
	
bool CvHttpRequestAsync::Execute( CEventListener* apEventListener, const Seconds& aTimeout )
{
	if ( m_maxThreads == 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR: CvHttpRequestAsync should be initialized with max concurrency > 0" );
		return false;
	}
	
	m_req.timeout = aTimeout.Value();
			
	m_pListener = apEventListener;
	m_bDone = false;
	
	CvMutexLock lock( m_mutexThreads );

	if ( m_countIdleThreads == 0 )
		CreateThread();

	m_queueExecute.Push( this );

	--m_countIdleThreads;		

	return true;
}

void CvHttpRequestAsync::SetContent( const char* apData, int64_t aSize )
{
	m_content.resize( aSize );
	memcpy( (void*)m_content.data(), apData, aSize );
	
	CvHttpRequest::SetContent( m_content.data(), aSize );
}
	
CvHttpRequestAsync::CThreadExecute::CThreadExecute(uint32_t aId) :
	CvThread( (CvString("async-http-req-") + CvString(aId)).c_str() )
{}

long CvHttpRequestAsync::CThreadExecute::Body(void* apArgs)
{
	while( true )
	{
		{
			CvMutexLock lock( m_mutexThreads );
			++m_countIdleThreads;
		}
		
		CvHttpRequestAsync* pRequest = NULL;
		
		if ( !m_queueExecute.Pop( pRequest ) )
		{
			LogMessage( enLogLevel_Error, "ERROR while trying to pop an async http request from the execution queue" );
			SleepFor( Millisecs(200) );
			continue;
		}
		
		enStatus_t status = ((CvHttpRequest*)pRequest)->Execute( pRequest->m_req.timeout, true );

		pRequest->m_bDone = true;
		
		if ( pRequest->m_pListener != NULL )
		{
			if ( status != enStatus_Ok )
				pRequest->m_pListener->OnHttpRequestError( pRequest, status );
			else
				pRequest->m_pListener->OnHttpRequestDone( pRequest );
		}
	}
			
	return 0;
}

