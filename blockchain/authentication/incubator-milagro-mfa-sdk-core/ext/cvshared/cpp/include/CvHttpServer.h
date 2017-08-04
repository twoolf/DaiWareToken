/* 
 * File:   CvHttpServer.h
 * Author: mony
 *
 * Created on August 27, 2012, 2:23 PM
 */

#ifndef CVHTTPSERVER_H
#define	CVHTTPSERVER_H

#include "CvHttpCommon.h"
#include "CvString.h"

#include <string.h>

#define HTTP_SERVER_OPTION_PORT					"port"
#define HTTP_SERVER_OPTION_NUM_THREADS			"num-threads"
#define HTTP_SERVER_OPTION_MAX_CONNECTIONS_NUM	"max-connections"

class CvHttpServer
{
public:
	typedef std::map<CvString,CvString>	CMapOptions;
	
	virtual ~CvHttpServer()	{}
	
protected:
	CvHttpServer()	{}
	CvHttpServer( IN CMapOptions& aOptions )	{ Init( aOptions ); }	
	CvHttpServer( const CvHttpServer& orig )	{}
	
	class CvMessage
	{
		friend class CvHttpServer;
		friend class CvHttpServerMg;
		friend class CvHttpServerUv;
		
	protected:
		CvMessage()	{}
		virtual ~CvMessage()	{}
		
		CvString		m_httpVersion;		// E.g. "1.0", "1.1"
		CMapHttpHeaders	m_mapHeaders;
		CvString		m_content;
	};
	
	class CvRequest : public CvMessage
	{
		friend class CvHttpServer;
		friend class CvHttpServerMg;
		friend class CvHttpServerUv;
		
	public:
		enHttpMethod_t		GetMethod() const		{ return m_method; }
		const CvString&		GetUri() const			{ return m_uri; }
		const CvString&		GetHttpVersion() const	{ return m_httpVersion; }
		const CvString&		GetQueryString() const	{ return m_queryString; }
		const CvString&		GetRemoteUser() const	{ return m_remoteUser; }
		const CvString&		GetRemoteIp() const		{ return m_remoteIp; }
		u_short				GetRemotePort() const	{ return m_remotePort; }
		bool				IsSSL() const			{ return m_bSSL; }
		inline CvString		GetHeaderValue( const CvString& aHeaderName ) const;
		const void*			GetContent() const		{ return m_content.data(); }
		
	private:
		CvRequest();
		CvRequest( const CvRequest& orig)	{}
		
		enHttpMethod_t	m_method;		// "GET", "POST", etc
		CvString		m_uri;			// URL-decoded URI
		CvString		m_queryString;
		CvString		m_remoteUser;	// Authenticated user
		CvString		m_remoteIp;		// Client's IP address
		u_short			m_remotePort;	// Client's port
		bool			m_bSSL;			// 1 if SSL-ed, 0 if not
	};
	
	class CvResponse : public CvMessage
	{
		friend class CvHttpServer;
		friend class CvHttpServerMg;
		friend class CvHttpServerUv;
		
	public:
		void		SetHttpVersion( const CvString& aHttpVersion )		{ m_httpVersion = aHttpVersion; }
		void		SetStatusCode( int aStatusCode )					{ m_statusCode = aStatusCode; }
		void		SetStatusMessage( const CvString& aStatusMessage )	{ m_statusMessage = aStatusMessage; }
		void		SetHeaderValue( const CvString& aHeaderName, const CvString& aHeaderValue )	{ m_mapHeaders[aHeaderName] = aHeaderValue; }
		inline void	SetContent( const void* apContent, uint32_t aLen );
		
	private:
		CvResponse();
		CvResponse( const CvResponse& orig )	{}
		
		void		operator>>( OUT CvString& aOutput ) const;
		
		int			m_statusCode;		// HTTP reply status code
		CvString	m_statusMessage;	// HTTP reply status code		
	};

	class CvContext
	{
		friend class CvHttpServer;
		friend class CvHttpServerMg;
		friend class CvHttpServerUv;
		
	public:
		virtual ~CvContext()	{}
		
		const CvRequest&	GetRequest() const	{ return m_request; }
		CvResponse&			GetResponse()		{ return m_response; }

		virtual bool		SendResponse() = 0;
		virtual void		CloseConnection() = 0;
		
	protected:
		CvContext()	{}
		CvContext( const CvContext& orig )	{}		
		
		CvRequest		m_request;
		CvResponse		m_response;
	};
	
	void Init( IN CMapOptions& aOptions )	{}
	
	bool Start()	{ return false; }

	virtual bool	OnReceiveRequest( INOUT CvContext& aContext ) = 0;
};


CvString CvHttpServer::CvRequest::GetHeaderValue( const CvString& aHeaderName ) const
{
	if ( m_mapHeaders.count( aHeaderName ) < 1 )
		return "";
	
	return m_mapHeaders.find( aHeaderName )->second;
}

void CvHttpServer::CvResponse::SetContent( const void* apContent, uint32_t aLen )
{
	if ( aLen > 0 )
	{
		m_content.resize( aLen );
		memcpy( (void*)m_content.data(), apContent, aLen );		
	}
	else
		m_content.clear();
	
	SetHeaderValue( HTTP_HEADER_CONTENT_LENGTH, CvString(aLen) );
}

#endif	/* CVHTTPSERVER_H */

