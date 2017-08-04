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

#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "JNICommon.h"
#include "mpin_sdk.h"

namespace sdk
{
typedef MPinSDK::IContext IContext;
typedef MPinSDK::IHttpRequest IHttpRequest;
typedef MPinSDK::IStorage IStorage;

class Context: public IContext
{
public:
	static Context* Instance(jobject jcontext);
	virtual IHttpRequest * CreateHttpRequest() const;
	virtual void ReleaseHttpRequest(IHttpRequest *request) const;
	virtual IStorage * GetStorage(IStorage::Type type) const;
	virtual MPinSDK::CryptoType GetMPinCryptoType() const;
	virtual ~Context();

private:
	Context(jobject jcontext);
	Context(Context const&){};
	Context& operator=(Context const&){ return *this;};
	static Context* m_pInstance;
	IStorage * m_pIstorageSecure;
	IStorage * m_pIstorageNonSecure;
};

}

#endif // _CONTEXT_H_
