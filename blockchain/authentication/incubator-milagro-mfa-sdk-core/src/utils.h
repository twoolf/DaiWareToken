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
 * Utility classes and functions
 */

#ifndef _MPIN_SDK_UTILS_H_
#define _MPIN_SDK_UTILS_H_

#include <map>
#include "json/reader.h"
#include "json/elements.h"
#include "json/writer.h"
#include "CvString.h"


namespace util
{

class String : public CvString
{
public:
    String() : CvString() {}
    String(const std::string& str) : CvString(str) {}
    String(const char *str) : CvString(str) {}
    String(const String& str) : CvString(str) {}
    String(const std::string& str, size_t pos, size_t size = npos) : CvString(str, pos, size) {}
    String(const char *str, size_t size) : CvString(str, size) {}
    String(size_t size, char c) : CvString(size, c) {}
    ~String();
    String& Trim(const std::string& chars = " \t\f\v\n\r");
    void Overwrite(char c = ' ');
    int GetHash() const;
};

void OverwriteString(std::string& str, char c = ' ');


class JsonObject : public json::Object
{
public:
    JsonObject();
    JsonObject(const json::Object& other);
    JsonObject& operator = (const json::Object& other);
    ~JsonObject();
    std::string ToString() const;
    bool Parse(const char *str);
    const char * GetStringParam(const char *name, const char *defaultValue = "") const;
    int GetIntParam(const char *name, int defaultValue = 0) const;
    int64_t GetInt64Param(const char *name, int64_t defaultValue = 0) const;
    bool GetBoolParam(const char *name, bool defaultValue = false) const;
    std::string GetParseError() const;
    
private:
    void Copy(const json::Object& other);

private:
    String m_parseError;
};

void OverwriteJsonValues(json::Object& object);
void OverwriteJsonValues(json::Array& array);
void OverwriteJsonValues(json::UnknownElement& element);

class StringMap : public std::map<String, String>
{
public:
    StringMap() {}
    StringMap(const json::Object& object);
    json::Object ToJsonObject() const;
    bool Put(const String& key, const String& value);
    void PutAll(const StringMap& other);
    const char * Get(const String& key) const;
};


std::string HexEncode(const char *str, size_t len);
std::string HexEncode(const std::string& str);
std::string HexDecode(const std::string& str);

}


#endif // _MPIN_SDK_UTILS_H_
