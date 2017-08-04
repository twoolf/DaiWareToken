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
/*! \file  CvMikey.h
    \brief C++ class implementing the MIKEY standard for key transport

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 31, 2013, 10:32 AM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

 C++ class implementing the MIKEY standard for key transport.
 The class API is adapted to the existing SkyKey soltuion.

*/

#ifndef CVMIKEY_H
#define	CVMIKEY_H

#include "CvCommon.h"

extern "C"
{
	#include "sakke_bn_c.h"
}

#include <string>

class CvMikey
{
public:
	typedef std::string	String;
	
	CvMikey( csprng* apRng = NULL );
	virtual ~CvMikey();
	
	int	GetSize( const String& aSenderId, const String& aReceiverId, const String& aSenderKms, const String& aReceiverKms ) const;
	
	bool	CreateMessage(  const String& aKey, const String& aSenderId, const String& aReceiverId, const String& aSenderKms, const String& aReceiverKms,
							const String& aSakkeParams, const String& aSakkeKmsPublicKey, const String& aEccsiSecret, const String& aEccsiPrivateKey,
							const String& aEccsiKmsPublicKey, OUT String& aPacket );
	
	bool	ProcessMessage( const String& aPacket, const String& aUserId, const String& aUserKms, const String& aSakkePrivateKey, const String& aSakkeParams,
							const String& aSakkeKmsPublicKey, const String& aEccsiKmsPublicKey, OUT String& aKey );
	
private:
	CvMikey(const CvMikey& orig)	{}

	csprng*	m_pRng;
};

#endif	/* CVMIKEY_H */

