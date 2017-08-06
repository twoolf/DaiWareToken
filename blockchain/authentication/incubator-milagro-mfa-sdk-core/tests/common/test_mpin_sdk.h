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

/*
 * M-Pin SDK test interface
 */

#ifndef _TEST_MPIN_SDK_H_
#define _TEST_MPIN_SDK_H_

#include "mpin_sdk.h"

class TestContext;

class TestMPinSDK : public MPinSDK
{
public:
    TestMPinSDK(TestContext& testContext);

    Status Init(const StringMap& config);

    Status StartRegistration(INOUT UserPtr user, const String& activateCode = "", const String& userData = "");
    Status RestartRegistration(INOUT UserPtr user, const String& userData = "");
    Status ConfirmRegistration(INOUT UserPtr user, const String& pushMessageIdentifier = "");
    Status FinishRegistration(INOUT UserPtr user, const String& pin);

    Status StartAuthentication(INOUT UserPtr user, const String& accessCode = "");
    Status FinishAuthentication(INOUT UserPtr user, const String& pin);
    Status FinishAuthentication(INOUT UserPtr user, const String& pin, OUT String& authResultData);
    Status FinishAuthenticationOTP(INOUT UserPtr user, const String& pin, OUT OTP& otp);
    Status FinishAuthenticationAN(INOUT UserPtr user, const String& pin, const String& accessNumber);

    Status GetSessionDetails(const String& accessCode, OUT SessionDetails& sessionDetails);
    bool Logout(IN UserPtr user);

private:
    TestContext& m_testContext;
};

#endif // _TEST_MPIN_SDK_H_
