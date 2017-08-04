/* 
 * File:   CvHttpServer.cpp
 * Author: mony
 * 
 * Created on August 27, 2012, 2:23 PM
 */

#include "CvHttpServer.h"

CvHttpServer::CvRequest::CvRequest() :
	m_method(enHttpMethod_Unknown),
	m_remotePort(-1),
	m_bSSL(false)
{
}

CvHttpServer::CvResponse::CvResponse()
{
	m_statusCode = 0;
	m_httpVersion = "1.1";
	SetHeaderValue( HTTP_HEADER_CONTENT_LENGTH, "0" );
}

void CvHttpServer::CvResponse::operator>>( OUT CvString& aOutput ) const
{
	aOutput.Format( "HTTP/%s %d %s\r\n", m_httpVersion.c_str(), m_statusCode, m_statusMessage.c_str() );
	
	for ( CMapHttpHeaders::const_iterator itr = m_mapHeaders.begin();
			itr != m_mapHeaders.end();
			++itr )
	{
		aOutput += itr->first;
		aOutput += ": ";
		aOutput += itr->second;
		aOutput += "\r\n";		
	}
	
	aOutput += "\r\n";
		
	if ( m_content.size() > 0 )
	{
		int lenHeaders = aOutput.size();
		
		aOutput.resize( lenHeaders + m_content.size() );
		memcpy( (uint8_t*)aOutput.data() + lenHeaders, m_content.data(), m_content.size() );
	}
}
