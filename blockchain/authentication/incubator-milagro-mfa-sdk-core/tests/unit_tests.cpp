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

#include "common/test_mpin_sdk.h"
#include "contexts/auto_context.h"
#include "common/access_number_thread.h"
#include "CvLogger.h"

#define BOOST_TEST_MODULE Simple testcases
#include "boost/test/included/unit_test.hpp"

typedef MPinSDK::User User;
typedef MPinSDK::UserPtr UserPtr;
typedef MPinSDK::Status Status;
typedef MPinSDK::String String;
using namespace boost::unit_test;


static char RECORDED_DATA_JSON[] = {
#include "unit_tests_recorded_data.inc"
};

class MemBuf : public std::streambuf
{
public:
    MemBuf(char *buf, size_t len)
    {
        this->setg(buf, buf, buf + len);
    }
};

static const char * GetRecordedDataFileName()
{
    int argc = framework::master_test_suite().argc;
    if (argc > 1)
    {
        char **argv = framework::master_test_suite().argv;
        return argv[1];
    }
    return "unit_tests_recorded_data.json";
}

class TestNameData : public TestContext::AutoContextData
{
public:
    virtual String Get() const
    {
        return framework::current_test_case().p_name.get();
    }
};

static TestNameData testNameData;
static AutoContext context(testNameData);
static TestMPinSDK sdk(context);
static MPinSDK::StringMap config;
static const char *backend = "http://10.10.40.62:8005";

static std::ostream& operator<<(std::ostream& ostr, const Status& s)
{
    ostr << s.GetStatusCode();
    return ostr;
}

BOOST_AUTO_TEST_CASE(testNoInit)
{
    unit_test_log.set_threshold_level(log_messages);

    BOOST_MESSAGE("Starting testNoInit...");

    CvShared::InitLogger("cvlog.txt", CvShared::enLogLevel_None);

    //context.EnterRequestRecorderMode(GetRecordedDataFileName());
    MemBuf buf(RECORDED_DATA_JSON, sizeof(RECORDED_DATA_JSON));
    std::istream recordedDataInputStream(&buf);
    context.EnterRequestPlayerMode(recordedDataInputStream);

    Status s = sdk.TestBackend("12354");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);

    s = sdk.SetBackend("12354");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);

    UserPtr user = sdk.MakeNewUser("testUser");

    s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.RestartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.FinishRegistration(user, "");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.FinishAuthentication(user, "");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    String authResultData;
    s = sdk.FinishAuthentication(user, "", authResultData);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    MPinSDK::OTP otp;
    s = sdk.FinishAuthenticationOTP(user, "", otp);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.FinishAuthenticationAN(user, "", "");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    BOOST_MESSAGE("    testNoInit finished");
}

BOOST_AUTO_TEST_CASE(testInit)
{
    BOOST_MESSAGE("Starting testInit...");

    config.Put(MPinSDK::CONFIG_BACKEND, backend);

    Status s = sdk.Init(config);

    BOOST_CHECK_EQUAL(s, Status::OK);

    BOOST_MESSAGE("    testInit finished");
}

BOOST_AUTO_TEST_CASE(testBackend)
{
    BOOST_MESSAGE("Starting testBackend...");

    Status s = sdk.TestBackend("https://m-pindemo.certivox.org");
    BOOST_CHECK_EQUAL(s, Status::OK);

    s = sdk.TestBackend("https://blabla.certivox.org");
    BOOST_CHECK_NE(s, Status::OK);

    BOOST_MESSAGE("    testBackend finished");
}

BOOST_AUTO_TEST_CASE(setBackend)
{
    BOOST_MESSAGE("Starting setBackend...");

    Status s = sdk.SetBackend("https://blabla.certivox.org");
    BOOST_CHECK_NE(s, Status::OK);

    s = sdk.SetBackend(backend);
    BOOST_CHECK_EQUAL(s, Status::OK);

    BOOST_MESSAGE("    setBackend finished");
}

BOOST_AUTO_TEST_CASE(testUsers1)
{
    BOOST_MESSAGE("Starting testUsers1...");

    UserPtr user = sdk.MakeNewUser("testUser");
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    Status s = sdk.StartRegistration(user);

    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    sdk.DeleteUser(user);
    std::vector<UserPtr> users;
    sdk.ListUsers(users);
    BOOST_CHECK(users.empty());

    BOOST_MESSAGE("    testUsers1 finished");
}

BOOST_AUTO_TEST_CASE(testUsers2)
{
    BOOST_MESSAGE("Starting testUsers2...");

    UserPtr user = sdk.MakeNewUser("testUser");
    Status s = sdk.StartRegistration(user);

    std::vector<UserPtr> users;
    sdk.ListUsers(users);
    BOOST_CHECK_EQUAL(users.size(), 1);

    std::vector<String> backends;
    sdk.ListBackends(backends);
    BOOST_CHECK_EQUAL(backends.size(), 1);

    if(!backends.empty())
    {
        users.clear();
        sdk.ListUsers(users, backends[0]);
        BOOST_CHECK_EQUAL(users.size(), 1);
    }

    users.clear();
    sdk.ListAllUsers(users);
    BOOST_CHECK_EQUAL(users.size(), 1);

    users.clear();
    sdk.ListUsers(users, backend);
    BOOST_CHECK_EQUAL(users.size(), 1);

    sdk.DeleteUser(users[0]);
    users.clear();
    sdk.ListUsers(users, backend);
    BOOST_CHECK(users.empty());

    BOOST_MESSAGE("    testUsers2 finished");
}

BOOST_AUTO_TEST_CASE(testUsers3)
{
    BOOST_MESSAGE("Starting testUsers3...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.FinishRegistration(user, "");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.RestartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    BOOST_MESSAGE("    testUsers3 finished");
}

BOOST_AUTO_TEST_CASE(testUsers4)
{
    BOOST_MESSAGE("Starting testUsers4...");

    int count = 10;
    for(int i = 0; i < count; ++i)
    {
        String id;
        id.Format("testUser%03d", i);
        UserPtr user = sdk.MakeNewUser(id);

        Status s = sdk.StartRegistration(user);
        BOOST_CHECK_EQUAL(s, Status::OK);
        BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);
    }

    std::vector<UserPtr> users;
    sdk.ListUsers(users);
    BOOST_CHECK_EQUAL(users.size(), count);

    for(std::vector<UserPtr>::iterator i = users.begin(); i != users.end(); ++i)
    {
        UserPtr user = *i;
        sdk.DeleteUser(user);
    }

    sdk.ListUsers(users);
    BOOST_CHECK(users.empty());

    BOOST_MESSAGE("    testUsers4 finished");
}

BOOST_AUTO_TEST_CASE(testUsers5)
{
    BOOST_MESSAGE("Starting testUsers5...");

    UserPtr user = sdk.MakeNewUser("testUser");
    sdk.StartRegistration(user);

    BOOST_CHECK(!sdk.CanLogout(user));
    BOOST_CHECK(!sdk.Logout(user));
    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testUsers5 finished");
}


BOOST_AUTO_TEST_CASE(testRegister1)
{
    BOOST_MESSAGE("Starting testRegister1...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testRegister1 finished");
}

BOOST_AUTO_TEST_CASE(testRegister2)
{
    BOOST_MESSAGE("Starting testRegister2...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.RestartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testRegister2 finished");
}

BOOST_AUTO_TEST_CASE(testRegister3)
{
    BOOST_MESSAGE("Starting testRegister3...");

    UserPtr user = sdk.MakeNewUser("");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::HTTP_REQUEST_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    s = sdk.FinishRegistration(user, "");
    BOOST_CHECK_EQUAL(s, Status::FLOW_ERROR);
    BOOST_CHECK_EQUAL(user->GetState(), User::INVALID);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testRegister3 finished");
}

BOOST_AUTO_TEST_CASE(testRegister4)
{
    BOOST_MESSAGE("Starting testRegister4...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "");
    BOOST_CHECK_EQUAL(s, Status::PIN_INPUT_CANCELED);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testRegister4 finished");
}

BOOST_AUTO_TEST_CASE(testAuthenticate1)
{
    BOOST_MESSAGE("Starting testAuthenticate1...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.FinishAuthentication(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    String authData;
    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthentication(user, "1234", authData);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthentication(user, "1235");
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_PIN);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    context.SetAdditionalContextData("SecondAuth");

    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthentication(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    context.SetAdditionalContextData("");

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testAuthenticate1 finished");
}

BOOST_AUTO_TEST_CASE(testAuthenticate2)
{
    BOOST_MESSAGE("Starting testAuthenticate2...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.FinishAuthentication(user, "1111");
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_PIN);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthentication(user, "1112");
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_PIN);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthentication(user, "1113");
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_PIN);
    BOOST_CHECK_EQUAL(user->GetState(), User::BLOCKED);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testAuthenticate2 finished");
}

BOOST_AUTO_TEST_CASE(testAuthenticateOTP)
{
    BOOST_MESSAGE("Starting testAuthenticateOTP...");

    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    MPinSDK::OTP otp;
    s = sdk.FinishAuthenticationOTP(user, "1234", otp);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);
    BOOST_CHECK_EQUAL(otp.status, Status::RESPONSE_PARSE_ERROR);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testAuthenticateOTP finished");
}

BOOST_AUTO_TEST_CASE(testAuthenticateAN1)
{
    BOOST_MESSAGE("Starting testAuthenticateAN1...");

    // Register user
    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    // Request access number
    MPinSDK::IHttpRequest *req = context.CreateHttpRequest();
    MPinSDK::StringMap headers;
    headers.Put("Content-Type", "application/json");
    headers.Put("Accept", "*/*");
    req->SetHeaders(headers);

    String url = String().Format("%s/rps/getAccessNumber", backend);
    bool res = req->Execute(MPinSDK::IHttpRequest::POST, url);
    BOOST_CHECK(res);
    BOOST_CHECK_EQUAL(req->GetHttpStatusCode(), 200);

    String data = req->GetResponseData();
    context.ReleaseHttpRequest(req);
    util::JsonObject json;
    res = json.Parse(data.c_str());
    BOOST_CHECK(res);

    String accessNumber = json.GetStringParam("accessNumber");
    BOOST_CHECK(accessNumber.length() > 0);

    // Start access number thread
    AccessNumberThread accessNumberThread(context);
    accessNumberThread.Start(backend, json.GetStringParam("webOTT"), sdk.GetClientParam("authenticateURL"));

    // Authenticate with access number
    s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.FinishAuthenticationAN(user, "1234", accessNumber);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    // The backend *must* support logout data
    BOOST_CHECK(sdk.CanLogout(user));
    BOOST_CHECK(sdk.Logout(user));

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testAuthenticateAN1 finished");
}

BOOST_AUTO_TEST_CASE(testAuthenticateAN2)
{
    BOOST_MESSAGE("Starting testAuthenticateAN2...");

    // Register user
    UserPtr user = sdk.MakeNewUser("testUser");

    Status s = sdk.StartRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.ConfirmRegistration(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::ACTIVATED);

    s = sdk.FinishRegistration(user, "1234");
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    // Request access number
    MPinSDK::IHttpRequest *req = context.CreateHttpRequest();
    MPinSDK::StringMap headers;
    headers.Put("Content-Type", "application/json");
    headers.Put("Accept", "*/*");
    req->SetHeaders(headers);

    String url = String().Format("%s/rps/getAccessNumber", backend);
    bool res = req->Execute(MPinSDK::IHttpRequest::POST, url);
    BOOST_CHECK(res);
    BOOST_CHECK_EQUAL(req->GetHttpStatusCode(), 200);

    String data = req->GetResponseData();
    context.ReleaseHttpRequest(req);
    util::JsonObject json;
    res = json.Parse(data.c_str());
    BOOST_CHECK(res);

    String accessNumber = json.GetStringParam("accessNumber");
    BOOST_CHECK(accessNumber.length() > 0);

    // Simulate wrong access number
    String originalAccessNumber = accessNumber;
    accessNumber[3] += (char) 1;
    if(accessNumber[3] > '9')
    {
        accessNumber[3] = '0';
    }

    // Start access number thread
    AccessNumberThread accessNumberThread(context);
    accessNumberThread.Start(backend, json.GetStringParam("webOTT"), sdk.GetClientParam("authenticateURL"));

    // Authenticate with access number
    s = sdk.StartAuthentication(user);
    BOOST_CHECK_EQUAL(s, Status::OK);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    s = sdk.FinishAuthenticationAN(user, "1234", accessNumber);
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_ACCESS_NUMBER);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    // The backend *must* support logout data
    BOOST_CHECK(!sdk.CanLogout(user));

    // Simulate wrong pin - must fail on access number validation too
    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthenticationAN(user, "1233", accessNumber);
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_ACCESS_NUMBER);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    // Fix access number - must fail with incorrect pin already
    accessNumber = originalAccessNumber;
    //s = sdk.StartAuthentication(user);
    s = sdk.FinishAuthenticationAN(user, "1235", accessNumber);
    BOOST_CHECK_EQUAL(s, Status::INCORRECT_PIN);
    BOOST_CHECK_EQUAL(user->GetState(), User::REGISTERED);

    sdk.DeleteUser(user);

    BOOST_MESSAGE("    testAuthenticateAN2 finished");
}
