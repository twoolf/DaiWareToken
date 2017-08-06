/***************************************************************
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 ***************************************************************/

#ifndef _JNI_COMMON_H_
#define _JNI_COMMON_H_

#include <jni.h>
#include <android/log.h>
#include "mpin_sdk.h"

/*
 * Helper macros
 */
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

/*
 * Macro to get the elements count in an array. Don't use it on zero-sized arrays
 */
#define ARR_LEN(x) ((int)(sizeof(x) / sizeof((x)[0])))

/*
 * Helper macro to initialize arrays with JNI methods for registration. Naming convention is ClassName_MethodName.
 * Beware for overloaded methods (with same name and different signature) - make sure they have unique names in C++ land
 */
#define NATIVE_METHOD(methodName, signature) { #methodName, signature, (void*) methodName }

#define  LOG_TAG    "CV"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))


/*
 * Helper functions
 */

JNIEnv* JNI_getJENV();

/*
 * Helper function to register native methods
 */
void RegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* methods, int numMethods);

void ReadJavaMap(JNIEnv* env, jobject jmap, MPinSDK::StringMap& map);

jobject MakeJavaStatus(JNIEnv* env, const MPinSDK::Status& status);

std::string JavaToStdString(JNIEnv* env, jstring jstr);

MPinSDK::UserPtr JavaToMPinUser(JNIEnv* env, jobject juser);

#endif // _JNI_COMMON_H_
