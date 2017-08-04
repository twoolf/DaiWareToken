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
/*! \file  CvSakke.h
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

#ifndef CVSAKKE_H
#define	CVSAKKE_H

extern "C"
{
	#include "sakke_bn_c.h"
}

#include "CvCommon.h"
//#include "../../DPKG/dpkg-server/DpkgConfig.h"

#include "big.h"
#include "zzn2.h"

#include <string>

//#define __LEGACY_IMPLEMENTATION__

#ifdef __LEGACY_IMPLEMENTATION__
	#define MR_PAIRING_BN

	#define AES_SECURITY	AS*8
	#define BN_BYTES		(((2 * AS) + 2 - ((2 * AS + 2) % 3)) / 3 * 4)
	#define KEY_BYTES		((AS + 2 - ((AS + 2) % 3)) / 3 * 4)
	#define G1_BYTES		(2 * BN_BYTES + 2 + 1)
	#define G2_BYTES		(4 * BN_BYTES + 2 + 3)
	#define ECN_BYTES		(2 * BN_BYTES + 2 + 1)

	#include "pfc.h"
#endif

class CvSakke
{
public:
	typedef std::string	String;
	
	CvSakke( csprng* apRng = NULL );
	virtual ~CvSakke();
	
	static miracl* mrsakdominit();
	
	void	GetMasterKey( OUT String& aMasterKey );
	void	GetPublicParams( OUT String& aPublicParams );
	
	void	GetPublicKey( const String& aMasterKey, OUT String& aPublicKey, const String& aPublicParams = "" );
	void	GetPrivateKey( const String& aMasterKey, const String& aIdentity, OUT String& aPrivateKey, const String& aPublicParams = "" );

	/*! \brief Encapsulate the Shared Secret Value (SSV) */
	bool	Encapsulate( const String& aPlainData, const String& aPublicKey, const String& aIdentity,
						OUT String& aEncapsulatedData, const String& aPublicParams = "" );

	/*! \brief Decapsulate the Shared Secret Value (SSV) */
	bool	Decapsulate( const String& aEncapsulatedData, const String& aPublicKey, const String& aIdentity,
						const String& aPrivateKey, OUT String& aPlainData, const String& aPublicParams = "" );

	/*! \brief Validate the Receiver Secret Key (RSK) */
	bool	ValidatePrivateKey( const String& aPublicKey, const String& aIdentity, const String& aPrivateKey, const String& aPublicParams = "" );
	
	Big	HashIdentity( const String& aString );
	
	int	GetLastError() const	{ return m_lastError; }	

	void	DumpDomain();
        
        sak_domain& GetSakkeDomain(); 
//      static CDpkgConfig* GetSakkeParams();
	
private:
	CvSakke(const CvSakke& orig)	{}
	
	class CMiracl
	{
	public:
		CMiracl( IN sak_domain& aSakkeDomain );
		~CMiracl();
	private:
#ifdef MR_GENERIC_AND_STATIC
		miracl	m_instance;
#endif
		miracl*	m_pMip;
	};
	
	void	HashIdentity( const String& aString, OUT octet& aID );
	bool	DecodePublicKey( const String& aPublicKey, OUT octet& aZ_H );
	bool	DecodePrivateKey( const String& aPrivateKey, OUT octet& aKbS );
	void	EncodeEncapsulatedData( const octet& aRbS, const octet& aH, OUT String& aEncapsulatedData );
	bool	DecodeEncapsulatedData( const String& aEncapsulatedData, OUT octet& aRbS, OUT octet& aH );	
	bool	DecodePublicParams( const String& aPublicParams, OUT octet& aPx, OUT octet& aPy );

#ifdef __LEGACY_IMPLEMENTATION__
	String	to_string( const Big& number );
	String	to_string( const ECn& point );
	String	to_string( const G1& point );
	String	to_string( const G2& point );
	
	Big		big_from_string( const String& str );
	ECn		ecn_from_string( const String& str );
	G1		g1_from_string( const String& str );
	G2		g2_from_string( const String& str );
	
	void	reverse( char* str, int len );
#endif
	
	sak_domain	m_sakkeDomain; 
        
	int		m_lastError;
	
	csprng*		m_pRng;
	
#ifdef __LEGACY_IMPLEMENTATION__
	PFC*		m_pPfc;
#endif
};

#endif	/* CVSAKKE_H */

