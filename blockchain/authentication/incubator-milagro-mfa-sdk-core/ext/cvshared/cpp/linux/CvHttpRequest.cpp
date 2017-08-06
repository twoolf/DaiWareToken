#include "CvHttpRequest.h"

#include "CvLogger.h"
#include "CvXcode.h"

#include <stdexcept>

#include <string.h>
#include <stdlib.h>

#include <openssl/crypto.h>

using namespace std;
using namespace CvShared;

#if defined(__linux__)
	CvShared::CvMutex* CvHttpRequest::COpenSslMt::m_lockArray = NULL;
#endif

struct CurlWriteBuffer
{
	const char *data;
	int64_t sizeleft;
    CvHttpRequest *request;
};

CvHttpRequest::CvHttpRequest( enHttpMethod_t method ) :
	m_bCancel(false), m_responseCode(0), m_progressUp(0), m_progressDown(0),
	m_bResponseDataStarted(false), m_mutex("http-request")
{
	m_mutex.Create();
	m_req.method = method;
}

CvHttpRequest::~CvHttpRequest()
{
}

void CvHttpRequest::SetHeaders( const CMapHttpHeaders& aHeaders )
{
	m_req.header_list = NULL;

	for ( CMapHttpHeaders::const_iterator itr = aHeaders.begin(); itr != aHeaders.end(); ++itr )
	{
		m_req.header_list = curl_slist_append( m_req.header_list,  (itr->first + ":" + itr->second).c_str() );
	}
}

const String& CvHttpRequest::GetResponseHeader( const String& aKey ) const
{
	static const String empty;

	if ( m_responseHeaders.count(aKey) < 1 )
		return empty;

	return m_responseHeaders.find(aKey)->second;
}

size_t CvHttpRequest::ReadResponse(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t bufferSize = size * nmemb;

	CvHttpRequest* pRequest = (CvHttpRequest*)stream;

	if ( pRequest->m_bResponseDataStarted )
	{
		size_t currSize = pRequest->m_response.size();
		pRequest->m_response.resize( currSize + bufferSize );
		memcpy( (void*)(pRequest->m_response.data() + currSize), ptr, bufferSize );
	}
	
	if ( bufferSize == strlen("\r\n") && strncmp( (const char*)ptr, "\r\n", bufferSize ) == 0 )
		pRequest->m_bResponseDataStarted = true;
	
	return bufferSize;
}

size_t CvHttpRequest::WriteData(void *ptr, size_t size, size_t nmemb, void *stream)
{
	struct CurlWriteBuffer *readBuf = (struct CurlWriteBuffer*)stream;
	size_t curlBufSize = size * nmemb;
	size_t retval = 0;

	if (curlBufSize < 1)
		return retval;

	if (readBuf->sizeleft > curlBufSize)
	{
		memcpy(ptr, (const void *) readBuf->data, curlBufSize);
		String s = String((char *) ptr);
		retval = curlBufSize;
		readBuf->sizeleft -= curlBufSize;
		readBuf->data += curlBufSize;
	}
	else
	{
		memcpy(ptr, (const void *) readBuf->data, readBuf->sizeleft);
		retval = readBuf->sizeleft;
		readBuf->sizeleft = 0;
	}
    
    readBuf->request->m_bResponseDataStarted = false;
	
	return retval;
}

size_t CvHttpRequest::WriteToFile(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = 0;
	if (stream)
	{
		written = fwrite(ptr, size, nmemb, stream);
		return written;
	}

	return written;
}

size_t CvHttpRequest::HeaderCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	CvHttpRequest* pRequest = (CvHttpRequest*)userdata;
	const char* pLine = (const char*)ptr;
	const char* pDelim = strchr( pLine, ':' );

	if ( pDelim != NULL )
	{
		CvString key( pLine, pDelim - pLine );
		CvString value( pDelim + 1 );

		// trim leading spaces
		value.TrimLeft( " " );
		// trim trailing \r\n
		value.TrimRight( "\r\n" );
		
		pRequest->m_responseHeaders[key] = value;

		if ( key == HTTP_HEADER_CONTENT_LENGTH )
			pRequest->m_contentSize = atol( value.c_str() );
	}

	return size * nmemb;
}

int CvHttpRequest::SetProgress(void *data, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CvHttpRequest *c = (CvHttpRequest *)data;
	
	bool cancel = false;
	{
		CvMutexLock lock(c->m_mutex);
		cancel = c->m_bCancel;
	}

	if (!cancel)
	{
		c->m_progressDown = dlnow;
		c->m_progressUp = ulnow;
	}
	else
	{
		return 42; // return non-zero value to cancel transfer
	}

	return 0;
}

void CvHttpRequest::Start()
{
	if ( m_req.data_size > 0 )
	{
		LogMessage( enLogLevel_Debug2, "==> [%s] HTTP request [%s] data-size [%lld] data [%s]",
				m_req.url.c_str(), HttpMethodEnumToString(m_req.method).c_str(), m_req.data_size, m_req.data );
	}
	else
	{
		LogMessage( enLogLevel_Debug2, "==> [%s] HTTP request [%s] data-size [0]",
				m_req.url.c_str(), HttpMethodEnumToString(m_req.method).c_str() );
	}
	
	CURLcode result;
	FILE* fp = NULL;
	CURL* curlSession = curl_easy_init();

	if (curlSession)
	{
		curl_easy_setopt(curlSession, CURLOPT_NOSIGNAL, 1);
		
		// set up stuff for regular GET request
		curl_easy_setopt(curlSession, CURLOPT_URL, m_req.url.c_str());
		
		if ( !m_req.fname.empty() )
		{
			fp = fopen(m_req.fname.c_str(), "wb");
			m_response = "ok";
			curl_easy_setopt(curlSession, CURLOPT_WRITEFUNCTION, CvHttpRequest::WriteToFile);
			curl_easy_setopt(curlSession, CURLOPT_WRITEDATA, fp);
			curl_easy_setopt(curlSession, CURLOPT_RESUME_FROM_LARGE, m_req.resume_from);
		}
		else
		{
			curl_easy_setopt(curlSession, CURLOPT_WRITEFUNCTION, CvHttpRequest::ReadResponse);
			curl_easy_setopt(curlSession, CURLOPT_WRITEDATA, this);
		}
		
		curl_easy_setopt(curlSession, CURLOPT_PROXY, m_req.proxy.c_str());	// if empty - proxy disabled
		curl_easy_setopt(curlSession, CURLOPT_NOPROGRESS, m_req.no_progress);
		curl_easy_setopt(curlSession, CURLOPT_PROGRESSFUNCTION, &CvHttpRequest::SetProgress);
		curl_easy_setopt(curlSession, CURLOPT_PROGRESSDATA, this);

		struct CurlWriteBuffer curlBuf;
		curlBuf.data = m_req.data;
		curlBuf.sizeleft = m_req.data_size;
        curlBuf.request = this;
		
		// following if's set up additional stuff only used in POST PUT DEL and HEAD requests
		if (m_req.method == enHttpMethod_POST)
		{
			curl_easy_setopt(curlSession, CURLOPT_POSTFIELDSIZE, m_req.data_size);
			curl_easy_setopt(curlSession, CURLOPT_POSTFIELDS, m_req.data);
		}
		else if (m_req.method == enHttpMethod_PUT)
		{
			curl_easy_setopt(curlSession, CURLOPT_UPLOAD, 1L);
			curl_easy_setopt(curlSession, CURLOPT_READDATA, &curlBuf);
			curl_easy_setopt(curlSession, CURLOPT_READFUNCTION, CvHttpRequest::WriteData);
			curl_easy_setopt(curlSession, CURLOPT_INFILESIZE, m_req.data_size);
		}
		else if (m_req.method == enHttpMethod_DEL)
		{
			curl_easy_setopt(curlSession, CURLOPT_CUSTOMREQUEST, HttpMethodEnumToString(enHttpMethod_DEL).c_str() );
		}
		else if (m_req.method == enHttpMethod_HEAD)
		{
			curl_easy_setopt(curlSession, CURLOPT_NOBODY, 1);
		}
		
		curl_easy_setopt(curlSession, CURLOPT_HEADER, 1);
		curl_easy_setopt(curlSession, CURLOPT_HEADERFUNCTION, CvHttpRequest::HeaderCallback);
		curl_easy_setopt(curlSession, CURLOPT_HEADERDATA, this);

		char Error[1024] = {'\0'};
		
		curl_easy_setopt(curlSession, CURLOPT_HTTPHEADER, m_req.header_list);
		curl_easy_setopt(curlSession, CURLOPT_ERRORBUFFER, Error);
		curl_easy_setopt(curlSession, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curlSession, CURLOPT_SSL_VERIFYHOST, 0);

		if ( m_req.timeout != TIMEOUT_INFINITE )
			curl_easy_setopt(curlSession, CURLOPT_TIMEOUT, m_req.timeout);
		
		result = curl_easy_perform(curlSession);
		if (result != 0)
		{
			LogMessage( enLogLevel_Error, "CURL error: %s", Error );
			m_response = Error;
		}

		if (fp != NULL)
			fclose(fp);

		curl_easy_getinfo(curlSession, CURLINFO_RESPONSE_CODE, &m_responseCode);
		
		curl_slist_free_all(m_req.header_list);
		curl_easy_cleanup(curlSession);

		LogMessage( enLogLevel_Debug2, "<-- [%s] HTTP response code [%d] data [%s]", m_req.url.c_str(), m_responseCode, m_response.c_str() );

		if (result == 0)
		{
			if (m_responseCode > 202)
			{
				LogMessage( enLogLevel_Debug2, "<-- [%s] HTTP response: %s", m_req.url.c_str(), m_response.c_str() );
				throw runtime_error( m_response.c_str() );
			}
		}
		else
		{
			throw runtime_error( curl_easy_strerror(result) );
		}
	}
}

CvHttpRequest::enStatus_t CvHttpRequest::Execute( const Seconds& aTimeout, bool abProgress )
{
	m_req.no_progress = abProgress ? 0 : 1; // this tells curl to enable or disable progress
	m_req.timeout = aTimeout.Value();
	
	try
	{
		Start();
	}
	catch (exception &e)
	{
		LogMessage( enLogLevel_Debug2, "<-- HTTP Error: %s", e.what() );
		
		if (m_responseCode == 0)
			return enStatus_NetworkError;
		if (m_responseCode >= 400 && m_responseCode < 500)
			return enStatus_ClientError;
		if (m_responseCode >= 500)
			return enStatus_ServerError;
	}
	
	return enStatus_Ok;
}

bool CvHttpRequest::EncodeURL( const String& aUrl, OUT String& aEncodedUrl )
{
	bool bOk = false;
	
	CURL* pCurl = curl_easy_init();
	
	char* pEncodedUrl = curl_easy_escape( pCurl, aUrl.c_str(), (int)aUrl.length() );
	
	if ( pEncodedUrl != NULL )
	{
		aEncodedUrl = pEncodedUrl;
		curl_free( pEncodedUrl );
		bOk = true;
	}
	
	curl_easy_cleanup( pCurl );
	
	return bOk;
}

void CvHttpRequest::Clear()
{
	m_responseCode = 0;
	m_progressUp = 0;
	m_progressDown = 0;
	m_contentSize = 0;
	m_bResponseDataStarted = false;
	
	{
		CvMutexLock lock(m_mutex);		
		m_bCancel = false;
	}
	
	m_response.clear();
	m_responseHeaders.clear();
	m_req.Clear();
}

void CvHttpRequest::sRequestData_t::Clear()
{
	header_list = NULL;
	data = NULL;
	data_size = 0;
	method = enHttpMethod_Unknown;
	no_progress = true;
	resume_from = 0;
	timeout = CvHttpRequest::TIMEOUT_INFINITE;
			
	url.clear();
	fname.clear();
	proxy.clear();
}

CvHttpRequest::COpenSslMt::COpenSslMt()
{
	int i;

	m_lockArray = new CvMutex[ CRYPTO_num_locks() ];
	
	for ( int i = 0; i < CRYPTO_num_locks(); ++i )
	{
		m_lockArray[i].Create();
	}

	CRYPTO_set_id_callback( ThreadId );
	CRYPTO_set_locking_callback( LockCallback );	
}

CvHttpRequest::COpenSslMt::~COpenSslMt()
{
	CRYPTO_set_locking_callback(NULL);

	delete[] m_lockArray;
}

void CvHttpRequest::COpenSslMt::LockCallback( int mode, int type, const char*, int )
{
	if (mode & CRYPTO_LOCK)
	{
		m_lockArray[type].Lock();
	}
	else
	{
		m_lockArray[type].Unlock();
	}
}

unsigned long CvHttpRequest::COpenSslMt::ThreadId()
{
	return (unsigned long)pthread_self();
}

