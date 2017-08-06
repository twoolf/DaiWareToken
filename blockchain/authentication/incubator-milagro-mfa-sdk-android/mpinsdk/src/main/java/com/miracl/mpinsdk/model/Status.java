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
package com.miracl.mpinsdk.model;


import java.io.Serializable;

public class Status implements Serializable {

    private final Code   mStatusCode;
    private final String mErrorMessage;


    public Status(int statusCode, String error) {
        this(Code.values()[statusCode], error);
    }


    public Status(Code statusCode, String error) {
        mStatusCode = statusCode;
        mErrorMessage = error;
    }

    public Code getStatusCode() {
        return mStatusCode;
    }

    public String getErrorMessage() {
        return mErrorMessage;
    }

    @Override
    public String toString() {
        return "Status [StatusCode=" + mStatusCode + ", ErrorMessage='" + mErrorMessage + "']";
    }

    public enum Code {
        OK, CANCELED_BY_USER, // Local error, returned when user cancels pin entering
        CRYPTO_ERROR, // Local error in crypto functions
        STORAGE_ERROR, // Local storage related error
        NETWORK_ERROR, // Local error - cannot connect to remote server (no internet, or invalid server/port)
        RESPONSE_PARSE_ERROR, // Local error - cannot parse json response from remote server (invalid json or unexpected
        // json structure)
        FLOW_ERROR, // Local error - improper MPinSDK class usage
        IDENTITY_NOT_AUTHORIZED, // Remote error - the remote server refuses user registration
        IDENTITY_NOT_VERIFIED, // Remote error - the remote server refuses user registration because identity is not
        // verified
        REQUEST_EXPIRED, // Remote error - the register/authentication request expired
        REVOKED, // Remote error - cannot get time permit (probably the user is temporary suspended)
        INCORRECT_PIN, // Remote error - user entered wrong pin
        INCORRECT_ACCESS_NUMBER, // Remote/local error - wrong access number (checksum failed or RPS returned 412)
        HTTP_SERVER_ERROR, // Remote error, that was not reduced to one of the above - the remote server returned
        // internal server error status (5xx)
        HTTP_REQUEST_ERROR, // Remote error, that was not reduced to one of the above - invalid data sent to server, the
        // remote server returned 4xx error status
        BAD_USER_AGENT, // Remote error - user agent not supported
        CLIENT_SECRET_EXPIRED // Remote error - re-registration required because server master secret expired
    }
}
