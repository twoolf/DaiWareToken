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

using MPinRC;
using MPinSDK.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.ApplicationModel.Resources;
using Windows.Data.Json;

namespace MPinSDK
{

    /// <summary>
    /// The MPin SDK version 2 class.
    /// </summary>
    [Windows.Foundation.Metadata.WebHostHidden]
    public class MPin : IDisposable
    {
        #region Members
        static MPinWrapper mPtr;
        private static int InvalidStatus = 6; // Flow error
        private static readonly object lockObject = new object();
        private IContext context { get; set; }
        #endregion

        #region C'tor
        /// <summary>
        /// Initializes a new instance of the <see cref="MPin" /> SDK class.
        /// </summary>
        public MPin()
        {
            mPtr = new MPinWrapper();
        }
        #endregion

        #region Methods
        /// <summary>
        /// Initializes the <see cref="MPin"/> SDK instance.
        /// </summary>
        /// <param name="config">A key-value map of configuration parameters. Unsupported parameters will be ignored. Currently, the Core recognized the following parameters: backend - the URL of the M-Pin back-end service (Mandatory) and rpsPrefix - the prefix that should be added for requests to the RPS (Optional). The default value is "rps". </param>
        /// <param name="context">An <see cref="IContext"/> instance.</param>
        /// <returns> A <see cref="Status"/> which indicates whether the operation was successful or not.</returns>
        public Status Init(IDictionary<string, string> config, IContext context = null)
        {
            if (context == null)
                context = new Context();

            if (config == null || context == null)
                return new Status(InvalidStatus, ResourceLoader.GetForCurrentView().GetString("WrongParameters")); 

            StatusWrapper sw;
            lock (lockObject)
            {
                sw = mPtr.Construct(config, context);
                this.context = context;
            }

            return new Status(sw.Code, sw.Error);
        }

        /// <summary>
        /// Creates a new <see cref="User"/> object.
        /// </summary>
        /// <param name="id">The unique identity of the user.</param>
        /// <param name="deviceName">Optional device name, which is passed to the RPA to store it and use it later to determine which M-Pin ID is associated with this device.</param>
        /// <returns> A <see cref="Status"/> which indicates whether the operation was successful or not.</returns>
        public User MakeNewUser(string id, string deviceName = "")
        {
            if (string.IsNullOrEmpty(id))
                return null;

            UserWrapper wrapper;
            lock (lockObject)
            {
                wrapper = mPtr.MakeNewUser(id, deviceName);
            }

            return new User(wrapper);
        }

        /// <summary>
        /// Deletes a <see cref="User"/> from the Users List maintained by the SDK and all the data related this User, such as the User’s M-Pin ID, State, and M-Pin Token.
        /// </summary>
        /// <param name="user">The user instance.</param>
        public void DeleteUser(User user)
        {
            Status st = null;
            if (AreParametersValid(ref st, user))
            {
                lock (lockObject)
                {
                    if (mPtr != null && user != null)
                        mPtr.DeleteUser(user.Wrapper);
                }
            }
        }

        /// <summary>
        /// Populates a list with all currently existing Users, irrespective of their state. (Different Users might be in different states, reflecting their registration status.) These are the users that are currently available in the SDK’s Users List.
        /// </summary>
        /// <param name="users">Returns a list of users in List format.</param>
        public void ListUsers(List<User> users)
        {
            if (users != null)
            {
                IList<UserWrapper> usersList = new List<UserWrapper>();
                mPtr.ListUsers(usersList);
                foreach (var user in usersList)
                {
                    users.Add(new User(user));
                }
            }
        }

        /// <summary>
        /// Initializes the registration process for a <see cref="User"/> which has been alredy created with the MakeNewUser method. This causes the RPA to begin an identity verification procedure for the User (like sending a verification email, for instance). At that, the User’s status changes to StartedRegistration and remains like this until the FinishRegistration method has been executed successfully.
        /// </summary>
        /// <param name="user">The <see cref="User"/> object instance.</param>
        /// <param name="userData"> Optionally, the application might pass additional userData which might help the RPA to verify the user identity. The RPA might decide to verify the identity without starting a verification process. In this case the Status of the call will still be Status::OK, but the User State will be Activated. </param>
        /// <returns> A <see cref="Status"/> which indicates whether the operation was successful or not.</returns>
        /// <remarks> Under certain scenarios, like a demo application, the RPA might be configured to verify identities without starting a verification process. In this case, the status of the call will still be OK, but the User state will be set to Activated. </remarks>
        public Status StartRegistration(User user, string activateData = "", string userData = "")
        {
            Status st = null;
            if (AreParametersValid(ref st, user))
            {
                StatusWrapper sw;
                lock (lockObject)
                {
                    sw = user != null ? mPtr.StartRegistration(user.Wrapper, activateData, userData) : new StatusWrapper() { Code = InvalidStatus, Error = ResourceLoader.GetForCurrentView().GetString("NullUser") };
                }

                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// This method re-initializes the registration process for a <see cref="User"/> that already started it. 
        /// </summary>
        /// <param name="user">The <see cref="User"/> object instance.</param>
        /// <param name="userData"> Optionally, the application might pass additional userData which might help the RPA to verify the user identity. The RPA might decide to verify the identity without starting a verification process. In this case the Status of the call will still be Status::OK, but the User State will be Activated. </param>
        /// <returns> A <see cref="Status"/> which indicates whether the operation was successful or not.</returns>
        /// <remarks>The difference between this method and the StartRegistration() is that during this one, no new M-Pin ID will be generated for the user, but the already generated one will be used. So StartRegistration can be called only for Users in the StartedRegistration state and RestartRegistration is designed to be used for Users in the Invalid state.</remarks>   
        public Status RestartRegistration(User user, string userData = "")
        {
            Status st = null;
            if (AreParametersValid(ref st, user))
            {
                StatusWrapper sw;
                lock (lockObject)
                {
                    sw = mPtr.RestartRegistration(user.Wrapper, userData);
                }

                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Finalizes the <see cref="User" /> registration process.
        /// </summary>
        /// <param name="user">The <see cref="User" /> object instance.</param>
        /// <param name="pin">The pin that the user just entered.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the operation was successful or not. On successful completion, the <see cref="User" /> state is set to Registered and the method returns OK.
        /// </returns>
        public Status FinishRegistration(User user, string pin)
        {
            Status st = null;
            if (AreParametersValid(ref st, user, pin))
            {
                StatusWrapper sw;
                lock (lockObject)
                {
                    sw = mPtr.FinishRegistration(user.Wrapper, pin);
                }

                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Confirms the <see cref="User" /> registration process.
        /// </summary>
        /// <param name="user">The <see cref="User" /> object instance.</param>
        /// <param name="pushMessageIdentifier">The push message identifier.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the operation was successful or not.
        /// </returns>
        public Status ConfirmRegistration(User user, string pushMessageIdentifier = "")
        {
            Status st = null;
            if (AreParametersValid(ref st, user, DefaultStringValue, DefaultStringValue, pushMessageIdentifier))
            {
                StatusWrapper sw;
                lock (lockObject)
                {
                    sw = mPtr.ConfirmRegistration(user.Wrapper, pushMessageIdentifier);
                }

                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Starts the authentication process of a <see cref="User" /> for the needs of the overlaying application. 
        /// </summary>
        /// <param name="user">The <see cref="User" /> to be authenticated.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the operation was successful or not.
        /// </returns>
        public Status StartAuthentication(User user)
        {
            Status st = null;
            if (AreParametersValid(ref st, user))
            {
                StatusWrapper sw = mPtr.StartAuthentication(user.Wrapper);
                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Checks if the specified access number is valid for the current user session.
        /// </summary>
        /// <param name="accessNumber">The access number.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the access number is valid or no.
        /// </returns>
        public Status CheckAccessNumber(string accessNumber)
        {
            if (string.IsNullOrEmpty(accessNumber))
                return new Status(InvalidStatus, ResourceLoader.GetForCurrentView().GetString("NullAccessNumber"));

            StatusWrapper sw = mPtr.CheckAccessNumber(accessNumber);
            return new Status(sw.Code, sw.Error);
        }

        /// <summary>
        /// Finishes the authentication process of a <see cref="User" /> for the needs of the overlaying application.
        /// </summary>
        /// <param name="user">The <see cref="User" /> to be authenticated.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the operation was successful or not.
        /// </returns>
        public Status FinishAuthentication(User user, string pin, string authResultData = null)
        {
            Status st = null;
            if (AreParametersValid(ref st, user, pin))
            {
                StatusWrapper sw = authResultData == null
                        ? mPtr.FinishAuthentication(user.Wrapper, pin)
                        : mPtr.FinishAuthentication(user.Wrapper, pin, authResultData);

                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Authenticates the <see cref="User" /> and, if authentication has been successful, the RPA issues One-Time Password (OTP) for authenticating with a RADIUS server. (The authentication itself doesn’t log the User in: instead, the result of the authentication is the issuing of the OTP.)
        /// </summary>
        /// <param name="user">The <see cref="User" /> to be authenticated.</param>
        /// <param name="pin">The PIN of the user.</param>
        /// <param name="otp">When the authentication is successful, in addition to the OK status, the method returns also an <see cref="OTP" /> structure generated by the RPA.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the operation was successful or not.
        /// </returns>
        public Status FinishAuthenticationOTP(User user, string pin, OTP otp)
        {
            if (otp == null)
                return FinishAuthentication(user, pin);

            Status st = null;
            if (AreParametersValid(ref st, user, pin))
            {
                StatusWrapper sw = mPtr.FinishAuthenticationOTP(user.Wrapper, pin, otp.Wrapper);
                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Authenticates a <see cref="User" /> against an Access Number provided by a PC/browser session. After this authentication, the user will be able to log-in on to the PC/browser with the provided Access Number while the authentication itself is performed on the user's mobile device.
        /// </summary>
        /// <param name="user">The <see cref="User" /> to be authenticated.</param>
        /// <param name="pin">The PIN of the user.</param>
        /// <param name="accessNumber">The Access Number provided by the PC/browser session. Required if Access Number authentication is being performed.</param>
        /// <returns>
        /// A <see cref="Status" /> which indicates whether the operation was successful or not.
        /// </returns>
        public Status FinishAuthenticationAN(User user, string pin, string accessNumber)
        {
            Status st = null;
            if (AreParametersValid(ref st, user, pin, accessNumber))
            {
                StatusWrapper sw = mPtr.FinishAuthenticationAN(user.Wrapper, pin, accessNumber);
                return new Status(sw.Code, sw.Error);
            }

            return st;
        }

        /// <summary>
        /// Tests whether the M-Pin back-end service is operational by sending a request for retrieving the Client settings to back-end’s URL.
        /// </summary>
        /// <param name="backend">The URL of the M-Pin back-end service to test.</param>
        /// <param name="rpsPrefix">An optional string representing the prefix for the requests to the RPS. Required only if the default prefix has been changed. If not provided, the value defaults to rps.</param>
        /// <returns> A <see cref="Status"/> which indicates whether the operation was successful or not.</returns>
        public Status TestBackend(string backend, string rpsPrefix = "")
        {
            StatusWrapper status;
            lock (lockObject)
            {
                status = mPtr.TestBackend(backend, rpsPrefix);
            }

            return new Status(status.Code, status.Error);
        }

        /// <summary>
        /// Modifies the currently configured M-Pin back-end service. The back-end is initially set at SDK initialization (i.e. through the <see cref="M:MPinSDK.MPin.Init"/> method), but it can be changed at any time using SetBackend.
        /// </summary>
        /// <param name="backend">The URL of the new M-Pin back-end service.</param>
        /// <param name="rpsPrefix">An optional string representing the prefix for the requests to the RPS. Required only if the default prefix has been changed. If not provided, the value defaults to rps.</param>
        /// <returns> A <see cref="Status"/> which indicates whether the operation was successful or not.</returns>
        public Status SetBackend(string backend, string rpsPrefix = "")
        {
            StatusWrapper status;
            lock (lockObject)
            {
                status = mPtr.SetBackend(backend, rpsPrefix);
            }

            return new Status(status.Code, status.Error);
        }

        /// <summary>
        /// Examines whether RPA supports logging out the <see cref="User" /> from the mobile device that have been used to provide the Access Number for authenticating the user to another device/browser session. Therefore, the method should be used after Access Number authentication, i.e. following the <see cref="M:MPinSDK.MPin.AuthenticateAN">AuthenticateAN(user, accessNumber)</see> method.
        /// </summary>
        /// <param name="user">The user.</param>
        /// <returns>True if the user can be logged out from the remote server, False - if (s)he cannot.</returns>
        public bool CanLogout(User user)
        {
            if (user == null)
                return false;

            bool canLogout;
            lock (lockObject)
            {
                canLogout = mPtr.CanLogout(user.Wrapper);
            }

            return canLogout;
        }

        /// <summary>
        /// Attempts to log out the end-user from a remote (browser) session after successful authentication through the <see cref="M:MPinSDK.MPin.AuthenticateAN">AuthenticateAN(user, accessNumber)</see> method. 
        /// <remarks>Before calling this method, make sure that the logout data has been provided by the RPA and that the logout operation is feasible.</remarks>
        /// </summary>
        /// <param name="user">The user.</param>
        /// <returns>True if the log-out request to the RPA has been successful, false - if failed.</returns>
        public bool Logout(User user)
        {
            if (user == null)
                return false;

            bool logout;
            lock (lockObject)
            {
                logout = mPtr.Logout(user.Wrapper);
            }

            return logout;
        }

        /// <summary>
        /// Returns the value for a Client Setting with the given key. Client settings that might interest the applications are: 
        /// accessNumberDigits - The number of access number digits that should be entered by the user, prior to calling <see cref="M:MPinSDK.MPin.AuthenticateAN">AuthenticateAN(user, accessNumber)</see> method. 
        /// setDeviceName - Indicator (true/false) whether the application should ask the user to insert a Device Name and pass it to the MakeNewUser() method.
        /// appID - The App ID used by the backend. The App ID is a unique ID assigned to each customer or application. It is a hex-encoded long numeric value. The App ID can be used only for information purposes, it doesn't affect the application's behavior in any way.
        /// </summary>
        /// <remarks> The value is returned as a <see cref="T:System.String"/> always, i.e. when a numeric or a boolean value is expected, the conversion should be handled by by the application.</remarks>
        /// <param name="key">The key.</param>
        /// <returns>А <see cref="T:System.String"/> value for a Client Setting with the given key.</returns>
        public string GetClientParam(string key)
        {
            if (string.IsNullOrEmpty(key))
                return string.Empty;

            string param = string.Empty;
            lock (lockObject)
            {
                param = mPtr.GetClientParam(key);
            }

            return param;
        }

        /// <summary>
        /// Returns the version of the M-Pin SDK.
        /// </summary>
        /// <returns>The version of the M-Pin SDK.</returns>
        public string GetVersion()
        {
            return mPtr.GetVersion();
        }

        #region IDisposable
        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            lock (lockObject)
            {
                mPtr.Destroy();
                mPtr = null;
            }
        }
        #endregion // IDisposable


        const string DefaultStringValue = " DEFAULT ";
        private bool AreParametersValid(ref Status st, User user = null, string pin = DefaultStringValue,
                                        string accessNumber = DefaultStringValue, string pushMessageIdentifier = DefaultStringValue)
        {
            if (user == null)
                st = new Status(InvalidStatus, ResourceLoader.GetForCurrentView().GetString("NullUser"));

            if (string.IsNullOrEmpty(pin))
                st = new Status(InvalidStatus, ResourceLoader.GetForCurrentView().GetString("EmptyPin"));

            if (string.IsNullOrEmpty(accessNumber))
                st = new Status(InvalidStatus, ResourceLoader.GetForCurrentView().GetString("NullAccessNumber"));

            // we still does not use this flow
            //if (string.IsNullOrEmpty(pushMessageIdentifier))
            //    st = new Status(InvalidStatus, ResourceLoader.GetForCurrentView().GetString("EmptyPushMsg"));

            return st == null ? true : false;
        }
        #endregion
    }
}
