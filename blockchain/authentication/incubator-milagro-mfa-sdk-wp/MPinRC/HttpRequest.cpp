#include "HttpRequest.h"
#include "Common.h"

using namespace MPinRC;
using namespace Platform;
using namespace Platform::Collections;

typedef MPinSDK::StringMap StringMap;

#pragma region HttpProxy
void HttpProxy::SetHeaders(const StringMap& headers)
{
	this->managedRequest->SetHeaders(Helper::ToManagedMap(headers));
}

void HttpProxy::SetQueryParams(const StringMap& queryParams)
{
	this->managedRequest->SetQueryParams(Helper::ToManagedMap(queryParams));
}

void HttpProxy::SetContent(const MPinSDK::String& data)
{
	this->managedRequest->SetContent(Helper::ToStringHat(data));
}

void HttpProxy::SetTimeout(int seconds)
{
	this->managedRequest->SetTimeout(seconds);
}

bool HttpProxy::Execute(Method method, const MPinSDK::String& url)
{
	Windows::Web::Http::HttpMethod^ mMethod = GetHttpMethod(method);
	bool succeeded = this->managedRequest->Execute(mMethod, Helper::ToStringHat(url));
	MPinSDK::String data = Helper::ToNativeString(this->managedRequest->GetResponseData());
	responseData.append(data);
	Windows::Foundation::Collections::IMap<String^, String^>^ mMap = this->managedRequest->GetResponseHeaders();
	responseHeaders = Helper::ToNativeStringMap(mMap);	
	httpResponseCode = this->managedRequest->GetHttpStatusCode();
	errorMessage = Helper::ToNativeString(this->managedRequest->GetExecuteErrorMessage());

	return succeeded;	
}

Windows::Web::Http::HttpMethod^ HttpProxy::GetHttpMethod(MPinSDK::IHttpRequest::Method nativeMethod)
{
	switch (nativeMethod)
	{	
	case MPinSDK::IHttpRequest::POST:
		return HttpMethod::Post;
	case MPinSDK::IHttpRequest::PUT:
		return HttpMethod::Put;
	case MPinSDK::IHttpRequest::DELETE:
		return HttpMethod::Delete;
	case MPinSDK::IHttpRequest::OPTIONS:
		return HttpMethod::Options;
	case MPinSDK::IHttpRequest::PATCH:
		return HttpMethod::Patch;	
	case MPinSDK::IHttpRequest::GET:
	default:
		return HttpMethod::Get;
	}
}

const MPinSDK::String& HttpProxy::GetExecuteErrorMessage() const
{
	return this->errorMessage;
}

int HttpProxy::GetHttpStatusCode() const
{
	return this->httpResponseCode;	
}

const StringMap& HttpProxy::GetResponseHeaders() const
{
	return responseHeaders;	
}

const MPinSDK::String& HttpProxy::GetResponseData() const
{
	return responseData;
}
#pragma endregion HttpProxy

