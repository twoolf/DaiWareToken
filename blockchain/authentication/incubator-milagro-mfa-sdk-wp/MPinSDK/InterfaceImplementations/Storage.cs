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
using Windows.Storage;
using Windows.Storage.Streams;

namespace MPinSDK
{
    class Storage : IStorage
    {        
        #region Fields
        StorageFolder localFolder = null;
        public const string MPIN_STORAGE = "tokens.json"; 
        public const string USER_STORAGE = "users.json";  

        private string path;
        private string Data { get; set; }

        public string ErrorMessage
        {
            private set;
            get;
        }
        #endregion

        #region C'tor
        public Storage(StorageType type) : base()
        {
            localFolder = ApplicationData.Current.LocalFolder;
            
            path = type == StorageType.SECURE ? MPIN_STORAGE : USER_STORAGE;
            this.Data = string.Empty;
        }
        #endregion // C'tor

        #region IStorage
        public bool SetData(string data)
        {
            lock (this.Data)
            {
                Task.Run(async () => { await SetDataAsync(data); }).Wait();
                return string.IsNullOrEmpty(this.ErrorMessage);
            }
        }

        public string GetData()
        {
            lock (this.Data)
            {
                Task.Run(async () => { await GetDataAsync(); }).Wait();
                return string.IsNullOrEmpty(this.Data) ? string.Empty : this.Data;
            }
        }

        public string GetErrorMessage()
        {
            return this.ErrorMessage;
        }
        #endregion // IStorage

        #region Methods

        private async Task SetDataAsync(string data)
        {
            this.ErrorMessage = string.Empty;
            byte[] fileBytes = System.Text.Encoding.UTF8.GetBytes(data.ToCharArray());
            var file = await GetFile();

            try
            {
                await FileIO.WriteTextAsync(file, data);
            }
            catch (Exception e)
            {
                this.ErrorMessage = e.Message;
            }
        }

        private async Task<StorageFile> GetFile()
        {
            StorageFile file;
            if (await IsFilePresent(path))
            {
                file = await localFolder.GetFileAsync(path);
            }
            else
            {
                file = await localFolder.CreateFileAsync(path, CreationCollisionOption.ReplaceExisting);
            }

            return file;
        }

        private async Task<bool> IsFilePresent(string fileName)
        {
            var allfiles = await localFolder.GetFilesAsync();
            foreach (var storageFile in allfiles)
            {
                if (storageFile.Name == fileName)
                {
                    return true;
                }
            }

            return false;
        }

        private async Task GetDataAsync()
        {
            this.ErrorMessage = string.Empty;
            try
            {
                var file = await GetFile();
                this.Data = await FileIO.ReadTextAsync(file);
            }
            catch (Exception e)
            {
                this.ErrorMessage = e.Message;
            }
        }

        #endregion // Methods
    }
}
