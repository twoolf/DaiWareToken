/* 
 * File:   CvStrongRng.cpp
 * Author: mony
 * 
 * Created on November 9, 2012, 4:44 PM
 */

#include <sys/types.h>

#include "CvStrongRng.h"
#include "CvEntropyServer.h"
#include "CvTime.h"
#include "CvMiraclDefs.h"

//old interface left for compatibility

namespace CvShared
{

bool CvStrongRng::m_bEnableEntropy = true;
String CvStrongRng::m_aEntropyServerUrl = "";
String CvStrongRng::m_aEntropyAlgorithm = "";

CvStrongRng::CvStrongRng(CSRNG_TYPE type)
{
   const int size = ( AES_SECURITY/sizeof(mr_small) );
   String random;
   char seed[size];

#ifdef WIN32

	if ( m_bEnableEntropy )
    {
        CvEntropyServer( m_aEntropyServerUrl ).Generate(	CvEntropyServer::StringToAlgorithm( m_aEntropyAlgorithm ),
											CvEntropyServer::enEncoding_Raw,
											size, random );
		memcpy((void*)seed, (void*)random.data(), size);
	}

#elif defined __linux__

    FILE* fdRandom;
    if ( m_bEnableEntropy )
    {
        CvEntropyServer( m_aEntropyServerUrl ).Generate(	CvEntropyServer::StringToAlgorithm( m_aEntropyAlgorithm ),
											CvEntropyServer::enEncoding_Raw,
											size, random );

		fdRandom = fmemopen( (void*)random.data(), size, "r" );

    }
    else
    {
        fdRandom = fopen( "/dev/urandom", "r" );
    }

	if ( fdRandom == NULL )
	{
		mr_berror( _MIPP_ MR_ERR_DEV_RANDOM );
	}
	
	for ( int i = 0; i < size; ++i )
	{
		int c = fgetc( fdRandom );
		seed[i] = c;
		if ( c == -1 )
		{
			mr_berror( _MIPP_ MR_ERR_DEV_RANDOM );
		}
	}

	if ( fdRandom != NULL )
	{
		fclose( fdRandom );
	}

#endif
	
	time_t tod;	
	time( &tod );
	
	strong_init( &m_csprng, size, seed, tod ); 
}
void CvStrongRng::Init( bool abEnableEntropy, const String& aEntropyServerUrl, const String& aEntropyAlgorithm )
{
    m_bEnableEntropy = abEnableEntropy;
    m_aEntropyServerUrl = aEntropyServerUrl;
    m_aEntropyAlgorithm = aEntropyAlgorithm;
}

//new interface
CvStrongRng::CvStrongRng(CSRNG_MODE mode) : ISystemSource(new SystemCSPRNG()), IDongleSource(new CDongleSource())
{
   const int size = ( AES_SECURITY/sizeof(mr_small) );
   char seed[size];

   time_t tod;	
   time( &tod );
	
   strong_init( &m_csprng, size, seed, tod ); 
}

CvStrongRng::~CvStrongRng(){}

csprng& CvStrongRng::CDongleSource::dongle_slurp(bool abEnableEntropy, const String& aEntropyServerUrl, const String& aEntropyAlgorithm)
{
    const int size = ( AES_SECURITY/sizeof(mr_small) );
	String random;
	char seed[size];
	
#ifdef WIN32
	
	if ( m_bEnableEntropy )
    {
        CvEntropyServer( m_aEntropyServerUrl ).Generate(	CvEntropyServer::StringToAlgorithm( m_aEntropyAlgorithm ),
											CvEntropyServer::enEncoding_Raw,
											size, random );
		
		memcpy((void*)seed, (void*)random.data(), size);
	}

#elif defined __linux__

	FILE* fdRandom;
    if ( abEnableEntropy )
    {
        CvEntropyServer( aEntropyServerUrl ).Generate(	CvEntropyServer::StringToAlgorithm( aEntropyAlgorithm ),
											CvEntropyServer::enEncoding_Raw,
											size, random );
        fdRandom = fmemopen( (void*)random.data(), size, "r" );
    }
    else
    {
        fdRandom = fopen( "/dev/urandom", "r" );
    }

	if ( fdRandom == NULL )
	{
		mr_berror( _MIPP_ MR_ERR_DEV_RANDOM );
	}

	
	
	for ( int i = 0; i < size; ++i )
	{
		int c = fgetc( fdRandom );
		seed[i] = c;
		if ( c == -1 )
		{
			mr_berror( _MIPP_ MR_ERR_DEV_RANDOM );
		}
	}

	if ( fdRandom != NULL )
	{
		fclose( fdRandom );
	}

#endif

	time_t tod;	
	time( &tod );
	
	strong_init( &m_csprng, size, seed, tod );
        
    return m_csprng;
}
}