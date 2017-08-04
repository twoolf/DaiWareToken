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

#include "Context.h"
#include "HTTPConnector.h"
#include "Storage.h"
#import "User.h"

namespace sdk_non_tee {
    
    typedef store::Storage Storage;
    typedef net::HTTPConnector HttpRequest;
    
    Context* Context::m_pInstance = NULL;

    Context * Context::Instance() {
        if(m_pInstance == NULL) {
            m_pInstance = new Context();
        }
        return m_pInstance;
    }
    
    Context::Context() {
        m_pIstorageSecure = new Storage(true);
        m_pIstorageNonSecure = new Storage(false);
    }
    
    IHttpRequest * Context::CreateHttpRequest() const{
        return new HttpRequest();
    }
    
    void Context::ReleaseHttpRequest(IHttpRequest *request) const {
        RELEASE(request)
    }
    
    IStorage * Context::GetStorage(IStorage::Type type) const {
        switch (type)
        {
            case MPinSDK::IStorage::SECURE:
                return m_pIstorageSecure;
            case MPinSDK::IStorage::NONSECURE:
                return m_pIstorageNonSecure;
            default:
                return NULL;
        }
    }
    
    CryptoType Context::GetMPinCryptoType() const {
        return MPinSDK::CRYPTO_NON_TEE;
    }
    
    Context::~Context() {
        RELEASE(m_pIstorageSecure)
        RELEASE(m_pIstorageNonSecure)
        RELEASE(m_pInstance)
    }
    
} /* namespace store */
