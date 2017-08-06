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
 *  MIRACL SAKKE implementation file
 *  Author: M. Scott 2012
 */

#include "eccsi_c.h"

/* Calculate a public/private EC GF(p) key pair. W=S.g mod EC(p),
 * where S is the secret key and W is the public key
 * If RNG is NULL then the private key is provided externally in S
 * otherwise it is generated randomly internally */

ECCSI_API int ECCSI_MASTER_KEY_PAIR_GENERATE(ecp_domain *DOM,csprng *RNG,octet* S,octet *W)
{
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
    big q,a,b,r,gx,gy,s,wx,wy;
    epoint *G,*WP;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ 9);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(9)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(9));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= ECCSI_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        q=mirvar_mem(_MIPP_ mem, 0);
        a=mirvar_mem(_MIPP_ mem, 1);
        b=mirvar_mem(_MIPP_ mem, 2);
        r=mirvar_mem(_MIPP_ mem, 3);
        gx=mirvar_mem(_MIPP_ mem, 4);
        gy=mirvar_mem(_MIPP_ mem, 5);
        s=mirvar_mem(_MIPP_ mem, 6);
        wx=mirvar_mem(_MIPP_ mem, 7);
        wy=mirvar_mem(_MIPP_ mem, 8);

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Gx,gx);
        bytes_to_big(_MIPP_ FS,DOM->Gy,gy);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        G=epoint_init_mem(_MIPP_ mem1,0);
        WP=epoint_init_mem(_MIPP_ mem1,1);
        if (!epoint_set(_MIPP_ gx,gy,0,G)) res=MR_ERR_BAD_PARAMETERS;
	}
	if (res==0)
	{

        if (RNG!=NULL)
            strong_bigrand(_MIPP_ RNG,r,s);
        else
        {
            bytes_to_big(_MIPP_ S->len,S->val,s);
            divide(_MIPP_ s,r,r);
        }

        ecurve_mult(_MIPP_ s,G,WP);        
        epoint_get(_MIPP_ WP,wx,wy);
    
        if (RNG!=NULL) S->len=big_to_bytes(_MIPP_ 0,s,S->val,FALSE);

		W->len=2*FS+1;	W->val[0]=4;
		big_to_bytes(_MIPP_ FS,wx,&(W->val[1]),TRUE);
		big_to_bytes(_MIPP_ FS,wy,&(W->val[FS+1]),TRUE);
    }

#ifndef MR_STATIC
    memkill(_MIPP_ mem,9);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(9));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return ECCSI_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return ECCSI_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

ECCSI_API int ECCSI_USER_KEY_PAIR_GENERATE(ecp_domain *DOM,csprng *RNG,octet* V,octet *ID,octet *KSAK,octet *KPAK,octet *SSK,octet *PVT)
{
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
	int i;
    char hh[HASH_BYTES];
	HASHFUNC SHA;
    big q,a,b,r,gx,gy,v,wx,wy,hs;
    epoint *G,*WP;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ 10);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(10)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(10));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= ECCSI_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        q=mirvar_mem(_MIPP_ mem, 0);
        a=mirvar_mem(_MIPP_ mem, 1);
        b=mirvar_mem(_MIPP_ mem, 2);
        r=mirvar_mem(_MIPP_ mem, 3);
        gx=mirvar_mem(_MIPP_ mem, 4);
        gy=mirvar_mem(_MIPP_ mem, 5);
        v=mirvar_mem(_MIPP_ mem, 6);
        wx=mirvar_mem(_MIPP_ mem, 7);
        wy=mirvar_mem(_MIPP_ mem, 8);
		hs=mirvar_mem(_MIPP_ mem, 9);

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Gx,gx);
        bytes_to_big(_MIPP_ FS,DOM->Gy,gy);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        G=epoint_init_mem(_MIPP_ mem1,0);
        WP=epoint_init_mem(_MIPP_ mem1,1);
        if (!epoint_set(_MIPP_ gx,gy,0,G)) res=MR_ERR_BAD_PARAMETERS;
	}
	if (res==0)
	{
        if (RNG!=NULL)
            strong_bigrand(_MIPP_ RNG,r,v);
        else
        {
            bytes_to_big(_MIPP_ V->len,V->val,v);
            divide(_MIPP_ v,r,r);
        }

		ecurve_mult(_MIPP_ v,G,WP);        
		epoint_get(_MIPP_ WP,wx,wy);

		if (RNG!=NULL)  V->len=big_to_bytes(_MIPP_ 0,v,V->val,FALSE); 
        
		PVT->len=2*FS+1;	PVT->val[0]=4;
		big_to_bytes(_MIPP_ FS,wx,&(PVT->val[1]),TRUE);
		big_to_bytes(_MIPP_ FS,wy,&(PVT->val[FS+1]),TRUE);

		SHS_INIT(&SHA);
/* first hash G */
		SHS_PROCESS(&SHA,0x04);
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,DOM->Gx[i]);
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,DOM->Gy[i]);
/* then KPAK, ID and PVT */
		for (i=0;i<KPAK->len;i++) SHS_PROCESS(&SHA,KPAK->val[i]);
		for (i=0;i<ID->len;i++) SHS_PROCESS(&SHA,ID->val[i]);
		for (i=0;i<PVT->len;i++) SHS_PROCESS(&SHA,PVT->val[i]);
		
		SHS_HASH(&SHA,hh);
		bytes_to_big(_MIPP_ HASH_BYTES,hh,hs);

		bytes_to_big(_MIPP_ KSAK->len,KSAK->val,wx);
		mad(_MIPP_ hs,v,wx,r,r,wy);
		SSK->len=big_to_bytes(_MIPP_ FS,wy,SSK->val,TRUE);
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,10);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(10));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return ECCSI_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return ECCSI_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

ECCSI_API int ECCSI_USER_KEY_PAIR_VALIDATE(ecp_domain *DOM,octet *ID,octet *KPAK,octet *HS,octet *SSK,octet *PVT)
{
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
	int i;
    char hh[HASH_BYTES];
	HASHFUNC SHA;
    big q,a,b,r,gx,gy,wx,wy,hs;
    epoint *G,*WP;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ 9);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(9)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(9));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= ECCSI_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        q=mirvar_mem(_MIPP_ mem, 0);
        a=mirvar_mem(_MIPP_ mem, 1);
        b=mirvar_mem(_MIPP_ mem, 2);
        r=mirvar_mem(_MIPP_ mem, 3);
        gx=mirvar_mem(_MIPP_ mem, 4);
        gy=mirvar_mem(_MIPP_ mem, 5);
        wx=mirvar_mem(_MIPP_ mem, 6);
        wy=mirvar_mem(_MIPP_ mem, 7);
		hs=mirvar_mem(_MIPP_ mem, 8);

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Gx,gx);
        bytes_to_big(_MIPP_ FS,DOM->Gy,gy);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        G=epoint_init_mem(_MIPP_ mem1,0);
		WP=epoint_init_mem(_MIPP_ mem1,1);
		if (!epoint_set(_MIPP_ gx,gy,0,G)) res=MR_ERR_BAD_PARAMETERS;
	}
	if (res==0)
	{
		bytes_to_big(_MIPP_ FS,&(PVT->val[1]),wx);
		bytes_to_big(_MIPP_ FS,&(PVT->val[FS+1]),wy);

		if (!epoint_set(_MIPP_ wx,wy,0,WP)) res=ECCSI_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		SHS_INIT(&SHA);
/* first hash G */
		SHS_PROCESS(&SHA,0x04);
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,DOM->Gx[i]);
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,DOM->Gy[i]);
/* then KPAK, ID and PVT */
		for (i=0;i<KPAK->len;i++) SHS_PROCESS(&SHA,KPAK->val[i]);
		for (i=0;i<ID->len;i++) SHS_PROCESS(&SHA,ID->val[i]);
		for (i=0;i<PVT->len;i++) SHS_PROCESS(&SHA,PVT->val[i]);
		
		SHS_HASH(&SHA,hh);
		bytes_to_big(_MIPP_ HASH_BYTES,hh,hs);

		HS->len=HASH_BYTES; for (i=0;i<HASH_BYTES;i++) HS->val[i]=hh[i];

		bytes_to_big(_MIPP_ SSK->len,SSK->val,wx);
		ecurve_mult(_MIPP_ wx,G,G);
		ecurve_mult(_MIPP_ hs,WP,WP);
		ecurve_sub(_MIPP_ WP,G);
		bytes_to_big(_MIPP_ FS,&(KPAK->val[1]),wx);
		bytes_to_big(_MIPP_ FS,&(KPAK->val[FS+1]),wy);

		epoint_set(_MIPP_ wx,wy,0,WP);
		if (!epoint_comp(_MIPP_ G,WP)) res=ECCSI_INVALID;
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,9);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(9));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return ECCSI_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return ECCSI_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

ECCSI_API int ECCSI_SIGN(ecp_domain *DOM,csprng *RNG,octet* J,octet *M,octet *ID,octet *KPAK,octet *HS,octet *SSK,octet *PVT,octet *SIG)
{
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
	int i,m;
    char hh[HASH_BYTES];
	char IOBUFF[FS];
	HASHFUNC SHA;
    big q,a,b,r,gx,gy,wx,wy,he,j;
    epoint *G,*WP;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ 10);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(10)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(10));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= ECCSI_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        q=mirvar_mem(_MIPP_ mem, 0);
        a=mirvar_mem(_MIPP_ mem, 1);
        b=mirvar_mem(_MIPP_ mem, 2);
        r=mirvar_mem(_MIPP_ mem, 3);
        gx=mirvar_mem(_MIPP_ mem, 4);
        gy=mirvar_mem(_MIPP_ mem, 5);
		j=mirvar_mem(_MIPP_ mem, 6);
        wx=mirvar_mem(_MIPP_ mem, 7);
        wy=mirvar_mem(_MIPP_ mem, 8);
		he=mirvar_mem(_MIPP_ mem, 9);

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Gx,gx);
        bytes_to_big(_MIPP_ FS,DOM->Gy,gy);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        G=epoint_init_mem(_MIPP_ mem1,0);
		WP=epoint_init_mem(_MIPP_ mem1,1);
		if (!epoint_set(_MIPP_ gx,gy,0,G))  res=MR_ERR_BAD_PARAMETERS;
	}
	if (res==0)
	{
        if (RNG!=NULL)
            strong_bigrand(_MIPP_ RNG,r,j);
        else
        {
            bytes_to_big(_MIPP_ J->len,J->val,j);
            divide(_MIPP_ j,r,r);
        }

		ecurve_mult(_MIPP_ j,G,WP);        
		epoint_get(_MIPP_ WP,wx,wy);

		bytes_to_big(_MIPP_ SSK->len,SSK->val,wy);

		big_to_bytes(_MIPP_ FS,wx,IOBUFF,TRUE); /* r */
		SHS_INIT(&SHA);
/* first hash HS */
		for (i=0;i<HS->len;i++) SHS_PROCESS(&SHA,HS->val[i]);
/* then r */
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,IOBUFF[i]);
/* finally M.. */
		for (i=0;i<M->len;i++) SHS_PROCESS(&SHA,M->val[i]);
		
		SHS_HASH(&SHA,hh);
		bytes_to_big(_MIPP_ HASH_BYTES,hh,he);

		mad(_MIPP_ wx,wy,he,r,r,wy);
		invmodp(_MIPP_ wy,r,wy);
		mad(_MIPP_ wy,j,j,r,r,wy);    /* s' */
		if (logb2(_MIPP_ wx)>FS*8) subtract(_MIPP_ r,wy,wy); /* s */

		SIG->len=4*FS+1;
/* SIG = r || s || PVT */	
		for (i=m=0;i<FS;i++) SIG->val[m++]=IOBUFF[i];
		big_to_bytes(_MIPP_ FS,wy,IOBUFF,TRUE);
		for (i=0;i<FS;i++) SIG->val[m++]=IOBUFF[i];
		for (i=0;i<PVT->len;i++) SIG->val[m++]=PVT->val[i];

	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,10);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(10));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return ECCSI_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return ECCSI_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

ECCSI_API int ECCSI_VERIFY(ecp_domain *DOM,octet *M,octet *ID,octet *KPAK,octet *SIG)
{
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
	int i;
    char hh[HASH_BYTES];
	HASHFUNC SHA;
    big q,a,b,r,gx,gy,wx,wy,he,hs;
    epoint *G,*WP;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ 10);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(10)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(10));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= ECCSI_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        q=mirvar_mem(_MIPP_ mem, 0);
        a=mirvar_mem(_MIPP_ mem, 1);
        b=mirvar_mem(_MIPP_ mem, 2);
        r=mirvar_mem(_MIPP_ mem, 3);
        gx=mirvar_mem(_MIPP_ mem, 4);
        gy=mirvar_mem(_MIPP_ mem, 5);
		hs=mirvar_mem(_MIPP_ mem, 6);
        wx=mirvar_mem(_MIPP_ mem, 7);
        wy=mirvar_mem(_MIPP_ mem, 8);
		he=mirvar_mem(_MIPP_ mem, 9);

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        G=epoint_init_mem(_MIPP_ mem1,0);
		WP=epoint_init_mem(_MIPP_ mem1,1);

/* extract PVT from signature */
		bytes_to_big(_MIPP_ FS,&(SIG->val[2*FS+1]),wx);
		bytes_to_big(_MIPP_ FS,&(SIG->val[3*FS+1]),wy);

		if (!epoint_set(_MIPP_ wx,wy,0,WP)) res=ECCSI_INVALID_PUBLIC_KEY;
	}	
    if (res==0)
    {
		SHS_INIT(&SHA);
/* first hash G */
		SHS_PROCESS(&SHA,0x04);
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,DOM->Gx[i]);
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,DOM->Gy[i]);
/* then KPAK, ID and PVT */
		for (i=0;i<KPAK->len;i++) SHS_PROCESS(&SHA,KPAK->val[i]);
		for (i=0;i<ID->len;i++) SHS_PROCESS(&SHA,ID->val[i]);
		for (i=0;i<2*FS+1;i++) SHS_PROCESS(&SHA,SIG->val[2*FS+i]);
		
		SHS_HASH(&SHA,hh);
		bytes_to_big(_MIPP_ HASH_BYTES,hh,hs);

/* extract signature */

		bytes_to_big(_MIPP_ FS,&SIG->val[0],wx);  /* get r */
		bytes_to_big(_MIPP_ FS,&SIG->val[FS],wy); /* get s */

		SHS_INIT(&SHA);
/* first hash HS */
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,hh[i]);
/* then r */
		for (i=0;i<FS;i++) SHS_PROCESS(&SHA,SIG->val[i]);
/* finally M.. */
		for (i=0;i<M->len;i++) SHS_PROCESS(&SHA,M->val[i]);

		SHS_HASH(&SHA,hh);
		bytes_to_big(_MIPP_ HASH_BYTES,hh,he);

		ecurve_mult(_MIPP_ hs,WP,WP);
		bytes_to_big(_MIPP_ FS,&(KPAK->val[1]),gx);
		bytes_to_big(_MIPP_ FS,&(KPAK->val[FS+1]),gy);
		if (!epoint_set(_MIPP_ gx,gy,0,G)) res=ECCSI_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		ecurve_add(_MIPP_ G,WP);     /* Y */
        bytes_to_big(_MIPP_ FS,DOM->Gx,gx);
        bytes_to_big(_MIPP_ FS,DOM->Gy,gy);

		if (!epoint_set(_MIPP_ gx,gy,0,G))  res=MR_ERR_BAD_PARAMETERS;
	}
	if (res==0)
	{
		mad(_MIPP_ wx,wy,wx,r,r,hs);
		mad(_MIPP_ wy,he,wy,r,r,wy);
		ecurve_mult2(_MIPP_ hs,WP,wy,G,G);

		epoint_get(_MIPP_ G,gx,gy);

		if (mr_compare(gx,wx)!=0) res=ECCSI_INVALID;
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,10);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(10));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return ECCSI_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return ECCSI_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
} 
