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
#include <collection.h>

#include "mpin_sdk.h"
#include "HttpRequest.h"
#include "Storage.h"

using namespace Platform::Collections;

/// <summary>
/// The MPinRC assembly ports all unmanaged files to managed code so the MPinSDK could be used by Windows Phone c# compiler.
/// </summary>
namespace MPinRC
{
	/// <summary>
	/// The CryptoType enumeration used for generating the supported Crypto Type on the specific platform.
	/// <remarks>Currently, only on the Android platform this method might return something different than Non-TEE Crypto. Other platforms will always return Non-TEE Crypto</remarks>
	/// </summary>
	public enum class CryptoType
	{
		CRYPTO_TEE,
		CRYPTO_NON_TEE
	};

	public enum class Mode
	{
		REGISTER,
		AUTHENTICATE
	};

#pragma region UserWrapper
	/// <summary>
	/// A wrapper class used to pass User data from managed to unmanaged User objects and vice versa.
	/// </summary>
	public ref class UserWrapper sealed
	{
	internal:
		MPinSDK::UserPtr user;
		UserWrapper(MPinSDK::UserPtr);

	public:
		Platform::String^ GetId();
		int GetState();
		void Destruct();
	};
#pragma endregion UserWrapper

#pragma region StatusWrapper
	/// <summary>
	/// A wrapper class used to pass Status data from managed to unmanaged Status objects and vice versa.
	/// </summary>
	public ref class StatusWrapper sealed
	{
	private:
		MPinSDK::Status status;

	internal:
		StatusWrapper(MPinSDK::Status::Code code) : status(code) {}
		StatusWrapper(MPinSDK::Status::Code code, MPinSDK::String error) : status(code, error) {}
		static MPinSDK::Status::Code ToCode(int codeInt);

	public:
		StatusWrapper() {}

		property int Code
		{
			int get() { return status.GetStatusCode(); }
			void set(int value)
			{
				status.SetStatusCode(StatusWrapper::ToCode(value));
			}
		}

		property Platform::String^ Error
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

	};
#pragma endregion StatusWrapper

#pragma region OTPWrapper
	/// <summary>
	/// A wrapper class used to pass OTP data from managed to unmanaged OTP objects and vice versa.
	/// </summary>
	public ref class OTPWrapper sealed
	{
	internal:
		MPinSDK::OTP otp;

	public:
		property Platform::String^ Otp
		{
			Platform::String^ get();
			void set(Platform::String^ value);
		}

		property int64 ExpireTime
		{
			int64 get() { return otp.expireTime; }
			void set(int64 value) { otp.expireTime = value; }
		}

		property int TtlSeconds
		{
			int get() { return otp.ttlSeconds; }
			void set(int value) { otp.ttlSeconds = value; }
		}

		property int64 NowTime
		{
			int64 get() { return otp.nowTime; }
			void set(int64 value) { otp.nowTime = value; }
		}

		property MPinRC::StatusWrapper^ Status
		{
			MPinRC::StatusWrapper^ get();
			void set(MPinRC::StatusWrapper^ value);
		}
	};
#pragma endregion OTPWrapper

#pragma region Helper
	/// <summary>
	/// A class with helper methods.
	/// </summary>
	public ref class Helper sealed
	{
	private:		
		void Log(Object^);
	internal:
		static MPinSDK::StringMap ToNativeStringMap(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ managedMap);
		static Platform::String^ ToStringHat(MPinSDK::String text);
		static MPinSDK::String ToNativeString(Platform::String^ text);
		static Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ ToManagedMap(const MPinSDK::StringMap& nMap);			
	};
#pragma endregion Helper
}