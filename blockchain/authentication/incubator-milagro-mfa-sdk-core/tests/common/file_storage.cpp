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

#include "file_storage.h"

#include <iostream>
#include <fstream>

typedef MPinSDK::String String;
using std::fstream;
using std::stringbuf;

FileStorage::FileStorage(const String& fileName) : m_fileName(fileName)
{
}

bool FileStorage::SetData(const String& data)
{
    fstream file(m_fileName.c_str(), fstream::out);
    file.clear();
    file.seekp(fstream::beg);
    file << data;
    file.close();

    return true;
}

bool FileStorage::GetData(String &data)
{
    fstream file(m_fileName.c_str(), fstream::in);
    stringbuf buf;
    file >> &buf;
    data = buf.str();
    file.close();

    return true;
}

const String& FileStorage::GetErrorMessage() const
{
    return m_errorMessage;
}
