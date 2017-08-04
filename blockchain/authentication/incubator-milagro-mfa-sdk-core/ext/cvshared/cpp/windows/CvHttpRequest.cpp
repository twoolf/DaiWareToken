/***************************************************************************************************************************************************************************************************************************
																																																						   *
This file is part of CertiVox M-Pin Client and Server Libraries.																																						   *
The CertiVox M-Pin Client and Server Libraries provide developers with an extensive and efficient set of strong authentication and cryptographic functions.																   *
For further information about its features and functionalities please refer to http://www.certivox.com																													   *
The CertiVox M-Pin Client and Server Libraries are free software: you can redistribute it and/or modify it under the terms of the BSD 3-Clause License http://opensource.org/licenses/BSD-3-Clause as stated below.		   *
The CertiVox M-Pin Client and Server Libraries are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
Note that CertiVox Ltd issues a patent grant for use of this software under specific terms and conditions, which you can find here: http://certivox.com/about-certivox/patents/											   * 	
Copyright (c) 2013, CertiVox UK Ltd																																														   *	
All rights reserved.																																																	   *
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:																			   *
•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.																						   *	
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.			   *	
•	Neither the name of CertiVox UK Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.								   *
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,																		   *
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS																	   *
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE																	   *	
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,														   *
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.																		   *	
																																																						   *
***************************************************************************************************************************************************************************************************************************/
/*! \file  CvHttpRequest.cpp
    \brief C++ class providing portable HTTP request functionality

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 28, 2012, 4:30 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : WinHttp.lib (Windows) libcurl.so (Linux)

 C++ class providing portable HTTP request functionality

*/

#include "CvHttpRequest.h"

#include "CvLogger.h"
#include "CvXcode.h"
#include "CvString.h"

#include <stdexcept>

#include <string.h>
#include <stdlib.h>

using namespace CvShared;
using namespace std;

CvHttpRequest::CvHttpRequest( enHttpMethod_t method ) :
	m_bCancel(false), m_responseCode(0), m_progressUp(0), m_progressDown(0),
	m_mutex("http-request")
{
	m_mutex.Create();
	m_req.method = method;

	DWORD tracing = 1;
	WinHttpSetOption( NULL, WINHTTP_OPTION_ENABLETRACING, &tracing, sizeof(tracing) );
}

CvHttpRequest::~CvHttpRequest()
{
}

void CvHttpRequest::SetHeaders( const CMapHttpHeaders& aHeaders )
{
	m_req.header_list.clear();

	for ( CMapHttpHeaders::const_iterator itr = aHeaders.begin(); itr != aHeaders.end(); ++itr )
	{
		if ( !itr->first.empty() && !itr->second.empty() )
			m_req.header_list += ( itr->first + ": " + itr->second + "\r\n" );
	}
}

const string& CvHttpRequest::GetResponseHeader( const string& aKey ) const
{
	static const string empty;

	if ( m_responseHeaders.count(aKey) < 1 )
		return empty;

	return m_responseHeaders.find(aKey)->second;
}

void CvHttpRequest::Start()
{
	if ( m_req.data_size > 0 )
	{
		LogMessage( enLogLevel_Debug2, "==> [%s] HTTP request [%s] data-size [%lld] data [%s]",
				m_req.url.c_str(), HttpMethodEnumToString(m_req.method).c_str(), m_req.data_size, m_req.data );
	}
	else
	{
		LogMessage( enLogLevel_Debug2, "==> [%s] HTTP request [%s] data-size [0]",
				m_req.url.c_str(), HttpMethodEnumToString(m_req.method).c_str() );
	}

//#ifdef _DEBUG
//	cout << "==> HTTP request [" << this << "] -- " << HttpMethodEnumToString(m_req.method) << " " << m_req.url << ", post-data size: " << m_req.data_size << endl;
//#else
//	cout << "==> HTTP request -- " << HttpMethodEnumToString(m_req.method) << " " << m_req.url << ", post-data size: " << m_req.data_size << endl;
//#endif

	m_progressUp = 0;
	m_progressDown = 0;

	{
		CvMutexLock lock(m_mutex);
		m_bCancel = false;
	}

	URL_COMPONENTS urlComp;
	// Initialize the URL_COMPONENTS structure.
	ZeroMemory( &urlComp, sizeof(urlComp) );
	urlComp.dwStructSize = sizeof(urlComp);

	// Set required component lengths to non-zero 
	// so that they are cracked.
	urlComp.dwSchemeLength    = (DWORD)-1;
	urlComp.dwHostNameLength  = (DWORD)-1;
	urlComp.dwUrlPathLength   = (DWORD)-1;
	urlComp.dwExtraInfoLength = (DWORD)-1;

	wstring url = StringToWstring( m_req.url );

	bool bOk = true;
	char Error[1024];

	wstring scheme;
	wstring hostname;
	wstring urlPath;
	wstring query;

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;

	// Parse the URL
	bOk = ( WinHttpCrackUrl( url.c_str(), 0, 0, &urlComp ) == TRUE );

	if( bOk )
	{
		if ( urlComp.lpszScheme != NULL && urlComp.dwSchemeLength > 0 )
			scheme.assign( urlComp.lpszScheme, urlComp.dwSchemeLength );

		if ( urlComp.lpszHostName != NULL && urlComp.dwHostNameLength > 0 )
			hostname.assign( urlComp.lpszHostName, urlComp.dwHostNameLength );

		if ( urlComp.lpszUrlPath != NULL && urlComp.dwUrlPathLength > 0 )
			urlPath.assign( urlComp.lpszUrlPath, urlComp.dwUrlPathLength );

		if ( urlComp.lpszExtraInfo != NULL && urlComp.dwExtraInfoLength > 0 )
			query.assign( urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength );
	}
	else
	{
		sprintf_s( Error, "Failed in WinHttpCrackUrl with error: %u", GetLastError() );
	}

	bool bHttps = ( scheme == L"https" );

	if ( bOk )
	{
		DWORD accessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
		LPCWSTR pProxyName = WINHTTP_NO_PROXY_NAME;
		wstring proxy;
		
		if ( !m_req.proxy.empty() )
		{
			accessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxy = StringToWstring( m_req.proxy );
			pProxyName = proxy.c_str();
		}

		hSession = WinHttpOpen( NULL, accessType, pProxyName, WINHTTP_NO_PROXY_BYPASS, 0 );

		if ( hSession == NULL )
		{
			sprintf_s( Error, "Failed in WinHttpOpen with error: %u", GetLastError() );
			bOk = false;
		}
	}

	if ( bOk )
	{
		INTERNET_PORT port = bHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
		if ( urlComp.nPort > 0 )
			port = urlComp.nPort;

		hConnect = WinHttpConnect( hSession, hostname.c_str(), port, 0 );

		if ( hConnect == NULL )
		{
			sprintf_s( Error, "Failed in WinHttpConnect with error: %u", GetLastError() );
			bOk = false;
		}
	}	
	
	if ( bOk )
	{
		hRequest = WinHttpOpenRequest( hConnect, StringToWstring( HttpMethodEnumToString(m_req.method) ).c_str(),
										urlComp.lpszUrlPath, NULL /* Default version HTTP/1.1 */,
										WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
										bHttps ? WINHTTP_FLAG_SECURE : 0 );

		if ( hRequest == NULL )
		{
			sprintf_s( Error, "Failed in WinHttpOpenRequest with error: %u", GetLastError() );
			bOk = false;
		}
	}

	if ( bOk )
	{
		// Prepare and send request
		LPCWSTR pHeaders = WINHTTP_NO_ADDITIONAL_HEADERS;
		wstring headers;

		if ( !m_req.header_list.empty() )
		{
			headers = StringToWstring( m_req.header_list );
			pHeaders = headers.c_str();
		}

//		LPVOID pData = WINHTTP_NO_REQUEST_DATA;
		DWORD dataLen = 0;

		if ( m_req.data != NULL && m_req.data_size > 0 )
		{
//			pData = m_req.data;
			dataLen = (DWORD)m_req.data_size;
		}

//		if ( WinHttpSendRequest( hRequest, pHeaders, -1, pData, dataLen, dataLen, 0 ) != TRUE )
		if ( WinHttpSendRequest( hRequest, pHeaders, -1, WINHTTP_NO_REQUEST_DATA, 0, dataLen, 0 ) != TRUE )
		{
			sprintf_s( Error, "Failed in WinHttpSendRequest with error: %u", GetLastError() );
			bOk = false;
		}

//		m_progressUp = dataLen;

		if ( bOk && dataLen )
		{
			if ( !SendRequestData( hRequest ) )
			{
				sprintf_s( Error, "Failed to send request data , error: %u", GetLastError() );
				bOk = false;
			}
		}
	}

	if ( bOk )
	{
		if ( WinHttpReceiveResponse( hRequest, NULL ) != TRUE )
		{
			sprintf_s( Error, "Failed in WinHttpReceiveResponse with error: %u", GetLastError() );
			bOk = false;
		}
	}
			
	m_responseCode = 0;
	DWORD err = 0;

	// For some reason, in some cases WinHttp calls fails with error 12030.
	// It happens mainly when sending requets to the local (127.0.0.1) CouchDB,
	// which sometimes closes the connection right after it sends the response.
	// We are going to ignore this error meanwhile

	if ( bOk )
	{
		if ( !ReadResponseCode( hRequest ) )
		{
			err = GetLastError();
			sprintf_s( Error, "Failed to read response code, error: %u", err );
			bOk = false;
		}
	}

	if ( bOk )
	{
		if ( !ReadResponseHeaders( hRequest ) )
		{
			err = GetLastError();
			sprintf_s( Error, "Failed to read response headers, error: %u", err );
			bOk = false;
		}
	}

	if ( bOk )
	{
		if ( !ReadResponseData( hRequest ) )
		{
			err = GetLastError();
			sprintf_s( Error, "Failed to read response data, error: %u", err );
			bOk = false;
		}
	}

	if ( hRequest != NULL )
		WinHttpCloseHandle( hRequest );

	if ( hConnect != NULL )
		WinHttpCloseHandle( hConnect );

	if ( hSession != NULL )
		WinHttpCloseHandle( hSession );

	if ( bOk )
	{
//#ifdef _DEBUG
//		cout << "<-- HTTP response [" << this << "]: [" << m_responseCode << "] " << m_response << endl;
//#else
//		cout << "<-- HTTP response: [" << m_responseCode << "]" << endl;
//#endif
		LogMessage( enLogLevel_Debug2, "<-- [%s] HTTP response-code [%d] data [%s]", m_req.url.c_str(), m_responseCode, m_response.c_str() );

		if ( m_responseCode > 202 )
		{
			LogMessage( enLogLevel_Error, "<-- [%s] HTTP response: %s", m_req.url.c_str(), m_response.c_str() );
			throw runtime_error( m_response.c_str() );
		}
	}
	else
	{
		LogMessage( enLogLevel_Error, "<-- [%s] HTTP error: %s",  m_req.url.c_str(), Error );
//		if ( err != ERROR_WINHTTP_CONNECTION_ERROR )
//			cout << "<-- HTTP [" << m_responseCode << "] error: " << string(Error) << endl;
		m_response = Error;

		throw runtime_error( Error );
	}
}

CvHttpRequest::enStatus_t CvHttpRequest::Execute( const Seconds& aTimeout, bool abProgress )
{
	m_req.no_progress = abProgress ? 0 : 1;
	m_req.timeout = aTimeout.Value();
	
	try
	{
		Start();
	}
	catch (exception &e)
	{
		LogMessage( enLogLevel_Error, "Error: %s", e.what() );
		
		if (m_responseCode == 0)
			return enStatus_NetworkError;
		if (m_responseCode >= 400 && m_responseCode < 500)
			return enStatus_ClientError;
		if (m_responseCode >= 500)
			return enStatus_ServerError;
	}
	
	return enStatus_Ok;
}

bool CvHttpRequest::SendRequestData( HINTERNET ahRequest )
{
	m_progressUp = 0;

	if ( m_req.data_size <= 0 )
		return true;

	while ( m_progressUp < m_req.data_size )
	{
		long long sizeRemaining = m_req.data_size - m_progressUp;
		long long sizeToSend = ( sizeRemaining > 1024) ? 1024 : sizeRemaining;

		DWORD sizeWritten = 0;
		if ( WinHttpWriteData( ahRequest, &m_req.data[m_progressUp], (DWORD)sizeToSend, &sizeWritten ) != TRUE )
			return false;

		m_progressUp += sizeWritten;

		if ( sizeWritten < sizeToSend )
			return false;
	}

	return true;
}

bool CvHttpRequest::ReadResponseCode( HINTERNET ahRequest )
{
	wchar_t buf[128] = {L'\0'};
	DWORD bufSize = sizeof(buf)/sizeof(wchar_t) - 1;

	if ( WinHttpQueryHeaders( ahRequest, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, buf, &bufSize, WINHTTP_NO_HEADER_INDEX ) != TRUE )
		return false;

	m_responseCode = _wtoi(buf);

	return true;
}

bool CvHttpRequest::ReadResponseHeaders( HINTERNET ahRequest )
{
	DWORD dwSize = 0;

	bool bResult = true;

	// Query the size
	WinHttpQueryHeaders( ahRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX );

	// Allocate memory for the buffer.
	if( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
	{
		wchar_t* pBuffer = new wchar_t[dwSize/sizeof(wchar_t)];

		// Now, use WinHttpQueryHeaders to retrieve the header.
		bResult = ( WinHttpQueryHeaders( ahRequest, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX, pBuffer, &dwSize, WINHTTP_NO_HEADER_INDEX ) == TRUE );

		if ( bResult )
		{
			wchar_t* pHeader = pBuffer;

			while ( *pHeader != L'\0' )
			{
				wstring header = pHeader;
				
				size_t posDelim = header.find( L':' );

				if ( posDelim != wstring::npos )
				{
					size_t posEndKey = header.find_last_not_of( L' ', posDelim-1 );
					wstring key = header.substr( 0, posEndKey+1 );

					size_t posStartValue = header.find_first_not_of( L' ', posDelim+1 );
					wstring value = header.substr( posStartValue );

					m_responseHeaders[ WstringToString(key) ] = WstringToString( value );
				}

				pHeader += wcslen( pHeader ) + 1;
			}
		}

		delete[] pBuffer;
	}
	else
		bResult = false;

	return bResult;
}

bool CvHttpRequest::ReadResponseData( HINTERNET ahRequest )
{
	m_progressDown = 0;

	m_req.resp_size = 0;
	m_response.clear();

	if ( !m_req.fname.empty() )
	{
		FILE* pFile = NULL;

		fopen_s( &pFile, m_req.fname.c_str(), "wb" );

		if ( pFile == NULL )
			return false;

		bool bOk = true;

		DWORD dwSize = 0;
		do
		{
			if ( WinHttpQueryDataAvailable( ahRequest, &dwSize ) == TRUE )
			{
				if ( dwSize > 0 )
				{
					char* pBuf = new char[dwSize];

					DWORD dwDownloaded;
					if ( WinHttpReadData( ahRequest, (LPVOID)pBuf, dwSize, &dwDownloaded ) == TRUE )
					{
						if ( fwrite( pBuf, 1, dwDownloaded, pFile ) < dwDownloaded )
							bOk = false;

						m_progressDown += dwDownloaded;
					}

					delete[] pBuf;
				}
			}
			else
				bOk = false;
		}
		while ( dwSize > 0 && bOk );

		fclose( pFile );

		if ( !bOk )
			return false;
	}
	else
	{
		DWORD dwSize = 0;
		do
		{
			if ( WinHttpQueryDataAvailable( ahRequest, &dwSize ) != TRUE )
			{
				return false;
			}

			if ( dwSize > 0 )
			{
				m_response.resize( (size_t)m_req.resp_size + dwSize );

				DWORD dwDownloaded;
				if ( WinHttpReadData( ahRequest, (LPVOID)(m_response.data() + m_req.resp_size), dwSize, &dwDownloaded ) != TRUE )
					return false;

				m_progressDown += dwDownloaded;
				m_req.resp_size += dwDownloaded;
			}
		}
		while ( dwSize > 0 );
	}

	return true;
}

bool CvHttpRequest::EncodeURL( const string& aUrl, OUT string& aEncodedUrl )
{
	URL_COMPONENTS urlComp;
	// Initialize the URL_COMPONENTS structure.
	ZeroMemory( &urlComp, sizeof(urlComp) );
	urlComp.dwStructSize = sizeof(urlComp);

	// Set required component lengths to non-zero 
	// so that they are cracked.
	urlComp.dwSchemeLength    = (DWORD)-1;
	urlComp.dwHostNameLength  = (DWORD)-1;
	urlComp.dwUrlPathLength   = (DWORD)-1;
	urlComp.dwExtraInfoLength = (DWORD)-1;

	wstring url = StringToWstring( aUrl );

	// Parse the URL
	bool bOk = ( WinHttpCrackUrl( url.c_str(), 0, 0, &urlComp ) == TRUE );
	
	if ( !bOk )
		return false;

	url.resize( url.length()*3 );
	DWORD size = (DWORD)url.length();

	bOk = ( WinHttpCreateUrl( &urlComp, ICU_ESCAPE, (LPWSTR)url.data(), &size ) == TRUE );

	if ( !bOk )
		return false;

	url.resize( size );

	aEncodedUrl = WstringToString( url );

	return true;
}

void CvHttpRequest::Clear()
{
	m_responseCode = 0;
	m_progressUp = 0;
	m_progressDown = 0;
	m_contentSize = 0;
	
	{
		CvMutexLock lock(m_mutex);		
		m_bCancel = false;
	}
	
	m_response.clear();
	m_responseHeaders.clear();
	m_req.Clear();
}

void CvHttpRequest::sRequestData_t::Clear()
{
	header_list.clear();
	data = NULL;
	data_size = 0;
	method = enHttpMethod_Unknown;
	no_progress = true;
	resume_from = 0;
	timeout = CvHttpRequest::TIMEOUT_INFINITE;

	url.clear();
	fname.clear();
	proxy.clear();
}
