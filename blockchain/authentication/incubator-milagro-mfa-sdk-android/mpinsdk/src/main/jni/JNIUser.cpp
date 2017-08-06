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

#include "JNIUser.h"
#include "JNICommon.h"


static void nDestruct(JNIEnv* env, jobject jobj, jlong jptr)
{
	delete (MPinSDK::UserPtr*) jptr;
}

static jstring nGetId(JNIEnv* env, jobject jobj, jlong jptr)
{
	return env->NewStringUTF( (*((const MPinSDK::UserPtr*)jptr))->GetId().c_str());
}

static jint nGetState(JNIEnv* env, jobject jobj, jlong jptr)
{
	return (*((const MPinSDK::UserPtr*)jptr))->GetState();
}

static jstring nGetBackend(JNIEnv* env, jobject jobj, jlong jptr)
{
    return env->NewStringUTF( (*((const MPinSDK::UserPtr*)jptr))->GetBackend().c_str());
}

static JNINativeMethod g_methodsUser[] =
{
	NATIVE_METHOD(nDestruct, "(J)V"),
	NATIVE_METHOD(nGetId, "(J)Ljava/lang/String;"),
	NATIVE_METHOD(nGetState, "(J)I"),
	NATIVE_METHOD(nGetBackend, "(J)Ljava/lang/String;")
};

void RegisterUserJNI(JNIEnv* env)
{
	RegisterNativeMethods(env, "com/miracl/mpinsdk/model/User", g_methodsUser, ARR_LEN(g_methodsUser));
}
