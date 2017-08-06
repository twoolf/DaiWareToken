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
/*! \file  ecp_c.h
    \brief MIRACL NIST P256 Elliptic Curve Parameters header file

*-  Project     : SkyKey SDK
*-  Authors     : M. Scott, modified by Mony Aladjem
*-  Company     : Certivox
*-  Created     : 2012
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

*/

#ifndef ECP_C
#define	ECP_C

#ifdef MR_SKYKEY_DLL

#ifdef SKYKEY_DLL
#define SKYKEY_API __declspec(dllexport)
#else
#define SKYKEY_API __declspec(dllimport)
#endif

#else

#define SKYKEY_API

#endif	//MR_SKYKEY_DLL

#ifdef	__cplusplus
extern "C"
{
#endif

#include "common_c.h"
#include "miracl.h"

#ifdef mr_compare
	#undef mr_compare
#endif

/* Elliptic Curve parameters - NIST P256 Curve */
extern const mr_small rom_ecp[];

#ifdef MR_SKYKEY_DLL
	#ifdef SKYKEY_DLL
	#define SKYKEY_API __declspec(dllexport)
	#else
	#define SKYKEY_API __declspec(dllimport)
	#endif
#else
	#define SKYKEY_API
#endif	//MR_SKYKEY_DLL

#define ECP_OK                     0
#define ECP_OUT_OF_MEMORY         -6
#define ECP_DIV_BY_ZERO           -7

/* ECp domain parameters */

typedef struct
{
    int nibbles;
    char Q[FS];
    char A[FS];
    char B[FS];
    char R[FS];
    char Gx[FS];
    char Gy[FS];
} ecp_domain;

/* ECCSI/ECDH support functions */

extern SKYKEY_API void ECP_DOMAIN_KILL(ecp_domain *);
extern SKYKEY_API int  ECP_DOMAIN_INIT(ecp_domain *,const void *);

#ifdef	__cplusplus
}
#endif

#endif	/* ECP_C */

