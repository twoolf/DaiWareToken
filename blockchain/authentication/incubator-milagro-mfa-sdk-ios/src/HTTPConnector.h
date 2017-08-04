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

#ifndef HTTPCONNECTOR_H_
#define HTTPCONNECTOR_H_

#include "def.h"
#include "Exceptions.h"


using namespace std;

namespace net {

	class HTTPConnector : public IHttpRequest {
		public:
            HTTPConnector(){}
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
        
            StringMap  m_queryParams;
            StringMap  m_requestHeaders;
            String m_bodyData;
            StringMap  m_responseHeaders;
            String m_errorMessage;
			String m_response;
			int m_statusCode;
            int timeout;

			HTTPConnector(const HTTPConnector &);
	};
}

#endif /* HTTPCONNECTOR_H_ */
