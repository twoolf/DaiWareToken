/* 
 * File:   CvHttpRequestAsync.h
 * Author: mony
 *
 * Created on October 10, 2012, 2:24 PM
 */

#ifndef CVHTTPREQUESTASYNC_H
#define	CVHTTPREQUESTASYNC_H

#include "CvHttpRequest.h"

#include "CvThread.h"
#include "CvQueue.h"

#include <set>

class CvHttpRequestAsync : public CvHttpRequest
{
public:
	
	CvHttpRequestAsync( enHttpMethod_t aMethod = enHttpMethod_GET );
	virtual ~CvHttpRequestAsync();
	
	static void Init( int aMaxConcurrentRequests );
	
	class CEventListener
	{
	public:
		virtual void OnHttpRequestDone( const CvHttpRequestAsync* apHttpRequest ) = 0;
		virtual void OnHttpRequestError( const CvHttpRequestAsync* apHttpRequest, enStatus_t aStatus ) = 0;
	};

	void Clear();
	bool Execute( CEventListener* apEventListener, const Seconds& aTimeout = TIMEOUT_INFINITE );
	
	void SetContent( const char* apData, int64_t aSize );
	
private:
	
	class CThreadExecute : public CvShared::CvThread
	{
	public:
		CThreadExecute(uint32_t aId);
	protected:
		virtual long		Body( void* apArgs );
	};

	typedef std::set<CThreadExecute*>				CSetThreads;
	typedef CvShared::CvQueue<CvHttpRequestAsync*>	CQueueExecute;
	
	CvHttpRequestAsync(const CvHttpRequestAsync& orig)	{}
	
	enStatus_t Execute( bool abProgress = false );
	
	static bool CreateThread();
	
	String					m_content;
	
	CEventListener*			m_pListener;
	bool					m_bDone;	
	
	static int				m_maxThreads;
	static CSetThreads		m_threads;
	static CvMutex			m_mutexThreads;
	static int				m_countIdleThreads;
	static CQueueExecute	m_queueExecute;
};

#endif	/* CVHTTPREQUESTASYNC_H */

