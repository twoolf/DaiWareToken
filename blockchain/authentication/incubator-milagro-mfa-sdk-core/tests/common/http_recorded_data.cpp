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

#include "http_recorded_data.h"
#include <fstream>

using std::fstream;
using std::cerr;
using std::endl;

typedef MPinSDK::String String;
typedef MPinSDK::IHttpRequest IHttpRequest;
typedef HttpRecordedData::Request Request;
typedef HttpRecordedData::Response Response;

namespace
{
    String MethodToString(IHttpRequest::Method method)
    {
        switch (method)
        {
        case IHttpRequest::GET:
            return "GET";
        case IHttpRequest::POST:
            return "POST";
        case IHttpRequest::PUT:
            return "PUT";
        case IHttpRequest::DELETE:
            return "DELETE";
        case IHttpRequest::OPTIONS:
        case IHttpRequest::PATCH:
        default:
            assert(false);
            return "Unknown";
        }
    }

    bool operator==(const Response& left, const Response& right)
    {
        if (left.success != right.success)
        {
            return false;
        }

        if (!left.success)
        {
            return true;
        }

        if (left.httpStatus != right.httpStatus)
        {
            return false;
        }

        if (left.httpStatus != 200)
        {
            return true;
        }

        return left.data == right.data;
    }
}

HttpRecordedData::Request::Request()
{
}

HttpRecordedData::Request::Request(IHttpRequest::Method _method, const String & _url, const String & _data, const String& _context) :
    method(MethodToString(_method)), url(_url), data(_data), context(_context)
{
}

HttpRecordedData::Request::Request(const json::Object & object) :
    method(((const json::String&) object["method"]).Value()),
    url(((const json::String&) object["url"]).Value()),
    data(((const json::String&) object["data"]).Value()),
    context(((const json::String&) object["context"]).Value())
{
}

json::Object HttpRecordedData::Request::ToJsonObject() const
{
    json::Object object;
    object["method"] = json::String(method);
    object["url"] = json::String(url);
    object["data"] = json::String(data);
    object["context"] = json::String(context);
    return object;
}

String HttpRecordedData::Request::GetKey() const
{
    return method + " " + url + " " + context;
}

HttpRecordedData::Response::Response() : success(false), httpStatus(0)
{
}

HttpRecordedData::Response::Response(bool _success, const String & _error, int _httpStatus, const StringMap & _headers, const String & _data) :
    success(_success), error(_error), httpStatus(_httpStatus), headers(_headers), data(_data)
{
}

HttpRecordedData::Response::Response(const json::Object & object) :
    success(((const json::Boolean&) object["success"]).Value()),
    error(((const json::String&) object["error"]).Value()),
    httpStatus(static_cast<int>(((const json::Number&) object["httpStatus"]).Value())),
    headers(object["headers"]),
    data(((const json::String&) object["data"]).Value())
{
}

json::Object HttpRecordedData::Response::ToJsonObject() const
{
    json::Object object;
    object["success"] = json::Boolean(success);
    object["error"] = json::String(error);
    object["httpStatus"] = json::Number(httpStatus);
    object["headers"] = headers.ToJsonObject();
    object["data"] = json::String(data);
    return object;
}

String HttpRecordedData::Response::ToString() const
{
    return success ? (String().Format("%d ", httpStatus) + data) : ("ERROR: " + error);
}

HttpRecordedData::HttpRecordedData()
{
    m_addRecordMutex.Create();
}

void HttpRecordedData::Record(const Request & request, const Response & response)
{
    CvShared::CvMutexLock lock(m_addRecordMutex);
    if (AddMapping(request, response))
    {
        json::Object record;
        record["request"] = request.ToJsonObject();
        record["response"] = response.ToJsonObject();
        m_data.Insert(record);
    }
}

bool HttpRecordedData::AddMapping(const Request & request, const Response & response)
{
    String key = request.GetKey();
    std::pair<ResponseMap::iterator, bool> res = m_responseMap.insert(std::make_pair(key, response));
    if(!res.second && !(res.first->second == response) && request.url.find("clientSettings") == String::npos)
    {
        cerr << "WARNING: Found ambiguous response for request" << endl;
        cerr << "  -      request: " + key << endl;
        cerr << "  - old response: " + res.first->second.ToString() << endl;
        cerr << "  - new response: " + response.ToString() << endl << endl;
    }
    return res.second;
}

Response HttpRecordedData::FindResponseFor(const Request & request) const
{
    String key = request.GetKey();
    ResponseMap::const_iterator i = m_responseMap.find(key);
    if (i == m_responseMap.end())
    {
        Response emptyResponse;
        emptyResponse.error = String().Format("Cannot find HttpRecordedData for request '%s'", key.c_str());
        cerr << "ERROR: " << emptyResponse.error << endl << endl;
        return emptyResponse;
    }

    return i->second;
}

void HttpRecordedData::SaveTo(std::ostream & outputStream) const
{
    json::Writer::Write(m_data, outputStream);
}

void HttpRecordedData::SaveTo(const String & fileName) const
{
    fstream file(fileName.c_str(), fstream::out);
    file.clear();
    file.seekp(fstream::beg);
    SaveTo(file);
    file.close();
}

bool HttpRecordedData::LoadFrom(std::istream & inputStream)
{
    try
    {
        m_data.Clear();
        json::Reader::Read(m_data, inputStream);

        m_responseMap.clear();
        for (json::Array::const_iterator i = m_data.Begin(); i != m_data.End(); ++i)
        {
            const json::Object& record = *i;
            AddMapping(Request(record["request"]), Response(record["response"]));
        }
    }
    catch (json::Exception& e)
    {
        cerr << "Failed to load HTTP recorded data: " << e.what() << endl;
        return false;
    }

    return true;
}

bool HttpRecordedData::LoadFrom(const String & fileName)
{
    fstream file(fileName.c_str(), fstream::in);
    bool res = LoadFrom(file);
    file.close();
    return res;
}
