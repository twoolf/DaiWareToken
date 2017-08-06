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

#include "access_number_thread.h"
#include "CvTime.h"

AccessNumberThread::AccessNumberThread(TestContext & context) : m_context(context), m_req(NULL), m_finished(false)
{
    m_mutex.Create();
}

AccessNumberThread::~AccessNumberThread()
{
    WaitWhileFinished();
}

void AccessNumberThread::Start(const String& backend, const String& webOTT, const String& authenticateURL)
{
    m_backend = backend;
    m_webOTT = webOTT;
    m_authenticateURL = authenticateURL;
    m_finished = false;
    m_req = m_context.CreateHttpRequest();
    Create(NULL);
}

long AccessNumberThread::Body(void*)
{
    StringMap headers;
    headers.Put("Content-Type", "application/json");
    headers.Put("Accept", "*/*");

    util::JsonObject json;
    json["webOTT"] = json::String(m_webOTT);
    String payload = json.ToString();

    String url = String().Format("%s/rps/accessnumber", m_backend.c_str());

    int retryCount = 0;
    while(m_req->GetHttpStatusCode() != 200 && retryCount++ < MAX_TRIES)
    {
        CvShared::SleepFor(CvShared::Millisecs(RETRY_INTERVAL_MILLISEC).Value());

        m_req->SetHeaders(headers);
        m_req->SetContent(payload);
        m_req->Execute(MPinSDK::IHttpRequest::POST, url);
    }

    json.Clear();
    util::JsonObject mpinResponse;
    mpinResponse.Parse(m_req->GetResponseData().c_str());
    json["mpinResponse"] = mpinResponse;
    payload = json.ToString();

    m_req->SetHeaders(headers);
    m_req->SetContent(payload);
    m_req->Execute(MPinSDK::IHttpRequest::POST, m_authenticateURL);

    m_context.ReleaseHttpRequest(m_req);
    m_req = NULL;

    {
        CvShared::CvMutexLock lock(m_mutex);
        m_finished = true;
    }

    return 0;
}

bool AccessNumberThread::IsFinished()
{
    CvShared::CvMutexLock lock(m_mutex);
    return m_finished;
}

void AccessNumberThread::WaitWhileFinished()
{
    while (!IsFinished())
    {
        CvShared::SleepFor(CvShared::Millisecs(WAIT_INTERVAL_MILLISEC).Value());
    }
}
