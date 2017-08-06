/* 
 * File:   CvHttpServerMg.h
 * Author: mony
 *
 * Created on October 8, 2012, 8:43 AM
 */

#ifndef CVHTTPSERVERMG_H
#define	CVHTTPSERVERMG_H

#include "CvHttpServer.h"

#include "mongoose/mongoose.h"

class CvHttpServerMg : public CvHttpServer
{
public:
	CvHttpServerMg() : m_bInitialized(false), m_port(-1), m_numOfThreads(-1), m_pContext(NULL)	{}
	CvHttpServerMg( IN CMapOptions& aOptions ) : m_pContext(NULL)	{ Init( aOptions ); }
	virtual ~CvHttpServerMg();
	
	void Init( IN CMapOptions& aOptions );
	
	bool Start();
	
protected:
	CvHttpServerMg(const CvHttpServer& orig)	{}
	
	enum enEvent_t
	{
		enEvent_NewRequest = MG_NEW_REQUEST,			// New HTTP request has arrived from the client
		enEvent_Error = MG_HTTP_ERROR,					// HTTP error must be returned to the client
		enEvent_EventLog = MG_EVENT_LOG,				// Mongoose logs an event, request_info.log_message
		enEvent_SSL = MG_INIT_SSL,						// Mongoose initializes SSL. Instead of mg_connection *,
														// SSL context is passed to the callback function.
		enEvent_RequestComplete = MG_REQUEST_COMPLETE,	// Mongoose has finished handling the request
		enEvent_WebSockConnect = MG_WEBSOCKET_CONNECT,	// Sent on HTTP connect, before websocket handshake.
														// If user callback returns NULL, then mongoose proceeds
														// with handshake, otherwise it closes the connection.
		enEvent_WebSockReady = MG_WEBSOCKET_READY,		// Handshake has been successfully completed.
		enEvent_WebSockMessage = MG_WEBSOCKET_MESSAGE,	// Incoming message from the client
		enEvent_WebSockClose = MG_WEBSOCKET_CLOSE		// Client has closed the connection
	};
	
	class CvContextMg : public CvContext
	{
		friend class CvHttpServerMg;
		
	public:
		virtual ~CvContextMg()	{}
		
		virtual bool	SendResponse();
		virtual void	CloseConnection();
		
	protected:
		CvContextMg( mg_connection* apConn ) : m_pConn(apConn)	{}
		CvContextMg( const CvContextMg& orig ) : m_pConn(orig.m_pConn)	{}
		
		mg_connection*	m_pConn;
	};
	
	void			BuildRequest( const mg_request_info* apRequestInfo, OUT CvRequest& aRequest );
	
	static void*	_RequestCallback( mg_event aEvent, struct mg_connection* apConn );
	virtual bool	OnReceiveRequest( INOUT CvContext& aContext ) = 0;
	
	bool			m_bInitialized;
	u_short			m_port;
	int				m_numOfThreads;
	mg_context*		m_pContext;
};

#endif	/* CVHTTPSERVERMG_H */

