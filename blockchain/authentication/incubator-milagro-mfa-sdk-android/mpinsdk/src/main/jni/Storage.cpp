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
 * Storage.cpp
 *
 *  Created on: Oct 28, 2014
 *      Author: georgi
 */

#include "Storage.h"

namespace store {

Storage::Storage(jobject context, bool isMpinType) {
	JNIEnv* p_jenv = JNI_getJENV();
	m_pjstorageCls = (jclass) p_jenv->NewGlobalRef(p_jenv->FindClass("com/miracl/mpinsdk/storage/Storage"));
	const jmethodID midInit = p_jenv->GetMethodID(m_pjstorageCls, "<init>", "(Landroid/content/Context;Z)V");
	m_pjstorage = p_jenv->NewGlobalRef(p_jenv->NewObject(m_pjstorageCls, midInit, context, isMpinType));

}

void Storage::setErrorMessage() {
	JNIEnv* p_jenv = JNI_getJENV();
	jclass cls = p_jenv->FindClass("com/miracl/mpinsdk/storage/Storage");
	const jmethodID midGetErrorMessage = p_jenv->GetMethodID(cls, "GetErrorMessage", "()Ljava/lang/String;");
	jstring jerror = static_cast<jstring>(p_jenv->CallObjectMethod(m_pjstorage, midGetErrorMessage));
	const char * c_error = "";
	if (jerror) c_error = (char *)p_jenv->GetStringUTFChars(jerror, NULL);
	m_errorMessage = c_error;
}

bool Storage::SetData(const String& data) {
	JNIEnv* p_jenv = JNI_getJENV();
	const jmethodID midSetData = p_jenv->GetMethodID(m_pjstorageCls, "SetData", "(Ljava/lang/String;)Z");
	jstring jdata = p_jenv->NewStringUTF(data.c_str());
	bool bresult = p_jenv->CallBooleanMethod(m_pjstorage, midSetData, jdata);
	if(bresult == false)  {
		setErrorMessage();
	}
	return bresult;
}

bool Storage::GetData(String &data) {
	JNIEnv* p_jenv = JNI_getJENV();
	const jmethodID midGetData = p_jenv->GetMethodID(m_pjstorageCls, "GetData", "()Ljava/lang/String;");
	jstring jreadData = (jstring)p_jenv->CallObjectMethod(m_pjstorage, midGetData);
	if(jreadData == NULL) {
		setErrorMessage();
		return false;
	}
	const char * c_data = (char *)p_jenv->GetStringUTFChars(jreadData, NULL);
	data.append(c_data);
	p_jenv->ReleaseStringUTFChars(jreadData, c_data);
	return true;
}

const String& Storage::GetErrorMessage() const { return m_errorMessage; }

Storage::~Storage() {
	JNIEnv* p_jenv = JNI_getJENV();
	if(p_jenv == NULL) {
		return;
	}
	RELEASE_JNIREF(p_jenv, m_pjstorageCls)
	RELEASE_JNIREF(p_jenv, m_pjstorage)
}

}
