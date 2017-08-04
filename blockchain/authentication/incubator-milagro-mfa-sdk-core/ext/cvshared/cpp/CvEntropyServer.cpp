/* 
 * File:   CvEntropyServer.cpp
 * Author: mony
 * 
 * Created on November 9, 2012, 2:35 PM
 */

#include "CvEntropyServer.h"

#include "CvHttpRequest.h"
#include "CvLogger.h"
#include "CvString.h"
#include "miracl.h"
#include "CvMiraclDefs.h"

#define ALGORITHM_STRING_UNSAFE			"unsafe"
#define ALGORITHM_STRING_ALF			"alf"
#define ALGORITHM_STRING_MT19937		"mt19937"
#define ALGORITHM_STRING_XORSHIFT128    "xorshift128"
#define ALGORITHM_STRING_UNKNOWN		"unknown"

#define ENCODING_STRING_RAW				"raw"
#define ENCODING_STRING_BASE64			"base64"
#define ENCODING_STRING_UNKNOWN			"unknown"

using namespace CvShared;

CvEntropyServer::CvEntropyServer( const String& aUrl ) :
	m_url(aUrl)
{
}

CvEntropyServer::~CvEntropyServer( )
{
}

bool CvEntropyServer::Generate( enAlgorithm_t aAlgorithm, enEncoding_t aEncoding, int aLength, OUT String& aEntropy )
{
	CvString url;
	
	url.Format( "%s/%s/%s/%d", m_url.c_str(), AlgorithmToString( aAlgorithm ), EncodingToString( aEncoding ), aLength );
	
	LogMessage( enLogLevel_Debug1, "Generating entropy from [%s]", url.c_str() );

	CvHttpRequest httpRequest( enHttpMethod_GET );
	
	httpRequest.SetUrl( url );
	
	if ( httpRequest.Execute( Seconds(10) ) != CvHttpRequest::enStatus_Ok )
	{
        LogMessage( enLogLevel_Debug1, "Failed to obtain any entropy" );
		return false;
	}
	
	aEntropy.resize( httpRequest.GetResponse().size() );
	memcpy( (void*)aEntropy.data(), httpRequest.GetResponse().data(), httpRequest.GetResponse().size() );
	
	LogMessage( enLogLevel_Debug1, "Successfully obtained [%d] bytes of entropy", aEntropy.size() );

	return true;
}

const char* CvEntropyServer::AlgorithmToString( CvEntropyServer::enAlgorithm_t aAlgorithm )
{
	switch( aAlgorithm )
	{
		case enAlgorithm_Unsafe: return ALGORITHM_STRING_UNSAFE;
		case enAlgorithm_Alf: return ALGORITHM_STRING_ALF;
		case enAlgorithm_Mt19937: return ALGORITHM_STRING_MT19937;
		case enAlgorithm_XorShift128: return ALGORITHM_STRING_XORSHIFT128;
	}
	
	return ALGORITHM_STRING_UNKNOWN;
}

const char* CvEntropyServer::EncodingToString( CvEntropyServer::enEncoding_t aEncoding )
{
	switch( aEncoding )
	{
		case enEncoding_Raw: return ENCODING_STRING_RAW;
		case enEncoding_Base64: return ENCODING_STRING_BASE64;
	}
	
	return ENCODING_STRING_UNKNOWN;
}

CvEntropyServer::enAlgorithm_t CvEntropyServer::StringToAlgorithm( const String& aAlgorithm )
{
	if ( aAlgorithm == ALGORITHM_STRING_UNSAFE )
		return enAlgorithm_Unsafe;
	if ( aAlgorithm == ALGORITHM_STRING_ALF )
		return enAlgorithm_Alf;
	if ( aAlgorithm == ALGORITHM_STRING_MT19937 )
		return enAlgorithm_Mt19937;
	if ( aAlgorithm == ALGORITHM_STRING_XORSHIFT128 )
		return enAlgorithm_XorShift128;
	
	return enAlgorithm_Unknown;
}

CvEntropyServer::enEncoding_t CvEntropyServer::StringToEncoding( const String& aEncoding )
{
	if ( aEncoding == ENCODING_STRING_RAW )
		return enEncoding_Raw;
	if ( aEncoding == ENCODING_STRING_BASE64 )
		return enEncoding_Base64;
	
	return enEncoding_Unknown;
}


SystemCSPRNG::SystemCSPRNG()
{
}

SystemCSPRNG::~SystemCSPRNG()
{
    strong_kill( &m_csprng );
}

csprng& SystemCSPRNG::Csprng()
{
	const int size = ( AES_SECURITY / sizeof (mr_small ) );
	unsigned char seed[size] = {0};

	//use system entropy
	rndPool( seed, (size_t)size );

	time_t tod;
	time( &tod );

	strong_init( &m_csprng, size, (char*)seed, tod );

	return m_csprng;
}

void SystemCSPRNG::rndPool(CvBytePtr prpool, size_t req_len)
{
#if defined (_WIN32)

	HCRYPTPROV hCryptProv = 0;

	SetSearchPathMode( BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE );
	LPCWSTR KEY_CONTAINER = L"CERTIVOXWINCSRNG";

	if ( !CryptAcquireContext( &hCryptProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) )
	{
		HRESULT HR = HRESULT_FROM_WIN32( GetLastError( ) );
		return;
	}

	if ( hCryptProv && !CryptGenRandom( hCryptProv, req_len, (BYTE*)prpool ) )
	{
		HRESULT HR = HRESULT_FROM_WIN32( GetLastError( ) );
		return;
	}

	if ( hCryptProv && !CryptReleaseContext( hCryptProv, 0 ) )
	{
		HRESULT HR = HRESULT_FROM_WIN32( GetLastError( ) );
		return;
	}
 
#elif defined(__linux__) || defined (__MACH__)
 
 #ifdef __linux__
  #define DEV_RANDOM "/dev/urandom"
 #else
  #define DEV_RANDOM "/dev/random"
 #endif
 
 FILE* fd = fopen( DEV_RANDOM, "r" );
 
 if ( fd != NULL )
 {
  size_t rsz = fread( prpool, 1, req_len, fd );
  fclose( fd );
 }
#endif
}
