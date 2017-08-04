/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

#include "HttpRequest.h"

CHttpRequest::CHttpRequest(const Seconds& aTimeout) : m_timeout(aTimeout)
{
}

CHttpRequest::~CHttpRequest()
{
}

void CHttpRequest::SetHeaders(const StringMap& headers)
{
	StringMap::const_iterator itr = headers.begin();
	for ( ; itr != headers.end(); ++itr )
	{
		m_requestHeaders[itr->first] = itr->second;
	}
}

void CHttpRequest::SetQueryParams(const StringMap& queryParams)
{
	m_queryParams = queryParams;
}

void CHttpRequest::SetContent(const String& data)
{
	m_requestData = data;	// For debugging
	m_request.SetContent( data.data(), data.length() );
}

void CHttpRequest::SetTimeout(int seconds)
{
	m_timeout = seconds;
}

bool CHttpRequest::Execute( MPinSDK::IHttpRequest::Method method, const String& url)
{
	enHttpMethod_t cvHttpMethod = enHttpMethod_Unknown;
	String strMethod;
	
	switch ( method )
	{
		case MPinSDK::IHttpRequest::GET: cvHttpMethod = enHttpMethod_GET; strMethod = "GET"; break;
        case MPinSDK::IHttpRequest::POST: cvHttpMethod = enHttpMethod_POST; strMethod = "POST"; break;
        case MPinSDK::IHttpRequest::PUT: cvHttpMethod = enHttpMethod_PUT; strMethod = "PUT"; break;
        case MPinSDK::IHttpRequest::DELETE: cvHttpMethod = enHttpMethod_DEL; strMethod = "DEL"; break;
	}
	
	if ( cvHttpMethod == enHttpMethod_Unknown )
	{
		m_errorMsg = "Unsupported HTTP method";
		return false;
	}
	
	if (m_requestHeaders.find("X-MIRACL-OS-Class") == m_requestHeaders.end())
	{
		m_requestHeaders["X-MIRACL-OS-Class"] = "linux";
	}
	
	m_request.SetHeaders( m_requestHeaders );

	String fullUrl = url;
	
	if ( !m_queryParams.empty() )
	{
		fullUrl += '?';
		
		StringMap::const_iterator itr = m_queryParams.begin();
		for ( ;itr != m_queryParams.end(); ++itr )
		{
			fullUrl += String().Format( "%s=%s&", itr->first.c_str(), itr->second.c_str() );
		}

		fullUrl.TrimRight("&");
	}
	
	//printf( "--> %s %s [%s]\n", strMethod.c_str(), fullUrl.c_str(), m_requestData.c_str() );

	m_request.SetMethod( cvHttpMethod );
	m_request.SetUrl( fullUrl );
	
	if ( m_request.Execute( m_timeout ) == CvHttpRequest::enStatus_NetworkError )
	{
		m_errorMsg = "Failed to execute HTTP request";
		return false;
	}
	
	const CMapHttpHeaders& headers = m_request.GetResponseHeaders();
	
	CMapHttpHeaders::const_iterator itr = headers.begin();
	for ( ;itr != headers.end(); ++itr )
	{
		m_responseHeaders[itr->first] = itr->second;
	}

	m_responseData = m_request.GetResponse();
	
	//printf( "<-- %ld [%s]\n", m_request.GetResponseCode(), m_responseData.c_str() );

	return true;
}

const MPinSDK::String& CHttpRequest::GetExecuteErrorMessage() const
{
	return m_errorMsg;
}

int CHttpRequest::GetHttpStatusCode() const
{
	return (int)m_request.GetResponseCode();
}

const MPinSDK::StringMap& CHttpRequest::GetResponseHeaders() const
{
	return m_responseHeaders;
}

const MPinSDK::String& CHttpRequest::GetResponseData() const
{
	return m_responseData;
}


