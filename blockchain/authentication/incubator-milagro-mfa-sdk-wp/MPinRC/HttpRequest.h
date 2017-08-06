// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <string>
#include <collection.h>
#include "mpin_sdk.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Web::Http;

namespace MPinRC
{
	public interface class IHttpRequest
	{
		virtual void SetHeaders(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ headers) = 0;
		virtual void SetQueryParams(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ queryParams) = 0;
		virtual void SetContent(String^ data) = 0;
		virtual void SetTimeout(int seconds) = 0;		
	
		virtual bool Execute(Windows::Web::Http::HttpMethod^ method, String^ url) = 0;
		
		virtual String^ GetExecuteErrorMessage() = 0;
		virtual int GetHttpStatusCode() = 0;
		virtual Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ GetResponseHeaders() = 0;
		virtual String^ GetResponseData() = 0;
	};
	
	class HttpProxy : public MPinSDK::IHttpRequest
	{
	private:
		MPinRC::IHttpRequest^ managedRequest;
		MPinSDK::String responseData;
		MPinSDK::String errorMessage;
		MPinSDK::StringMap responseHeaders;
		int httpResponseCode;

		Windows::Web::Http::HttpMethod^ GetHttpMethod(MPinSDK::IHttpRequest::Method nativeMethod);
		
	public:
		HttpProxy(MPinRC::IHttpRequest^ request) { this->managedRequest = request; };
		~HttpProxy() {};

		virtual void SetHeaders(const MPinSDK::StringMap& headers);
		virtual void SetQueryParams(const MPinSDK::StringMap& queryParams);
		virtual void SetContent(const MPinSDK::String& data);
		virtual void SetTimeout(int seconds);
		virtual bool Execute(Method method, const MPinSDK::String& url);
		virtual const MPinSDK::String& GetExecuteErrorMessage() const;
		virtual int GetHttpStatusCode() const;
		virtual const MPinSDK::StringMap& GetResponseHeaders() const;
		virtual const MPinSDK::String& GetResponseData() const;
	};	
}