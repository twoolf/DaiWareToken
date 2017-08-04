#include "Storage.h"
#include "Common.h"

using namespace MPinRC;
using namespace Windows::Storage;

#pragma region StorageProxy implementation
bool StorageProxy::SetData(const MPinSDK::String& data)
{
	bool result = this->managedStorage->SetData(Helper::ToStringHat(data));
	UpdateErrorMessage();
	return result;
}

bool StorageProxy::GetData(OUT MPinSDK::String &data)
{
	bool result = true;
	try
	{
		String^ managedData = this->managedStorage->GetData();
		data = Helper::ToNativeString(managedData);
	}
	catch (Platform::Exception^ e)
	{
		result = false;
	}

	UpdateErrorMessage();
	return result;
}

const MPinSDK::String& StorageProxy::GetErrorMessage() const
{
	return errorMessage;
}

void StorageProxy::UpdateErrorMessage()
{
	Platform::String^ error = this->managedStorage->GetErrorMessage();
	this->errorMessage = Helper::ToNativeString(error);	
}
#pragma endregion StorageProxy