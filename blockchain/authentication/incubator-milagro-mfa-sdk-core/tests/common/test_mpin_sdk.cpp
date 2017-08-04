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

#include "test_mpin_sdk.h"
#include "test_context.h"

typedef MPinSDK::Status Status;

TestMPinSDK::TestMPinSDK(TestContext & testContext) : m_testContext(testContext)
{
}

Status TestMPinSDK::Init(const StringMap & config)
{
    return MPinSDK::Init(config, &m_testContext);
}

Status TestMPinSDK::StartRegistration(INOUT UserPtr user, const String & activateCode, const String & userData)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + activateCode + "-" + userData);
    Status s = MPinSDK::StartRegistration(user, activateCode, userData);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::RestartRegistration(INOUT UserPtr user, const String & userData)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + userData);
    Status s = MPinSDK::RestartRegistration(user, userData);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::ConfirmRegistration(INOUT UserPtr user, const String & pushMessageIdentifier)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + pushMessageIdentifier);
    Status s = MPinSDK::ConfirmRegistration(user, pushMessageIdentifier);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::FinishRegistration(INOUT UserPtr user, const String & pin)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + pin);
    Status s = MPinSDK::FinishRegistration(user, pin);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::StartAuthentication(INOUT UserPtr user, const String & accessCode)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + accessCode);
    Status s = MPinSDK::StartAuthentication(user, accessCode);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::FinishAuthentication(INOUT UserPtr user, const String & pin)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + pin);
    Status s = MPinSDK::FinishAuthentication(user, pin);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::FinishAuthentication(INOUT UserPtr user, const String & pin, OUT String & authResultData)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + pin + "-ARD");
    Status s = MPinSDK::FinishAuthentication(user, pin, authResultData);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::FinishAuthenticationOTP(INOUT UserPtr user, const String & pin, OUT OTP & otp)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + pin + "-OTP");
    Status s = MPinSDK::FinishAuthenticationOTP(user, pin, otp);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::FinishAuthenticationAN(INOUT UserPtr user, const String & pin, const String & accessNumber)
{
    m_testContext.SetRequestContextData(user->GetId() + "-" + pin + "-" + accessNumber);
    Status s = MPinSDK::FinishAuthenticationAN(user, pin, accessNumber);
    m_testContext.SetRequestContextData("");
    return s;
}

Status TestMPinSDK::GetSessionDetails(const String & accessCode, OUT SessionDetails & sessionDetails)
{
    m_testContext.SetRequestContextData(accessCode);
    Status s = MPinSDK::GetSessionDetails(accessCode, sessionDetails);
    m_testContext.SetRequestContextData("");
    return s;
}

bool TestMPinSDK::Logout(IN UserPtr user)
{
    m_testContext.SetRequestContextData(user->GetId());
    bool res = MPinSDK::Logout(user);
    m_testContext.SetRequestContextData("");
    return res;
}
