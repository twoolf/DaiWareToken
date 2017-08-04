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

#ifndef _TEST_HTTP_REQUEST_H_
#define _TEST_HTTP_REQUEST_H_

#include "mpin_sdk.h"

class HttpRequest : public MPinSDK::IHttpRequest
{
public:
    typedef MPinSDK::String String;
    typedef MPinSDK::StringMap StringMap;

    HttpRequest() : m_timeout(0), m_httpStatusCode(0) {}
    virtual void SetHeaders(const StringMap& headers);
    virtual void SetQueryParams(const StringMap& queryParams);
    virtual void SetContent(const String& data);
    virtual void SetTimeout(int seconds);
    virtual bool Execute(Method method, const String& url);
    virtual const String& GetExecuteErrorMessage() const;
    virtual int GetHttpStatusCode() const;
    virtual const StringMap& GetResponseHeaders() const;
    virtual const String& GetResponseData() const;

private:
    StringMap m_requestHeaders;
    String m_requestQueryParams;
    String m_requestData;
    int m_timeout;
    int m_httpStatusCode;
    StringMap m_responseHeaders;
    String m_responseData;
    String m_errorMessage;
};


#endif // _TEST_HTTP_REQUEST_H_
