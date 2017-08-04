/* 
 * File:   CvHttpServerMg.cpp
 * Author: mony
 * 
 * Created on October 8, 2012, 8:43 AM
 */

#include "CvHttpServerMg.h"

#include "CvLogger.h"

#include <arpa/inet.h>
#include <sys/types.h>

using namespace CvShared;

#define HTTP_SERVER_MG_OPTION_PORT			"listening_ports"
#define HTTP_SERVER_MG_OPTION_NUM_THREADS	"num_threads"

CvHttpServerMg::~CvHttpServerMg()
{
	if ( m_pContext != NULL )
		mg_stop( m_pContext );
}

void CvHttpServerMg::Init( CMapOptions& aOptions )
{
	if ( aOptions.count(HTTP_SERVER_OPTION_PORT) < 0 )
		LogMessage( enLogLevel_Error, "ERROR: No listening port is provided in HTTP server options." );
	else
		m_port = (u_short)aOptions[HTTP_SERVER_OPTION_PORT].Ulong();
	
	if ( aOptions.count(HTTP_SERVER_OPTION_NUM_THREADS) < 0 )
		LogMessage( enLogLevel_Error, "ERROR: No number of threads is provided in HTTP server options." );
	else
		m_numOfThreads = (int)aOptions[HTTP_SERVER_OPTION_NUM_THREADS].Long();

	m_bInitialized = true;
}
	
bool CvHttpServerMg::Start()
{
	if ( !m_bInitialized )
		return false;
	
	const char* options[32] = { NULL };

	CvString port((uint32_t)m_port);
	CvString numOfThreads((long)m_numOfThreads);
	
	int i = 0;
	
	options[i++] = HTTP_SERVER_MG_OPTION_PORT; options[i++] = port.c_str();
	options[i++] = HTTP_SERVER_MG_OPTION_NUM_THREADS; options[i++] = numOfThreads.c_str();
	options[i++] = NULL;
	
	m_pContext = mg_start( _RequestCallback, this, options );
	
	return true;
}
	
void* CvHttpServerMg::_RequestCallback( mg_event aEvent, mg_connection* apConn )
{
	if ( aEvent != MG_NEW_REQUEST )
		return NULL;
	
	const mg_request_info* apRequestInfo = mg_get_request_info( apConn );
	CvHttpServerMg* pThis = (CvHttpServerMg*)mg_get_user_data( apConn );
	
	CvContextMg context(apConn);
	CvRequest& request = context.m_request;
	
	pThis->BuildRequest( apRequestInfo, request );
	
	if ( request.m_mapHeaders.count( HTTP_HEADER_CONTENT_LENGTH ) > 0 )
	{
		uint32_t	len = request.m_mapHeaders[HTTP_HEADER_CONTENT_LENGTH].Ulong();
		request.m_content.resize( len );
		mg_read( apConn, (void*)request.m_content.data(), len );
	}

	uint32_t dataSize = request.GetHeaderValue( HTTP_HEADER_CONTENT_LENGTH ).Ulong();
	if ( dataSize > 0 )
	{
		LogMessage( enLogLevel_Debug3, "<== [%s] HTTP request [%s] uri [%s] data-size [%d] data [%s]",
				request.GetRemoteIp().c_str(), apRequestInfo->request_method, request.GetUri().c_str(),
				dataSize, (const char*)request.GetContent() );
	}
	else
	{
		LogMessage( enLogLevel_Debug3, "<== [%s] HTTP request [%s] uri [%s] data-size [0]",
				request.GetRemoteIp().c_str(), apRequestInfo->request_method, request.GetUri().c_str() );
	}
	
	if ( !pThis->OnReceiveRequest( context ) )
		return NULL;	//request was not handled
	
	return (void*)"(dummy)";	//Request was handled
}

void CvHttpServerMg::BuildRequest( const mg_request_info* apRequestInfo, OUT CvRequest& aRequest )
{
	aRequest.m_remotePort = apRequestInfo->remote_port;
	aRequest.m_bSSL = ( apRequestInfo->is_ssl != 0 );
			
	if ( apRequestInfo->request_method != NULL )
		aRequest.m_method = HttpMethodStringToEnum( apRequestInfo->request_method );
	
	if ( apRequestInfo->uri != NULL )
		aRequest.m_uri = apRequestInfo->uri;
	
	if ( apRequestInfo->http_version != NULL )
		aRequest.m_httpVersion = apRequestInfo->http_version;
	
	if ( apRequestInfo->query_string != NULL )
		aRequest.m_queryString = apRequestInfo->query_string;
	
	if ( apRequestInfo->remote_user != NULL )
		aRequest.m_remoteUser = apRequestInfo->remote_user;
	
	for ( int i = 0; i < apRequestInfo->num_headers; ++i )
	{
		aRequest.m_mapHeaders[ apRequestInfo->http_headers[i].name ] = apRequestInfo->http_headers[i].value;
	}
	
	struct in_addr addr;
	addr.s_addr = htonl(apRequestInfo->remote_ip);
	aRequest.m_remoteIp = inet_ntoa( addr );
}

bool CvHttpServerMg::CvContextMg::SendResponse()
{
	CvString strResponse;
	m_response >> strResponse;
	
	int sent = mg_write( m_pConn, strResponse.data(), strResponse.length() );

	if ( sent != strResponse.length() )
	{
		LogMessage( enLogLevel_Error, "ERROR in HTTP server while sending [%d] bytes to [%s]",
				strResponse.length(), m_request.GetRemoteIp().c_str() );
		return false;
	}
	
	LogMessage( enLogLevel_Debug3, "--> [%s]: %s", m_request.GetRemoteIp().c_str(), strResponse.c_str() );
	
	return true;
}

void CvHttpServerMg::CvContextMg::CloseConnection()
{
	mg_close_connection( m_pConn );
}
