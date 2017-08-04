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

#ifndef _TEST_CONTEXT_H_
#define _TEST_CONTEXT_H_

#include "mpin_sdk.h"
#include "http_recorded_data.h"

class TestContext : public MPinSDK::IContext
{
public:
    typedef MPinSDK::String String;
    typedef MPinSDK::IHttpRequest IHttpRequest;

    class AutoContextData
    {
    public:
        virtual ~AutoContextData() {}
        virtual String Get() const = 0;
    };

    TestContext();
    TestContext(const AutoContextData& autoContextData);
    ~TestContext();
    void EnterRequestRecorderMode(const String& recordedDataFile);
    void EnterRequestPlayerMode(const String& recordedDataFile);
    void EnterRequestPlayerMode(std::istream& recordedDataInputStream);
    virtual IHttpRequest * CreateHttpRequest() const;
    virtual void ReleaseHttpRequest(IN IHttpRequest *request) const;
    void SetRequestContextData(const String& requestContextData);
    void SetAdditionalContextData(const String& additionalContextData);

protected:
    String GetRequestContextData() const;

    enum Mode { MODE_MAKE_REAL_REQUESTS, MODE_USE_RECORDED_REQUESTS, MODE_RECORD_REAL_REQUESTS };
    Mode m_mode;
    String m_recordedDataFile;
    HttpRecordedData m_recordedData;
    String m_requestContextData;
    const AutoContextData *m_autoContextData;
    String m_additionalContextData;
};

#endif // _TEST_CONTEXT_H_
