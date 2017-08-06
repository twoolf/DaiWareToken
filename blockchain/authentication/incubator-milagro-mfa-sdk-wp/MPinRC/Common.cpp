#include "Common.h"
#include <string>
#include <ctime>
// for debugging only
#include <windows.h>
#include <codecvt>

using namespace MPinRC;
using namespace Platform;
using namespace Platform::Collections;
using namespace std;

#pragma region Helper

MPinSDK::StringMap Helper::ToNativeStringMap(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ managedMap)
{
	MPinSDK::StringMap map = MPinSDK::StringMap();
	for (auto pair : managedMap)
	{
		MPinSDK::String key = Helper::ToNativeString(pair->Key);
		MPinSDK::String value = Helper::ToNativeString(pair->Value);
		std::pair<MPinSDK::String, MPinSDK::String> nPair(key, value);
		map.insert(nPair);
	}

	return map;
}

Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ Helper::ToManagedMap(const MPinSDK::StringMap& nMap)
{
	Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ map = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();

	for each(auto pair in nMap)
	{
		Platform::String^ key = Helper::ToStringHat(pair.first);
		Platform::String^ value = Helper::ToStringHat(pair.second);

		map->Insert(key, value);
	}

	return map;
}

MPinSDK::String Helper::ToNativeString(Platform::String^ text)
{
	std::wstring textWString(text->Begin());
	std::string textStr;
	utf8::utf16to8(textWString.begin(), textWString.end(), std::back_inserter(textStr));
	return textStr;
}

Platform::String^ Helper::ToStringHat(MPinSDK::String text)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	std::wstring textStr = convert.from_bytes(text.data());
	return ref new Platform::String(textStr.c_str());
}

void Helper::Log(Object^ parameter)
{
	auto paraString = parameter->ToString();
	auto formattedTest = std::wstring(paraString->Data()).append(L"\r\n");
	OutputDebugString(formattedTest.c_str());
}

#pragma endregion Helper

#pragma region UserWrapper

UserWrapper::UserWrapper(MPinSDK::UserPtr ptr)
{
	this->user = ptr;
}

Platform::String^ UserWrapper::GetId()
{
	return Helper::ToStringHat(user->GetId());
}

int UserWrapper::GetState()
{
	return this->user->GetState();
}

void UserWrapper::Destruct()
{
}
#pragma endregion UserWrapper

#pragma region StatusWrapper
MPinSDK::Status::Code StatusWrapper::ToCode(int codeInt)
{
	switch (codeInt)
	{
	case 1:
		return MPinSDK::Status::Code::PIN_INPUT_CANCELED;
	case 2:
		return MPinSDK::Status::Code::CRYPTO_ERROR;
	case 3:
		return MPinSDK::Status::Code::STORAGE_ERROR;
	case 4:
		return MPinSDK::Status::Code::NETWORK_ERROR;
	case 5:
		return MPinSDK::Status::Code::RESPONSE_PARSE_ERROR;
	case 6:
		return MPinSDK::Status::Code::FLOW_ERROR;
	case 7:
		return MPinSDK::Status::Code::IDENTITY_NOT_AUTHORIZED;
	case 8:
		return MPinSDK::Status::Code::IDENTITY_NOT_VERIFIED;
	case 9:
		return MPinSDK::Status::Code::REQUEST_EXPIRED;
	case 10:
		return MPinSDK::Status::Code::REVOKED;
	case 11:
		return MPinSDK::Status::Code::INCORRECT_PIN;
	case 12:
		return MPinSDK::Status::Code::INCORRECT_ACCESS_NUMBER;
	case 13:
		return MPinSDK::Status::Code::HTTP_SERVER_ERROR;
	case 14:
		return MPinSDK::Status::Code::HTTP_REQUEST_ERROR;
	default:
		return MPinSDK::Status::Code::OK;
	}
}

Platform::String^ StatusWrapper::Error::get()
{
	return Helper::ToStringHat(status.GetErrorMessage());
}

void StatusWrapper::Error::set(Platform::String^ value)
{
	status.SetErrorMessage(Helper::ToNativeString(value));
}

#pragma endregion StatusWrapper

#pragma region OtpWrapper

Platform::String^ OTPWrapper::Otp::get()
{
	return Helper::ToStringHat(otp.otp);
}
void OTPWrapper::Otp::set(Platform::String^ value)
{
	otp.otp = Helper::ToNativeString(value);
}

MPinRC::StatusWrapper^ OTPWrapper::Status::get()
{
	return ref new MPinRC::StatusWrapper(otp.status.GetStatusCode(), otp.status.GetErrorMessage());
}
void OTPWrapper::Status::set(MPinRC::StatusWrapper^ value)
{
	MPinSDK::Status::Code code = StatusWrapper::ToCode(value->Code);
	MPinSDK::String error = Helper::ToNativeString(value->Error);
	MPinSDK::Status newStatus = MPinSDK::Status(code, error);
	otp.status = newStatus;
}

#pragma endregion OtpWrapper
