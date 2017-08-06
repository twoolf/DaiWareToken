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
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MPinSDK.Models
{
    /// <summary>
    /// Status class used to indicate whether an operation is successful or not.
    /// </summary>
    public class Status
    {
        internal StatusWrapper Wrapper { get; set; }

        /// <summary>
        /// Initializes a new instance of the <see cref="Status"/> class.
        /// </summary>
        /// <param name="statusCode">The status code.</param>
        /// <param name="error">The error message.</param>
        public Status(int statusCode, String error)
        {
            this.Wrapper = new StatusWrapper();
            this.StatusCode = (Code)Enum.GetValues(typeof(Code)).GetValue(statusCode);
            this.ErrorMessage = error;            
        }

        /// <summary>
        /// Gets the status code returned from the server.
        /// </summary>
        /// <value>
        /// The status code returned from the server.
        /// </value>
        public Code StatusCode
        {
            get
            {
                return (Code)this.Wrapper.Code;
            }
            private set
            {
                this.Wrapper.Code = (int)value;
            }
        }

        /// <summary>
        /// Gets or sets the message of the error if there is such one.
        /// </summary>
        /// <value>
        /// The error message.
        /// </value>
        public String ErrorMessage
        {
            get
            {
                return this.Wrapper.Error;
            }
            set
            {
                this.Wrapper.Error = value;
            }
        }

        /// <summary>
        /// Returns a <see cref="System.String" /> that represents this instance.
        /// </summary>
        /// <returns>
        /// A <see cref="System.String" /> that represents this instance.
        /// </returns>
        public override string ToString()
        {
            return "Status [StatusCode=" + this.StatusCode + ", ErrorMessage='" + this.ErrorMessage + "']";
        }

        /// <summary>
        /// Determines whether the specified <see cref="System.Object" />, is equal to this instance.
        /// </summary>
        /// <param name="obj">The <see cref="System.Object" /> to compare with this instance.</param>
        /// <returns>
        ///   <c>true</c> if the specified <see cref="System.Object" /> is equal to this instance; otherwise, <c>false</c>.
        /// </returns>
        public override bool Equals(object obj)
        {
            Status objToCompare = (Status)obj;
            if (objToCompare == null)
                return false;

            if (false == this.StatusCode.Equals(objToCompare.StatusCode))
                return false;

            if (false == this.ErrorMessage.Equals(objToCompare.ErrorMessage))
                return false;

            return true;
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table. 
        /// </returns>
        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        [Flags]
        public enum Code
        {
            /// <summary>
            /// Successful authentication.
            /// </summary>
            OK,
            /// <summary>
            /// Local error, returned when user cancels pin entering
            /// </summary>
            PinInputCanceled,
            /// <summary>
            /// Local error in crypto functions
            /// </summary>
            CryptoError,
            /// <summary>
            /// Local storage related error
            /// </summary>
            StorageError,
            /// <summary>
            /// Local error - cannot connect to remote server (no internet, or invalid server/port)
            /// </summary>
            NetworkError,
            /// <summary>
            /// Local error - cannot parse json response from remote server (invalid json or unexpected json structure)
            /// </summary>
            ResponseParseError,
            /// <summary>
            /// Local error - unproper MPinSDK class usage
            /// </summary>
            FlowError,
            /// <summary>
            /// Remote error - the remote server refuses user registration
            /// </summary>
            IdentityNotAuthorized,
            /// <summary>
            /// Remote error - the remote server refuses user registration because identity is not verified
            /// </summary>
            IdentityNotVerified,
            /// <summary>
            /// Remote error - the register/authentication request expired
            /// </summary>
            RequestExpired,
            /// <summary>
            /// Remote error - cannot get time permit (propably the user is temporary suspended)
            /// </summary>
            Revoked,
            /// <summary>
            /// Remote error - user entered wrong pin
            /// </summary>
            IncorrectPIN,
            /// <summary>
            /// Remote/local error - wrong access number (checksum failed or RPS returned 412)
            /// </summary>
            IncorrectAccessNumber,
            /// <summary>
            /// Remote error, that was not reduced to one of the above - the remote server returned internal server error status (5xx)
            /// </summary>
            HttpServerError,
            /// <summary>
            /// Remote error, that was not reduced to one of the above - invalid data sent to server, the remote server returned 4xx error status
            /// </summary>
            HttpRequestError
        }
    }
}
