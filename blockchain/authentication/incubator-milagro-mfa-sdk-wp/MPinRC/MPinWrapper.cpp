#include "MPinWrapper.h"

using namespace MPinRC;

#pragma region MPinWrapper
MPinWrapper::MPinWrapper() : sdk(new MPinSDK)
{
}

MPinWrapper::~MPinWrapper()
{
	if (sdk != nullptr)
		delete sdk;
}

void MPinWrapper::Destroy()
{
	this->sdk->Destroy();
}

void MPinWrapper::ClearUsers()
{
	this->sdk->ClearUsers();
}

MPinRC::StatusWrapper^ MPinWrapper::Construct(Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ config, MPinRC::IContext^ context)
{
	MPinSDK::StringMap map = Helper::ToNativeStringMap(config);
	this->proxy = new ContextProxy(context);
	MPinSDK::Status s = sdk->Init(map, proxy);

	return ref new StatusWrapper(s.GetStatusCode(), s.GetErrorMessage());
}

void MPinWrapper::ListUsers(Windows::Foundation::Collections::IVector<UserWrapper^>^ users)
{
	std::vector<MPinSDK::UserPtr> _users;
	sdk->ListUsers(_users);
	for each (MPinSDK::UserPtr user in _users)
	{
		MPinRC::UserWrapper^ uw = ref new MPinRC::UserWrapper(user);
		users->Append(uw);
	}
}

UserWrapper^ MPinWrapper::MakeNewUser(Platform::String^ id, Platform::String^ deviceName)
{
	MPinSDK::String nativeID = Helper::ToNativeString(id);
	MPinSDK::String nativeDeviceName = Helper::ToNativeString(deviceName);

	MPinSDK::UserPtr userPtr = this->sdk->MakeNewUser(nativeID, nativeDeviceName);
	MPinRC::UserWrapper^ uw = ref new MPinRC::UserWrapper(userPtr);

	return uw;
}

void MPinWrapper::DeleteUser(MPinRC::UserWrapper^ user)
{
	MPinSDK::UserPtr up = (MPinSDK::UserPtr)user->user;
	sdk->DeleteUser(up);
}

MPinRC::StatusWrapper^ MPinWrapper::StartRegistration(MPinRC::UserWrapper^ user, Platform::String^ activateCode, Platform::String^ userData)
{
	MPinSDK::String userStringData = Helper::ToNativeString(userData);
	MPinSDK::String activateCodeStringData = Helper::ToNativeString(activateCode);
	MPinSDK::Status st = sdk->StartRegistration(user->user, activateCodeStringData, userStringData);
	return ref new MPinRC::StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::RestartRegistration(MPinRC::UserWrapper^ user, Platform::String^ userData)
{
	MPinSDK::String userStringData = Helper::ToNativeString(userData);
	MPinSDK::Status st = sdk->RestartRegistration(user->user, userStringData);
	return ref new MPinRC::StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::ConfirmRegistration(MPinRC::UserWrapper^ user, Platform::String^ pushMessageIdentifier)
{
	MPinSDK::String nativePuchMsgId = Helper::ToNativeString(pushMessageIdentifier);
	MPinSDK::Status st = sdk->ConfirmRegistration(user->user, nativePuchMsgId);
	return ref new MPinRC::StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::FinishRegistration(MPinRC::UserWrapper^ user, Platform::String^ pin)
{
	MPinSDK::String nativePin = Helper::ToNativeString(pin);

	MPinSDK::Status st = sdk->FinishRegistration(user->user, nativePin);
	return ref new MPinRC::StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::StartAuthentication(MPinRC::UserWrapper^ user)
{
	MPinSDK::Status st = sdk->StartAuthentication(user->user);
	return ref new StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::CheckAccessNumber(Platform::String^ accessNumber)
{
	MPinSDK::String nativeAN = Helper::ToNativeString(accessNumber);
	MPinSDK::Status st = sdk->CheckAccessNumber(nativeAN);
	return ref new StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::FinishAuthentication(MPinRC::UserWrapper^ user, Platform::String^ pin)
{
	MPinSDK::String nativePIN = Helper::ToNativeString(pin);
	MPinSDK::Status st = sdk->FinishAuthentication(user->user, nativePIN);
	return ref new StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::FinishAuthentication(MPinRC::UserWrapper^ user, Platform::String^ pin, Platform::String^ authResultData)
{
	MPinSDK::String nativePIN = Helper::ToNativeString(pin);
	MPinSDK::String aRD = Helper::ToNativeString(authResultData);
	MPinSDK::Status st = sdk->FinishAuthentication(user->user, nativePIN, aRD);
	return ref new StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::FinishAuthenticationOTP(MPinRC::UserWrapper^ user, Platform::String^ pin, MPinRC::OTPWrapper^ otp)
{
	if (otp == nullptr)
	{
		throw ref new InvalidArgumentException("OTP should not be null!");
	}

	MPinSDK::String nativePIN = Helper::ToNativeString(pin);
	MPinSDK::Status st = sdk->FinishAuthenticationOTP(user->user, nativePIN, otp->otp);
	return ref new StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::FinishAuthenticationAN(MPinRC::UserWrapper^ user, Platform::String^ pin, Platform::String^ accessNumber)
{
	MPinSDK::String nativePIN = Helper::ToNativeString(pin);
	const MPinSDK::String accessNumberString = Helper::ToNativeString(accessNumber);
	MPinSDK::Status st = sdk->FinishAuthenticationAN(user->user, nativePIN, accessNumberString);
	return ref new StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::TestBackend(Platform::String^ server, Platform::String^ rpsPrefix)
{
	MPinSDK::String ntvServer = Helper::ToNativeString(server);
	MPinSDK::String ntvRpsPrefix = Helper::ToNativeString(rpsPrefix);
	MPinSDK::Status st = this->sdk->TestBackend(ntvServer, ntvRpsPrefix);
	return ref new MPinRC::StatusWrapper(st.GetStatusCode(), st.GetErrorMessage());
}

MPinRC::StatusWrapper^ MPinWrapper::SetBackend(Platform::String^ server, Platform::String^ rpsPrefix)
{
	MPinSDK::String ntvServer = Helper::ToNativeString(server);
	MPinSDK::String ntvRpsPrefix = Helper::ToNativeString(rpsPrefix);
	MPinSDK::Status st = this->sdk->SetBackend(ntvServer, ntvRpsPrefix);
	return ref new MPinRC::StatusWrapper(st.GetStatusCode(), (st.GetErrorMessage()));
}

bool MPinWrapper::CanLogout(UserWrapper^ user)
{
	return this->sdk->CanLogout(user->user);
}

bool MPinWrapper::Logout(UserWrapper^ user)
{
	return this->sdk->Logout(user->user);
}

Platform::String^ MPinWrapper::GetClientParam(Platform::String^ key)
{
	MPinSDK::String nKey = Helper::ToNativeString(key);
	return Helper::ToStringHat(this->sdk->GetClientParam(nKey));
}

Platform::String^ MPinWrapper::GetVersion()
{
	const char * nativeVersion = sdk->GetVersion();
	return Helper::ToStringHat(nativeVersion);
}
#pragma endregion MPinWrapper

#pragma region ContextProxy
ContextProxy::ContextProxy(MPinRC::IContext^ context)
{
	this->managedContext = context;
}

MPinSDK::IHttpRequest* ContextProxy::CreateHttpRequest() const
{
	MPinRC::IHttpRequest^ httpRequest = this->managedContext->CreateHttpRequest();
	return new MPinRC::HttpProxy(httpRequest);
}

void ContextProxy::ReleaseHttpRequest(IN IHttpRequest *request) const
{
	delete request;
}

MPinSDK::IStorage* ContextProxy::GetStorage(MPinSDK::IStorage::Type type) const
{
	MPinRC::StorageType managedType = type == MPinSDK::IStorage::Type::SECURE
		? MPinRC::StorageType::SECURE
		: MPinRC::StorageType::NONSECURE;

	MPinRC::IStorage^ storage = this->managedContext->GetStorage(managedType);
	MPinSDK::IStorage* nStorage = new MPinRC::StorageProxy(storage);
	return nStorage;
}

MPinSDK::CryptoType ContextProxy::GetMPinCryptoType() const
{
	MPinRC::CryptoType type = this->managedContext->GetMPinCryptoType();
	switch (type)
	{
	case MPinRC::CryptoType::CRYPTO_TEE:
		return MPinSDK::CryptoType::CRYPTO_TEE;
	case MPinRC::CryptoType::CRYPTO_NON_TEE:
	default:
		return MPinSDK::CryptoType::CRYPTO_NON_TEE;
	}
}

#pragma endregion ContextProxy