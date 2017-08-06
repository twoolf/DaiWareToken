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
/*! \file  octet_c.h
    \brief Portable Octet Structure

*-  Project     : SkyKey SDK
*-  Authors     : M. Scott, modified by Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 10, 2013, 5:01 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

*/

#ifndef OCTET_C_H
#define	OCTET_C_H

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

#include "miracl.h"

#ifdef mr_compare
	#undef mr_compare
#endif

/* portable representation of a big positive number */

typedef struct
{
    int len;
    int max;
    char *val;
} octet;

/* Octet string handlers */

SKYKEY_API void OCTET_OUTPUT(const octet *);

SKYKEY_API void OCTET_CLEAR(octet *);
SKYKEY_API void OCTET_EMPTY(octet *);

SKYKEY_API void OCTET_JOIN_STRING(const char *,octet *);
SKYKEY_API void OCTET_JOIN_BYTES(const char *,int,octet *);
SKYKEY_API void OCTET_JOIN_BYTE(int,int,octet *);
SKYKEY_API void OCTET_JOIN_LONG(long,int, octet *y);
SKYKEY_API void OCTET_JOIN_OCTET(const octet *, octet *);

SKYKEY_API void OCTET_XOR(const octet *,octet *);
SKYKEY_API void OCTET_XOR_BYTE(octet *,int);

SKYKEY_API void OCTET_TO_BASE64(const octet *, char *);
SKYKEY_API void OCTET_FROM_BASE64(const char *, octet *);

SKYKEY_API void OCTET_COPY(const octet *, octet *);
SKYKEY_API BOOL OCTET_COMPARE(const octet *, const octet *);
SKYKEY_API void OCTET_CHOP(octet *, int, octet *);

SKYKEY_API void OCTET_KILL(octet *);

#ifdef	__cplusplus
}
#endif

#endif	/* OCTET_C_H */

