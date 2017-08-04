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

namespace MPinSDK
{
    /// <summary>
    /// A Context class implementing the <see cref="T:MPinRC.IContext">IContext</see> interface which "bundles" all the rest of the interfaces. An instance of this class is provided to the Core and the others are used/accessed through it. 
    /// </summary>
    public class Context : IContext
    {
        #region Members
        IStorage storageSecure { get; set; }
        IStorage storageNonsecure { get; set; }
        IHttpRequest httpRequest { get; set; }
        #endregion

        #region IContext
        /// <summary>
        /// Creates a new HTTP request instance that conforms with <see cref="T:MPinRC.IHttpRequest">IHttpRequest</see> interface.
        /// </summary>
        /// <returns>An instance of a class, implementing <see cref="T:MPinRC.IHttpRequest">IHttpRequest</see> interface. </returns>
        public IHttpRequest CreateHttpRequest()
        {
            if (this.httpRequest == null)
                this.httpRequest = new HTTPConnector();

            return this.httpRequest;
        }

        /// <summary>
        /// Destroys/releases a previously created HTTP request instance.
        /// </summary>
        /// <param name="request">The request.</param>
        public void ReleaseHttpRequest(IHttpRequest request)
        { }

        /// <summary>
        /// Creates a Storage class implementation, which conforms to <see cref="T:MPinRC.IStorage">IStorage</see> interface,
        /// depending on the specified type.
        /// </summary>
        /// <param name="type">The <see cref="T:MPinRC.StorageType">Storage type</see>.</param>
        /// <returns>A Storage class implementation.</returns>
        public IStorage GetStorage(MPinRC.StorageType type)
        {
            if (type == StorageType.SECURE)
            {
                if (storageSecure == null)
                {
                    storageSecure = new Storage(StorageType.SECURE);
                }

                return storageSecure;
            }

            if (storageNonsecure == null)
            {
                storageNonsecure = new Storage(StorageType.NONSECURE);
            }

            return storageNonsecure;
        }

        /// <summary>
        /// This method provides an information regarding the supported Crypto Type on the specific platform. Currently, only on the Android platform this method might return something different than Non-TEE Crypto. Other platforms always returns Non-TEE Crypto. 
        /// </summary>
        /// <returns></returns>
        public CryptoType GetMPinCryptoType()
        {
            return CryptoType.CRYPTO_NON_TEE;
        }
        #endregion
    }
}
