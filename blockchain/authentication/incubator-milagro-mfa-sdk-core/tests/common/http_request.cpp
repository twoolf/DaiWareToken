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

/*
 * MPinSDK::IHttpRequest implementation used for tests
 */

#include "http_request.h"
#include "CvHttpRequest.h"

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;
typedef MPinSDK::IHttpRequest IHttpRequest;

static enHttpMethod_t MPinToCvMethod(IHttpRequest::Method method)
{
    switch(method)
    {
    case IHttpRequest::GET:
        return enHttpMethod_GET;
    case IHttpRequest::POST:
        return enHttpMethod_POST;
    case IHttpRequest::PUT:
        return enHttpMethod_PUT;
    case IHttpRequest::DELETE:
        return enHttpMethod_DEL;
    case IHttpRequest::OPTIONS:
    case IHttpRequest::PATCH:
    default:
        assert(false);
        return enHttpMethod_Unknown;
    }
}

void HttpRequest::SetHeaders(const StringMap& headers)
{
    m_requestHeaders = headers;
}

void HttpRequest::SetQueryParams(const StringMap& queryParams)
{
    //m_requestQueryParams = queryParams;
    // TODO: Implement this
    assert(false);
}

void HttpRequest::SetContent(const String& data)
{
    m_requestData = data;
}

void HttpRequest::SetTimeout(int seconds)
{
    m_timeout = seconds;
}

bool HttpRequest::Execute(Method method, const String& url)
{
    m_httpStatusCode = 0;
    m_responseHeaders.clear();
    m_responseData.clear();
    m_errorMessage.clear();

    CvHttpRequest *cvReq = new CvHttpRequest(MPinToCvMethod(method));

    CMapHttpHeaders cvHeaders;
    for(StringMap::iterator i = m_requestHeaders.begin(); i != m_requestHeaders.end(); ++i)
    {
        cvHeaders[i->first] = i->second;
    }
    cvReq->SetHeaders(cvHeaders);

    if(!m_requestData.empty())
    {
        cvReq->SetContent(m_requestData.c_str(), m_requestData.length());
    }

    cvReq->SetUrl(url);

    CvShared::Seconds timeout = CvHttpRequest::TIMEOUT_INFINITE;
    if(m_timeout > 0)
    {
        timeout = m_timeout;
    }

    CvHttpRequest::enStatus_t cvStatus = cvReq->Execute(timeout);
    if(cvStatus == CvHttpRequest::enStatus_NetworkError)
    {
        m_errorMessage = cvReq->GetResponse();
        delete cvReq;
        return false;
    }

    m_httpStatusCode = cvReq->GetResponseCode();
    assert(m_httpStatusCode != 0);

    const CMapHttpHeaders& cvResponseHeaders = cvReq->GetResponseHeaders();
    for(CMapHttpHeaders::const_iterator i = cvResponseHeaders.begin(); i != cvResponseHeaders.end(); ++i)
    {
        m_responseHeaders[i->first] = i->second;
    }

    m_responseData = cvReq->GetResponse();

    delete cvReq;

    m_requestHeaders.clear();
    m_requestQueryParams.clear();
    m_requestData.clear();
    m_timeout = 0;

    return true;
}

const String& HttpRequest::GetExecuteErrorMessage() const
{
    return m_errorMessage;
}

int HttpRequest::GetHttpStatusCode() const
{
    return m_httpStatusCode;
}

const StringMap& HttpRequest::GetResponseHeaders() const
{
    return m_responseHeaders;
}

const String& HttpRequest::GetResponseData() const
{
    return m_responseData;
}
