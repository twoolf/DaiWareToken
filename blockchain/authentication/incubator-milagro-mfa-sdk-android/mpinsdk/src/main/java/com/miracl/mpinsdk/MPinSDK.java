/***************************************************************
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 ***************************************************************/
package com.miracl.mpinsdk;


import android.content.Context;

import com.miracl.mpinsdk.model.OTP;
import com.miracl.mpinsdk.model.ServiceDetails;
import com.miracl.mpinsdk.model.SessionDetails;
import com.miracl.mpinsdk.model.Status;
import com.miracl.mpinsdk.model.User;

import java.io.Closeable;
import java.util.List;
import java.util.Map;


public class MPinSDK implements Closeable {

    public static final String CONFIG_BACKEND = "backend";
    private long mPtr;

    public MPinSDK() {
        mPtr = nConstruct();
    }

    @Override
    public void close() {
        synchronized (this) {
            nDestruct(mPtr);
            mPtr = 0;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        close();
        super.finalize();
    }

    public Status Init(Map<String, String> config, Context context) {
        return nInit(mPtr, config, context);
    }

    public Status Init(Map<String, String> config, Context context, Map<String, String> customHeaders) {
        return nInitWithCustomHeaders(mPtr, config, context, customHeaders);
    }

    public void SetClientId(String clientId) {
        nSetClientId(mPtr, clientId);
    }


    public Status TestBackend(String server) {
        return nTestBackend(mPtr, server);
    }

    public Status TestBackend(String server, String rpsPrefix) {
        return nTestBackendRPS(mPtr, server, rpsPrefix);
    }

    public Status SetBackend(String server) {
        return nSetBackend(mPtr, server);
    }

    public Status SetBackend(String server, String rpsPrefix) {
        return nSetBackendRPS(mPtr, server, rpsPrefix);
    }

    public User MakeNewUser(String id) {
        return nMakeNewUser(mPtr, id, "");
    }

    public User MakeNewUser(String id, String deviceName) {
        return nMakeNewUser(mPtr, id, deviceName);
    }

    public Status StartRegistration(User user) {
        return nStartRegistration(mPtr, user, "", "");
    }

    public Status StartRegistration(User user, String activateCode) {
        return nStartRegistration(mPtr, user, activateCode, "");
    }

    public Status StartRegistration(User user, String activateCode, String userData) {
        return nStartRegistration(mPtr, user, activateCode, userData);
    }

    public Status RestartRegistration(User user) {
        return nRestartRegistration(mPtr, user, "");
    }

    public Status RestartRegistration(User user, String userData) {
        return nRestartRegistration(mPtr, user, userData);
    }

    public Status ConfirmRegistration(User user) {
        return nConfirmRegistration(mPtr, user, "");
    }

    public Status ConfirmRegistration(User user, String pushMessageIdentifier) {
        return nConfirmRegistration(mPtr, user, pushMessageIdentifier);
    }

    public Status FinishRegistration(User user, String pin) {
        return nFinishRegistration(mPtr, user, pin);
    }

    public Status StartAuthentication(User user) {
        return nStartAuthentication(mPtr, user);
    }

    public Status StartAuthentication(User user, String accessCode) {
        return nStartAuthenticationAccessCode(mPtr, user, accessCode);
    }

    public Status CheckAccessNumber(String accessNumber) {
        return nCheckAccessNumber(mPtr, accessNumber);
    }

    public Status FinishAuthentication(User user, String pin) {
        return nFinishAuthentication(mPtr, user, pin);
    }

    public Status FinishAuthentication(User user, String pin, StringBuilder authResultData) {
        return nFinishAuthenticationResultData(mPtr, user, pin, authResultData);
    }

    public Status FinishAuthenticationOTP(User user, String pin, OTP otp) {
        return nFinishAuthenticationOTP(mPtr, user, pin, otp);
    }

    public Status FinishAuthenticationAN(User user, String pin, String accessNumber) {
        return nFinishAuthenticationAN(mPtr, user, pin, accessNumber);
    }

    public Status FinishAuthenticationMFA(User user, String pin, StringBuilder authzCode) {
        return nFinishAuthenticationMFA(mPtr, user, pin, authzCode);
    }

    public Status GetSessionDetails(String accessCode, SessionDetails sessionDetails) {
        return nGetSessionDetails(mPtr, accessCode, sessionDetails);
    }

    public Status GetServiceDetails(String url, ServiceDetails serviceDetails) {
        return nGetServiceDetails(mPtr, url, serviceDetails);
    }

    public void DeleteUser(User user) {
        nDeleteUser(mPtr, user);
    }

    public Status ListUsers(List<User> users) {
        return nListUsers(mPtr, users);
    }

    public Status ListAllUsers(List<User> users) {
        return nListAllUsers(mPtr, users);
    }

    public Status ListUsers(List<User> users, String backend) {
        return nListUsersForBackend(mPtr, users, backend);
    }

    public Status ListBackends(List<String> backends) {
        return nListBackends(mPtr, backends);
    }

    public String GetVersion() {
        return nGetVersion(mPtr);
    }

    public boolean CanLogout(User user) {
        return nCanLogout(mPtr, user);
    }

    public boolean Logout(User user) {
        return nLogout(mPtr, user);
    }

    public String GetClientParam(String key) {
        return nGetClientParam(mPtr, key);
    }

    private native long nConstruct();

    private native void nDestruct(long ptr);

    private native Status nInit(long ptr, Map<String, String> config, Context context);

    private native Status nInitWithCustomHeaders(long ptr, Map<String, String> config, Context context,
                                                 Map<String, String> customHeaders);

    private native void nSetClientId(long ptr, String clientId);

    private native Status nTestBackend(long ptr, String server);

    private native Status nTestBackendRPS(long ptr, String server, String rpsPrefix);

    private native Status nSetBackend(long ptr, String server);

    private native Status nSetBackendRPS(long ptr, String server, String rpsPrefix);

    private native User nMakeNewUser(long ptr, String id, String deviceName);

    private native Status nStartRegistration(long ptr, User user, String activateCode, String userData);

    private native Status nRestartRegistration(long ptr, User user, String userData);

    private native Status nConfirmRegistration(long ptr, User user, String pushMessageIdentifier);

    private native Status nFinishRegistration(long ptr, User user, String pin);

    private native Status nStartAuthentication(long ptr, User user);

    private native Status nStartAuthenticationAccessCode(long ptr, User user, String accessCode);

    private native Status nCheckAccessNumber(long ptr, String accessNumber);

    private native Status nFinishAuthentication(long ptr, User user, String pin);

    private native Status nFinishAuthenticationResultData(long ptr, User user, String pin, StringBuilder authResultData);

    private native Status nFinishAuthenticationOTP(long ptr, User user, String pin, OTP otp);

    private native Status nFinishAuthenticationAN(long ptr, User user, String pin, String accessNumber);

    private native Status nFinishAuthenticationMFA(long ptr, User user, String pin, StringBuilder authzCode);

    private native Status nGetSessionDetails(long ptr, String accessCode, SessionDetails sessionDetails);

    private native Status nGetServiceDetails(long ptr, String url, ServiceDetails serviceDetails);

    private native void nDeleteUser(long ptr, User user);

    private native Status nListUsers(long ptr, List<User> users);

    private native Status nListAllUsers(long ptr, List<User> users);

    private native Status nListUsersForBackend(long ptr, List<User> users, String backend);

    private native Status nListBackends(long ptr, List<String> backends);

    private native String nGetVersion(long ptr);

    private native boolean nCanLogout(long ptr, User user);

    private native boolean nLogout(long ptr, User user);

    private native String nGetClientParam(long ptr, String key);
}
