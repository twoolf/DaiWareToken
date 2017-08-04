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
/*! \file  CvSakke.cpp
    \brief C++ wrapper for the MIRACL SAKKE functionality

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 24, 2013, 4:59 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

 C++ wrapper for the MIRACK/SkyKeyXT SAKKE functionality.
 The class API is adapted to the existing SkyKey soltuion.

*/

#include "CvSakke.h"

#include "CvXcode.h"
#include "CvString.h"
#include "big.h"
#include "zzn2.h"

#ifdef __LEGACY_IMPLEMENTATION__
	#include "pfc.h"
#endif

using namespace CvShared;

CvSakke::CMiracl::CMiracl( sak_domain& aSakkeDomain )   //about to be deprecated
{
#ifdef MR_GENERIC_AND_STATIC
	m_pMip = mirsys( &m_instance, aSakkeDomain.nibbles, 16 );
#else
	m_pMip = mirsys( aSakkeDomain.nibbles, 16 );
#endif

	Big a = from_binary( FS, aSakkeDomain.A );
	Big b = from_binary( FS, aSakkeDomain.B );
	Big q = from_binary( FS, aSakkeDomain.Q );
	
        m_pMip->ERCON=TRUE;
	m_pMip->TWIST = MR_SEXTIC_D;
	if ( ( aSakkeDomain.flags & 1 ) == 1 )
		m_pMip->TWIST = MR_SEXTIC_M;

	ecurve_init( _MIPP_ a.getbig(), b.getbig(), q.getbig(), MR_PROJECTIVE );
}

CvSakke::CMiracl::~CMiracl()
{
	mirexit( _MIPPO_ );
}
	
CvSakke::CvSakke(csprng* apRng) :
	m_lastError(SAKKE_OK),
	m_pRng(apRng)
{
	m_lastError = SAKKE_DOMAIN_INIT( &m_sakkeDomain, rom_sakke );
	
	CMiracl miracl( m_sakkeDomain );
	
	//Init P
	//Init Q
	//
	//
	
#ifdef __LEGACY_IMPLEMENTATION__
	m_pPfc = new PFC(AES_SECURITY,apRng);
#endif	
}

CvSakke::~CvSakke()
{
#ifdef __LEGACY_IMPLEMENTATION__
	{
		CMiracl miracl( m_sakkeDomain );		
		delete m_pPfc;
	}
#endif
	
	SAKKE_DOMAIN_KILL( &m_sakkeDomain );
}

miracl* CvSakke::mrsakdominit() //re-init miracl
{
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,2*FS,16);
#else
    miracl *mr_mip=mirsys(2*FS,16);
#endif
	
	mr_mip->IOBASE = 16;
	mr_mip->TWIST = MR_SEXTIC_D;	
	
	return mr_mip;
}

void CvSakke::GetMasterKey( OUT String& aMasterKey )
{
#ifdef __LEGACY_IMPLEMENTATION__
	CMiracl miracl( m_sakkeDomain );
	
	Big s;
	m_pPfc->random(s);
	aMasterKey = to_string(s);
#endif
}

void CvSakke::GetPublicParams( OUT String& aPublicParams )
{
#ifdef __LEGACY_IMPLEMENTATION__
	CMiracl miracl( m_sakkeDomain );
	
	G1 p;
	G2 q;

	m_pPfc->random(p);
	m_pPfc->random(q);

	aPublicParams = to_string(p) + "#" + to_string(q);
#endif
}

void CvSakke::GetPublicKey( const String& aMasterKey, OUT String& aPublicKey, const String& aPublicParams )
{
#ifdef __LEGACY_IMPLEMENTATION__
	CMiracl miracl( m_sakkeDomain );
	
	size_t pos = aPublicParams.find("#");
	
	if ( pos == String::npos )
		return;
	
	G1 point = g1_from_string( aPublicParams.substr(0, pos) );
	Big master = big_from_string( aMasterKey );
	point = m_pPfc->mult( point, master );

	aPublicKey = to_string(point);
#endif
}

void CvSakke::GetPrivateKey( const String& aMasterKey, const String& aIdentity, OUT String& aPrivateKey, const String& aPublicParams )
{
#ifdef __LEGACY_IMPLEMENTATION__
	CMiracl miracl( m_sakkeDomain );
	
	size_t pos = aPublicParams.find("#");

	Big master = big_from_string( aMasterKey );
	Big h = HashIdentity( aIdentity );

	h = ( h + master ) % m_pPfc->order();
	
	h = inverse( h, m_pPfc->order() );
	
	if (h < 0)
	{
		h += m_pPfc->order();
	}

	G2 point = g2_from_string( aPublicParams.substr(pos + 1) );
	point = m_pPfc->mult( point, h );

	aPrivateKey	= to_string(point);
#endif	
}

bool CvSakke::Encapsulate( const String& aPlainData, const String& aPublicKey, const String& aIdentity,
							String& aEncapsulatedData, const String& aPublicParams )
{
#ifdef __LEGACY_IMPLEMENTATION__

	CMiracl miracl( m_sakkeDomain );
	
	size_t pos = aPublicParams.find("#");

	if ( pos == String::npos )
		return false;
	
	G1 Z_S = g1_from_string( aPublicKey );
	G1 P = g1_from_string( aPublicParams.substr(0, pos) );
	G2 point2 = g2_from_string( aPublicParams.substr(pos + 1) );
	GT g = m_pPfc->pairing( point2, P );

	char SSV[AS] = {0};
	Big k = big_from_string( aPlainData );	// SSV
	to_binary( k, sizeof(SSV), SSV, TRUE );

	char hash[HASH_BYTES] = {0};
	
	Big h = HashIdentity( aIdentity );
	int len = to_binary( h, sizeof(hash), hash, FALSE );

	char buf[AS+HASH_BYTES] = {0};
	
	// SSV || b
	memcpy( buf, SSV, sizeof(SSV) );
	memcpy( buf + sizeof(SSV), hash, len );

	// r = HashToIntegerRange( SSV || b, q, Hash )
	Big r = m_pPfc->hash_to_group( buf, sizeof(SSV) + len );

	// RbS = [r]([b]P + Z_S)
	G1 RbS = m_pPfc->mult( ( m_pPfc->mult( P, h ) + Z_S ), r );

	// HashToIntegerRange( g^r, 2^n, Hash )
	h = m_pPfc->hash_to_aes_key( m_pPfc->power( g, r ) );
	to_binary( h, AS, (char*)buf, TRUE );
	reverse( buf, AS );

	// H := SSV XOR HashToIntegerRange( g^r, 2^n, Hash )
	unsigned char H[AS] = {0};	
	for ( int i = 0; i < AS; ++i )
	{
		H[i] = SSV[i] ^ buf[i];
	}

	Big x, y;
	RbS.g.get( x, y );

	len = 0;
	char encodedRbS[2*FS+1] = {0};
	encodedRbS[len++] = 0x04;
	len += to_binary( x, FS, encodedRbS + len, TRUE );
	len += to_binary( y, FS, encodedRbS + len, TRUE );
	String str;
	CvBase64::Encode( (uint8_t*)encodedRbS, sizeof(encodedRbS), str );

	CvBase64::Encode( (uint8_t*)H, sizeof(H), aEncapsulatedData );
	aEncapsulatedData += ",";
	aEncapsulatedData += str;

	return true;
	
#else
	
	char SSV[AS] = {0};
	char Z_S[2*FS+1] = {0};
	char ID[FS] = {0};		
	char RbS[2*FS+1] = {0};
	char H[AS] = {0};
	
	octet octetSSV = { 0, sizeof(SSV), SSV };
	octet octetZ_S = { 0, sizeof(Z_S), Z_S };
	octet octetID = { 0, sizeof(ID), ID };
	octet octetRbS = { 0, sizeof(RbS), RbS };
	octet octetH = { 0, sizeof(H), H };

	OCTET_JOIN_BYTES( aPlainData.data(), aPlainData.size(), &octetSSV );
	
	if ( !DecodePublicKey( aPublicKey, octetZ_S ) )
		return false;
	
	HashIdentity( aIdentity, octetID );
	
	m_lastError = SAKKE_KEY_ENCAPSULATE( &m_sakkeDomain, &octetSSV, &octetZ_S, &octetID, &octetRbS, &octetH );

	EncodeEncapsulatedData( octetRbS, octetH, aEncapsulatedData );
	
	return m_lastError == SAKKE_OK;
	
#endif
}

bool CvSakke::Decapsulate( const String& aEncapsulatedData, const String& aPublicKey,
							const String& aIdentity, const String& aPrivateKey, String& aPlainData,
							const String& aPublicParams )
{
#ifdef __LEGACY_IMPLEMENTATION__

	CMiracl miracl( m_sakkeDomain );
	
	size_t pos = aPublicParams.find("#");

	if ( pos == String::npos )
		return false;
	
	G1 Z_S = g1_from_string( aPublicKey );
	G1 P = g1_from_string( aPublicParams.substr(0, pos) );
	G2 point2 = g2_from_string( aPublicParams.substr(pos+1) );

	G2 KbS = g2_from_string( aPrivateKey );	// KbS

	pos = aEncapsulatedData.find(",");
	
	if ( pos == String::npos )
		return false;
	
	String decodedHint;
	CvBase64::Decode( aEncapsulatedData.substr(0, pos), decodedHint );
	
	/* Remove the prefix and recover the point. */
	String decodedRbS;
	CvBase64::Decode( aEncapsulatedData.substr(pos+1), decodedRbS );

	Big x = from_binary( FS, (char*)decodedRbS.data() + 1 );
	Big y = from_binary( FS, (char*)decodedRbS.data() + 1 + FS );	
	
	G1 RbS;
	RbS.g.set( x, y );	// RbS

	GT w = m_pPfc->pairing( KbS, RbS );	// w := < RbS, KbS >
	
	Big h = m_pPfc->hash_to_aes_key(w);	// HashToIntegerRange( w, 2^n, Hash )
	
	char buf[AS+HASH_BYTES] = {0};
	to_binary( h, AS, buf, TRUE );
	reverse( buf, AS );

	// SSV = H XOR HashToIntegerRange( w, 2^n, Hash )
	unsigned char SSV[AS] = {0};	
	for ( int i = 0; i < sizeof(SSV); ++i )
	{
		SSV[i] = decodedHint[i] ^ buf[i];
	}

	h = HashIdentity( aIdentity );
	
	char hash[HASH_BYTES] = {0};
	int len = to_binary( h, sizeof(hash), hash, FALSE );

	memcpy( buf, SSV, sizeof(SSV) );
	memcpy( buf + sizeof(SSV), hash, len );	// SSV || b

	// r = HashToIntegerRange( SSV || b, q, Hash )
	Big r = m_pPfc->hash_to_group( buf, sizeof(SSV) + len );

	// TEST = [r]([b]P + Z_S)
	// if ( TEST != RbS )
	if ( RbS != m_pPfc->mult( m_pPfc->mult(P, h) + Z_S, r ) )
	{
		aPlainData.clear();
		return false;
	}

	CvBase64::Encode( SSV, sizeof(SSV), aPlainData );
	return true;

#else
	
	char SSV[AS] = {0};
	char Z_S[2*FS+1] = {0};
	char ID[FS] = {0};		
	char RbS[2*FS+1] = {0};
	char H[AS] = {0};
	char KbS[4*FS] = {0};
	
	octet octetRbS = { 0, sizeof(RbS), RbS };
	octet octetH = { 0, sizeof(H), H };	
	octet octetZ_S = { 0, sizeof(Z_S), Z_S };
	octet octetID = { 0, sizeof(ID), ID };
	octet octetKbS = { 0, sizeof(KbS), KbS };
	octet octetSSV = { 0, sizeof(SSV), SSV };
	
	if ( !DecodeEncapsulatedData( aEncapsulatedData, octetRbS, octetH ) )
		return false;
	if ( !DecodePublicKey( aPublicKey, octetZ_S ) )
		return false;
	if ( !DecodePrivateKey( aPrivateKey, octetKbS ) )
		return false;

	HashIdentity( aIdentity, octetID );	
	
	m_lastError = SAKKE_KEY_DECAPSULATE( &m_sakkeDomain, &octetRbS, &octetH, &octetZ_S, &octetID, &octetKbS, &octetSSV );

	if ( m_lastError != SAKKE_OK )
		return false;
	
	aPlainData.resize( octetSSV.len );
	memcpy( (char*)aPlainData.data(), octetSSV.val, octetSSV.len );
	
	return true;
	
#endif
}

bool CvSakke::ValidatePrivateKey( const String& aPublicKey, const String& aIdentity, const String& aPrivateKey, const String& aPublicParams )
{
#ifdef __LEGACY_IMPLEMENTATION__
	
	CMiracl miracl( m_sakkeDomain );
	
	size_t pos = aPublicParams.find("#");

	G1 point1 = g1_from_string( aPublicParams.substr(0, pos) );
	G2 point2 = g2_from_string( aPublicParams.substr(pos + 1) );
	GT g = m_pPfc->pairing( point2, point1 );

	G1 Z_S = g1_from_string( aPublicKey );
	G2 KbS = g2_from_string( aPrivateKey );

	Big h = HashIdentity( aIdentity );

	G1 p = m_pPfc->mult( point1, h ) + Z_S;

	Big x, y;	
	p.g.get( x, y );

	GT e = m_pPfc->pairing( KbS, p );
	
	return ( e == g );
	
#else
	
	char Z_S[2*FS+1] = {0};
	char ID[FS] = {0};		
	char KbS[4*FS] = {0};
	
	octet octetZ_S = { 0, sizeof(Z_S), Z_S };
	octet octetID = { 0, sizeof(ID), ID };
	octet octetKbS = { 0, sizeof(KbS), KbS };

	if ( !DecodePublicKey( aPublicKey, octetZ_S ) )
		return false;
	if ( !DecodePrivateKey( aPrivateKey, octetKbS ) )
		return false;

	HashIdentity( aIdentity, octetID );	
	
	m_lastError = SAKKE_SECRET_KEY_VALIDATE( &m_sakkeDomain, &octetID, &octetZ_S, &octetKbS );

	return m_lastError == SAKKE_OK;
	
#endif
}

// Hash a zero-terminated String to a number < modulus
Big CvSakke::HashIdentity( const String& aString )
{
#ifndef __LEGACY_IMPLEMENTATION__
	CMiracl miracl( CvSakke::m_sakkeDomain );
#endif
	
    unsigned char s[HASH_BYTES];
    int i;
    HASHFUNC sh;

    SHS_INIT(&sh);

    for( i = 0; i < (int)aString.length(); ++i )
    {
        SHS_PROCESS( &sh, aString[i] );
    }
	
    SHS_HASH( &sh, (char*)s );
	
	const Big p = get_modulus();
	
    Big h = 1;
	int j = 0;
	i = 1;
	
    forever
    {
        h *= 256;
        if ( j == HASH_BYTES )
		{
			h += i++;
			j=0;
		}
        else
			h+=s[j++];
		
        if ( h >= p )
			break;
    }
	
    h %= p;
	
	Big order = from_binary( sizeof(m_sakkeDomain.R), m_sakkeDomain.R );

	return h % order;
}

void CvSakke::HashIdentity( const String& aString, octet& aID )
{
	Big hash = HashIdentity( aString );
	
#ifndef __LEGACY_IMPLEMENTATION__	
	CMiracl miracl( m_sakkeDomain );
#endif
	
	OCTET_CLEAR( &aID );
	aID.len = to_binary( hash, aID.max, aID.val, TRUE );
}

void DumpBuffer( char* apBuf, int aLength )
{
	for ( int i = 0; i < aLength; ++i )
	{
		printf( "%02X ", (uint8_t)apBuf[i] );
	}
}

void CvSakke::DumpDomain()
{
	printf( "Beta = " );
	DumpBuffer( m_sakkeDomain.Beta, sizeof(m_sakkeDomain.Beta) );
	printf( "\n" );
	
	printf( "R = " );
	DumpBuffer( m_sakkeDomain.R, sizeof(m_sakkeDomain.R) );
	printf( "\n" );
}


sak_domain& CvSakke::GetSakkeDomain() //temporary
{   return m_sakkeDomain; 
}

/*
CDpkgConfig* CvSakke::GetSakkeParams() //temporary
{ return CDpkgConfig::Instance(); 
}
*/
bool CvSakke::DecodePublicKey( const String& aPublicKey, OUT octet& aZ_S )
{
	OCTET_CLEAR(&aZ_S);
	
	if ( aPublicKey[0] != '[' || aPublicKey[aPublicKey.length()-1] != ']' )
		return false;
	
	CvString publicKey = aPublicKey;
	
	publicKey.TrimLeft( "[" );
	publicKey.TrimRight( "]" );	
	
	vector<CvString> tokens;
	publicKey.Tokenize( ",", tokens );
	
	if ( tokens.size() != 2 )
		return false;
	
	OCTET_JOIN_BYTE( 4, 1, &aZ_S );
	
	for ( vector<CvString>::const_iterator itr = tokens.begin();
		 itr != tokens.end();
		 ++itr )
	{
		String decoded;

		CvBase64::Decode( *itr, decoded );
		if ( decoded.size() != FS )
			return false;
		OCTET_JOIN_BYTES( decoded.data(), FS, &aZ_S );
	}
	
	return true;
}

bool CvSakke::DecodePrivateKey( const String& aPrivateKey, OUT octet& aKbS )
{
	CMiracl miracl( m_sakkeDomain );
	
	OCTET_CLEAR(&aKbS);
	
	if ( aPrivateKey[0] != '[' || aPrivateKey[aPrivateKey.length()-1] != ']' )
		return false;
	
	CvString privateKey = aPrivateKey;
	
	privateKey.TrimLeft( "[" );
	privateKey.TrimRight( "]" );	
	
	vector<CvString> tokens;
	privateKey.Tokenize( ",", tokens );
	
	if ( tokens.size() != 4 )
		return false;
	
	Big xx, xy, yx, yy;
	Big* pBigs[] = { &xx, &xy, &yx, &yy };
	
	int i = 0;
	for ( vector<CvString>::const_iterator itr = tokens.begin();
		 itr != tokens.end();
		 ++itr, ++i )
	{
		String decoded;
		CvBase64::Decode( *itr, decoded );
		bytes_to_big( _MIPP_ (int)decoded.size(), decoded.data(), pBigs[i]->getbig() );		
	}
	
	ZZn2 x( xx, xy );
	ZZn2 y( yx, yy );
	
	aKbS.len = 4*FS;
	big_to_bytes( _MIPP_ FS, x.getzzn2()->a, &aKbS.val[0], TRUE );
	big_to_bytes( _MIPP_ FS, x.getzzn2()->b, &aKbS.val[FS], TRUE);
	big_to_bytes( _MIPP_ FS, y.getzzn2()->a, &aKbS.val[2*FS], TRUE);
	big_to_bytes( _MIPP_ FS, y.getzzn2()->b, &aKbS.val[3*FS], TRUE);

	return true;
}

void CvSakke::EncodeEncapsulatedData( const octet& aRbS, const octet& aH, OUT String& aEncapsulatedData )
{
	String encodedRbS;
	CvBase64::Encode( (uint8_t*)aRbS.val, aRbS.len, encodedRbS );
	
	String encodedHint;
	CvBase64::Encode( (uint8_t*)aH.val, aH.len, encodedHint );
	
	aEncapsulatedData = encodedHint + ',' + encodedRbS;
}

bool CvSakke::DecodeEncapsulatedData( const String& aEncapsulatedData, OUT octet& aRbS, OUT octet& aH )
{
	OCTET_CLEAR(&aRbS);
	OCTET_CLEAR(&aH);
	
	CvString encapsulatedData = aEncapsulatedData;
	
	vector<CvString> tokens;
	encapsulatedData.Tokenize( ",", tokens );
	
	if ( tokens.size() != 2 )
		return false;
	
	String decoded;

	CvBase64::Decode( tokens[0], decoded );
	if ( decoded.size() != aH.max )
		return false;
	OCTET_JOIN_BYTES( decoded.data(), aH.max, &aH );

	decoded.clear();
	CvBase64::Decode( tokens[1], decoded );
	if ( decoded.size() != aRbS.max )
		return false;
	OCTET_JOIN_BYTES( decoded.data(), aRbS.max, &aRbS );

	return true;
}

bool CvSakke::DecodePublicParams( const String& aPublicParams, OUT octet& aPx, OUT octet& aPy )
{
	CvString publicParams = aPublicParams;
	
	vector<CvString> tokens;
	publicParams.Tokenize( "#", tokens );
	
	if ( tokens.size() != 2 )
		return false;
	
	tokens[0].TrimLeft("[");
	tokens[0].TrimRight("]");
	
	vector<CvString> tokensP;
	tokens[0].Tokenize( ",", tokensP );
	
	if ( tokensP.size() != 2 )
		return false;
	
	String decoded;
	CvBase64::Decode( tokensP[0], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aPx );

	decoded.clear();
	CvBase64::Decode( tokensP[1], decoded );
	OCTET_JOIN_BYTES( decoded.data(), (int)decoded.size(), &aPy );
	
	return true;
}

#ifdef __LEGACY_IMPLEMENTATION__

String CvSakke::to_string( const Big& number )
{
	unsigned char buffer[BN_BYTES] = { 0 };
	int len;
	String str;

	len = to_binary( number, 2*AS, (char *)buffer, FALSE );
	CvBase64::Encode( buffer, len, str );

	return str;
}

Big CvSakke::big_from_string( const String& str )
{
	String decoded;
	CvBase64::Decode( str, decoded );
	
	return from_binary( (int)decoded.size(), (char*)decoded.data() );
}

String CvSakke::to_string( const ECn& point )
{
	Big x, y;
	point.get(x, y);
	
	return "[" + to_string(x) + "," + to_string(y) + "]";
}

ECn CvSakke::ecn_from_string( const String& str )
{
	ECn result;
	
	size_t pos = str.find(",");

	if ( pos == String::npos )
		return result;
	
	Big x, y;
	
	String decoded;
	CvBase64::Decode( str.substr( 1, pos-1 ), decoded );
	x = from_binary( (int)decoded.size(), (char*)decoded.data() );
	
	decoded.clear();
	CvBase64::Decode( str.substr( pos+1, BN_BYTES ), decoded );	
	y = from_binary( (int)decoded.size(), (char*)decoded.data() );
	
	result.set( x, y );
	
	return result;
}

String CvSakke::to_string( const G1& point )
{
	Big x, y;
	point.g.get(x, y);
	
	return "[" + to_string(x) + "," + to_string(y) + "]";
}

G1 CvSakke::g1_from_string( const String& str )
{
	G1 result;
	size_t pos = str.find(",");

	if ( pos == String::npos )
		return result;

	Big x, y;
	
	String decoded;
	CvBase64::Decode( str.substr( 1, pos-1 ), decoded );
	x = from_binary( (int)decoded.size(), (char*)decoded.data() );
	
	decoded.clear();
	CvBase64::Decode( str.substr( pos+1, BN_BYTES ), decoded );	
	y = from_binary( (int)decoded.size(), (char*)decoded.data() );
	
	result.g.set(x, y);

	return result;
}

String CvSakke::to_string( const G2& point )
{
	ZZn2 x, y;
	point.g.get(x, y);
	
	Big xx[2], yy[2];	
	x.get(xx[0], xx[1]);
	y.get(yy[0], yy[1]);

	return "[" + to_string(xx[0]) + "," + to_string(xx[1]) + "," + to_string(yy[0]) + "," + to_string(yy[1]) + "]";
}

G2 CvSakke::g2_from_string( const String& str )
{
	G2 result;
	
	size_t pos = str.find(",");
	if ( pos == String::npos )
		return result;
	
	Big xx[2], yy[2];
	size_t posStart = 1;
	
	String decoded;
	CvBase64::Decode( str.substr( posStart, pos-1 ), decoded );
	xx[0] = from_binary( (int)decoded.size(), (char*)decoded.data() );
	
	posStart = pos+1;
	pos = str.find( ",", posStart );
	if ( pos == String::npos )
		return result;
	
	decoded.clear();
	CvBase64::Decode( str.substr( posStart, BN_BYTES ), decoded );	
	xx[1] = from_binary( (int)decoded.size(), (char*)decoded.data() );

	posStart = pos+1;
	pos = str.find( ",", posStart );
	if ( pos == String::npos )
		return result;
	
	decoded.clear();
	CvBase64::Decode( str.substr( posStart, BN_BYTES ), decoded );	
	yy[0] = from_binary( (int)decoded.size(), (char*)decoded.data() );

	posStart = pos+1;
	
	decoded.clear();
	CvBase64::Decode( str.substr( posStart, BN_BYTES ), decoded );	
	yy[1] = from_binary( (int)decoded.size(), (char*)decoded.data() );
	
	ZZn2 x( xx[0], xx[1] );
	ZZn2 y( yy[0], yy[1] );
	
	result.g.set( x, y );
	
	return result;
}

void CvSakke::reverse( char* str, int len )
{
	char tmp;
	for ( int i = 0; i < len/2; ++i )
	{
		tmp = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = tmp;
	}
}

#endif	// __LEGACY_IMPLEMENTATION__