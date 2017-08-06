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
 * MPinSDK::IHttpRequest implementation used for to reproduce recorded http requests
 */

#include "http_player.h"

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;

void HttpPlayer::SetHeaders(const StringMap & headers)
{
}

void HttpPlayer::SetQueryParams(const StringMap & queryParams)
{
    assert(false);
}

void HttpPlayer::SetContent(const String & data)
{
    m_requestData = data;
}

void HttpPlayer::SetTimeout(int seconds)
{
}

bool HttpPlayer::Execute(Method method, const String & url)
{
    m_response = m_recordedData.FindResponseFor(Request(method, url, m_requestData, m_context));
    return m_response.success;
}

const String & HttpPlayer::GetExecuteErrorMessage() const
{
    return m_response.error;
}

int HttpPlayer::GetHttpStatusCode() const
{
    return m_response.httpStatus;
}

const StringMap & HttpPlayer::GetResponseHeaders() const
{
    return m_response.headers;
}

const String & HttpPlayer::GetResponseData() const
{
    return m_response.data;
}
