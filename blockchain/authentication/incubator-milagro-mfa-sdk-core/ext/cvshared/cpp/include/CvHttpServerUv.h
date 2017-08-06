/* 
 * File:   CvHttpServerUv.h
 * Author: mony
 *
 * Created on August 27, 2012, 2:23 PM
 */

#ifndef CVHTTPSERVERUV_H
#define	CVHTTPSERVERUV_H

#include "CvHttpServer.h"

#include "CvString.h"
#include "CvMutex.h"

#include "libuv/include/uv.h"
#include "http-parser/http_parser.h"

#include <map>
#include <set>

#include <stdarg.h>
#include <string.h>

class CvHttpServerUv : public CvHttpServer
{
	friend class CvContextUv;

public:

	CvHttpServerUv() : m_bInitialized(false), m_port(-1), m_maxConnections(-1)	{}
	CvHttpServerUv( IN CMapOptions& aOptions )	{ Init( aOptions ); }
	virtual ~CvHttpServerUv();
	
	void Init( IN CMapOptions& aOptions );
	
	bool Start();
	
protected:
	typedef CvShared::CvMutex		CvMutex;
	typedef CvShared::CvMutexLock	CvMutexLock;
	
	CvHttpServerUv(const CvHttpServerUv& orig)	{}
	
	class CvContextUv : public CvContext
	{
		friend class CvHttpServerUv;
		
	public:
		virtual ~CvContextUv()	{}
		
		virtual bool	SendResponse();
		virtual void	CloseConnection();
		
	protected:
		CvContextUv( CvHttpServerUv* apServer = NULL );
		CvContextUv( const CvContextUv& orig )	{}
		
		CvHttpServerUv*	m_pServer;		
		uv_tcp_t		m_hConn;
		// Request fields
		CvString		m_requestBuf;
		// HTTP prase fields
		http_parser		m_httpParser;
		CvString		m_lastHeaderField;
	};
	
	typedef void* CvContextHandle;
	
	class CMapHandleToServer : private std::map<const uv_tcp_t*,CvHttpServerUv*>
	{
	public:
		CMapHandleToServer();
		inline void				Insert( const uv_tcp_t* apHandle, CvHttpServerUv* apServer );
		inline CvHttpServerUv*	Find( const uv_tcp_t* apHandle );
		inline void				Remove( const uv_tcp_t* apHandle );
	private:
		CvMutex		m_mutex;
	};
	
	class CSetContexts : private std::set<const CvContextUv*>
	{
	public:
		CSetContexts();
		CvMutexLock		Lock()		{ return CvMutexLock(m_mutex); }
		inline void		Insert( const CvContextUv* apContext );
		inline bool		Find( const CvContextUv* apContext );
		inline void		Remove( const CvContextUv* apContext );
	private:
		CvMutex		m_mutex;
	};
	
	static void		CloseConnection( uv_tcp_t* apHandle );
	
	static void		_OnConnect( uv_stream_t* apServerHandle, int aStatus );
	static void		_OnClose( uv_handle_t* apConnectionHandle )	;
	static uv_buf_t         _OnAlloc( uv_handle_t* apConnection, size_t aSuggestedSize );
	static void		_OnRead( uv_stream_t* aConnectionHandle, ssize_t aSizeToRead, uv_buf_t aBuf );
	static void		_OnAfterWrite( uv_write_t* apRequest, int aStatus );

//	static int		_OnHttpHeadersComplete( http_parser* apHttpParser );
	static int		_OnHttpMessageComplete( http_parser* apHttpParser );
	static int		_OnHttpUrl( http_parser* apHttpParser, const char *at, size_t length );
	static int		_OnHttpHeaderField( http_parser* apHttpParser, const char *at, size_t length );
	static int		_OnHttpHeaderValue( http_parser* apHttpParser, const char *at, size_t length );
	static int		_OnHttpBody( http_parser* apHttpParser, const char *at, size_t length );
	
	CvMutexLock		LockContext( const CvContextHandle ahContext, OUT CvContext*& apContext );
	
	virtual bool	OnReceiveRequest( IN CvContextHandle ahContext ) = 0;
	
	virtual bool	OnReceiveRequest( INOUT CvContext& aContext )	{ return false; }
	
	bool			m_bInitialized;
	u_short			m_port;
	int				m_maxConnections;
	
	uv_loop_t*		m_uvLoop;
	uv_tcp_t		m_hServer;
	
	static CMapHandleToServer	m_mapHandleToServer;
	static CSetContexts			m_setValidContexts;
	static http_parser_settings	m_httpParserSettings;
};

#endif	/* CVHTTPSERVERUV_H */

