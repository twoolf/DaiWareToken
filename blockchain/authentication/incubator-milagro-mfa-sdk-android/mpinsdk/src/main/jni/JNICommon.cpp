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
/*
 * JNICommon.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: ogi
 */

#include "JNICommon.h"
#include "JNIUser.h"
#include "JNIMPinSDK.h"
#include "JNIMPinSDK.h"

static JavaVM * g_jvm;

JNIEnv* JNI_getJENV()
{
	 JNIEnv* env;
	 if(g_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
	 {
		 return NULL;
	 }
	 return env;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	g_jvm = vm;
	JNIEnv* env = JNI_getJENV();

	RegisterMPinSDKJNI(env);
	RegisterUserJNI(env);

	return JNI_VERSION_1_6;
}

void RegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* methods, int numMethods)
{
	jclass cls = env->FindClass(className);

	if(!cls)
	{
		env->FatalError("RegisterNativeMethods failed");
		return;
	}

	if(env->RegisterNatives(cls, methods, numMethods) < 0)
	{
		env->FatalError("RegisterNativeMethods failed");
		return;
	}
}

void ReadJavaMap(JNIEnv* env, jobject jmap, MPinSDK::StringMap& map)
{
	jclass clsMap = env->FindClass("java/util/Map");
	jclass clsSet = env->FindClass("java/util/Set");
	jclass clsIterator = env->FindClass("java/util/Iterator");

	jmethodID midKeySet = env->GetMethodID(clsMap, "keySet", "()Ljava/util/Set;");
	jobject jkeySet = env->CallObjectMethod(jmap, midKeySet);

	jmethodID midIterator = env->GetMethodID(clsSet, "iterator", "()Ljava/util/Iterator;");
	jobject jkeySetIter = env->CallObjectMethod(jkeySet, midIterator);

	jmethodID midHasNext = env->GetMethodID(clsIterator, "hasNext", "()Z");
	jmethodID midNext = env->GetMethodID(clsIterator, "next", "()Ljava/lang/Object;");

	jmethodID midGet = env->GetMethodID(clsMap, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");

	map.clear();

	while(env->CallBooleanMethod(jkeySetIter, midHasNext)) {
		jstring jkey = (jstring) env->CallObjectMethod(jkeySetIter, midNext);
		jstring jvalue = (jstring) env->CallObjectMethod(jmap, midGet, jkey);

		const char* cstr = env->GetStringUTFChars(jkey, NULL);
		MPinSDK::String key(cstr);
		env->ReleaseStringUTFChars(jkey, cstr);
		cstr = env->GetStringUTFChars(jvalue, NULL);
		MPinSDK::String value(cstr);
		env->ReleaseStringUTFChars(jvalue, cstr);

		map[key] = value;
	}
}

jobject MakeJavaStatus(JNIEnv* env, const MPinSDK::Status& status)
{
	jclass clsStatus = env->FindClass("com/miracl/mpinsdk/model/Status");
	jmethodID ctorStatus = env->GetMethodID(clsStatus, "<init>", "(ILjava/lang/String;)V");
	return env->NewObject(clsStatus, ctorStatus, (jint) status.GetStatusCode(), env->NewStringUTF(status.GetErrorMessage().c_str()));
}

std::string JavaToStdString(JNIEnv* env, jstring jstr)
{
	const char* cstr = env->GetStringUTFChars(jstr, NULL);
	std::string str(cstr);
	env->ReleaseStringUTFChars(jstr, cstr);
	return str;
}

MPinSDK::UserPtr JavaToMPinUser(JNIEnv* env, jobject juser)
{
	jclass clsUser = env->FindClass("com/miracl/mpinsdk/model/User");
	jfieldID fidPtr = env->GetFieldID(clsUser, "mPtr", "J");
	return *((MPinSDK::UserPtr*) env->GetLongField(juser, fidPtr));
}
