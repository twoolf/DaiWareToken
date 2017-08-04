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

#ifndef _MPIN_SDK_TEST_ACCESS_NUMBER_THREAD_H_
#define _MPIN_SDK_TEST_ACCESS_NUMBER_THREAD_H_

#include "mpin_sdk.h"
#include "test_context.h"
#include "CvThread.h"
#include "CvMutex.h"

class AccessNumberThread : public CvShared::CvThread
{
public:
    typedef MPinSDK::String String;
    typedef MPinSDK::StringMap StringMap;
    typedef MPinSDK::IHttpRequest IHttpRequest;

    AccessNumberThread(TestContext& context);
    virtual ~AccessNumberThread();
    void Start(const String& backend, const String& webOTT, const String& authenticateURL);
    virtual long Body(void*);
    bool IsFinished();
    void WaitWhileFinished();

    static const int MAX_TRIES = 5;
    static const int RETRY_INTERVAL_MILLISEC = 1000;
    static const int WAIT_INTERVAL_MILLISEC = 100;

private:
    String m_backend;
    String m_webOTT;
    String m_authenticateURL;
    TestContext& m_context;
    IHttpRequest *m_req;
    bool m_finished;
    CvShared::CvMutex m_mutex;
};

#endif // _MPIN_SDK_TEST_ACCESS_NUMBER_THREAD_H_
