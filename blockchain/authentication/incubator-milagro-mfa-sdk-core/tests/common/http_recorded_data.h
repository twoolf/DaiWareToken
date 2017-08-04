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
 * Http requests recorded data
 */

#ifndef _TEST_HTTP_RECORDED_DATA_H_
#define _TEST_HTTP_RECORDED_DATA_H_

#include "mpin_sdk.h"
#include "CvMutex.h"

class HttpRecordedData
{
public:
    typedef MPinSDK::String String;
    typedef MPinSDK::StringMap StringMap;
    typedef MPinSDK::IHttpRequest IHttpRequest;

    class Request
    {
    public:
        Request();
        Request(IHttpRequest::Method _method, const String& _url, const String& _data, const String& _context);
        Request(const json::Object& object);
        json::Object ToJsonObject() const;
        String GetKey() const;

        String method;
        String url;
        String data;
        String context;
    };

    class Response
    {
    public:
        Response();
        Response(bool _success, const String& _error, int _httpStatus, const StringMap& _headers, const String& _data);
        Response(const json::Object& object);
        json::Object ToJsonObject() const;
        String ToString() const;

        bool success;
        String error;
        int httpStatus;
        StringMap headers;
        String data;
    };

    HttpRecordedData();
    void Record(const Request& request, const Response& response);
    Response FindResponseFor(const Request& request) const;
    void SaveTo(std::ostream& outputStream) const;
    void SaveTo(const String& fileName) const;
    bool LoadFrom(std::istream& inputStream);
    bool LoadFrom(const String& fileName);

private:
    bool AddMapping(const Request& request, const Response& response);

    json::Array m_data;
    typedef std::map<String, Response> ResponseMap;
    ResponseMap m_responseMap;
    CvShared::CvMutex m_addRecordMutex;
};

#endif // _TEST_HTTP_RECORDED_DATA_H_
