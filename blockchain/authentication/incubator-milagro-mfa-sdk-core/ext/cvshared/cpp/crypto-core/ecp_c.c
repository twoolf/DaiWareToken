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
/*! \file  ecp_c.c
    \brief MIRACL NIST P256 Elliptic Curve Parameters implementation file

*-  Project     : SkyKey SDK
*-  Authors     : M. Scott, modified by Mony Aladjem
*-  Company     : Certivox
*-  Created     : 2012
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

*/

#include "ecp_c.h"

/* Elliptic Curve parameters - NIST P256 Curve */

#if MIRACL==64

const mr_small rom_ecp[]={
0xffffffffffffffff,0xffffffff,0x0,0xffffffff00000001,
0x3bce3c3e27d2604b,0x651d06b0cc53b0f6,0xb3ebbd55769886bc,0x5ac635d8aa3a93e7,
0xf3b9cac2fc632551,0xbce6faada7179e84,0xffffffffffffffff,0xffffffff00000000,
0xf4a13945d898c296,0x77037d812deb33a0,0xf8bce6e563a440f2,0x6b17d1f2e12c4247,
0xcbb6406837bf51f5,0x2bce33576b315ece,0x8ee7eb4a7c0f9e16,0x4fe342e2fe1a7f9b};

#elif MIRACL==32

const mr_small rom_ecp[]={
0xffffffff,0xffffffff,0xffffffff,0x0,0x0,0x0,0x1,0xffffffff,
0x27d2604b,0x3bce3c3e,0xcc53b0f6,0x651d06b0,0x769886bc,0xb3ebbd55,0xaa3a93e7,0x5ac635d8,
0xfc632551,0xf3b9cac2,0xa7179e84,0xbce6faad,0xffffffff,0xffffffff,0x0,0xffffffff,
0xd898c296,0xf4a13945,0x2deb33a0,0x77037d81,0x63a440f2,0xf8bce6e5,0xe12c4247,0x6b17d1f2,
0x37bf51f5,0xcbb64068,0x6b315ece,0x2bce3357,0x7c0f9e16,0x8ee7eb4a,0xfe1a7f9b,0x4fe342e2};

#endif


/* Initialise the EC GF(p) domain structure
 * It is assumed that the EC domain details are obtained from ROM
 */

SKYKEY_API int ECP_DOMAIN_INIT(ecp_domain *DOM,const void *rom)
{ /* get domain details from ROM     */
 
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,2*FS,16);
#else
    miracl *mr_mip=mirsys(2*FS,16);
#endif
    big q,r,gx,gy,a,b;
    int words,promptr,err,res=ECP_OK;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ 6);;
#else
    char mem[MR_BIG_RESERVE(6)];
    memset(mem,0,MR_BIG_RESERVE(6));
#endif

	DOM->nibbles=2*FS;
	words=MR_ROUNDUP(FS*8,MIRACL);

	if (mr_mip==NULL || mem==NULL) res= ECP_OUT_OF_MEMORY;

    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        q=mirvar_mem(_MIPP_ mem, 0);
        a=mirvar_mem(_MIPP_ mem, 1);
        b=mirvar_mem(_MIPP_ mem, 2);
        r=mirvar_mem(_MIPP_ mem, 3);
        gx=mirvar_mem(_MIPP_ mem, 4);
        gy=mirvar_mem(_MIPP_ mem, 5);

		promptr=0;
		init_big_from_rom(q,words,(const mr_small *)rom,words*5,&promptr);  /* Read in prime modulus q from ROM   */
		init_big_from_rom(b,words,(const mr_small *)rom,words*5,&promptr);  /* Read in curve parameter b from ROM */
 		init_big_from_rom(r,words,(const mr_small *)rom,words*5,&promptr);  /* Read in curve parameter r from ROM */
 		init_big_from_rom(gx,words,(const mr_small *)rom,words*5,&promptr);  /* Read in curve parameter gx from ROM */
		init_big_from_rom(gy,words,(const mr_small *)rom,words*5,&promptr);  /* Read in curve parameter gy from ROM */
		convert(_MIPP_ -3,a);
		add(_MIPP_ q,a,a);

		big_to_bytes(_MIPP_ FS,q,DOM->Q,TRUE);
		big_to_bytes(_MIPP_ FS,a,DOM->A,TRUE);
		big_to_bytes(_MIPP_ FS,b,DOM->B,TRUE);
		big_to_bytes(_MIPP_ FS,r,DOM->R,TRUE);
		big_to_bytes(_MIPP_ FS,gx,DOM->Gx,TRUE);
		big_to_bytes(_MIPP_ FS,gy,DOM->Gy,TRUE);
	}
#ifndef MR_STATIC
    memkill(_MIPP_ mem,6);
#else
    memset(mem,0,MR_BIG_RESERVE(6));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return ECP_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return ECP_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

/*** EC GF(p) primitives - support functions ***/
/* destroy the EC GF(p) domain structure */

SKYKEY_API void ECP_DOMAIN_KILL(ecp_domain *DOM)
{
	int i;
	for (i=0;i<FS;i++)
	{
		DOM->Q[i]=0;
		DOM->A[i]=0;
		DOM->B[i]=0;
		DOM->R[i]=0;
		DOM->Gx[i]=0;
		DOM->Gy[i]=0;
	}
}
