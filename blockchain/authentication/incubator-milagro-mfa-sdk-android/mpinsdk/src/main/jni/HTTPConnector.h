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
 * HTTPConnector.h
 *
 *  Created on: Oct 20, 2014
 *      Author: georgi.georgiev
 */

#ifndef HTTPCONNECTOR_H_
#define HTTPCONNECTOR_H_

#include "JNICommon.h"
#include "Exceptions.h"


using namespace std;

namespace net {

	typedef MPinSDK::String String;
	typedef MPinSDK::StringMap StringMap;
	typedef MPinSDK::IHttpRequest IHttpRequest;

	class HTTPConnector : public IHttpRequest {
		public:
			HTTPConnector(JNIEnv*) throw(IllegalArgumentException);

			virtual void SetHeaders(const StringMap& headers);
			virtual void SetQueryParams(const StringMap& queryParams);
			virtual void SetContent(const String& data);
			virtual void SetTimeout(int seconds);
			virtual bool Execute(Method method, const String& url);
			virtual const String& GetExecuteErrorMessage() const;
			virtual int GetHttpStatusCode() const;
			virtual const StringMap& GetResponseHeaders() const;
			virtual const String& GetResponseData() const;

			virtual  ~HTTPConnector();

		private:
			JNIEnv* m_pjenv;

			// JNI CLASES ::
			jclass m_pjhttpRequestCls;
			jclass m_pjhashtableCls;

			// JNI OBJECTS ::
			jobject m_pjhttpRequest;

			// C++ Member variables
			String m_errorMessage;
			StringMap  m_responseHeaders;
			String m_response;
			int m_statusCode;

			HTTPConnector();
			HTTPConnector(const HTTPConnector &);
			jobject createJavaMap(const StringMap& map);
			void convertJHashtable2StringMap(jobject jhashtable, StringMap & a_map) throw(IllegalArgumentException);
	};
}


#endif /* HTTPCONNECTOR_H_ */
