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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MPinRC;
using System.Diagnostics;

namespace MPinSDK.Models
{
    /// <summary>
    /// The User object which represents an end-user of the M-Pin authentication.
    /// </summary>
    [DebuggerDisplay("{Id}, State: {UserState}")]
    public class User : IDisposable
    {
        private UserWrapper user;
        /// <summary>
        /// The states the <see cref="User"/> object supports. They reflect the advancement the physical user through the stages of the registration process and their current permissions to access the system.
        /// </summary>
        public enum State
        {
            /// <summary>
            /// The <see cref="User"/> object has been created, but the registration process has not started yet; any newly created Users are in Invalid state. (To begin User registration, call the StartRegistration method.) The Invalid state is also temporarily assigned to a User that has just been deleted until the User is physically deleted from memory.
            /// </summary>
            Invalid,
            /// <summary>
            /// The <see cref="User"/> object has been created, and the User registration process has started but not yet completed. The User’s state remains StartedRegistration until the FinishRegistration method is executed successfully. The StartedRegistration state indicates that if the registration procedure needs to be done anew, you must use the RestartRegistration method and not the StartRegistration method (The StartRegistration method will return FlowError in this case.).
            /// </summary>
            StartedRegistration,
            /// <summary>
            ///  Temporary state for the special case in which a <see cref="User"/> can be registered without going through a verification process, e.g. in case of a demo app. In this special case, the Activated state is assigned to the User upon StartRegistration, which allows the FinsihRegistration method to be called and to succeed without waiting for identity verification.
            /// </summary>
            Activated,
            /// <summary>
            /// The <see cref="User"/> registration has completed successfully and the User can now authenticate to the M-Pin System.
            /// </summary>
            Registered,
            /// <summary>
            /// State assigned to a <see cref="User"/> upon reaching the maximum allowed number of unsuccessful login attempts (3 by default, configurable through the maxInvalidLoginAttempts option the in the RPS.) Once this state is set, the end-user is blocked and should re-register.
            /// </summary>
            Blocked
        };

        internal User(UserWrapper user)
        {
            this.Wrapper = user;
        }

        /// <summary>
        /// Gets the unique identifier of the <see cref="User"/> object.
        /// </summary>
        /// <value>
        /// The unique identifier of the <see cref="User"/> object.
        /// </value>
        public String Id
        {
            get
            {
                return this.Wrapper.GetId();
            }
        }

        /// <summary>
        /// Gets the current state of the <see cref="User"/> object instance.
        /// </summary>
        /// <value>
        /// The current state of the user.
        /// </value>
        public State UserState
        {
            get
            {
                switch (this.Wrapper.GetState())
                {
                    case 1:
                        return State.StartedRegistration;
                    case 2:
                        return State.Activated;
                    case 3:
                        return State.Registered;
                    case 4:
                        return State.Blocked;
                    default:
                        return State.Invalid;
                }
            }
        }


        /// <summary>
        /// Returns a <see cref="System.String" /> that represents this <see cref="User"/> object instance.
        /// </summary>
        /// <returns>
        /// A <see cref="System.String" /> that represents this <see cref="User"/> object instance.
        /// </returns>
        public override string ToString()
        {
            return this.Id;
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
            return this.Id.Equals(((User)obj).Id);
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

        #region IDisposable
        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            lock (this)
            {
                this.Wrapper.Destruct();
            }
        }
        #endregion // IDisposable

        internal UserWrapper Wrapper
        {
            get
            {
                return this.user;
            }
            private set
            {
                this.user = value;
            }
        }
    }
}
