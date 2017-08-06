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
 * MPinSDK::IHttpRequest implementation used for to record test http requests
 */

#include "http_recorder.h"
#include "http_recorded_data.h"
#include <cassert>

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;
typedef HttpRecordedData::Request Request;
typedef HttpRecordedData::Response Response;

void HttpRecorder::SetHeaders(const StringMap & headers)
{
    m_request.SetHeaders(headers);
}

void HttpRecorder::SetQueryParams(const StringMap & queryParams)
{
    assert(false);
}

void HttpRecorder::SetContent(const String & data)
{
    m_requestData = data;
    m_request.SetContent(data);
}

void HttpRecorder::SetTimeout(int seconds)
{
    m_request.SetTimeout(seconds);
}

bool HttpRecorder::Execute(Method method, const String & url)
{
    bool res = m_request.Execute(method, url);

    m_recorder.Record(Request(method, url, m_requestData, m_context),
        Response(res, m_request.GetExecuteErrorMessage(), m_request.GetHttpStatusCode(), m_request.GetResponseHeaders(), m_request.GetResponseData()));

    return res;
}

const String & HttpRecorder::GetExecuteErrorMessage() const
{
    return m_request.GetExecuteErrorMessage();
}

int HttpRecorder::GetHttpStatusCode() const
{
    return m_request.GetHttpStatusCode();
}

const StringMap & HttpRecorder::GetResponseHeaders() const
{
    return m_request.GetResponseHeaders();
}

const String & HttpRecorder::GetResponseData() const
{
    return m_request.GetResponseData();
}
