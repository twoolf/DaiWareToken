/***************************************************************************************************************************************************************************************************************************
																																																						   *
This file is part of CertiVox M-Pin Client and Server Libraries.																																						   *
The CertiVox M-Pin Client and Server Libraries provide developers with an extensive and efficient set of strong authentication and cryptographic functions.																   *
For further information about its features and functionalities please refer to http://www.certivox.com																													   *
The CertiVox M-Pin Client and Server Libraries are free software: you can redistribute it and/or modify it under the terms of the BSD 3-Clause License http://opensource.org/licenses/BSD-3-Clause as stated below.		   *
The CertiVox M-Pin Client and Server Libraries are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
Note that CertiVox Ltd issues a patent grant for use of this software under specific terms and conditions, which you can find here: http://certivox.com/about-certivox/patents/											   * 	
Copyright (c) 2013, CertiVox UK Ltd																																														   *	
All rights reserved.																																																	   *
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:																			   *
�	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.																						   *	
�	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.			   *	
�	Neither the name of CertiVox UK Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.								   *
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,																		   *
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS																	   *
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE																	   *	
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,														   *
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.																		   *	
																																																						   *
***************************************************************************************************************************************************************************************************************************/
/*! \file  CvEccsi.cpp
    \brief C++ wrapper for the MIRACL ECCSI functionality

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 28, 2013, 1:59 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

 C++ wrapper for the MIRACK/SkyKeyXT ECCSI functionality.
 The class API is adapted to the existing SkyKey soltuion.

*/
#include "CvXcode.h"
#include "CvString.h"

#include "CvMiraclDefs.h"
#include "CvEccsi.h"

#include "big.h"

#include <memory>

using namespace CvShared;

/* global hash functions used here only*/
static Big hash1( const ECn& point, const ECn& public_key, const char* identity, int length, const ECn& pvt, const Big& order )
{
	int len, prefix = 0x04;
	Big x, y;
	char buffer[BN_BYTES + 1];
	char h[HASH_LEN];
	sha256 hash;

	shs256_init( &hash );

	point.get( x, y );
	len = to_binary( x, 2 * AES_SECURITY / 8, buffer, TRUE );

	shs256_process( &hash, prefix );
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}
	
	len = to_binary( y, 2 * AES_SECURITY / 8, buffer, TRUE );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}

	public_key.get( x, y );
	len = to_binary( x, 2 * AES_SECURITY / 8, buffer, TRUE );

	shs256_process( &hash, prefix );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}
	
	len = to_binary( y, 2 * AES_SECURITY / 8, buffer, TRUE );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}

	for ( int i = 0; i < length; i++ )
	{
		shs256_process( &hash, identity[i] );
	}

	pvt.get( x, y );
	len = to_binary( x, 2 * AES_SECURITY / 8, buffer, TRUE );
	shs256_process( &hash, prefix );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}
	
	len = to_binary( y, 2 * AES_SECURITY / 8, buffer, TRUE );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}

	shs256_hash( &hash, h );
	x = from_binary( HASH_LEN, h );
	
	return (x % order );
}

static Big hash2( const Big& hs, const Big& r, const char* message, int length, const Big& order )
{
	int len;
	Big x;
	char buffer[BN_BYTES + 1];
	char h[HASH_LEN];
	sha256 hash;

	shs256_init( &hash );

	len = to_binary( hs, 2 * AES_SECURITY / 8, buffer, TRUE );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}
	
	len = to_binary( r, 2 * AES_SECURITY / 8, buffer, TRUE );
	
	for ( int i = 0; i < len; i++ )
	{
		shs256_process( &hash, buffer[i] );
	}
	
	for ( int i = 0; i < length; i++ )
	{
		shs256_process( &hash, message[i] );
	}

	shs256_hash( &hash, h );
	x = from_binary( HASH_LEN, h );
	
	return (x % order );
}

CvEccsi::CMiracl::CMiracl( ecp_domain& aEccsiDomain ) //about to be deprecated
{
#ifdef MR_GENERIC_AND_STATIC
	m_pMip = mirsys( &m_instance, aEccsiDomain.nibbles, 16 );
#else
	m_pMip = mirsys( aEccsiDomain.nibbles, 16 );
#endif

	Big a = from_binary( FS, aEccsiDomain.A );
	Big b = from_binary( FS, aEccsiDomain.B );
	Big q = from_binary( FS, aEccsiDomain.Q );
	
    m_pMip->ERCON=TRUE;

	ecurve_init( _MIPP_ a.getbig(), b.getbig(), q.getbig(), MR_PROJECTIVE );
}

CvEccsi::CMiracl::~CMiracl()
{
	mirexit( _MIPPO_ );
}

miracl* CvEccsi::mriniteccsi()
{
	miracl* pMip = mirsys( 64, 16 );
	
	pMip->IOBASE = 16;
	
	return pMip;
}

CvEccsi::CvEccsi( csprng* apRng ) :
	m_pRng(apRng), m_lastError(ECCSI_OK)
{
	m_lastError = ECP_DOMAIN_INIT( &m_eccsiDomain, rom_ecp );
}

CvEccsi::~CvEccsi()
{
	ECP_DOMAIN_KILL( &m_eccsiDomain );	
}

bool CvEccsi::GenerateMasterKeyPair(OUT String& aPrivateKey, OUT String& aPublicKey)
{
	char arrPublicKey [2*FS+1],
             arrPrivateKey[2*FS+1];

	octet public_key =  {0, 2*FS+1, arrPublicKey}, 
              private_key = {0, 2*FS+1, arrPrivateKey};

	m_lastError = ECCSI_MASTER_KEY_PAIR_GENERATE(&m_eccsiDomain, m_pRng, &private_key, &public_key);

	CvBase64::Encode((uint8_t*)public_key.val,  public_key.len,  aPublicKey);
	CvBase64::Encode((uint8_t*)private_key.val, private_key.len, aPrivateKey);

	return (m_lastError == ECCSI_OK);
}

bool CvEccsi::GenerateMasterPublicWithExternalPrivate(IN const String& aPrivateKey, OUT String& aPublicKey)
{
	char arrPublicKey[2*FS+1];

	String decPrivateKey;

	octet public_key =  {0, 2*FS+1, arrPublicKey}; 

	CvBase64::Decode(aPrivateKey, decPrivateKey);

	std::unique_ptr<char[]> arrPrivateKey( new char[decPrivateKey.size()] );

	octet octPVK = {0, (int)decPrivateKey.size(), arrPrivateKey.get()};

	OCTET_JOIN_BYTES(decPrivateKey.data(), octPVK.max, &octPVK);

	m_lastError = ECCSI_MASTER_KEY_PAIR_GENERATE(&m_eccsiDomain, NULL, &octPVK, &public_key);
	
	CvBase64::Encode((uint8_t*)public_key.val,  public_key.len,  aPublicKey);
	
	return (m_lastError == ECCSI_OK);
}

bool CvEccsi::GenerateUserKeyPair(IN const String& aUserId, IN const String& aPrivateKeyKSAK, IN const String& aPublicKeyKPAK, OUT String& SSK, OUT String& PVT)
{
	String decTrg;
	char chV[FS], // size = ?  chV receives id unique entropy 
		ch_kpak[2*FS+1], 
		ch_ssk[FS], 
		ch_pvt[2*FS+1];

	octet V = {0, 2*FS+1, chV}, 
		ssk = {0, FS, ch_ssk}, 
		pvt = {0, 2*FS, ch_pvt};


	CvBase64::Decode(aPrivateKeyKSAK, decTrg);
	std::unique_ptr<char[]> ch_ksak( new char[decTrg.size( )] );
	octet ksak = {0, (int)decTrg.size(), ch_ksak.get()};
	OCTET_JOIN_BYTES(decTrg.data(), ksak.max, &ksak);

	CvBase64::Decode(aPublicKeyKPAK, decTrg);
	octet kpak = {0, (int)decTrg.size(), ch_kpak};
	OCTET_JOIN_BYTES(decTrg.data(), kpak.max, &kpak);

	octet Id = {0, (int)aUserId.length(), const_cast<char*>(aUserId.c_str())};

	m_lastError = ECCSI_USER_KEY_PAIR_GENERATE(&m_eccsiDomain, m_pRng, &V, &Id, &ksak, &kpak, &ssk, &pvt);

	CvBase64::Encode((uint8_t*)ssk.val, ssk.len, SSK);
	CvBase64::Encode((uint8_t*)pvt.val, pvt.len, PVT);

	return (m_lastError == ECCSI_OK);
}

bool CvEccsi::ValidateSecret( const String& aIdentity, const String& aPublicKey, const String& aSecret, const String& aPrivateKey )
{
	char KPAK[2*FS+1] = {0};
	char SSK[FS] = {0};
	char PVT[2*FS+1] = {0};
	char HS[FS] = {0};
	
	octet octetID = { (int)aIdentity.length(), (int)aIdentity.length(), (char*)aIdentity.c_str() };
	octet octetKPAK = { 0, sizeof(KPAK), KPAK };
	octet octetHS = { 0, sizeof(HS), HS };
	octet octetSSK = { 0,sizeof(SSK), SSK };
	octet octetPVT = { 0, sizeof(PVT), PVT };
	
	if ( !DecodePublicKey( aPublicKey, octetKPAK ) )
		return false;
	if ( !DecodePrivateKey( aPrivateKey, octetPVT ) )
		return false;
	if ( !DecodeSecret( aSecret, octetSSK ) )
		return false;
	
	HashHS( octetKPAK, aIdentity, octetPVT, octetHS );

	m_lastError = ECCSI_USER_KEY_PAIR_VALIDATE( &m_eccsiDomain, &octetID, &octetKPAK, &octetHS, &octetSSK, &octetPVT );

	return m_lastError == ECCSI_OK;
}

//static void printBuffer( const char* apPrefix, const char* apBuffer, int aLength )
//{
//	printf( "%s [", apPrefix );
//	for ( int i = 0; i < aLength; ++i )
//	{
//		if ( i+1 < aLength )
//			printf( "%hhu,", apBuffer[i] );
//		else
//			printf( "%hhu", apBuffer[i] );			
//	}
//	printf( "]\n" );
//}

/*! \brief Sign a message */
bool CvEccsi::Sign( const char* apMessage, int aMsgLen, const String& aIdentity, const String& aPublicKey,
					const String& aSecret, const String& aPrivateKey, OUT String& aSignature )
{
	if ( m_pRng == NULL )
		return false;
	
	char KPAK[2*FS+1] = {0};
	char SSK[FS] = {0};
	char PVT[2*FS+1] = {0};
	char HS[FS] = {0};
	char SIGNATURE[4*FS+1] = {0};
	char J[FS];
	
	octet octetM = { aMsgLen, aMsgLen, (char*)apMessage };
	octet octetID = { (int)aIdentity.length(), (int)aIdentity.length(), (char*)aIdentity.c_str() };
	octet octetKPAK = { 0, sizeof(KPAK), KPAK };
	octet octetHS = { 0, sizeof(HS), HS };
	octet octetSSK = { 0,sizeof(SSK), SSK };
	octet octetPVT = { 0, sizeof(PVT), PVT };
	octet octetSIGNATURE = { 0, sizeof(SIGNATURE), SIGNATURE };
	/* Random non-zero value \f$ j \in  F_q \f$ */
	octet octetJ = { 0, sizeof(J), J };
	
	if ( !DecodePublicKey( aPublicKey, octetKPAK ) )
		return false;
	if ( !DecodePrivateKey( aPrivateKey, octetPVT ) )
		return false;
	if ( !DecodeSecret( aSecret, octetSSK ) )
		return false;
	
	HashHS( octetKPAK, aIdentity, octetPVT, octetHS );

//	printBuffer( "Signing message:\n", octetM.val, octetM.len );
	
	while ( octetSIGNATURE.len == 0 )
	{
		/* By passing &RNG to ECCSI_SIGN a random value of j is used */
		m_lastError = ECCSI_SIGN( &m_eccsiDomain, m_pRng, &octetJ, &octetM, &octetID, &octetKPAK,
								&octetHS, &octetSSK, &octetPVT, &octetSIGNATURE );
	
		if ( m_lastError != ECCSI_OK )
			return false;
	}
	
	CvBase64::Encode( (uint8_t*)octetSIGNATURE.val, octetSIGNATURE.len, aSignature );
	
	return true;
}

/*! \brief Verify the signature */
bool CvEccsi::Verify( const char* apMessage, int aMsgLen, const String& aIdentity, const String& aPublicKey, const String& aSignature )
{
	String decodedSignature;
	CvBase64::Decode( aSignature, decodedSignature );
	
	char KPAK[2*FS+1] = {0};
	
	octet octetM = { aMsgLen, aMsgLen, (char*)apMessage };
	octet octetID = { (int)aIdentity.length(), (int)aIdentity.length(), (char*)aIdentity.c_str() };
	octet octetKPAK = { 0, sizeof(KPAK), KPAK };
	octet octetSIGNATURE = { (int)decodedSignature.size(), (int)decodedSignature.size(), (char*)decodedSignature.data() };
	
	if ( !DecodePublicKey( aPublicKey, octetKPAK ) )
		return false;
	
//	printBuffer( "Verifying message:\n", octetM.val, octetM.len );
	
	m_lastError = ECCSI_VERIFY( &m_eccsiDomain, &octetM, &octetID, &octetKPAK, &octetSIGNATURE );

	return m_lastError == ECCSI_OK;
}

bool CvEccsi::DecodePrivateKey( const String& aPrivateKey, OUT octet& aPVT )
{
	OCTET_CLEAR(&aPVT);
	
	String decodedPrivateKey;
	CvBase64::Decode( aPrivateKey, decodedPrivateKey );
	
	if ( decodedPrivateKey.size() != aPVT.max ) // incorrect, no need here, could be > 0
		return false;
		
	OCTET_JOIN_BYTES( decodedPrivateKey.data(), aPVT.max, &aPVT );
		
	return true;
}

bool CvEccsi::DecodeSecret( const String& aSecret, OUT octet& aSSK )
{
	OCTET_CLEAR(&aSSK);
	
	String decodedSecret;
	CvBase64::Decode( aSecret, decodedSecret );
	
	if ( decodedSecret.size() != aSSK.max )
		return false;
	
	OCTET_JOIN_BYTES( decodedSecret.data(), aSSK.max, &aSSK );
	
	return true;	
}

bool CvEccsi::DecodePublicKey( const String& aPublicKey, OUT octet& aKPAK )
{
	OCTET_CLEAR(&aKPAK);
	
	if ( aPublicKey[0] != '[' || aPublicKey[aPublicKey.length()-1] != ']' ) //why ???
		return false;
	
	CvString publicKey = aPublicKey;
	
	publicKey.TrimLeft( "[" );
	publicKey.TrimRight( "]" );	
	
	vector<CvString> tokens;
	publicKey.Tokenize( ",", tokens );
	
	if ( tokens.size() != 2 )
		return false;
	
	OCTET_JOIN_BYTE( 4, 1, &aKPAK );
	
	for ( vector<CvString>::const_iterator itr = tokens.begin();
		 itr != tokens.end();
		 ++itr )
	{
		String decoded;

		CvBase64::Decode( *itr, decoded );
		if ( decoded.size() != FS )
			return false;
		OCTET_JOIN_BYTES( decoded.data(), FS, &aKPAK );
	}
	
	return true;
}

bool CvEccsi::HashHS( const octet& aKPAK, const String& aIdentity, const octet& aPVT, octet& aHS )
{
	CMiracl miracl( m_eccsiDomain ); //D2: need Miracl obj here due to use of Big numbers
	
	OCTET_CLEAR(&aHS);
	
	int prefix = 0x04;
	HASHFUNC hash;

	SHS_INIT( &hash );

	SHS_PROCESS( &hash, prefix );
	
	for ( int i = 0; i < sizeof(m_eccsiDomain.Gx); ++i )
	{
		SHS_PROCESS( &hash, m_eccsiDomain.Gx[i] );
	}
	
	for ( int i = 0; i < sizeof(m_eccsiDomain.Gy); ++i )
	{
		SHS_PROCESS( &hash, m_eccsiDomain.Gy[i] );
	}

	for ( int i = 0; i < aKPAK.len; ++i )
	{
		SHS_PROCESS( &hash, aKPAK.val[i] );
	}
	
	for ( size_t i = 0; i < aIdentity.length(); ++i )
	{
		SHS_PROCESS( &hash, aIdentity[i] );
	}

	for ( int i = 0; i < aPVT.len; ++i )
	{
		SHS_PROCESS( &hash, aPVT.val[i] );
	}

	char h[HASH_BYTES] = {0};
	
	SHS_HASH( &hash, h );
	
	Big x = from_binary( sizeof(h), h );
	Big order = from_binary( sizeof(m_eccsiDomain.R), m_eccsiDomain.R ); // R = order eccsi parameter

	x = x % order;
	
	aHS.len = to_binary( x, aHS.max, aHS.val );
	
	return true;
}

void CvEccsi::EncodePvt( const ECn& aPvt, OUT String& aEncodedPvt )
{
	Big x, y;	
	aPvt.get( x, y );
	
	int prefix = 0x4;
	char tmp[2 * BN_BYTES + 1] = {'\0'};
	tmp[0] = prefix;
	
	int len = 1;	
	
	len += to_binary( x, 2 * AES_SECURITY / 8, tmp + len, TRUE );
	len += to_binary( y, 2 * AES_SECURITY / 8, tmp + len /*1 + 2 * AES_SECURITY / 8*/, TRUE );
	
	CvBase64::Encode( (const uint8_t*)tmp, len, aEncodedPvt );
}

bool CvEccsi::DecodePvt( const String& aEncodedPvt, OUT ECn& aPvt )
{
	String str;
	CvBase64::Decode( aEncodedPvt, str );
	
	char* pBuffer = (char*)str.c_str();	

	Big x = from_binary( 2 * AES_SECURITY / 8, pBuffer + 1 );
	Big y = from_binary( 2 * AES_SECURITY / 8, pBuffer + 2 * AES_SECURITY / 8 + 1 );
	
	return aPvt.set( x, y );
}

void CvEccsi::GetMasterKey( OUT Big& aMasterKey )
{
	aMasterKey = strong_rand( m_pRng, GetOrder() );
}

void CvEccsi::GetPublicKey( const Big& aMasterKey, OUT ECn& aPublicKey )
{
	aPublicKey = aMasterKey * GetGenerator();
}

void CvEccsi::GetPrivateKey( const Big& aRand, OUT ECn& aPvt )
{
	/* Compute PVT = [v]G in affine coordinates. */
	aPvt = aRand * GetGenerator();
}

void CvEccsi::GetPrivateKey( const Big& aRand, OUT String& aPrivateKey )
{
	ECn pvt;
	GetPrivateKey( aRand, pvt );
	
	/* Export PVT. */
	EncodePvt( pvt, aPrivateKey );
}

void CvEccsi::GetSecret( const Big& aMasterKey, const Big& aRand, const char* aIdentity, int aLength, const ECn& aPublicKey, const ECn& aPvt, OUT Big& aSecret )
{
	/* Compute HS = hash( G || KPAK || ID || PVT ), an N-octet integer. */
	Big hs = hash1( GetGenerator(), aPublicKey, aIdentity, aLength, aPvt, GetOrder() );

	/* Compute SSK = ( KSAK + HS * v ) modulo q. */
	aSecret = ( aMasterKey + modmult( hs, aRand, GetOrder() ) ) % GetOrder();
}

void CvEccsi::GetSecret( const Big& aMasterKey, const Big& aRand, const char* aIdentity, int aLength, const ECn& aPublicKey, const String& aPrivateKey, OUT Big& aSecret )
{
	ECn pvt;
	DecodePvt( aPrivateKey, pvt );
	
	GetSecret( aRand, aMasterKey, aIdentity, aLength, aPublicKey, pvt, aSecret );
}
