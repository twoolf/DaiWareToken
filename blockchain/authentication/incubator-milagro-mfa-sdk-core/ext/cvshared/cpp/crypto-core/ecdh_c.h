/***************************************************************************
                                                                           *
Copyright 2013 CertiVox UK Ltd.                                            *
                                                                           *
This file is part of CertiVox SkyKey XT Crypto SDK.                        *
                                                                           *
The CertiVox SkyKey XT Crypto SDK provides developers with an              *
extensive and efficient set of cryptographic functions.                    *
For further information about its features and functionalities please      *
refer to http://www.certivox.com                                           *
                                                                           *
* The CertiVox SkyKey XT Crypto SDK is free software: you can              *
  redistribute it and/or modify it under the terms of the                  *
  GNU Affero General Public License as published by the                    *
  Free Software Foundation, either version 3 of the License,               *
  or (at your option) any later version.                                   *
                                                                           *
* The CertiVox SkyKey XT Crypto SDK is distributed in the hope             *
  that it will be useful, but WITHOUT ANY WARRANTY; without even the       *
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
  See the GNU Affero General Public License for more details.              *
                                                                           *
* You should have received a copy of the GNU Affero General Public         *
  License along with CertiVox MIRACL Crypto SDK.                           *
  If not, see <http://www.gnu.org/licenses/>.                              *
                                                                           *
You can be released from the requirements of the license by purchasing     *
a commercial license. Buying such a license is mandatory as soon as you    *
develop commercial activities involving the CertiVox SkyKey XT Crypto SDK  *
without disclosing the source code of your own applications, or shipping   *
the CertiVox SkyKey XT Crypto SDK with a closed source product.            *
                                                                           *
***************************************************************************/
/*
 *  MIRACL ECDH header file
 *  Author: M. Scott 2012
 */

#ifndef ECDH_H
#define ECDH_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "ecp_c.h"
#include "octet_c.h"
#include "common_c.h"
#include "miracl.h"

#ifdef mr_compare
	#undef mr_compare
#endif

#ifdef ECDH_DLL

#ifdef P1363_DLL
#define ECDH_API __declspec(dllexport)
#else
#define ECDH_API __declspec(dllimport)
#endif

#else
#define ECDH_API
#endif

#define ECDH_OK                     0
#define ECDH_DOMAIN_ERROR          -1
#define ECDH_INVALID_PUBLIC_KEY    -2
#define ECDH_ERROR                 -3
#define ECDH_INVALID               -4
#define ECDH_DOMAIN_NOT_FOUND      -5
#define ECDH_OUT_OF_MEMORY         -6
#define ECDH_DIV_BY_ZERO           -7
#define ECDH_BAD_ASSUMPTION        -8

ECDH_API void ECDH_HASH(octet *,octet *);
ECDH_API BOOL ECDH_HMAC(octet *,octet *,int,octet *);
ECDH_API void ECDH_KDF1(octet *,int,octet *);
ECDH_API void ECDH_KDF2(octet *,octet *,int,octet *);
ECDH_API void ECDH_PBKDF2(octet *,octet *,int,int,octet *);
ECDH_API void AES_CBC_IV0_ENCRYPT(octet *,octet *,octet *);
ECDH_API BOOL AES_CBC_IV0_DECRYPT(octet *,octet *,octet *);

/* ECDH primitives - support functions */

ECDH_API int  ECP_KEY_PAIR_GENERATE(ecp_domain *,csprng *,octet *,octet *);
ECDH_API int  ECP_PUBLIC_KEY_VALIDATE(ecp_domain *,BOOL,octet *);

/* ECDH primitives */

ECDH_API int  ECPSVDP_DH(ecp_domain *,octet *,octet *,octet *);
ECDH_API int  ECPSVDP_DHC(ecp_domain *,octet *,octet *,BOOL,octet *);

/* ECIES functions */
ECDH_API void ECP_ECIES_ENCRYPT(ecp_domain *,octet *,octet *,csprng *,octet *,octet *,int,octet *,octet *,octet *);
ECDH_API BOOL ECP_ECIES_DECRYPT(ecp_domain *,octet *,octet *,octet *,octet *,octet *,octet *,octet *);

/* ECDSA functions */
ECDH_API int  ECPSP_DSA(ecp_domain *,csprng *,octet *,octet *,octet *,octet *);
ECDH_API int  ECPVP_DSA(ecp_domain *,octet *,octet *,octet *,octet *);

#ifdef	__cplusplus
}
#endif

#endif	// ECDH_H

