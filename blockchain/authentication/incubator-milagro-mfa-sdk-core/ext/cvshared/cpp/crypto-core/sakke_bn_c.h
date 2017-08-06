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

#ifndef SAKKE_H
#define SAKKE_H

#include "octet_c.h"
#include "common_c.h"
#include "miracl.h"

#ifdef mr_compare
	#undef mr_compare
#endif

#define GS			FS  /* Group Size */
#define WINDOW_SIZE	4
#define ROM_SIZE	14

/* Elliptic Curve parameters - BN Curve */
extern const mr_small rom_sakke[];

typedef struct
{
    zzn4 a;
    zzn4 b;
	zzn4 c;
    BOOL unitary;
	BOOL miller;
} zzn12;

#ifdef MR_SAKKE_DLL

#ifdef SAKKE_DLL
#define SAKKE_API __declspec(dllexport)
#else
#define SAKKE_API __declspec(dllimport)
#endif

#else
#define SAKKE_API
#endif

#define SAKKE_OK                     0
#define SAKKE_DOMAIN_ERROR          -1
#define SAKKE_INVALID_PUBLIC_KEY    -2
#define SAKKE_ERROR                 -3
#define SAKKE_INVALID               -4
#define SAKKE_DOMAIN_NOT_FOUND      -5
#define SAKKE_OUT_OF_MEMORY         -6
#define SAKKE_DIV_BY_ZERO           -7
#define SAKKE_BAD_ASSUMPTION        -8

/* SAKKE domain parameters */

typedef struct
{
    int nibbles;
    int flags;
    char X[FS];
    char Q[FS];
    char A[FS];
    char B[FS];
    char R[FS];
    char Px[FS];
    char Py[FS];
    char Beta[FS];
    char Qxa[FS];
    char Qxb[FS];
    char Qya[FS];
    char Qyb[FS];
    char Fa[FS];
    char Fb[FS];
    char G[4][FS];
} sak_domain;

/* SAKKE Auxiliary Functions */

//extern SAKKE_API void CREATE_CSPRNG(csprng *,octet *);
//extern SAKKE_API void KILL_CSPRNG(csprng *);

/* SAKKE support functions */

extern SAKKE_API void SAKKE_DOMAIN_KILL(sak_domain *);
extern SAKKE_API int  SAKKE_DOMAIN_INIT(sak_domain *,const void *);

/* SAKKE primitives */

extern SAKKE_API int  SAKKE_MASTER_KEY_PAIR_GENERATE(sak_domain *,csprng *,octet *,octet *);
extern SAKKE_API int  SAKKE_GET_USER_SECRET_KEY(sak_domain *,octet* ,octet *,octet *);
extern SAKKE_API int  SAKKE_KEY_ENCAPSULATE(sak_domain *,octet *,octet *,octet *,octet *,octet *);
extern SAKKE_API int  SAKKE_KEY_DECAPSULATE(sak_domain *,octet *,octet *,octet *,octet *,octet *,octet *);
extern SAKKE_API int  SAKKE_SECRET_KEY_VALIDATE(sak_domain *,octet *,octet *,octet *);

#endif

