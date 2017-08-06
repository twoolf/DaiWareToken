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
* MPinSDK::IContext tests base implementation
*/

#include "test_context.h"
#include "../common/http_request.h"
#include "../common/http_recorder.h"
#include "../common/http_player.h"

typedef MPinSDK::String String;
typedef MPinSDK::IHttpRequest IHttpRequest;

TestContext::TestContext() : m_mode(MODE_MAKE_REAL_REQUESTS), m_autoContextData(NULL)
{
}

TestContext::TestContext(const AutoContextData& autoContextData) : m_autoContextData(&autoContextData)
{
}

TestContext::~TestContext()
{
    if (m_mode == MODE_RECORD_REAL_REQUESTS)
    {
        m_recordedData.SaveTo(m_recordedDataFile);
    }
}

void TestContext::EnterRequestRecorderMode(const String& recordedDataFile)
{
    m_mode = MODE_RECORD_REAL_REQUESTS;
    m_recordedDataFile = recordedDataFile;
}

void TestContext::EnterRequestPlayerMode(const String& recordedDataFile)
{
    m_mode = MODE_USE_RECORDED_REQUESTS;
    m_recordedDataFile = recordedDataFile;
    m_recordedData.LoadFrom(recordedDataFile);
}

void TestContext::EnterRequestPlayerMode(std::istream & recordedDataInputStream)
{
    m_mode = MODE_USE_RECORDED_REQUESTS;
    m_recordedData.LoadFrom(recordedDataInputStream);
}

IHttpRequest * TestContext::CreateHttpRequest() const
{
    switch (m_mode)
    {
    case MODE_MAKE_REAL_REQUESTS:
        return new HttpRequest();
    case MODE_USE_RECORDED_REQUESTS:
        return new HttpPlayer(const_cast<HttpRecordedData&>(m_recordedData), GetRequestContextData());
    case MODE_RECORD_REAL_REQUESTS:
        return new HttpRecorder(const_cast<HttpRecordedData&>(m_recordedData), GetRequestContextData());
    default:
        assert(false);
        return NULL;
    }
}

void TestContext::ReleaseHttpRequest(IN IHttpRequest * request) const
{
    delete request;
}

void TestContext::SetRequestContextData(const String & requestContextData)
{
    m_requestContextData = requestContextData;
}

void TestContext::SetAdditionalContextData(const String & additionalContextData)
{
    m_additionalContextData = additionalContextData;
}

String TestContext::GetRequestContextData() const
{
    return m_requestContextData + (m_autoContextData ? ("@" + m_autoContextData->Get()) : "") + m_additionalContextData;
}
