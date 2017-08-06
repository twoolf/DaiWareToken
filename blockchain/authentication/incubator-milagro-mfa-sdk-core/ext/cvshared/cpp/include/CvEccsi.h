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
/*! \file  CvEccsi.h
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

#ifndef CVECCSI_H
#define	CVECCSI_H

extern "C"
{
	#include "eccsi_c.h"
}

#include "CvCommon.h"

#include "big.h"
#include "ecn.h"

#include <string>

class CvEccsi
{
public:
	typedef std::string	String;
	
	CvEccsi( csprng* apRng = NULL );
	virtual ~CvEccsi();
	
	static miracl* mriniteccsi(); 
	
        /*! \brief ECCSI with miracl primitives interface */
        
        /*! \brief Generate ECCSI master key */
        void	GetMasterKey( OUT Big& aMasterKey );
        
        /*! \brief Generate ECCSI public key */
	void	GetPublicKey( const Big& aMasterKey, OUT ECn& aPublicKey );
	
        /*! \brief Generate ECCSI private key as ECn class */
	void	GetPrivateKey( const Big& aRand, OUT ECn& aPvt );
	
        /*! \brief Generate ECCSI private key as b64 string */
        void	GetPrivateKey( const Big& aRand, OUT String& aPrivateKey );
	
        /*! \brief Generate ECCSI id secret using ECn formated private key */
	void	GetSecret( const Big& aMasterKey, const Big& aRand, const char* aIdentity, int aLength, const ECn& aPublicKey, const ECn& aPvt, OUT Big& aSecret );	
	
        /*! \brief Generate ECCSI id secret using b64 formated private key */
        void	GetSecret( const Big& aMasterKey, const Big& aRand, const char* aIdentity, int aLength, const ECn& aPublicKey, const String& aPrivateKey, OUT Big& aSecret );
	
        /*! \brief Transform ECCSI private key from ECn to b64 string */
        void	EncodePvt( const ECn& aPvt, OUT String& aEncodedPvt );
        
        /*! \brief Transform b64 encoded ECCSI private key to ECn class */
	bool	DecodePvt( const string& aEncodedPvt, OUT ECn& aPvt );
        
        /*! \brief Get Big number value from the ECP domain */
        const Big&	GetOrder()      { return m_pStaticParams->m_order; }
        
        /*! \brief Get ECn value coordinates on the current curve */
	const ECn&	GetGenerator()	{ return m_pStaticParams->m_generator; }
        
        /*! End of ECCSI with miracl primitives interface. */
        
	/*! \brief Generate prublic/private EC GF(p) key pair */
	bool GenerateMasterKeyPair(OUT String& aPrivateKey, OUT String& aPublicKey);

	/*! \brief Generate prublic key with externally provided private key */
	bool GenerateMasterPublicWithExternalPrivate(IN const String& aPrivateKey, OUT String& aPublicKey);

	/*! \brief Generate user private/public key pair (SSK,PVT) */
	bool GenerateUserKeyPair(IN const String& aUserId, IN const String& aPrivateKeyKSAK, IN const String& aPublicKeyKPAK, OUT String& SSK, OUT String& PVT);

	/*! \brief Validate the Secret Signing Key (SSK) */
	bool	ValidateSecret( const String& aIdentity, const String& aPublicKey, const String& aSecret, const String& aPrivateKey );

	/*! \brief Sign a message */
	bool	Sign( const char* apMessage, int aMsgLen, const String& aIdentity, const String& aPublicKey,
				  const String& aSecret, const String& aPrivateKey, OUT String& aSignature );

	/*! \brief Verify the signature */
	bool	Verify( const char* apMessage, int aMsgLen, const String& aIdentity, const String& aPublicKey, const String& aSignature );
	
	int	GetLastError() const	{ return m_lastError; }
        
    //    void InitEccsiParams(); deprecated
	
private:
	CvEccsi(const CvEccsi& orig)	{}
	
	class CMiracl //about to be deprecated
	{
	public:
		CMiracl( IN ecp_domain& aEccsiDomain );
		~CMiracl();
	private:
#ifdef MR_GENERIC_AND_STATIC
		miracl	m_instance;
#endif
		miracl*	m_pMip;
	};
	
	bool	DecodePrivateKey( const String& aPrivateKey, OUT octet& aPVT );
	bool	DecodeSecret( const String& aSecret, OUT octet& aSSK );
	bool	DecodePublicKey( const String& aPublicKey, OUT octet& aKPAK );	
	bool	HashHS( const octet& aKPAK, const String& aIdentity, const octet& aPVT, octet& aHS );

	ecp_domain		m_eccsiDomain;
	int			m_lastError;
	
	csprng*			m_pRng;	// Random Number Generator	
        
        struct sStaticParams_t //about to be deprecated
	{
		Big	m_p;
		int	m_A;
		Big	m_B;
		Big	m_order; // <=> R
		ECn	m_generator;
	};
	
	sStaticParams_t* m_pStaticParams;
};

#endif	/* CVECCSI_H */

