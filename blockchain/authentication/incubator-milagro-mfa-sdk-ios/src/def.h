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

#ifndef DEF_H_
#define DEF_H_

#include "mpin_sdk.h"

#define RELEASE(pointer)  \
    if ((pointer) != NULL ) { \
        delete (pointer);    \
        (pointer) = NULL;    \
    } \

#define RELEASE_JNIREF(env , ref)  \
    if ((ref) != NULL ) { \
        (env)->DeleteGlobalRef((ref)); \
        (ref) = NULL;    \
    } \

/// input output parameter
#define IN
#define OUT

typedef MPinSDK::String String;
typedef MPinSDK::IContext IContext;
typedef MPinSDK::IHttpRequest IHttpRequest;
typedef MPinSDK::IStorage IStorage;
typedef MPinSDK::StringMap StringMap;
typedef IHttpRequest::Method Method;
typedef MPinSDK::CryptoType CryptoType;

static const String kEmptyString = "";
static const String kNegativeString = "-1";

/*
 * Macro to get the elements count in an array. Don't use it on zero-sized arrays
 */
#define ARR_LEN(x) ((int)(sizeof(x) / sizeof((x)[0])))

#endif /* DEF_H_ */
