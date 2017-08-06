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
/*! \file  CvEcdh.cpp
    \brief C++ wrapper for the MIRACL ECDH functionality

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 28, 2013, 5:28 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

 C++ wrapper for the MIRACK/SkyKeyXT ECDH functionality.
 The class API is adapted to the existing SkyKey soltuion.

*/

#include "CvEcdh.h"

#include "CvXcode.h"
#include "CvString.h"

#include "octet_c.h"

using namespace CvShared;

CvEcdh::CvEcdh( csprng* apRng, const Salt_t aSalt ) :
	m_pRng( apRng ), m_lastError(ECDH_OK)
{
	m_octetSALT.len = sizeof(Salt_t)/sizeof(int);
	m_octetSALT.max = sizeof(SALT);
	m_octetSALT.val = SALT;
	
	if ( aSalt != NULL )
	{
		for ( int i = 0; i < m_octetSALT.len; i++ )
			m_octetSALT.val[i] = aSalt[i];
	}
	else
	if ( m_pRng != NULL )
	{
		// SALT needs to be provided from external source
		for ( int i = 0; i < m_octetSALT.len; i++ )
			m_octetSALT.val[i] = strong_rng( m_pRng );
	}
	else
	{
		// Default SALT
		for ( int i = 0; i < m_octetSALT.len; i++ )
			m_octetSALT.val[i] = i+1;
	}
	
	m_lastError = ECP_DOMAIN_INIT( &m_ecdhDomain, rom_ecp );
}

CvEcdh::~CvEcdh()
{
	ECP_DOMAIN_KILL( &m_ecdhDomain );
}

bool CvEcdh::GenerateKeyPair( const String& aPassword )
	{
	char PW[256] = {0};
	char S[FS] = {0};
	char W[2*FS+1] = {0};
	
	octet octetPW = { 0, sizeof(PW), PW };
	octet octetS = { 0, sizeof(S), S };
	octet octetW = { 0, sizeof(W), W };		

	OCTET_JOIN_STRING( aPassword.c_str(), &octetPW );

	// private key S of size FS bytes derived from Password and Salt
	ECDH_PBKDF2( &octetPW, &m_octetSALT, 1000, FS, &octetS );

	// Generate Key pair S/W
	m_lastError = ECP_KEY_PAIR_GENERATE( &m_ecdhDomain, NULL, &octetS, &octetW );
	if ( m_lastError != ECDH_OK )
		return false;
	
	m_lastError = ECP_PUBLIC_KEY_VALIDATE( &m_ecdhDomain, TRUE, &octetW );
	if ( m_lastError != ECDH_OK )
		return false;

	CvBase64::Encode( (uint8_t*)octetS.val, octetS.len, m_privateKey );
	CvBase64::Encode( (uint8_t*)octetW.val, octetW.len, m_publicKey );

	return true;
}

String CvEcdh::DeriveKey( const String& aPassword )
{
	char PW[256] = {0};
	char S[FS] = {0};
	
	octet octetPW = { 0, sizeof(PW), PW };
	octet octetS = { 0, sizeof(S), S };

	OCTET_JOIN_STRING( aPassword.c_str(), &octetPW );

	/* private key S of size FS bytes derived from Password and Salt */

	ECDH_PBKDF2( &octetPW, &m_octetSALT, 1000, FS, &octetS );
	
	String privateKey;
	CvBase64::Encode( (uint8_t*)octetS.val, octetS.len, privateKey );	

	return privateKey;
}

bool CvEcdh::AuthenticateExternal( const String& aExternalPublicKey, const String& aCommonKey )
{
	char S[FS] = {0};
	char W[2*FS+1] = {0};
	char Z0[FS] = {0};
	char Z1[FS] = {0};
	
	octet octetS = { 0, sizeof(S), S };
	octet octetW = { 0, sizeof(W), W };
	octet octetZ0 = { 0, sizeof(Z0), Z0 };
	octet octetZ1 = { 0, sizeof(Z1), Z1 };	

	//GF2 private key
	String decodedPrivateKey;
	CvBase64::Decode( GetPrivateKey(), decodedPrivateKey );
	OCTET_JOIN_BYTES( decodedPrivateKey.data(), (int)decodedPrivateKey.size(), &octetS );
	
	//dGF2 public key
	String decodedExternalPublicKey;
	CvBase64::Decode( aExternalPublicKey, decodedExternalPublicKey );
	OCTET_JOIN_BYTES( decodedExternalPublicKey.data(), (int)decodedExternalPublicKey.size(), &octetW );	

	// Calculate common key using DH - IEEE 1363 method
	m_lastError = ECPSVDP_DH( &m_ecdhDomain, &octetS, &octetW, &octetZ0 );
	
	if ( m_lastError != ECDH_OK )
		return false;
	
	String decodedCommonKey;
	CvBase64::Decode( aCommonKey, decodedCommonKey );
	OCTET_JOIN_BYTES( decodedCommonKey.data(), (int)decodedCommonKey.size(), &octetZ1 );

	return ( OCTET_COMPARE( &octetZ0, &octetZ1 ) == TRUE );
}

bool CvEcdh::ComputeCommonKey( const String& aExternalPublicKey, OUT String& aCommonKey )
{
	char S[FS] = {0};
	char W[2*FS+1] = {0};
	char Z[FS] = {0};
	
	octet octetS = { 0, sizeof(S), S };
	octet octetW = { 0, sizeof(W), W };
	octet octetZ = { 0, sizeof(Z), Z };
	
	//GF2 private key
	String decodedPrivateKey;
	CvBase64::Decode( GetPrivateKey(), decodedPrivateKey );
	OCTET_JOIN_BYTES( decodedPrivateKey.data(), (int)decodedPrivateKey.size(), &octetS );

	//dGF2 public key
	String decodedExternalPublicKey;
	CvBase64::Decode( aExternalPublicKey, decodedExternalPublicKey );
	OCTET_JOIN_BYTES( decodedExternalPublicKey.data(), (int)decodedExternalPublicKey.size(), &octetW );	
	
	// Calculate common key using DH - IEEE 1363 method
	m_lastError = ECPSVDP_DH( &m_ecdhDomain, &octetS, &octetW, &octetZ );

	if ( m_lastError != ECDH_OK )
		return false;
	
	CvBase64::Encode( (uint8_t*)octetZ.val, octetZ.len, aCommonKey );

	return true;
}

bool CvEcdh::EciesEncrypt( const String& aKey, const String& aMessage, OUT String& aCipher )
{
	if ( m_pRng == NULL )
		return false;

	char P1[] = { 0x0, 0x1, 0x2 };
	char P2[] = { 0x0, 0x1, 0x2, 0x3 };	
	char W[2*FS+1] = {0};
	char M[32] = {0};
	char V[2*FS+1] = {0};
	char C[64] = {0};
	char T[32] = {0};
			
	octet octetP1 = { sizeof(P1), sizeof(P1), P1 };
	octet octetP2 = { sizeof(P2), sizeof(P2), P2 };	
	octet octetW = { 0, sizeof(W), W };
	octet octetM = { 0, sizeof(M), M };	
	octet octetV = { 0, sizeof(V), V };	
	octet octetC = { 0, sizeof(C), C };
	octet octetT = { 0, sizeof(T), T };	
	
	//key: base64 to octet
	String decodedKey;
	CvBase64::Decode( aKey, decodedKey );
	OCTET_JOIN_BYTES( decodedKey.data(), (int)decodedKey.size(), &octetW );
	
	String decodedMessage;
	CvBase64::Decode( aMessage, decodedMessage );
	OCTET_JOIN_BYTES( decodedMessage.data(), (int)decodedMessage.size(), &octetM );

	ECP_ECIES_ENCRYPT( &m_ecdhDomain, &octetP1, &octetP2, m_pRng, &octetW, &octetM,
					12, &octetV, &octetC, &octetT );

	EncodeCipher( octetV, octetC, octetT, aCipher );
	
	return true;
}

bool CvEcdh::EciesDecrypt( const String& aKey, const String& aCipher, OUT String& aPlain )
{
	char P1[] = { 0x0, 0x1, 0x2 };
	char P2[] = { 0x0, 0x1, 0x2, 0x3 };	
	char S[FS] = {0};
	char M[32] = {0};
	char V[2*FS+1] = {0};
	char C[64] = {0};
	char T[32] = {0};
	
	octet octetP1 = { sizeof(P1), sizeof(P1), P1 };
	octet octetP2 = { sizeof(P2), sizeof(P2), P2 };	
	octet octetS = { 0, sizeof(S), S };
	octet octetM = { 0, sizeof(M), M };	
	octet octetV = { 0, sizeof(V), V };	
	octet octetC = { 0, sizeof(C), C };
	octet octetT = { 0, sizeof(T), T };	
	
	//key: base64 to octet
	String decodedKey;
	CvBase64::Decode( aKey, decodedKey );
	OCTET_JOIN_BYTES( decodedKey.data(), (int)decodedKey.size(), &octetS );

	//decompose V,C,T to octet
	if ( !DecodeCipher( aCipher, octetV, octetC, octetT ) )
		return false; //bad signature

	if ( !ECP_ECIES_DECRYPT( &m_ecdhDomain, &octetP1, &octetP2, &octetV,
							&octetC, &octetT, &octetS, &octetM ) )
	{
		return false;
	}
	
	CvBase64::Encode( (uint8_t*)octetM.val, octetM.len, aPlain );

	return true;
}

bool CvEcdh::EcdsaSign( const String& aKey, const String& aData, OUT String& aSignature )
{
	if ( m_pRng == NULL )
		return false;

	char S[FS] = {0};
	char M[32] = {0};
	char CS[FS] = {0};
	char DS[FS] = {0};
	
	octet octetS = { 0, sizeof(S), S };
	octet octetM = { 0, sizeof(M), M };	
	octet octetCS = { 0, sizeof(CS), CS };
	octet octetDS = { 0, sizeof(DS), DS };
	
	String decodedKey;
	CvBase64::Decode( aKey, decodedKey );
	OCTET_JOIN_BYTES( decodedKey.data(), (int)decodedKey.size(), &octetS );
	
	String decodedData;
	CvBase64::Decode( aData, decodedData );
	OCTET_JOIN_BYTES( decodedData.data(), (int)decodedData.size(), &octetM );
	
	m_lastError = ECPSP_DSA( &m_ecdhDomain, m_pRng, &octetS, &octetM, &octetCS, &octetDS );
	
	if ( m_lastError != ECDH_OK )
	{
		return false;
	}
	
	EncodeSignature( octetCS, octetDS, aSignature );

	return true;
}

bool CvEcdh::EcdsaVerify( const String& aKey, const String& aData, const String& aSignature )
{
	char W[2*FS+1] = {0};
	char M[32] = {0};
	char CS[FS] = {0};
	char DS[FS] = {0};
	
	octet octetW = { 0, sizeof(W), W };
	octet octetM = { 0, sizeof(M), M };	
	octet octetCS = { 0, sizeof(CS), CS };
	octet octetDS = { 0, sizeof(DS), DS };

	String decodedKey;
	CvBase64::Decode( aKey, decodedKey );
	OCTET_JOIN_BYTES( decodedKey.data(), (int)decodedKey.size(), &octetW );
	
	String decodedData;
	CvBase64::Decode( aData, decodedData );
	OCTET_JOIN_BYTES( decodedData.data(), (int)decodedData.size(), &octetM );
	
	if ( !DecodeSignature( aSignature, octetCS, octetDS ) )
		return false; //bad signature
	
	m_lastError = ECPVP_DSA( &m_ecdhDomain, &octetW, &octetM, &octetCS, &octetDS );
	
	return m_lastError == ECDH_OK;
}

void CvEcdh::EncodeCipher( const octet& aV, const octet& aC, const octet& aT, OUT String& aCipher )
{
	String b64;
	CvBase64::Encode( (uint8_t*)aV.val, aV.len, b64 );
	aCipher = b64;
	aCipher += ',';

	b64.clear();
	CvBase64::Encode( (uint8_t*)aC.val, aC.len, b64 );
	aCipher += b64;
	aCipher += ',';
	
	b64.clear();
	CvBase64::Encode( (uint8_t*)aT.val, aT.len, b64 );
	aCipher += b64;
}

bool CvEcdh::DecodeCipher( const String& aCipher, OUT octet& aV, OUT octet& aC, OUT octet& aT )
{
	CvString cipher(aCipher);
	
	OCTET_CLEAR( &aV );
	OCTET_CLEAR( &aC );
	OCTET_CLEAR( &aT );	
	
	vector<CvString> tokens;
	cipher.Tokenize( ",", tokens );
	
	if ( tokens.size() != 3 )
		return false;
	
	String decoded;
	CvBase64::Decode( tokens[0], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aV );
	
	decoded.clear();
	CvBase64::Decode( tokens[1], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aC );

	decoded.clear();
	CvBase64::Decode( tokens[2], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aT );

	return true;
}

void CvEcdh::EncodeSignature( const octet& aCS, const octet& aDS, OUT String& aSignature )
{
	String b64;
	CvBase64::Encode( (uint8_t*)aCS.val, aCS.len, b64 );
	aSignature = b64;
	aSignature += ',';

	b64.clear();
	CvBase64::Encode( (uint8_t*)aDS.val, aDS.len, b64 );
	aSignature += b64;
}

bool CvEcdh::DecodeSignature( const String& aSignature, OUT octet& aCS, OUT octet& aDS )
{
	CvString signature(aSignature);
	
	OCTET_CLEAR( &aCS );
	OCTET_CLEAR( &aDS );
	
	vector<CvString> tokens;
	signature.Tokenize( ",", tokens );
	
	if ( tokens.size() != 2 )
		return false;
	
	String decoded;
	CvBase64::Decode( tokens[0], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aCS );
	
	decoded.clear();
	CvBase64::Decode( tokens[1], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aDS );

	return true;
}
