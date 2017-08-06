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
 * HTTPConnector.cpp
 *
 *  Created on: Oct 20, 2014
 *      Author: georgi.georgiev
 */

#include "HTTPConnector.h"

namespace net {

HTTPConnector::HTTPConnector(JNIEnv* env) throw (IllegalArgumentException) :
		m_pjenv(env), m_response(""), m_errorMessage(""), m_statusCode(0) {
	if (m_pjenv == NULL)
		throw IllegalArgumentException(
				"NULL pointer JNIEnv is passed to the HTTPConnector constructor");
	m_pjhttpRequestCls = reinterpret_cast<jclass>(m_pjenv->NewGlobalRef(
			m_pjenv->FindClass("com/miracl/mpinsdk/net/HTTPConnector")));
	m_pjhashtableCls = reinterpret_cast<jclass>(m_pjenv->NewGlobalRef(
			m_pjenv->FindClass("java/util/Hashtable")));
	const jmethodID midInit = m_pjenv->GetMethodID(m_pjhttpRequestCls, "<init>",
			"()V");
	m_pjhttpRequest = reinterpret_cast<jobject>(m_pjenv->NewGlobalRef(
			m_pjenv->NewObject(m_pjhttpRequestCls, midInit)));
}

jobject HTTPConnector::createJavaMap(const StringMap& map) {
	const jmethodID midInit = m_pjenv->GetMethodID(m_pjhashtableCls, "<init>",
			"(I)V");
	const jmethodID midPUT = m_pjenv->GetMethodID(m_pjhashtableCls, "put",
			"(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

	jobject jhashtable = m_pjenv->NewObject(m_pjhashtableCls, midInit,
			map.size());

	for (StringMap::const_iterator it = map.begin(); it != map.end(); ++it) {
		jstring key = m_pjenv->NewStringUTF(it->first.c_str());
		jstring value = m_pjenv->NewStringUTF(it->second.c_str());
		m_pjenv->CallObjectMethod(jhashtable, midPUT, key, value);
		m_pjenv->DeleteLocalRef(key);
		m_pjenv->DeleteLocalRef(value);
	}

	return jhashtable;
}

void HTTPConnector::convertJHashtable2StringMap(jobject jhashtable, StringMap & a_map) throw (IllegalArgumentException) {
	if (jhashtable == NULL)
		throw IllegalArgumentException(
				"NULL hashtable parrameter is passed to HTTPConnector::convertJHashtable2StringMap");

	const jclass jhashtableCls = m_pjenv->FindClass("java/util/Hashtable");
	const jclass jenumerationCls = m_pjenv->FindClass("java/util/Enumeration");

	if (!m_pjenv->IsInstanceOf(jhashtable, jhashtableCls))
		throw IllegalArgumentException(
				"input parameter jhashtable is not instance of Hashtable <String , String> class. :: HTTPConnector::convertJHashtable2StringMap");

	const jmethodID size = m_pjenv->GetMethodID(jhashtableCls, "size", "()I");
	const jmethodID keys = m_pjenv->GetMethodID(jhashtableCls, "keys",
			"()Ljava/util/Enumeration;");
	const jmethodID get = m_pjenv->GetMethodID(jhashtableCls, "get",
			"(Ljava/lang/Object;)Ljava/lang/Object;");
	const jmethodID hasMoreElements = m_pjenv->GetMethodID(jenumerationCls,
			"hasMoreElements", "()Z");
	const jmethodID nextElement = m_pjenv->GetMethodID(jenumerationCls,
			"nextElement", "()Ljava/lang/Object;");

	jint hashtablesize = m_pjenv->CallIntMethod(jhashtable, size);
	if (hashtablesize <= 0)
		return;

	jobject jenumeration = m_pjenv->CallObjectMethod(jhashtable, keys);
	if (jenumeration == NULL)
		throw IllegalArgumentException(
				"HTTPConnector::convertJHashtable2StringMap :: An error has occured while getting reference to Hashtable Enumeration Interface!");
	;

	while (m_pjenv->CallBooleanMethod(jenumeration, hasMoreElements) == JNI_TRUE) {
		jstring key = (jstring) m_pjenv->CallObjectMethod(jenumeration,
				nextElement);
		jstring value = (jstring) m_pjenv->CallObjectMethod(jhashtable, get,
				key);

		const char * keyChars = m_pjenv->GetStringUTFChars(key, NULL);
		const char * valueChars = (char *) m_pjenv->GetStringUTFChars(value,
				NULL);

		a_map[keyChars] = valueChars;

		m_pjenv->ReleaseStringUTFChars(key, keyChars);
		m_pjenv->ReleaseStringUTFChars(value, valueChars);
	}
}

void HTTPConnector::SetHeaders(const StringMap& headers) {
	const jmethodID midSetHeaders = m_pjenv->GetMethodID(m_pjhttpRequestCls,
			"SetHeaders", "(Ljava/util/Hashtable;)V");
	jobject jhashtable = createJavaMap(headers);
	m_pjenv->CallVoidMethod(m_pjhttpRequest, midSetHeaders, jhashtable);
}

void HTTPConnector::SetQueryParams(const StringMap& queryParams) {
	const jmethodID midSetQueryParams = m_pjenv->GetMethodID(m_pjhttpRequestCls,
			"SetQueryParams", "(Ljava/util/Hashtable;)V");
	jobject jhashtable = createJavaMap(queryParams);
	m_pjenv->CallVoidMethod(m_pjhttpRequest, midSetQueryParams, jhashtable);
}

void HTTPConnector::SetContent(const String& data) {
	const jmethodID midSetContent = m_pjenv->GetMethodID(m_pjhttpRequestCls,
			"SetContent", "(Ljava/lang/String;)V");
	jstring message = m_pjenv->NewStringUTF(data.c_str());
	m_pjenv->CallVoidMethod(m_pjhttpRequest, midSetContent, message);
}

void HTTPConnector::SetTimeout(int seconds) {
	const jmethodID midSetContent = m_pjenv->GetMethodID(m_pjhttpRequestCls,
			"SetContent", "(Ljava/lang/String;)V");
	m_pjenv->CallVoidMethod(m_pjhttpRequest, midSetContent, seconds);
}

bool HTTPConnector::Execute(Method method, const String& url) {
	const jmethodID midExecute = m_pjenv->GetMethodID(m_pjhttpRequestCls,
			"Execute", "(ILjava/lang/String;)Z");
	const jfieldID fidStatusCode = m_pjenv->GetFieldID(m_pjhttpRequestCls,
			"statusCode", "I");
	const jfieldID fidResponseHeaders = m_pjenv->GetFieldID(m_pjhttpRequestCls,
			"responseHeaders", "Ljava/util/Hashtable;");
	const jfieldID fidResponseData = m_pjenv->GetFieldID(m_pjhttpRequestCls,
			"responseData", "Ljava/lang/String;");
	const jfieldID fidErrorMessage = m_pjenv->GetFieldID(m_pjhttpRequestCls,
			"errorMessage", "Ljava/lang/String;");
	m_errorMessage = "";

	jboolean rc = m_pjenv->CallBooleanMethod(m_pjhttpRequest, midExecute,
			(jint) method, m_pjenv->NewStringUTF(url.c_str()));

	m_statusCode = m_pjenv->GetIntField(m_pjhttpRequest, fidStatusCode);

	jobject jresponseHeaders = m_pjenv->GetObjectField(m_pjhttpRequest,
			fidResponseHeaders);
	try {
		convertJHashtable2StringMap(jresponseHeaders, m_responseHeaders);
	} catch (IllegalArgumentException &e) {
		m_errorMessage = e.what();
	}

	jstring jresponseData = reinterpret_cast<jstring>((m_pjenv->GetObjectField(
			m_pjhttpRequest, fidResponseData)));
	if (jresponseData != NULL) {
		const char *cRespStr = m_pjenv->GetStringUTFChars(jresponseData, NULL);
		m_response = cRespStr;
		m_pjenv->ReleaseStringUTFChars(jresponseData, cRespStr);
	}

	jstring jerrorMessage = reinterpret_cast<jstring>((m_pjenv->GetObjectField(
			m_pjhttpRequest, fidErrorMessage)));
	if (jerrorMessage != NULL) {
		const char *cErrorStr = m_pjenv->GetStringUTFChars(jerrorMessage, NULL);
		m_errorMessage += cErrorStr;
		m_pjenv->ReleaseStringUTFChars(jerrorMessage, cErrorStr);
	}

	return (rc == JNI_TRUE);
}

const String& HTTPConnector::GetExecuteErrorMessage() const {
	return m_errorMessage;
}

int HTTPConnector::GetHttpStatusCode() const {
	return m_statusCode;
}

const StringMap& HTTPConnector::GetResponseHeaders() const {
	return m_responseHeaders;
}

const String& HTTPConnector::GetResponseData() const {
	return m_response;
}

HTTPConnector::~HTTPConnector() {
	if (m_pjenv == NULL)
		return;
	RELEASE_JNIREF(m_pjenv, m_pjhttpRequestCls)
	RELEASE_JNIREF(m_pjenv, m_pjhashtableCls)
	RELEASE_JNIREF(m_pjenv, m_pjhttpRequest)
}

}
