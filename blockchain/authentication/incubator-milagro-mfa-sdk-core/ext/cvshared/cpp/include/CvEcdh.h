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
/*! \file  CvEcdh.h
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

#ifndef CVECDH_H_INCLUDED
#define CVECDH_H_INCLUDED

#include "CvCommon.h"

#include "ecdh_c.h"

#include <string>

class CvEcdh
{
public:
	typedef std::string	String;	
	typedef int			Salt_t[8];
	
	CvEcdh( csprng* apRng = NULL, const Salt_t aSalt = NULL );
	virtual ~CvEcdh();

	bool	GenerateKeyPair( const String& aPassword );
	String	DeriveKey( const String& aPassword );

	/**
		ECDH
	*/
	bool	AuthenticateExternal( const String& aExternalPublicKey, const String& aCommonKey );

	const String&	GetPrivateKey() const	{ return m_privateKey; }
	const String&	GetPublicKey() const	{ return m_publicKey; }
	
	void	SetPrivateKey( const String& aPrivateKey )	{ m_privateKey = aPrivateKey; }
	void	SetPublicKey( const String& aPublicKey )	{ m_publicKey = aPublicKey; }
	
	bool	ComputeCommonKey( const String& aExternalPublicKey, OUT String& aCommonKey );

	/**
		ECIES
	*/
	bool	EciesEncrypt( const String& aKey, const String& aMessage, OUT String& aCipher );
	bool	EciesDecrypt( const String& aKey, const String& aCipher, OUT String& aPlain );

	/**
		ECDSA
	*/
	bool	EcdsaSign( const String& aKey, const String& aData, OUT String& aSignature );
	bool	EcdsaVerify( const String& aKey, const String& aData, const String& aSignature );

	int		GetLastError() const	{ return m_lastError; }
	
private:
	CvEcdh( const CvEcdh& orig )	{}
	
	void	EncodeCipher( const octet& aV, const octet& aC, const octet& aT, OUT String& aCipher );
	bool	DecodeCipher( const String& aCipher, OUT octet& aV, OUT octet& aC, OUT octet& aT );
	
	void	EncodeSignature( const octet& aCS, const octet& aDS, OUT String& aSignature );
	bool	DecodeSignature( const String& aSignature, OUT octet& aCS, OUT octet& aDS );

	ecp_domain	m_ecdhDomain;
	int		m_lastError;
	
	csprng*		m_pRng;
	
	char		SALT[32];
	octet		m_octetSALT;

	String		m_publicKey;
	String		m_privateKey;
};

#endif // CVECDH_H_INCLUDED
