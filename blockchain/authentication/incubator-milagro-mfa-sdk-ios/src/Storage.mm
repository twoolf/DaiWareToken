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

#include "Storage.h"
#include "def.h"

#define SECURE_STORE    "/secure_store.txt"
#define STORE           "/store.txt"

namespace store {
    
static String inMemoryStore = "";
static String secureInMemoryStore = "";
    
Storage::Storage(bool isMpinType) : m_isMpinType(isMpinType), store((isMpinType)? (secureInMemoryStore):(inMemoryStore)) {
    if(m_isMpinType)  readStringFromFile(SECURE_STORE, secureInMemoryStore);
    else readStringFromFile(STORE, inMemoryStore);
}
    
void Storage::readStringFromFile(const String & aFileName, OUT String & aData) {
    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = [NSString stringWithUTF8String:aFileName.c_str()];
    NSString *fileAtPath = [filePath stringByAppendingString:fileName];
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileAtPath]) return;
    NSError * error = nil;
    NSString * readData = [NSString stringWithContentsOfFile:fileAtPath encoding:NSUTF8StringEncoding error:&error];
    if(error != nil)    m_errorMessage = [error.localizedDescription UTF8String];
    else  aData = [readData UTF8String];
}

void Storage::writeStringToFile(const String & aFileName, const IN String & aData) {
    NSString *filePath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *fileName = [NSString stringWithUTF8String:aFileName.c_str()];
    NSString *fileAtPath = [filePath stringByAppendingString:[NSString stringWithFormat:@"/%@",fileName]];
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileAtPath]) {
        [[NSFileManager defaultManager] createFileAtPath:fileAtPath contents:nil attributes:nil];
    }
    [[[NSString stringWithUTF8String:aData.c_str()] dataUsingEncoding:NSUTF8StringEncoding] writeToFile:fileAtPath atomically:NO];
}

bool Storage::SetData(const String& data) {
    store = data;
    Save();
    return TRUE;
}

bool Storage::GetData(String &data) {
    if(!m_errorMessage.empty()) return FALSE;
    data = store;
    return TRUE;
}

const String& Storage::GetErrorMessage() const { return m_errorMessage; }

    void Storage::Save() {
        if(m_isMpinType)  writeStringToFile(SECURE_STORE, secureInMemoryStore);
        else writeStringToFile(STORE, inMemoryStore);
    }
    
Storage::~Storage() {
    Save();
    }
}