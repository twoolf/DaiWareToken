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
 *  MIRACL SAKKE header file
 *  Author: M. Scott 2012
 */

#ifndef ECCSI_H
#define ECCSI_H

#include "ecp_c.h"
#include "octet_c.h"
#include "common_c.h"
#include "miracl.h"

#ifdef mr_compare
	#undef mr_compare
#endif

/* Elliptic Curve parameters - NIST P256 Curve */
extern const mr_small rom_eccsi[];

#ifdef MR_ECCSI_DLL

#ifdef ECCSI_DLL
#define ECCSI_API __declspec(dllexport)
#else
#define ECCSI_API __declspec(dllimport)
#endif

#else
#define ECCSI_API
#endif

#define ECCSI_OK                     0
#define ECCSI_DOMAIN_ERROR          -1
#define ECCSI_INVALID_PUBLIC_KEY    -2
#define ECCSI_ERROR                 -3
#define ECCSI_INVALID               -4
#define ECCSI_DOMAIN_NOT_FOUND      -5
#define ECCSI_OUT_OF_MEMORY         -6
#define ECCSI_DIV_BY_ZERO           -7
#define ECCSI_BAD_ASSUMPTION        -8

/* ECCSI primitives */

extern ECCSI_API int  ECCSI_MASTER_KEY_PAIR_GENERATE(ecp_domain *,csprng *,octet *,octet *);
extern ECCSI_API int  ECCSI_USER_KEY_PAIR_GENERATE(ecp_domain *,csprng *,octet*,octet *,octet *,octet *,octet *,octet *);
extern ECCSI_API int  ECCSI_USER_KEY_PAIR_VALIDATE(ecp_domain *,octet *,octet *,octet *,octet *,octet *);
extern ECCSI_API int  ECCSI_SIGN(ecp_domain *,csprng *,octet*,octet *,octet *,octet *K,octet *,octet *,octet *,octet *);
extern ECCSI_API int  ECCSI_VERIFY(ecp_domain *,octet *,octet *,octet *,octet *);

#endif