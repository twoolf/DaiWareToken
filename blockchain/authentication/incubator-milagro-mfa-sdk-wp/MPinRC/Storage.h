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

#pragma once

#include <string>
#include "mpin_sdk.h"

using namespace Platform;

namespace MPinRC
{
	/// <summary>
	/// Defines a type of a Storage.
	/// </summary>
	public enum class StorageType
	{
		SECURE,
		NONSECURE
	};

	public interface class IStorage
	{
		virtual bool SetData(String^ data);
		virtual String^ GetData();
		virtual String^ GetErrorMessage();
	};

	class StorageProxy : public MPinSDK::IStorage
	{
	private:
		MPinSDK::String errorMessage;
		MPinRC::IStorage^ managedStorage;

		void UpdateErrorMessage();

	public:
		StorageProxy(MPinRC::IStorage^ storage) { this->managedStorage = storage; };

		virtual bool SetData(const MPinSDK::String& data);
		virtual bool GetData(OUT MPinSDK::String &data);
		virtual const MPinSDK::String& GetErrorMessage() const;
	};
}