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

#include "mpin_sdk.h"
#include "Common.h"

namespace MPinRC
{
#pragma region IContext
	/// <summary>
	/// The Context Interface is the one that "bundles" all the rest of the interfaces. Only this interface is provided to the Core and the others are used/accessed through it.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public interface class IContext
	{
	public:
		virtual IHttpRequest^ CreateHttpRequest() = 0;
		virtual void ReleaseHttpRequest(IHttpRequest^ request) = 0;
		virtual IStorage^ GetStorage(MPinRC::StorageType type) = 0;
		virtual CryptoType GetMPinCryptoType() = 0;
	};

	class ContextProxy : public MPinSDK::IContext
	{
	private:
		MPinRC::IContext^ managedContext;

	public:
		ContextProxy(MPinRC::IContext^ context);

		typedef MPinSDK::IHttpRequest IHttpRequest;
		typedef MPinSDK::CryptoType CryptoType;
		typedef MPinSDK::IStorage IStorage;

		virtual IHttpRequest * CreateHttpRequest() const;
		virtual void ReleaseHttpRequest(IN IHttpRequest *request) const;
		virtual IStorage * GetStorage(IStorage::Type type) const;
		virtual CryptoType GetMPinCryptoType() const;
	};

#pragma endregion IContext

#pragma region MPinWrapper
	public ref class MPinWrapper sealed
	{
	private:
		MPinSDK* sdk;
		MPinRC::ContextProxy* proxy;

	public:

		MPinWrapper();
		virtual ~MPinWrapper();
		void Destroy();
		void ClearUsers();

		MPinRC::StatusWrapper^ Construct(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ config, MPinRC::IContext^ context);

		void ListUsers(Windows::Foundation::Collections::IVector<UserWrapper^>^ users);

		UserWrapper^ MakeNewUser(Platform::String^ id, Platform::String^ deviceName);
		void DeleteUser(UserWrapper^ user);

		MPinRC::StatusWrapper^ StartRegistration(MPinRC::UserWrapper^ user, Platform::String^ activateCode, Platform::String^ userData);
		MPinRC::StatusWrapper^ RestartRegistration(MPinRC::UserWrapper^ user, Platform::String^ userData);
		MPinRC::StatusWrapper^ ConfirmRegistration(MPinRC::UserWrapper^ user, Platform::String^ pushMessageIdentifier);
		MPinRC::StatusWrapper^ FinishRegistration(MPinRC::UserWrapper^ user, Platform::String^ pin);

		MPinRC::StatusWrapper^ StartAuthentication(MPinRC::UserWrapper^ user);
		MPinRC::StatusWrapper^ CheckAccessNumber(Platform::String^ accessNumber);
		MPinRC::StatusWrapper^ FinishAuthentication(MPinRC::UserWrapper^ user, Platform::String^ pin);
		MPinRC::StatusWrapper^ FinishAuthentication(MPinRC::UserWrapper^ user, Platform::String^ pin, Platform::String^ authResultData);
		MPinRC::StatusWrapper^ FinishAuthenticationOTP(MPinRC::UserWrapper^ user, Platform::String^ pin, MPinRC::OTPWrapper^ otp);
		MPinRC::StatusWrapper^ FinishAuthenticationAN(MPinRC::UserWrapper^ user, Platform::String^ pin, Platform::String^ accessNumber);

		MPinRC::StatusWrapper^ TestBackend(Platform::String^ server, Platform::String^ rpsPrefix);
		MPinRC::StatusWrapper^ SetBackend(Platform::String^ server, Platform::String^ rpsPrefix);

		bool CanLogout(UserWrapper^ user);
		bool Logout(UserWrapper^ user);
		Platform::String^ GetClientParam(Platform::String^ key);
		Platform::String^ GetVersion();
	};
#pragma region MPinWrapper
}