/* 
 * File:   CvHttpServerUv.cpp
 * Author: mony
 * 
 * Created on August 27, 2012, 2:23 PM
 */

#include "CvHttpServerUv.h"

#include "CvLogger.h"

#include <arpa/inet.h>
#include <sys/types.h>

//#define __DEBUG__
		
CvHttpServerUv::CMapHandleToServer		CvHttpServerUv::m_mapHandleToServer;
CvHttpServerUv::CSetContexts			CvHttpServerUv::m_setValidContexts;
http_parser_settings					CvHttpServerUv::m_httpParserSettings;

using namespace CvShared;

struct sWriteReq_t
{
	uv_write_t	m_writeReq;
	CvString	m_responseBuf;	
	uv_buf_t	m_writeBuf;
};

CvHttpServerUv::CMapHandleToServer::CMapHandleToServer() :
	m_mutex("map-http-handle-to-server")
{
	m_mutex.Create();
}

void CvHttpServerUv::CMapHandleToServer::Insert( const uv_tcp_t* apHandle, CvHttpServerUv* apServer )
{
	CvMutexLock lock(m_mutex);
	(*this)[apHandle] = apServer;
}

void CvHttpServerUv::CMapHandleToServer::Remove( const uv_tcp_t* apHandle )
{
	CvMutexLock lock(m_mutex);
	erase( apHandle );
}

CvHttpServerUv* CvHttpServerUv::CMapHandleToServer::Find( const uv_tcp_t* apHandle )
{
	CvMutexLock lock(m_mutex);
	
	const_iterator itr = find( apHandle );
	
	if ( itr == end() )
		return NULL;
	
	return itr->second;
}

CvHttpServerUv::CSetContexts::CSetContexts() :
	m_mutex("set-uv-contexts")
{
	m_mutex.Create();
}

void CvHttpServerUv::CSetContexts::Insert( const CvContextUv* apContext )
{
	CvMutexLock lock(m_mutex);
	insert(apContext);
//	LogMessage( enLogLevel_Debug3, "=====> Inserted valid context (%p)", apContext );		
}

void CvHttpServerUv::CSetContexts::Remove( const CvContextUv* apContext )
{
	CvMutexLock lock(m_mutex);
	erase(apContext);
//	LogMessage( enLogLevel_Debug3, "=====> Removed valid context (%p)", apContext );			
}

bool CvHttpServerUv::CSetContexts::Find( const CvContextUv* apContext )
{
	CvMutexLock lock(m_mutex);
	
	if ( find(apContext) == end() )
	{
//		LogMessage( enLogLevel_Debug3, "=====> Context (%p) IS NOT VALID", apContext );
		return false;
	}
	
//	LogMessage( enLogLevel_Debug3, "=====> Context (%p) is valid", apContext );
	
	return true;
}

CvHttpServerUv::~CvHttpServerUv()
{
	m_mapHandleToServer.Remove( &m_hServer );
}

void CvHttpServerUv::Init( IN CMapOptions& aOptions )
{
	if ( aOptions.count(HTTP_SERVER_OPTION_PORT) < 0 )
		LogMessage( enLogLevel_Error, "ERROR: No listening port is provided in HTTP server options." );
	else
		m_port = (u_short)aOptions[HTTP_SERVER_OPTION_PORT].Ulong();
	
	if ( aOptions.count(HTTP_SERVER_OPTION_MAX_CONNECTIONS_NUM) < 0 )
		LogMessage( enLogLevel_Error, "ERROR: No max number of connection is provided in HTTP server options." );
	else
		m_maxConnections = (int)aOptions[HTTP_SERVER_OPTION_MAX_CONNECTIONS_NUM].Long();
	
	m_bInitialized = true;
	
//	m_httpParserSettings.on_headers_complete = _OnHttpHeadersComplete;
	m_httpParserSettings.on_message_complete = _OnHttpMessageComplete;
	m_httpParserSettings.on_header_field = _OnHttpHeaderField;
	m_httpParserSettings.on_header_value = _OnHttpHeaderValue;
	m_httpParserSettings.on_body = _OnHttpBody;
	m_httpParserSettings.on_url = _OnHttpUrl;
}
	
bool CvHttpServerUv::Start()
{
	if ( !m_bInitialized )
		return false;
	
	m_uvLoop = uv_default_loop();

	if ( uv_tcp_init( m_uvLoop, &m_hServer ) != 0 )
	{
		LogMessage( enLogLevel_Error, "HTTP Server libuv failed to init: %s", uv_strerror( uv_last_error(m_uvLoop) ) );
		return false;
	}

	struct sockaddr_in address = uv_ip4_addr( "0.0.0.0", m_port );

	if ( uv_tcp_bind( &m_hServer, address ) != 0 )
	{
		LogMessage( enLogLevel_Error, "HTTP Server libuv failed in bind to port [%d]: %s", m_port, uv_strerror( uv_last_error(m_uvLoop) ) );
		return false;
	}
	
	uv_listen( (uv_stream_t*)&m_hServer, m_maxConnections, _OnConnect );

	m_mapHandleToServer.Insert( &m_hServer, this );
	
	LogMessage( enLogLevel_Debug1, "HTTP Server libuv started on port %d", m_port );

	uv_run( m_uvLoop );
  
	return true;
}

void CvHttpServerUv::CloseConnection( uv_tcp_t* apHandle )
{
	if ( uv_is_closing((uv_handle_t*)apHandle) )
	{
#ifdef __DEBUG__
		LogMessage( enLogLevel_Debug3, "-----> Connection (%p) is already closing", apHandle );
#endif
		return;
	}
	
	m_setValidContexts.Remove( (CvContextUv*)apHandle->data );	
	
#ifdef __DEBUG__
	LogMessage( enLogLevel_Debug3, "-----> Closing connection (%p) with flags [%x]", apHandle, apHandle->flags );
#endif
	
	uv_close( (uv_handle_t*)apHandle, _OnClose );
}
	
void CvHttpServerUv::_OnConnect( uv_stream_t* apServerHandle, int aStatus )
{
	CvHttpServerUv* pServer = m_mapHandleToServer.Find( (uv_tcp_t*)apServerHandle );
	
	if ( pServer == NULL )
		return;
	
	CvContextUv* pContext = new CvContextUv( pServer );

	uv_tcp_init( pServer->m_uvLoop, &pContext->m_hConn );
	
	pContext->m_hConn.data = pContext;
	
	http_parser_init( &pContext->m_httpParser, HTTP_REQUEST );

	pContext->m_httpParser.data = pContext;

	if ( uv_accept( apServerHandle, (uv_stream_t*)&pContext->m_hConn ) != 0 )
	{
		LogMessage( enLogLevel_Error, "HTTP Server libuv failed to accept connection: %s", uv_strerror( uv_last_error(pServer->m_uvLoop) ) );
		return;
	}

	uv_read_start( (uv_stream_t*)&pContext->m_hConn, _OnAlloc, _OnRead );
	
#ifdef __DEBUG__	
	LogMessage( enLogLevel_Debug3, "-----> _OnConnect: connection (%p) with flags [%x]", &pContext->m_hConn, pContext->m_hConn.flags );	
#endif
	
	m_setValidContexts.Insert( pContext );	
}

void CvHttpServerUv::_OnClose( uv_handle_t* apConnectionHandle )
{
    CvContextUv* pContext = (CvContextUv*)apConnectionHandle->data;
	
#ifdef __DEBUG__	
	LogMessage( enLogLevel_Debug3, "-----> _OnClose: connection (%p) with flags [%x]", &pContext->m_hConn, pContext->m_hConn.flags );
#endif
	
    delete pContext;
}

uv_buf_t CvHttpServerUv::_OnAlloc( uv_handle_t* apConnectionHandle, size_t aSuggestedSize )
{
	CvContextUv* pContext = (CvContextUv*)apConnectionHandle->data;
	
	pContext->m_requestBuf.resize( aSuggestedSize );
	
	uv_buf_t buf;
	buf.base = (char*)pContext->m_requestBuf.data();
	buf.len = aSuggestedSize;
	
	return buf;
}

void CvHttpServerUv::_OnRead( uv_stream_t* aConnectionHandle, ssize_t aSizeToRead, uv_buf_t aBuf )
{
	CvContextUv* pContext = (CvContextUv*)aConnectionHandle->data;

#ifdef __DEBUG__	
	LogMessage( enLogLevel_Debug3, "-----> _OnRead: connection (%p) with flags [%x]", &pContext->m_hConn, pContext->m_hConn.flags );
#endif
	
	if ( aSizeToRead < 0 )
	{
		uv_err_t err = uv_last_error( pContext->m_pServer->m_uvLoop );
		if ( err.code != UV_EOF )
		{
			LogMessage( enLogLevel_Error, "HTTP Server libuv failed in read: %s", uv_strerror(err) );
		}

		pContext->CloseConnection();
		
		return;
	}
	
	size_t sizeParsed = http_parser_execute( &pContext->m_httpParser, &m_httpParserSettings, aBuf.base, aSizeToRead );
	
	if ( sizeParsed < aSizeToRead )
	{
		LogMessage( enLogLevel_Error, "HTTP Server libuv failed parse data" );
		
		pContext->CloseConnection();
	}
}

void CvHttpServerUv::_OnAfterWrite( uv_write_t* apRequest, int aStatus )
{
	CvContextUv* pContext = (CvContextUv*)apRequest->handle->data;
		
#ifdef __DEBUG__
	LogMessage( enLogLevel_Debug3, "-----> _OnAfterWrite: request [%p] connection (%p) with flags [%x]", apRequest, &pContext->m_hConn, pContext->m_hConn.flags );
#endif
	
	if ( aStatus != 0 )
	{
		LogMessage( enLogLevel_Error, "HTTP Server libuv failed to write response: %s",
				uv_strerror( uv_last_error(pContext->m_pServer->m_uvLoop) ) );
	}
	
	delete (sWriteReq_t*)apRequest->data;
	
//	pContext->CloseConnection();
}

int CvHttpServerUv::_OnHttpUrl( http_parser* apHttpParser, const char *at, size_t length )
{
	CvContextUv* pContext = (CvContextUv*)apHttpParser->data;

	pContext->m_request.m_uri.assign( at, length );

	size_t pos = pContext->m_request.m_uri.find('?');
	if ( pos != CvString::npos )
	{
		pContext->m_request.m_queryString = pContext->m_request.m_uri.substr( pos + 1 );
		pContext->m_request.m_uri.resize( pos );
	}

	return 0;
}

int CvHttpServerUv::_OnHttpHeaderField( http_parser* apHttpParser, const char *at, size_t length )
{
	CvContextUv* pContext = (CvContextUv*)apHttpParser->data;

	if ( !pContext->m_lastHeaderField.empty() )
		pContext->m_request.m_mapHeaders[ pContext->m_lastHeaderField ] = "";
		
	pContext->m_lastHeaderField.assign( at, length );

        return 0;
}

int CvHttpServerUv::_OnHttpHeaderValue( http_parser* apHttpParser, const char *at, size_t length )
{
	CvContextUv* pContext = (CvContextUv*)apHttpParser->data;
	
	if ( !pContext->m_lastHeaderField.empty() )
	{
		pContext->m_request.m_mapHeaders[ pContext->m_lastHeaderField ] = CvString( at, length );
		pContext->m_lastHeaderField.clear();
	}

        return 0;
}

//int CvHttpServerUv::_OnHttpHeadersComplete( http_parser* apHttpParser )
//{
//    CvContextUv* pContext = (CvContextUv*)apHttpParser->data;
////    pClient->setCookieID( );
//
//    return 0;
//}

int CvHttpServerUv::_OnHttpBody( http_parser* apHttpParser, const char *at, size_t length )
{
	CvContextUv* pContext = (CvContextUv*)apHttpParser->data;	

	pContext->m_request.m_content.assign( at, length );
	
        return 0;
}

int CvHttpServerUv::_OnHttpMessageComplete( http_parser* apHttpParser )
{
	CvContextUv* pContext = (CvContextUv*)apHttpParser->data;
	
	const char* pHttpMethod = http_method_str( (http_method)apHttpParser->method );
	
	pContext->m_request.m_method = HttpMethodStringToEnum( pHttpMethod );
	
	CvRequest& request = pContext->m_request;
	
	uint32_t dataSize = request.GetHeaderValue( HTTP_HEADER_CONTENT_LENGTH ).Ulong();
	if ( dataSize > 0 )
	{
		LogMessage( enLogLevel_Debug3, "<== [%s] HTTP request [%s] uri [%s] data-size [%d] data [%s]",
				request.GetRemoteIp().c_str(), pHttpMethod, request.GetUri().c_str(),
				dataSize, (const char*)request.GetContent() );
	}
	else
	{
		LogMessage( enLogLevel_Debug3, "<== [%s] HTTP request [%s] uri [%s] data-size [0]",
				request.GetRemoteIp().c_str(), pHttpMethod, request.GetUri().c_str() );
	}
	
	if ( !pContext->m_pServer->OnReceiveRequest( (CvContextHandle)pContext ) )
		return 0;
	
    return 0;
}

CvMutexLock CvHttpServerUv::LockContext( const CvContextHandle ahContext, OUT CvContext*& apContext )
{
	CvMutexLock lock = m_setValidContexts.Lock();
	
	apContext = NULL;
	
	CvContextUv* pContext = (CvContextUv*)ahContext;
	
	if ( m_setValidContexts.Find(pContext) )
		apContext = pContext;
	
	return lock;
}
	
CvHttpServerUv::CvContextUv::CvContextUv( CvHttpServerUv* apServer ) :
	m_pServer(apServer)
{
	memset( &m_hConn, 0, sizeof(m_hConn) );
	memset( &m_httpParser, 0, sizeof(m_httpParser) );
}

bool CvHttpServerUv::CvContextUv::SendResponse()
{
	CvMutexLock lock = CvHttpServerUv::m_setValidContexts.Lock();
	
	if ( !CvHttpServerUv::m_setValidContexts.Find( this ) )
	{
		LogMessage( enLogLevel_Warning, "Context (%p) is already INVALID", this );
		return false;
	}
	
	if ( uv_is_closing((uv_handle_t*)&m_hConn) )
	{
		LogMessage( enLogLevel_Warning, "Connection (%p) is already closing", &m_hConn );
		return false;
	}
	
	sWriteReq_t* pWriteRequest = new sWriteReq_t;
	
	m_response >> pWriteRequest->m_responseBuf;
	
	pWriteRequest->m_writeBuf.base = (char*)pWriteRequest->m_responseBuf.data();
	pWriteRequest->m_writeBuf.len = pWriteRequest->m_responseBuf.length();
	
#ifdef __DEBUG__
	LogMessage( enLogLevel_Debug3, "-----> SendResponse: connection (%p) with flags [%x] write request [%p]",
			&m_hConn, m_hConn.flags, &pWriteRequest->m_writeReq );
#endif

        int rc = uv_write( &pWriteRequest->m_writeReq, (uv_stream_t*)&m_hConn,
			&pWriteRequest->m_writeBuf, 1, CvHttpServerUv::_OnAfterWrite );

	pWriteRequest->m_writeReq.data = pWriteRequest;
			
	if ( rc != 0 )
	{
		LogMessage( enLogLevel_Error, "ERROR in HTTP server while sending [%d] bytes to [%s]: %s",
				pWriteRequest->m_responseBuf.length(), m_request.GetRemoteIp().c_str(), uv_strerror( uv_last_error(m_pServer->m_uvLoop) ) );
		return false;
	}
	
	LogMessage( enLogLevel_Debug3, "--> [%s]: %s", m_request.GetRemoteIp().c_str(), pWriteRequest->m_responseBuf.c_str() );

	return true;
}

void CvHttpServerUv::CvContextUv::CloseConnection()
{
	CvHttpServerUv::CloseConnection( &m_hConn );
}
