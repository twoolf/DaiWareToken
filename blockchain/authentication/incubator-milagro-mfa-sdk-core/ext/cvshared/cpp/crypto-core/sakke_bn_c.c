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

#include "sakke_bn_c.h"

/* Elliptic Curve parameters - BN Curve */

#if MIRACL==64

const mr_small rom_sakke[]={
0x4080000000000001,0x0,0x0,0x8000000000000000, /* x */
0x2,0x0,0x0,0x0,                               /* B */
0xA700000000000012,0x6121000000000013,0xBA344D8000000008,0x2523648240000001, /* Px = -1 */
0x1,0x0,0x0,0x0, /* Py = 1 */
0x353F63AD74319C04,0xF68AFDBF9B933998,0x28E05B3AAF153F82,0x3C67A5CB50A75BD, /* Qxa */
0x23559C8A12B5637F,0x5B5051B1119E373B,0x278F3D149BAC8FAA,0x86C6D36FDAF0244, /* Qxb */
0x8AB9CC634607E059,0x51430509C32A6440,0xBA739B657113D84,0x62039BE3E8F0691,  /* Qya */
0xC51DD369F21FF550,0xE12AC7E5BA650CC3,0x3861D7D21AE532BD,0xAB7E3D96F16C979, /* Qyb */
0xE17DE6C06F2A6DE9,0x850974924D3F77C2,0xB6499B50A846953F,0x1B377619212E7C8C, /* Fx */
0xC582193F90D5922A,0xDC178B6DB2C08850,0x3EAB22F57B96AC8,0x9EBEE691ED18375,   /* Fy */
0xC7A13EEB84FC4D90,0xDED2FCCC99325126,0x243D96869388BA9D,0x2ECBA233662E4F4, /* e(Q,P) */
0xDFF634F6E95886F4,0x69D8DC50976E5896,0x58024379211120E6,0x11BD28EB94B339B2,
0x6869D25EA266113C,0x8B1B60952876029F,0x54268FD85C6BE959,0xE7BC033696EF5EB,
0xA86D12D2BC5CC688,0x61F15304E5103800,0x2AB63413218C9570,0x75178FA782F8119};

#elif MIRACL==32

const mr_small rom_sakke[]={
0x1,0x40800000,0x0,0x0,0x0,0x0,0x0,0x80000000,
0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x12,0xA7000000,0x13,0x61210000,0x8,0xBA344D80,0x40000001,0x25236482,
0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x74319C04,0x353F63AD,0x9B933998,0xF68AFDBF,0xAF153F82,0x28E05B3A,0xB50A75BD,0x3C67A5C,
0x12B5637F,0x23559C8A,0x119E373B,0x5B5051B1,0x9BAC8FAA,0x278F3D14,0xFDAF0244,0x86C6D36,
0x4607E059,0x8AB9CC63,0xC32A6440,0x51430509,0x57113D84,0xBA739B6,0x3E8F0691,0x62039BE,
0xF21FF550,0xC51DD369,0xBA650CC3,0xE12AC7E5,0x1AE532BD,0x3861D7D2,0x6F16C979,0xAB7E3D9,
0x6F2A6DE9,0xE17DE6C0,0x4D3F77C2,0x85097492,0xA846953F,0xB6499B50,0x212E7C8C,0x1B377619,
0x90D5922A,0xC582193F,0xB2C08850,0xDC178B6D,0x57B96AC8,0x3EAB22F,0x1ED18375,0x9EBEE69,
0x84FC4D90,0xC7A13EEB,0x99325126,0xDED2FCCC,0x9388BA9D,0x243D9686,0x3662E4F4,0x2ECBA23,
0xE95886F4,0xDFF634F6,0x976E5896,0x69D8DC50,0x211120E6,0x58024379,0x94B339B2,0x11BD28EB,
0xA266113C,0x6869D25E,0x2876029F,0x8B1B6095,0x5C6BE959,0x54268FD8,0x696EF5EB,0xE7BC033,
0xBC5CC688,0xA86D12D2,0xE5103800,0x61F15304,0x218C9570,0x2AB63413,0x782F8119,0x75178FA};

#endif

static void hash(octet *p,int n,octet *x,octet *w)
{
    int i,hlen,c[4];
    HASHFUNC sha;
    char hh[HASH_BYTES];

    hlen=HASH_BYTES;

    SHS_INIT(&sha);
    if (p!=NULL)
        for (i=0;i<p->len;i++) SHS_PROCESS(&sha,p->val[i]);
	if (n>=0)
    {
        c[0]=(n>>24)&0xff;
        c[1]=(n>>16)&0xff;
        c[2]=(n>>8)&0xff;
        c[3]=(n)&0xff;
		for (i=0;i<4;i++) SHS_PROCESS(&sha,c[i]);
    }
    if (x!=NULL)
        for (i=0;i<x->len;i++) SHS_PROCESS(&sha,x->val[i]);    
	
       
    SHS_HASH(&sha,hh);
   
    OCTET_EMPTY(w);
    OCTET_JOIN_BYTES(hh,hlen,w);
    for (i=0;i<hlen;i++) hh[i]=0;
}

static void HashToIntegerRange(_MIPD_ big n,big q,big v)
{
	int i,k,hlen,len;
	char h[HASH_BYTES],t[HASH_BYTES],a[HASH_BYTES];
	octet H={0,sizeof(h),h};
	octet T={0,sizeof(t),t};
	octet A={0,sizeof(a),a};

#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif

	hlen=8*HASH_BYTES;

	expb2(_MIPP_ hlen,mr_mip->w1);

	A.len=big_to_bytes(_MIPP_ HASH_BYTES,n,A.val,TRUE);
	k=logb2(_MIPP_ q);
	len=k/hlen; if (k%hlen!=0) len++;
	zero(v);

	H.len=HASH_BYTES;
	for (i=0;i<HASH_BYTES;i++) H.val[i]=0;

	for (i=0;i<len;i++)
	{
		hash(&H,-1,NULL,&H);
		hash(&H,-1,&A,&T);
		bytes_to_big(_MIPP_ T.len,T.val,mr_mip->w2);
		multiply(_MIPP_ v,mr_mip->w1,v);
		add(_MIPP_ v,mr_mip->w2,v);
	}
	divide(_MIPP_ v,q,q);
}

/*
void ecn_print(_MIPD_ epoint *P)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	if (P->marker==MR_EPOINT_INFINITY)
	{
		printf("[Infinity]\n");
		return;
	}
	epoint_norm(_MIPP_ P);
	printf("["); 
	redc(_MIPP_ P->X,P->X);
	otstr(_MIPP_ P->X,mr_mip->IOBUFF);
	nres(_MIPP_ P->X,P->X);
	printf("%s,",mr_mip->IOBUFF);
	redc(_MIPP_ P->Y,P->Y);
	otstr(_MIPP_ P->Y,mr_mip->IOBUFF);
	nres(_MIPP_ P->Y,P->Y);
	printf("%s]\n",mr_mip->IOBUFF);
}

void zzn2_print(_MIPD_ zzn2 *x)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	printf("("); 
	redc(_MIPP_ x->a,x->a);
	otstr(_MIPP_ x->a,mr_mip->IOBUFF); 
	nres(_MIPP_ x->a,x->a);
	printf("%s,",mr_mip->IOBUFF); 
	redc(_MIPP_ x->b,x->b);
	otstr(_MIPP_ x->b,mr_mip->IOBUFF); 
	nres(_MIPP_ x->b,x->b);
	printf("%s)",mr_mip->IOBUFF);
}

void zzn4_print(_MIPD_ zzn4 *x)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	printf("("); zzn2_print(_MIPP_ &(x->a)); printf(","); zzn2_print(_MIPP_ &(x->b));  printf(")");
}

void zzn12_print(_MIPD_ zzn12 *x)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	printf("("); zzn4_print(_MIPP_ &(x->a)); printf(","); zzn4_print(_MIPP_ &(x->b)); printf(","); zzn4_print(_MIPP_ &(x->c)); printf(")"); printf("\n");
}

void ecn2_print(_MIPD_ ecn2 *P)
{
	#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
	#endif
	if (P->marker==MR_EPOINT_INFINITY)
	{
		printf("[Infinity]\n");
		return;
	}
	ecn2_norm(_MIPP_ P);
	printf("[");
	zzn2_print(_MIPP_ &(P->x));
	printf(",");
	zzn2_print(_MIPP_ &(P->y));
	printf("]\n");
}
*/
void zzn2_alloc(_MIPD_ zzn2 *x,char *mem,int *i)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	x->a=mirvar_mem(_MIPP_ mem, (*i)++);
	x->b=mirvar_mem(_MIPP_ mem, (*i)++);

}

void zzn4_alloc(_MIPD_ zzn4 *x,char *mem,int *i)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	zzn2_alloc(_MIPP_ &(x->a),mem,i);
	zzn2_alloc(_MIPP_ &(x->b),mem,i);
	x->unitary=FALSE;
}

void zzn12_alloc(_MIPD_ zzn12 *x,char *mem,int *i)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	zzn4_alloc(_MIPP_ &(x->a),mem,i);
	zzn4_alloc(_MIPP_ &(x->b),mem,i);
	zzn4_alloc(_MIPP_ &(x->c),mem,i);
	x->unitary=FALSE;
	x->miller=FALSE;
}

void ecn2_alloc(_MIPD_ ecn2 *P,char *mem,int *i)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	zzn2_alloc(_MIPP_ &(P->x),mem,i);
	zzn2_alloc(_MIPP_ &(P->y),mem,i);
	zzn2_alloc(_MIPP_ &(P->z),mem,i);
	P->marker=MR_EPOINT_INFINITY;
}

/* Frobenius x^p. Assumes p=1 mod 6 */

#define MR_ZZN12_POWQ_RESERVE 4

void zzn12_powq(_MIPD_ zzn2 *w,zzn12 *x)
{
	zzn2 ww,www;
	int num=0;
	BOOL ku,km;

#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_POWQ_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_POWQ_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_POWQ_RESERVE));
#endif
	zzn2_alloc(_MIPP_ &ww,mem,&num);
	zzn2_alloc(_MIPP_ &www,mem,&num);

	zzn2_sqr(_MIPP_ w,&ww);
	zzn2_mul(_MIPP_ w,&ww,&www);

    ku=x->unitary;
	km=x->miller;

	zzn4_powq(_MIPP_ &www,&(x->a));
	zzn4_powq(_MIPP_ &www,&(x->b));
	zzn4_powq(_MIPP_ &www,&(x->c));

	zzn4_smul(_MIPP_ &(x->b),w,&(x->b));
	zzn4_smul(_MIPP_ &(x->c),&ww,&(x->c));

	x->unitary=ku;
	x->miller=km;
#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_POWQ_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_POWQ_RESERVE));
#endif

}

BOOL zzn12_iszero(zzn12 *x)
{
    if (zzn4_iszero(&(x->a)) && zzn4_iszero(&(x->b)) && zzn4_iszero(&(x->c))) return TRUE;
    return FALSE;
}

void zzn12_copy(zzn12 *x,zzn12 *w)
{
    if (x==w) return;
    zzn4_copy(&(x->a),&(w->a));
    zzn4_copy(&(x->b),&(w->b));
	zzn4_copy(&(x->c),&(w->c));
	w->unitary=x->unitary;
	w->miller=x->miller;
}

void zzn12_from_int(_MIPD_ int i,zzn12 *x)
{
	zzn4_from_int(_MIPP_ i,&(x->a));
	zzn4_zero(&(x->b));
	zzn4_zero(&(x->c));
	x->unitary=FALSE;
	x->miller=FALSE;
	if (i==1) x->unitary=TRUE;
}

void zzn12_from_zzn4s(zzn4 *x,zzn4 *y,zzn4 *z,zzn12 *w)
{
    zzn4_copy(x,&(w->a));
    zzn4_copy(y,&(w->b));
    zzn4_copy(z,&(w->c));
	w->unitary=FALSE;
	w->miller=FALSE;
}

void zzn12_conj(_MIPD_ zzn12 *x,zzn12 *w)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	zzn12_copy(x,w);
	zzn4_conj(_MIPP_ &(w->a),&(w->a));
	zzn4_conj(_MIPP_ &(w->b),&(w->b));
	zzn4_conj(_MIPP_ &(w->c),&(w->c));
   
    zzn4_negate(_MIPP_ &(w->b),&(w->b));
}

BOOL zzn12_compare(zzn12 *x,zzn12 *y)
{
    if (zzn4_compare(&(x->a),&(y->a)) && zzn4_compare(&(x->b),&(y->b)) && zzn4_compare(&(x->c),&(y->c))) return TRUE;
    return FALSE;
}

#define MR_ZZN12_SQR_RESERVE 16

void zzn12_sqr(_MIPD_ zzn12 *x,zzn12 *w)
{
	int num=0;
	zzn4 A,B,C,D;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_SQR_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_SQR_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_SQR_RESERVE));
#endif
	zzn4_alloc(_MIPP_ &A,mem,&num);
	zzn4_alloc(_MIPP_ &B,mem,&num);
	zzn4_alloc(_MIPP_ &C,mem,&num);
	zzn4_alloc(_MIPP_ &D,mem,&num);

    zzn12_copy(x,w);
	if (x->unitary)
	{ /* Granger & Scott PKC 2010 - only 3 squarings! */

/*		A=a; a*=a; D=a; a+=a; a+=D; A.conj(); A+=A; a-=A; */
		zzn4_copy(&(w->a),&A);
		zzn4_sqr(_MIPP_ &(w->a),&(w->a));
		zzn4_copy(&(w->a),&D);
		zzn4_add(_MIPP_ &(w->a),&(w->a),&(w->a));
		zzn4_add(_MIPP_ &(w->a),&D,&(w->a));
		zzn4_conj(_MIPP_ &A,&A);
		zzn4_add(_MIPP_ &A,&A,&A);
		zzn4_sub(_MIPP_ &(w->a),&A,&(w->a));

/* 		B=c; B*=B; B=tx(B); D=B; B+=B; B+=D; */
		zzn4_copy(&(w->c),&B);
		zzn4_sqr(_MIPP_ &B,&B);
		zzn4_tx(_MIPP_ &B);
		zzn4_copy(&B,&D);
		zzn4_add(_MIPP_ &B,&B,&B);
		zzn4_add(_MIPP_ &B,&D,&B);

/* 		C=b; C*=C;          D=C; C+=C; C+=D; */
		zzn4_copy(&(w->b),&C);
		zzn4_sqr(_MIPP_ &C,&C);
		zzn4_copy(&C,&D);
		zzn4_add(_MIPP_ &C,&C,&C);
		zzn4_add(_MIPP_ &C,&D,&C);

/* 		b.conj(); b+=b; c.conj(); c+=c; c=-c;
		b+=B; c+=C; */
		zzn4_conj(_MIPP_ &(w->b),&(w->b));
		zzn4_add(_MIPP_ &(w->b),&(w->b),&(w->b));
		zzn4_conj(_MIPP_ &(w->c),&(w->c));
		zzn4_add(_MIPP_ &(w->c),&(w->c),&(w->c));
		zzn4_negate(_MIPP_ &(w->c),&(w->c));
		zzn4_add(_MIPP_ &(w->b),&B,&(w->b));
		zzn4_add(_MIPP_ &(w->c),&C,&(w->c));

	}
	else
	{
	if (!x->miller)
	{
/*			A=a; A*=A;	B=b*c; B+=B; */
		zzn4_copy(&(w->a),&A);
		zzn4_sqr(_MIPP_ &A,&A);
		zzn4_mul(_MIPP_ &(w->b),&(w->c),&B);
		zzn4_add(_MIPP_ &B,&B,&B);	

/*			C=c; C*=C;	D=a*b; D+=D; c+=(a+b); c*=c; */
		zzn4_copy(&(w->c),&C);
		zzn4_sqr(_MIPP_ &C,&C);
		zzn4_mul(_MIPP_ &(w->a),&(w->b),&D);
		zzn4_add(_MIPP_ &D,&D,&D);
		zzn4_add(_MIPP_ &(w->c),&(w->a),&(w->c));
		zzn4_add(_MIPP_ &(w->c),&(w->b),&(w->c));
		zzn4_sqr(_MIPP_ &(w->c),&(w->c));

/*			a=A+tx(B);	b=D+tx(C);	c-=(A+B+C+D); */
		zzn4_sub(_MIPP_ &(w->c),&A,&(w->c));
		zzn4_sub(_MIPP_ &(w->c),&B,&(w->c));
		zzn4_sub(_MIPP_ &(w->c),&C,&(w->c));
		zzn4_sub(_MIPP_ &(w->c),&D,&(w->c));
		zzn4_tx(_MIPP_ &B); zzn4_tx(_MIPP_ &C);
		zzn4_add(_MIPP_ &A,&B,&(w->a));
		zzn4_add(_MIPP_ &D,&C,&(w->b));

	}
	else
	{
/* 			A=a; A*=A;  C=c; C*=b; C+=C; D=c; D*=D; c+=a;  */ 
		zzn4_copy(&(w->a),&A);
		zzn4_sqr(_MIPP_ &A,&A);
		zzn4_copy(&(w->c),&C);
		zzn4_mul(_MIPP_ &C,&(w->b),&C);
		zzn4_add(_MIPP_ &C,&C,&C);
		zzn4_copy(&(w->c),&D);
		zzn4_sqr(_MIPP_ &D,&D);
		zzn4_add(_MIPP_ &(w->c),&(w->a), &(w->c));

/* 			B=b; B+=c; B*=B; c-=b; c*=c; */
		zzn4_copy(&(w->b),&B);
		zzn4_add(_MIPP_ &B,&(w->c),&B);
		zzn4_sqr(_MIPP_ &B,&B);
		zzn4_sub(_MIPP_ &(w->c),&(w->b),&(w->c));
		zzn4_sqr(_MIPP_ &(w->c),&(w->c));

/* 			C+=C; A+=A; D+=D;  */
		zzn4_add(_MIPP_ &C,&C,&C);
		zzn4_add(_MIPP_ &A,&A,&A);
		zzn4_add(_MIPP_ &D,&D,&D);

/* 			a=A+tx(C); b=B-c-C+tx(D); c+=B-A-D;   */
		zzn4_sub(_MIPP_ &B,&(w->c),&(w->b));
		zzn4_sub(_MIPP_ &(w->b),&C,&(w->b));
		zzn4_tx(_MIPP_ &C);
		zzn4_add(_MIPP_ &A,&C,&(w->a));
		zzn4_sub(_MIPP_ &(w->c),&D,&(w->c));
		zzn4_tx(_MIPP_ &D);
		zzn4_add(_MIPP_ &(w->b),&D,&(w->b));
		zzn4_sub(_MIPP_ &(w->c),&A,&(w->c));
		zzn4_add(_MIPP_ &(w->c),&B,&(w->c));
	}
	}
#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_SQR_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_SQR_RESERVE));
#endif
}

#define MR_ZZN12_MUL_RESERVE 24

void zzn12_mul(_MIPD_ zzn12 *x,zzn12 *y,zzn12 *w)
{
	int num=0;
    zzn4 Z0,Z1,Z2,Z3,T0,T1;
	BOOL zero_c,zero_b;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_MUL_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_MUL_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_MUL_RESERVE));
#endif
	zzn4_alloc(_MIPP_ &Z0,mem,&num);
	zzn4_alloc(_MIPP_ &Z1,mem,&num);
	zzn4_alloc(_MIPP_ &Z2,mem,&num);
	zzn4_alloc(_MIPP_ &Z3,mem,&num);
	zzn4_alloc(_MIPP_ &T0,mem,&num);
	zzn4_alloc(_MIPP_ &T1,mem,&num);

	zero_c=zzn4_iszero(&(y->c));
	zero_b=zzn4_iszero(&(y->b));

/*     Z0=a*x.a; if (!zero_b) Z2=b*x.b;  */

	zzn4_mul(_MIPP_ &(x->a),&(y->a),&Z0);
	if (!zero_b) zzn4_mul(_MIPP_ &(x->b),&(y->b),&Z2);

/*     T0=a+b; T1=x.a+x.b; Z1=T0*T1; Z1-=Z0; if (!zero_b) Z1-=Z2; T0=b+c; T1=x.b+x.c; Z3=T0*T1; if (!zero_b) Z3-=Z2; */
	zzn4_add(_MIPP_ &(x->a),&(x->b),&T0);
	zzn4_add(_MIPP_ &(y->a),&(y->b),&T1);
	zzn4_mul(_MIPP_ &T0,&T1,&Z1);
	zzn4_sub(_MIPP_ &Z1,&Z0,&Z1);
	if (!zero_b) zzn4_sub(_MIPP_ &Z1,&Z2,&Z1);
	zzn4_add(_MIPP_ &(x->b),&(x->c),&T0);
	zzn4_add(_MIPP_ &(y->b),&(y->c),&T1);
	zzn4_mul(_MIPP_ &T0,&T1,&Z3);
	if (!zero_b) zzn4_sub(_MIPP_ &Z3,&Z2,&Z3);

/*     T0=a+c; T1=x.a+x.c; T0*=T1; if (!zero_b) Z2+=T0; else Z2=T0; Z2-=Z0; */
	zzn4_add(_MIPP_ &(x->a),&(x->c),&T0);
	zzn4_add(_MIPP_ &(y->a),&(y->c),&T1);
	zzn4_mul(_MIPP_ &T0,&T1,&T0);
	if (!zero_b) zzn4_add(_MIPP_ &Z2,&T0,&Z2);
	else         zzn4_copy(&T0,&Z2);
	zzn4_sub(_MIPP_ &Z2,&Z0,&Z2);

/* 	b=Z1; if (!zero_c) 	{ Z4=c*x.c; Z2-=Z4; Z3-=Z4; b+=tx(Z4);} a=Z0+tx(Z3); c=Z2; */
    zzn4_copy(&Z1,&(w->b));
	if (!zero_c)
	{
		zzn4_mul(_MIPP_ &(x->c),&(y->c),&T0);
		zzn4_sub(_MIPP_ &Z2,&T0,&Z2);
		zzn4_sub(_MIPP_ &Z3,&T0,&Z3);
		zzn4_tx(_MIPP_ &T0);
		zzn4_add(_MIPP_ &(w->b),&T0,&(w->b));
	}
	zzn4_tx(_MIPP_ &Z3);
	zzn4_add(_MIPP_ &Z0,&Z3,&(w->a));
	zzn4_copy(&Z2,&(w->c));

	if (x->unitary && y->unitary) w->unitary=TRUE;
	else w->unitary=FALSE;

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_MUL_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_MUL_RESERVE));
#endif
}

#define MR_ZZN12_INV_RESERVE 16

void zzn12_inv(_MIPD_ zzn12 *w)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	int num=0;
	zzn4 f0,f1,f2,f3;
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_INV_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_INV_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_INV_RESERVE));
#endif
	zzn4_alloc(_MIPP_ &f0,mem,&num);
	zzn4_alloc(_MIPP_ &f1,mem,&num);
	zzn4_alloc(_MIPP_ &f2,mem,&num);
	zzn4_alloc(_MIPP_ &f3,mem,&num);

	if (w->unitary)
	{
		zzn12_conj(_MIPP_ w,w);
	}
	else
	{
		zzn4_sqr(_MIPP_ &(w->a),&f0);
		zzn4_mul(_MIPP_ &(w->b),&(w->c),&f1);
		zzn4_tx(_MIPP_ &f1);
		zzn4_sub(_MIPP_ &f0,&f1,&f0);

		zzn4_sqr(_MIPP_ &(w->c),&f1);
		zzn4_tx(_MIPP_ &f1);
		zzn4_mul(_MIPP_ &(w->a),&(w->b),&f2);
		zzn4_sub(_MIPP_ &f1,&f2,&f1);

		zzn4_sqr(_MIPP_ &(w->b),&f2);
		zzn4_mul(_MIPP_ &(w->a),&(w->c),&f3);
		zzn4_sub(_MIPP_ &f2,&f3,&f2);

		zzn4_mul(_MIPP_ &(w->b),&f2,&f3);
		zzn4_tx(_MIPP_ &f3);
		zzn4_mul(_MIPP_ &(w->a),&f0,&(w->a));
		zzn4_add(_MIPP_ &f3,&(w->a),&f3);
		zzn4_mul(_MIPP_ &(w->c),&f1,&(w->c));
		zzn4_tx(_MIPP_ &(w->c));
		zzn4_add(_MIPP_ &f3,&(w->c),&f3);

		zzn4_inv(_MIPP_ &f3);

		zzn4_mul(_MIPP_ &f0,&f3,&(w->a));
		zzn4_mul(_MIPP_ &f1,&f3,&(w->b));
		zzn4_mul(_MIPP_ &f2,&f3,&(w->c));

	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_INV_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_INV_RESERVE));
#endif
}


/* very simple powering - k is always sparse, but could be negative - w=x^k
   NOTE x and w MUST be distinct */

void zzn12_pow(_MIPD_ zzn12 *x,big k,zzn12 *w)
{
	int i,nb,num=0;
	BOOL invert_it=FALSE;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	big e=mr_mip->w15;

	if (size(k)<0)
	{
		invert_it=TRUE;
		negify(k,e);
	}
	else copy(k,e);

	if (size(e)==0)
		zzn12_from_int(_MIPP_ 1, w);
	else
	{
		nb=logb2(_MIPP_ e);

		zzn12_copy(x,w);
		if (nb>1) for (i=nb-2;i>=0;i--)
		{
			zzn12_sqr(_MIPP_ w,w);
			if (mr_testbit(_MIPP_ e,i)) zzn12_mul(_MIPP_ w,x,w);
		}
		if (invert_it) zzn12_inv(_MIPP_ w);
	}
}

void trace(_MIPD_ zzn12 *x,zzn4 *r)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif

	zzn4_imul(_MIPP_ &(x->a),3,r);
}

#define MR_XTR_A_RESERVE 8

void xtr_A(_MIPD_ zzn4 *w,zzn4 *x,zzn4 *y,zzn4 *z,zzn4 *r)
{
	int num=0;
	zzn4 t1,t2;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_  MR_XTR_A_RESERVE);
#else
    char mem[MR_BIG_RESERVE( MR_XTR_A_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE( MR_XTR_A_RESERVE));
#endif

	zzn4_alloc(_MIPP_ &t1,mem,&num);
	zzn4_alloc(_MIPP_ &t2,mem,&num);
    zzn4_copy(x,r);

	zzn4_sub(_MIPP_ w,y,&t1);
	zzn4_smul(_MIPP_ &t1,&(r->a),&t1);

	zzn4_add(_MIPP_ w,y,&t2);
	zzn4_smul(_MIPP_ &t2,&(r->b),&t2);
	zzn4_tx(_MIPP_ &t2);

	zzn4_add(_MIPP_ &t1,&t2,r);
	zzn4_add(_MIPP_ r,z,r);

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_XTR_A_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_XTR_A_RESERVE));
#endif
}

#define MR_XTR_D_RESERVE 4

void xtr_D(_MIPD_ zzn4 *x,zzn4 *r)
{
	int num=0;
	zzn4 w;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_  MR_XTR_D_RESERVE);
#else
    char mem[MR_BIG_RESERVE( MR_XTR_D_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE( MR_XTR_D_RESERVE));
#endif
	zzn4_alloc(_MIPP_ &w,mem,&num);
    zzn4_copy(x,r);
	zzn4_conj(_MIPP_ r,&w);
	zzn4_add(_MIPP_ &w,&w,&w);
	zzn4_sqr(_MIPP_ r,r);
	zzn4_sub(_MIPP_ r,&w,r);

#ifndef MR_STATIC
    memkill(_MIPP_ mem,  MR_XTR_D_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE( MR_XTR_D_RESERVE));
#endif
}

#define MR_XTR_POW_RESERVE 17

void xtr_pow(_MIPD_ zzn4 *x,big n,zzn4 *r)
{
	int i,par,nb,num=0;
	big v;
	zzn4 t,a,b,c;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_  MR_XTR_POW_RESERVE);
#else
    char mem[MR_BIG_RESERVE( MR_XTR_POW_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE( MR_XTR_POW_RESERVE));
#endif
	zzn4_alloc(_MIPP_ &t,mem,&num);
	zzn4_alloc(_MIPP_ &a,mem,&num);
	zzn4_alloc(_MIPP_ &b,mem,&num);
	zzn4_alloc(_MIPP_ &c,mem,&num);
	v=mirvar_mem(_MIPP_  mem, num++);
    
	zzn4_from_int(_MIPP_ 3,&a);
	zzn4_copy(x,&b);
	xtr_D(_MIPP_ x,&c);

	par=subdiv(_MIPP_ n,2,v);
	if (par==0)	decr(_MIPP_ v,1,v);

	nb=logb2(_MIPP_ v);

    for (i=nb-1;i>=0;i--)
    {
		if (!mr_testbit(_MIPP_ v,i))
		{
			zzn4_copy(&b,&t);
			zzn4_conj(_MIPP_ x,x);
			zzn4_conj(_MIPP_ &c,&c);
			xtr_A(_MIPP_ &a,&b,x,&c,&b);
			zzn4_conj(_MIPP_ x,x);
			xtr_D(_MIPP_ &t,&c);
			xtr_D(_MIPP_ &a,&a);
		}
		else
		{
			zzn4_conj(_MIPP_ &a,&t);
			xtr_D(_MIPP_ &b,&a);
			xtr_A(_MIPP_ &c,&b,x,&t,&b);
			xtr_D(_MIPP_ &c,&c);
		}
	}

	if (par==0) zzn4_copy(&c,r);
	else zzn4_copy(&b,r);

#ifndef MR_STATIC
    memkill(_MIPP_ mem,  MR_XTR_POW_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE( MR_XTR_POW_RESERVE));
#endif
}

/* apply endomorphism (x,y) = (Beta*x,y) where Beta is cube root of unity */

void endomorph(_MIPD_ big beta,epoint *P)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	nres_modmult(_MIPP_ P->X,beta,P->X);
}

/* Fast multiplication of A by p (for Trace-Zero group members only) */

#define MR_ZZN12_QPF_RESERVE 4

void q_power_frobenius(_MIPD_ zzn2 *f,ecn2 *A)
{
	int num=0;
	zzn2 w,r;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_QPF_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_QPF_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_QPF_RESERVE));
#endif

	zzn2_alloc(_MIPP_ &w,mem,&num);
	zzn2_alloc(_MIPP_ &r,mem,&num);

	zzn2_copy(f,&r);
	if (mr_mip->TWIST==MR_SEXTIC_M) zzn2_inv(_MIPP_  &r);  /* could be precalculated */

	zzn2_sqr(_MIPP_ &r,&w);
	zzn2_conj(_MIPP_ &(A->x),&(A->x));
	zzn2_conj(_MIPP_ &(A->y),&(A->y));
	zzn2_conj(_MIPP_ &(A->z),&(A->z));

	zzn2_mul(_MIPP_ &(A->x),&w,&(A->x));
	zzn2_mul(_MIPP_ &(A->y),&w,&(A->y));
	zzn2_mul(_MIPP_ &(A->y),&r,&(A->y));
#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_QPF_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_QPF_RESERVE));
#endif
}

#define MR_ZZN12_LINE_RESERVE 20

void line(_MIPD_ ecn2 *A,ecn2 *C,ecn2 *B,zzn2 *lam,zzn2 *extra,BOOL Doubling,big Qx,big Qy,zzn12 *w)
{
	int num=0;
	zzn4 nn,dd,cc;
	zzn2 t1,t2,t3,gz;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_LINE_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_LINE_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_LINE_RESERVE));
#endif
	zzn4_alloc(_MIPP_ &nn,mem,&num);
	zzn4_alloc(_MIPP_ &dd,mem,&num);
	zzn4_alloc(_MIPP_ &cc,mem,&num);
	zzn2_alloc(_MIPP_ &t1,mem,&num);
	zzn2_alloc(_MIPP_ &t2,mem,&num);
	zzn2_alloc(_MIPP_ &t3,mem,&num);
	zzn2_alloc(_MIPP_ &gz,mem,&num);

	if (Doubling)
	{
		ecn2_getz(_MIPP_ A,&t1);
		zzn2_sqr(_MIPP_ &t1,&t1);
		ecn2_getz(_MIPP_ C,&gz);
		if (mr_mip->TWIST==MR_SEXTIC_M)
		{
			zzn2_mul(_MIPP_ &gz,&t1,&t2);
			zzn2_from_zzn(Qy,&t3);
			zzn2_txx(_MIPP_ &t3);
			zzn2_mul(_MIPP_ &t3,&t2,&t3);
			zzn2_mul(_MIPP_ lam,&(A->x),&t2);
			zzn2_sub(_MIPP_ &t2,extra,&t2);
			zzn4_from_zzn2s(&t3,&t2,&nn);
			zzn2_mul(_MIPP_ &t1,lam,&t2);
			zzn2_smul(_MIPP_ &t2,Qx,&t2);
			zzn2_negate(_MIPP_ &t2,&t2);
			zzn4_from_zzn2h(&t2,&cc);
			zzn4_zero(&dd);
		}
		if (mr_mip->TWIST==MR_SEXTIC_D)
		{
			zzn2_mul(_MIPP_ &gz,&t1,&t3);
			zzn2_smul(_MIPP_ &t3,Qy,&t3);
			zzn2_mul(_MIPP_ lam,&(A->x),&t2);
			zzn2_sub(_MIPP_ &t2,extra,&t2);
			zzn4_from_zzn2s(&t3,&t2,&nn);
			zzn4_zero(&cc);
			zzn2_mul(_MIPP_ &t1,lam,&t2);
			zzn2_smul(_MIPP_ &t2,Qx,&t2);
			zzn2_negate(_MIPP_ &t2,&t2);
			zzn4_from_zzn2(&t2,&dd);
		}
	}
	else
	{
		ecn2_getz(_MIPP_ C,&gz);
		if (mr_mip->TWIST==MR_SEXTIC_M)
		{
			zzn2_from_zzn(Qy,&t3);
			zzn2_txx(_MIPP_ &t3);
			zzn2_mul(_MIPP_ &t3,&gz,&t3);
			zzn2_mul(_MIPP_ &gz,&(B->y),&t1);
			zzn2_mul(_MIPP_ lam,&(B->x),&t2);
			zzn2_sub(_MIPP_ &t2,&t1,&t2);
			zzn4_from_zzn2s(&t3,&t2,&nn);
			zzn2_smul(_MIPP_ lam,Qx,&t2);
			zzn2_negate(_MIPP_ &t2,&t2);
			zzn4_from_zzn2h(&t2,&cc);
			zzn4_zero(&dd);
		}
		if (mr_mip->TWIST==MR_SEXTIC_D)
		{
			zzn2_smul(_MIPP_ &gz,Qy,&t3);
			zzn2_mul(_MIPP_ &gz,&(B->y),&t1);
			zzn2_mul(_MIPP_ lam,&(B->x),&t2);
			zzn2_sub(_MIPP_ &t2,&t1,&t2);
			zzn4_from_zzn2s(&t3,&t2,&nn);
			zzn2_smul(_MIPP_ lam,Qx,&t2);
			zzn2_negate(_MIPP_ &t2,&t2);
			zzn4_from_zzn2(&t2,&dd);
			zzn4_zero(&cc);
		}
	}
	zzn12_from_zzn4s(&nn,&dd,&cc,w);

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_LINE_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_LINE_RESERVE));
#endif
}

#define MR_ZZN12_G_RESERVE 10

void g(_MIPD_ ecn2* A,ecn2 *B,big Qx,big Qy,zzn12 *w)
{
	int num=0;
	zzn2 lam,extra;
	ecn2 P;
	BOOL Doubling;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_G_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_G_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_G_RESERVE));
#endif
	zzn2_alloc(_MIPP_ &lam,mem,&num);
	zzn2_alloc(_MIPP_ &extra,mem,&num);
	ecn2_alloc(_MIPP_ &P,mem,&num);

	ecn2_copy(A,&P);
	Doubling=ecn2_add2(_MIPP_ B,A,&lam,&extra);

	if (ecn2_iszero(A))
	{
		zzn12_from_int(_MIPP_ 1,w);
	}
	else
	{
		line(_MIPP_ &P,A,B,&lam,&extra,Doubling,Qx,Qy,w);
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_G_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_G_RESERVE));
#endif
}

/* R-ate Pairing G2 x G1 -> GT */
#define MR_ZZN12_MILLER_RESERVE 31
BOOL rate_miller(_MIPD_ ecn2 *P,epoint *Q,big x,zzn2 *f,zzn12 *res)
{
 int i,nb,num=0;
 ecn2 A,KA;
 zzn2 AX,AY;
 big n,qx,qy;
 zzn12 t0;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_MILLER_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_MILLER_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_MILLER_RESERVE));
#endif
 n=mirvar_mem(_MIPP_  mem, num++);
 qx=mirvar_mem(_MIPP_  mem, num++);
 qy=mirvar_mem(_MIPP_  mem, num++);
 zzn2_alloc(_MIPP_ &AX,mem,&num);
 zzn2_alloc(_MIPP_ &AY,mem,&num);
 ecn2_alloc(_MIPP_ &A,mem,&num);
 ecn2_alloc(_MIPP_ &KA,mem,&num);
 zzn12_alloc(_MIPP_ &t0,mem,&num);
 premult(_MIPP_ x,6,n);
 incr(_MIPP_ n,2,n);
 if (size(x)<0) negify(n,n);
 ecn2_copy(P,&A);
 nb=logb2(_MIPP_ n);
 zzn12_from_int(_MIPP_ 1,res);
 res->miller=TRUE;
 epoint_norm(_MIPP_ Q);
 copy(Q->X,qx);
 copy(Q->Y,qy);
 for (i=nb-2;i>=0;i--)
 {
  zzn12_sqr(_MIPP_ res,res);
  g(_MIPP_ &A,&A,qx,qy,&t0);
  zzn12_mul(_MIPP_ res,&t0,res);
  if (mr_testbit(_MIPP_ n,i))
  {
   g(_MIPP_ &A,P,qx,qy,&t0);
   zzn12_mul(_MIPP_ res,&t0,res);
  }
 }
 ecn2_copy(P,&KA);
 q_power_frobenius(_MIPP_ f,&KA);
 if (size(x)<0)
 {
  ecn2_negate(_MIPP_ &A,&A);
  zzn12_conj(_MIPP_ res,res);
 }
 g(_MIPP_ &A,&KA,qx,qy,&t0);
 zzn12_mul(_MIPP_ res,&t0,res);
 
 q_power_frobenius(_MIPP_ f,&KA);
 ecn2_negate(_MIPP_ &KA,&KA);
 g(_MIPP_ &A,&KA,qx,qy,&t0);
 zzn12_mul(_MIPP_ res,&t0,res);
 zzn12_copy(res,&t0);
 zzn12_conj(_MIPP_ res,res);
 zzn12_inv(_MIPP_ &t0);
 
 zzn12_mul(_MIPP_ res,&t0,res);
 res->miller=FALSE;
 zzn12_copy(res,&t0);
 zzn12_powq(_MIPP_ f,res);
 zzn12_powq(_MIPP_ f,res);
 zzn12_mul(_MIPP_ res,&t0,res);
 if (zzn12_iszero(res)) return FALSE;
#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_MILLER_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_MILLER_RESERVE));
#endif
 return TRUE;
}


#define MR_ZZN12_FEXP_RESERVE 72
void rate_fexp(_MIPD_ big x,zzn2 *f,zzn12 *res)
{
 int num=0;
 zzn12 x0,x1,x2,x3,x4,x5;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_FEXP_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_FEXP_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_FEXP_RESERVE));
#endif
 zzn12_alloc(_MIPP_ &x0,mem,&num);
 zzn12_alloc(_MIPP_ &x1,mem,&num);
 zzn12_alloc(_MIPP_ &x2,mem,&num);
 zzn12_alloc(_MIPP_ &x3,mem,&num);
 zzn12_alloc(_MIPP_ &x4,mem,&num);
 zzn12_alloc(_MIPP_ &x5,mem,&num);
 res->unitary=TRUE;
 negify(x,x);
 zzn12_copy(res,&x5);
 zzn12_powq(_MIPP_ f,&x5);
 zzn12_copy(&x5,&x0);
 zzn12_powq(_MIPP_ f,&x0);
 zzn12_mul(_MIPP_ &x5,res,&x5);
 zzn12_mul(_MIPP_ &x0,&x5,&x0);
 zzn12_powq(_MIPP_ f,&x0);
 zzn12_conj(_MIPP_ res,&x1);
 zzn12_pow(_MIPP_ res,x,&x4); /* pow */
 zzn12_pow(_MIPP_ &x4,x,&x2); /* pow */
 zzn12_copy(&x4,&x3);
 zzn12_powq(_MIPP_ f,&x3);
 zzn12_copy(&x2,res);
 zzn12_pow(_MIPP_ res,x,&x5); /* pow */
 zzn12_copy(&x5,res);
 zzn12_powq(_MIPP_ f,res);
 zzn12_mul(_MIPP_ &x5,res,res);
 zzn12_conj(_MIPP_ &x2,&x5);
 zzn12_powq(_MIPP_ f,&x2);
 zzn12_conj(_MIPP_ &x2,&x2);
 zzn12_mul(_MIPP_ &x4,&x2,&x4);
 zzn12_conj(_MIPP_ &x2,&x2);
 zzn12_powq(_MIPP_ f,&x2);
 negify(x,x);
 zzn12_sqr(_MIPP_ res,res);
 zzn12_mul(_MIPP_ res,&x4,&x4);
 zzn12_mul(_MIPP_ &x4,&x5,&x4);
 zzn12_mul(_MIPP_ &x3,&x5,res);
 zzn12_mul(_MIPP_ res,&x4,res);
 zzn12_mul(_MIPP_ &x4,&x2,&x4);
 zzn12_sqr(_MIPP_ res,res);
 zzn12_mul(_MIPP_ res,&x4,res);
 zzn12_sqr(_MIPP_ res,res);
 zzn12_mul(_MIPP_ res,&x1,&x4);
 zzn12_mul(_MIPP_ res,&x0,res);
 zzn12_sqr(_MIPP_ &x4,&x4);
 zzn12_mul(_MIPP_ &x4,res,&x4);
 zzn12_copy(&x4,res);
#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_FEXP_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_FEXP_RESERVE));
#endif
 return;
}

#define MR_ZZN12_COF_RESERVE 12
/* Faster Hashing to G2 - Fuentes-Castaneda, Knapp and Rodriguez-Henriquez */
void cofactor(_MIPD_ zzn2 *f,big x,ecn2 *S)
{
	int num=0;
	ecn2 T,K;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_COF_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_COF_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_COF_RESERVE));
#endif
	ecn2_alloc(_MIPP_ &T,mem,&num);
	ecn2_alloc(_MIPP_ &K,mem,&num);

	ecn2_copy(S,&T);
	ecn2_mul(_MIPP_ x,&T);
	ecn2_norm(_MIPP_ &T);
	ecn2_copy(&T,&K);
	ecn2_add(_MIPP_ &K,&K);
	ecn2_add(_MIPP_ &T,&K);
	ecn2_norm(_MIPP_ &K);

	q_power_frobenius(_MIPP_ f,&K);
	q_power_frobenius(_MIPP_ f,S);
	q_power_frobenius(_MIPP_ f,S);
	q_power_frobenius(_MIPP_ f,S);

	ecn2_add(_MIPP_ &T,S);
	ecn2_add(_MIPP_ &K,S);

	q_power_frobenius(_MIPP_ f,&T);
	q_power_frobenius(_MIPP_ f,&T);

	ecn2_add(_MIPP_ &T,S);
	ecn2_norm(_MIPP_ S);

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_COF_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_COF_RESERVE));
#endif
}

#define MR_ZZN12_MEM_RESERVE 25
/* test if a ZZn12 element is of order q
   test r^q = r^p+1-t =1, so test r^p=r^(t-1) */
BOOL member(_MIPD_ zzn2 *f,big x,zzn12 *m)
{
	int num=0;
	zzn12 w,r;
	big six;
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_MEM_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_MEM_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_MEM_RESERVE));
#endif
	zzn12_alloc(_MIPP_ &w,mem,&num);
	zzn12_alloc(_MIPP_ &r,mem,&num);
	six=mirvar_mem(_MIPP_  mem, num++);

	convert(_MIPP_ 6,six);

	zzn12_copy(m,&r);
	zzn12_pow(_MIPP_ &r,x,&w); zzn12_pow(_MIPP_ &w,x,&r); zzn12_pow(_MIPP_ &r,six,&w); zzn12_copy(&w,&r);
	zzn12_copy(m,&w);
	zzn12_powq(_MIPP_ f,&w);

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_MEM_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_MEM_RESERVE));
#endif
	return zzn12_compare(&w,&r);
}

void glv(_MIPD_ big e,big r,big W[2],big B[2][2],big u[2])
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	int i,j;
	big v[2];
	v[0]=mr_mip->w3;
	v[1]=mr_mip->w4;
	for (i=0;i<2;i++)
	{
		mad(_MIPP_ W[i],e,e,r,v[i],v[i]);
		zero(u[i]);
	}
	copy(e,u[0]);
	for (i=0;i<2;i++)
		for (j=0;j<2;j++)
		{
			multiply(_MIPP_ v[j],B[j][i],mr_mip->w5);
			subtract(_MIPP_ u[i],mr_mip->w5,u[i]);
		}
}

void galscott(_MIPD_ big e,big r,big W[4],big B[4][4],big u[4])
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	int i,j;
	big v[4];
	v[0]=mr_mip->w3;
	v[1]=mr_mip->w4;
	v[2]=mr_mip->w5;
	v[3]=mr_mip->w6;
	for (i=0;i<4;i++)
	{
		mad(_MIPP_ W[i],e,e,r,v[i],v[i]);
		zero(u[i]);
	}

	copy(e,u[0]);
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
		{
			multiply(_MIPP_ v[j],B[j][i],mr_mip->w7);
			subtract(_MIPP_ u[i],mr_mip->w7,u[i]);
		}
}

/* generates p, r and beta very quickly from x for BN curves */

void getprb(_MIPD_ big x,big p,big r,big beta)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	/* p=36*pow(x,4)+36*pow(x,3)+24*x*x+6*x+1; */

	multiply(_MIPP_ x,x,mr_mip->w1); /* x^2 */
	multiply(_MIPP_ x,mr_mip->w1,mr_mip->w2); /* x^3 */
	multiply(_MIPP_ mr_mip->w1,mr_mip->w1,mr_mip->w3); /* x^4 */
	
	add(_MIPP_ mr_mip->w2,mr_mip->w3,p);
	premult(_MIPP_ p,36,p);
	copy(p,r);  /* p = r = 36x^4+36x^3 */
	
	add(_MIPP_ mr_mip->w1,mr_mip->w2,beta);
	premult(_MIPP_ beta,18,beta); /* beta=18x^3+18x^2 */

	premult(_MIPP_ mr_mip->w1,6,mr_mip->w1); /* w1=6x^2 */

	premult(_MIPP_ mr_mip->w1,3,mr_mip->w4);
	add(_MIPP_ r,mr_mip->w4,r);    /* r= 36x^4+36x^3 +18x^3 */

	premult(_MIPP_ mr_mip->w1,4,mr_mip->w4);
	add(_MIPP_ p,mr_mip->w4,p); /* p= 36x^4+36x^3+24x^3 */

	premult(_MIPP_ x,3,mr_mip->w1); /* w1=3x */

	premult(_MIPP_ mr_mip->w1,2,mr_mip->w4); /* w4=6x */

	add(_MIPP_ p,mr_mip->w4,p);
	add(_MIPP_ r,mr_mip->w4,r);
	incr(_MIPP_ p,1,p);
	incr(_MIPP_ r,1,r);

	premult(_MIPP_ mr_mip->w1,3,mr_mip->w4);
	add(_MIPP_ beta,mr_mip->w4,beta);
	incr(_MIPP_ beta,2,beta);
	subtract(_MIPP_ p,beta,beta);

}

/* Calculate matrix for GLV method */
void matrix2(_MIPD_ big x,big W[2],big B[2][2])
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
/* 6x^2+4x+1 */
	premult(_MIPP_ x,3,W[0]);
	incr(_MIPP_ W[0],2,W[0]);
	premult(_MIPP_ W[0],2,W[0]); multiply(_MIPP_ W[0],x,W[0]);
	incr(_MIPP_ W[0],1,W[0]);
/* -(2x+1) */
	premult(_MIPP_ x,2,W[1]);
	incr(_MIPP_ W[1],1,W[1]);
	negify(W[1],W[1]);
/* 6x^2+2x */
	premult(_MIPP_ x,3,B[0][0]);
	incr(_MIPP_ B[0][0],1,B[0][0]);
	premult(_MIPP_ B[0][0],2,B[0][0]); multiply(_MIPP_ B[0][0],x,B[0][0]);

	copy(W[1],B[0][1]);
	copy(W[1],B[1][0]);
	negify(W[0],B[1][1]);
}

/* calculate matrix for Galbraith-Scott method */
void matrix4(_MIPD_ big x,big W[4],big B[4][4])
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
/* 2x^2+3x+1 */
	premult(_MIPP_ x,2,W[0]);
	incr(_MIPP_ W[0],3,W[0]);
	multiply(_MIPP_ W[0],x,W[0]);
	incr(_MIPP_ W[0],1,W[0]);
/* 12x^3+8x^2+x */
	premult(_MIPP_ x,3,W[1]);
	incr(_MIPP_ W[1],2,W[1]);
	premult(_MIPP_ W[1],4,W[1]); multiply(_MIPP_ W[1],x,W[1]);
	incr(_MIPP_ W[1],1,W[1]);
	multiply(_MIPP_ W[1],x,W[1]);
/* 6x^3+4x^2+x */
	premult(_MIPP_ x,3,W[2]);
	incr(_MIPP_ W[2],2,W[2]);
	premult(_MIPP_ W[2],2,W[2]); multiply(_MIPP_ W[2],x,W[2]);
	incr(_MIPP_ W[2],1,W[2]);
	multiply(_MIPP_ W[2],x,W[2]);
/* -2*x*x-x */
	premult(_MIPP_ x,2,W[3]);
	incr(_MIPP_ W[3],1,W[3]);
	multiply(_MIPP_ W[3],x,W[3]);
	negify(W[3],W[3]);

	incr(_MIPP_ x,1,B[0][0]);
	copy(x,B[0][1]);
	copy(x,B[0][2]);
	premult(_MIPP_ x,-2,B[0][3]);

	premult(_MIPP_ x,2,B[1][0]);
	incr(_MIPP_ B[1][0],1,B[1][0]);
	negify(x,B[1][1]);
	negify(B[0][0],B[1][2]);
	negify(x,B[1][3]);
	premult(_MIPP_ x,2,B[2][0]);
	incr(_MIPP_ B[2][0],1,B[2][1]);
	copy(B[2][1],B[2][2]);
	copy(B[2][1],B[2][3]);
	decr(_MIPP_ x,1,B[3][0]);
	premult(_MIPP_ B[2][1],2,B[3][1]);
	decr(_MIPP_ B[2][2],2,B[3][2]);
	negify(B[3][2],B[3][2]);
	copy(B[3][0],B[3][3]);
}


/* Use GLV endomorphism idea for multiplication in G1 - Q=e*P */

#define MR_ZZN12_G1M_RESERVE 8

void G1_mult(_MIPD_ epoint *P,big e,big beta,big r,big x,epoint *Q)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	int i,j,num=0;
	big u[2];
	big W[2],B[2][2];
	epoint *PP;
#ifndef MR_STATIC
	char *mem1=ecp_memalloc(_MIPP_ 1);
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_G1M_RESERVE);
#else
	char mem1[MR_ECP_RESERVE(1)];   
    char mem[MR_BIG_RESERVE(MR_ZZN12_G1M_RESERVE)];
    memset(mem1,0,MR_ECP_RESERVE(1)); 
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_G1M_RESERVE));
#endif
	PP=epoint_init_mem(_MIPP_ mem1,0); 
	
	for (i=0;i<2;i++)
	{
		u[i]=mirvar_mem(_MIPP_  mem, num++);
		W[i]=mirvar_mem(_MIPP_ mem,num++);
		for (j=0;j<2;j++)
			B[i][j]=mirvar_mem(_MIPP_ mem,num++);
	}

	matrix2(_MIPP_ x,W,B);

	glv(_MIPP_ e,r,W,B,u);
	epoint_copy(P,Q); epoint_copy(P,PP);
	endomorph(_MIPP_ beta,Q);
	ecurve_mult2(_MIPP_ u[0],PP,u[1],Q,Q);

#ifndef MR_STATIC
	ecp_memkill(_MIPP_ mem1,1);
    memkill(_MIPP_ mem, MR_ZZN12_G1M_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_G1M_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(1));
#endif
}

/*.. for multiplication in G2 */

#define MR_ZZN12_G2M_RESERVE 48

void G2_mult(_MIPD_ ecn2 *P,big e,zzn2 *f,big r,big x,ecn2 *Q)
{
#ifdef MR_OS_THREADS
    miracl *mr_mip=get_mip();
#endif
	int i,j,num=0;
	big u[4];
	big W[4],B[4][4];
	ecn2 PP[4];
#ifndef MR_STATIC
    char *mem = (char *)memalloc(_MIPP_ MR_ZZN12_G2M_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_ZZN12_G2M_RESERVE)];
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_G2M_RESERVE));
#endif
	for (i=0;i<4;i++)
	{
		u[i]=mirvar_mem(_MIPP_  mem, num++);
		W[i]=mirvar_mem(_MIPP_ mem,num++);
		for (j=0;j<4;j++)
			B[i][j]=mirvar_mem(_MIPP_ mem,num++);
	}

	ecn2_alloc(_MIPP_ &PP[0],mem,&num);
	ecn2_alloc(_MIPP_ &PP[1],mem,&num);
	ecn2_alloc(_MIPP_ &PP[2],mem,&num);
	ecn2_alloc(_MIPP_ &PP[3],mem,&num);

	matrix4(_MIPP_ x,W,B);
	galscott(_MIPP_ e,r,W,B,u);

	ecn2_copy(P,&PP[0]);
	for (i=1;i<4;i++)
	{
		ecn2_copy(&PP[i-1],&PP[i]); 
		q_power_frobenius(_MIPP_ f,&PP[i]);
	}

/* deal with -ve multipliers */
	for (i=0;i<4;i++)
	{
		if (size(u[i])<0)
			{negify(u[i],u[i]);  ecn2_negate(_MIPP_ &PP[i],&PP[i]);}
	}
	ecn2_mult4(_MIPP_ u,PP,Q);

#ifndef MR_STATIC
    memkill(_MIPP_ mem, MR_ZZN12_G2M_RESERVE);
#else
    memset(mem, 0, MR_BIG_RESERVE(MR_ZZN12_G2M_RESERVE));
#endif
}

///* Initialise a Cryptographically Strong Random Number Generator from 
//   an octet of raw random data */
//
//
//SAKKE_API void CREATE_CSPRNG(csprng *RNG,octet *RAW)
//{
//    strong_init(RNG,RAW->len,RAW->val,0L);
//}
//
//SAKKE_API void KILL_CSPRNG(csprng *RNG)
//{
//    strong_kill(RNG);
//}


/* Initialise the SAKKE_BN domain structure
 * It is assumed that the EC domain details are obtained from ROM
 */

#define MR_SAKKE_BN_INIT_RESERVE 26

SAKKE_API int SAKKE_DOMAIN_INIT(sak_domain *DOM,const void *rom)
{ /* get domain details from ROM     */
	int i,pt,num=0;
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,2*FS,16);
	/* printf("mips size: %d\n", sizeof(*mr_mip)); */
#else
    miracl *mr_mip=mirsys(2*FS,16);
#endif
    big x,q,r,px,py,a,b,beta,xx,yy,g[4];
	ecn2 Q;
	epoint *P;
	zzn2 f,qx,qy;
    int words,promptr,err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ MR_SAKKE_BN_INIT_RESERVE);
	char *mem1=(char *)ecp_memalloc(_MIPP_ 1);
#else
    char mem[MR_BIG_RESERVE(MR_SAKKE_BN_INIT_RESERVE)];
    char mem1[MR_ECP_RESERVE(1)];
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_INIT_RESERVE));
	memset(mem1,0,MR_ECP_RESERVE(1));
#endif
	DOM->nibbles=2*FS;
	words=MR_ROUNDUP(FS*8,MIRACL);

	if (mr_mip==NULL || mem==NULL) res= SAKKE_OUT_OF_MEMORY;

    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        x=mirvar_mem(_MIPP_ mem, num++);
        q=mirvar_mem(_MIPP_ mem, num++);
        a=mirvar_mem(_MIPP_ mem, num++);
		b=mirvar_mem(_MIPP_ mem, num++);
        r=mirvar_mem(_MIPP_ mem, num++);
        px=mirvar_mem(_MIPP_ mem, num++);
        py=mirvar_mem(_MIPP_ mem, num++);
        xx=mirvar_mem(_MIPP_ mem, num++);
		yy=mirvar_mem(_MIPP_ mem, num++);
        beta=mirvar_mem(_MIPP_ mem, num++);
		ecn2_alloc(_MIPP_ &Q,mem,&num);
		zzn2_alloc(_MIPP_ &f,mem,&num);
		zzn2_alloc(_MIPP_ &qx,mem,&num);
		zzn2_alloc(_MIPP_ &qy,mem,&num);
		for (i=0;i<4;i++) g[i]=mirvar_mem(_MIPP_ mem, num++);

/* read in from PROM and make simple integrity checks */

		promptr=0;
		init_big_from_rom(x,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);  /* Read in BN parameter from ROM   */
		sftbit(_MIPP_ x,-(FS*8-2),xx);  /* top 2 bits encode extra info */
		pt=size(xx);
		expb2(_MIPP_ (FS*8-2),xx);
		divide(_MIPP_ x,xx,xx);

		mr_mip->TWIST=MR_SEXTIC_D;
		if ((pt&1)==1) mr_mip->TWIST=MR_SEXTIC_M;
		if ((pt&2)==2) negify(x,x); 

		init_big_from_rom(b,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);  /* Read in curve parameter b from ROM   */

		getprb(_MIPP_ x,q,r,beta);
		zero(a);
		ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);

		nres(_MIPP_ beta,beta);

 		init_big_from_rom(px,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);  /* Read in curve parameter gx from ROM */
		init_big_from_rom(py,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);  /* Read in curve parameter gy from ROM */
        P=epoint_init_mem(_MIPP_ mem1,0);
        if (!epoint_set(_MIPP_ px,py,0,P)) res=SAKKE_INVALID_PUBLIC_KEY;

	}
	if (res==0)
	{
		init_big_from_rom(xx,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);
		init_big_from_rom(yy,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);

		zzn2_from_bigs(_MIPP_ xx,yy,&qx);
		init_big_from_rom(xx,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);
		init_big_from_rom(yy,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);

		zzn2_from_bigs(_MIPP_ xx,yy,&qy);
		if (!ecn2_set(_MIPP_ &qx,&qy,&Q))  res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		init_big_from_rom(xx,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);
		init_big_from_rom(yy,words,(const mr_small *)rom,words*ROM_SIZE,&promptr);
		zzn2_from_bigs(_MIPP_ xx,yy,&f);

		for (i=0;i<4;i++)
		{
			init_big_from_rom(g[i],words,(const mr_small *)rom,words*ROM_SIZE,&promptr);
			nres(_MIPP_ g[i],g[i]);
		}
 	
	}
	if (res==0)
	{
		DOM->flags=pt;
		big_to_bytes(_MIPP_ FS,x,DOM->X,TRUE); /* bigs here */
		big_to_bytes(_MIPP_ FS,q,DOM->Q,TRUE);
		big_to_bytes(_MIPP_ FS,a,DOM->A,TRUE);
		big_to_bytes(_MIPP_ FS,b,DOM->B,TRUE);
		big_to_bytes(_MIPP_ FS,r,DOM->R,TRUE);
		big_to_bytes(_MIPP_ FS,px,DOM->Px,TRUE); 
		big_to_bytes(_MIPP_ FS,py,DOM->Py,TRUE);
		big_to_bytes(_MIPP_ FS,beta,DOM->Beta,TRUE);/* nresidues from here */
		big_to_bytes(_MIPP_ FS,qx.a,DOM->Qxa,TRUE); 
		big_to_bytes(_MIPP_ FS,qx.b,DOM->Qxb,TRUE);
		big_to_bytes(_MIPP_ FS,qy.a,DOM->Qya,TRUE);
		big_to_bytes(_MIPP_ FS,qy.b,DOM->Qyb,TRUE);
		big_to_bytes(_MIPP_ FS,f.a,DOM->Fa,TRUE);
		big_to_bytes(_MIPP_ FS,f.b,DOM->Fb,TRUE);
		for (i=0;i<4;i++) big_to_bytes(_MIPP_ FS,g[i],DOM->G[i],TRUE);
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,MR_SAKKE_BN_INIT_RESERVE);
    ecp_memkill(_MIPP_ mem1,1);
#else
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_INIT_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(1));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return SAKKE_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return SAKKE_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

SAKKE_API void SAKKE_DOMAIN_KILL(sak_domain *DOM)
{
	int i,j;
	for (i=0;i<FS;i++)
	{
		DOM->X[i]=0;
		DOM->Q[i]=0;
		DOM->A[i]=0;
		DOM->B[i]=0;
		DOM->R[i]=0;
		DOM->Px[i]=0;
		DOM->Py[i]=0;
		DOM->Beta[i]=0;
		DOM->Qxa[i]=0;
		DOM->Qxb[i]=0;		
		DOM->Qya[i]=0;		
		DOM->Qyb[i]=0;	
		DOM->Fa[i]=0;
		DOM->Fb[i]=0;
		for (j=0;j<4;j++)
			DOM->G[j][i]=0;
	}
}

/* Calculate a public/private EC GF(p) key pair. W=S.G mod EC(p),
 * where S is the secret key and W is the public key
 * If RNG is NULL then the private key is provided externally in S
 * otherwise it is generated randomly internally */

#define MR_SAKKE_BN_MASTER_KEY_RESERVE 9

SAKKE_API int SAKKE_MASTER_KEY_PAIR_GENERATE(sak_domain *DOM,csprng *RNG,octet* S,octet *W)
{
	int flags,num=0;
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
    big x,q,a,b,r,s,px,py,beta;
    epoint *P;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ MR_SAKKE_BN_MASTER_KEY_RESERVE);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 1);
#else
    char mem[MR_BIG_RESERVE(MR_SAKKE_BN_MASTER_KEY_RESERVE)];
    char mem1[MR_ECP_RESERVE(1)];
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_MASTER_KEY_RESERVE));
	memset(mem1,0,MR_ECP_RESERVE(1));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= SAKKE_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
		x=mirvar_mem(_MIPP_ mem, num++);
        q=mirvar_mem(_MIPP_ mem, num++);
        a=mirvar_mem(_MIPP_ mem, num++);
        b=mirvar_mem(_MIPP_ mem, num++);
        r=mirvar_mem(_MIPP_ mem, num++);
        s=mirvar_mem(_MIPP_ mem, num++);
        px=mirvar_mem(_MIPP_ mem, num++);
        py=mirvar_mem(_MIPP_ mem, num++);
        beta=mirvar_mem(_MIPP_ mem, num++);

		flags=DOM->flags;

        bytes_to_big(_MIPP_ FS,DOM->X,x);
		mr_mip->TWIST=MR_SEXTIC_D;
		if ((flags&1)==1) mr_mip->TWIST=MR_SEXTIC_M;
		if ((flags&2)==2) negify(x,x); 

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Px,px);
        bytes_to_big(_MIPP_ FS,DOM->Py,py);
	bytes_to_big(_MIPP_ FS,DOM->Beta,beta);

        if (RNG!=NULL) 
            strong_bigrand(_MIPP_ RNG,r,s);
        else
        {
            bytes_to_big(_MIPP_ S->len,S->val,s);
            divide(_MIPP_ s,r,r);
	}
        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        P=epoint_init_mem(_MIPP_ mem1,0);
        if (!epoint_set(_MIPP_ px,py,0,P)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		G1_mult(_MIPP_ P,s,beta,r,x,P);   
        epoint_get(_MIPP_ P,px,py);

		W->len=2*FS+1;	W->val[0]=4;
		big_to_bytes(_MIPP_ FS,px,&(W->val[1]),TRUE);
		big_to_bytes(_MIPP_ FS,py,&(W->val[FS+1]),TRUE);
    
        if (RNG!=NULL) S->len=big_to_bytes(_MIPP_ 0,s,S->val,FALSE);
    }

#ifndef MR_STATIC
    memkill(_MIPP_ mem,MR_SAKKE_BN_MASTER_KEY_RESERVE);
    ecp_memkill(_MIPP_ mem1,1);
#else
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_MASTER_KEY_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(1));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return SAKKE_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return SAKKE_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

#define MR_SAKKE_BN_ENCAPSULATE_RESERVE 15

SAKKE_API int SAKKE_KEY_ENCAPSULATE(sak_domain *DOM,octet *SSV,octet *Z,octet *ID,octet *R,octet *H)
{
	int flags,num=0;
	char ww[4*FS],ht[HASH_BYTES];
	octet W={0,sizeof(ww),ww};
	octet HT={0,sizeof(ht),ht};

#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
    big x,q,a,b,r,u,v,t,px,py,beta;
    epoint *P,*ZS;
	zzn4 g;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ MR_SAKKE_BN_ENCAPSULATE_RESERVE);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(MR_SAKKE_BN_ENCAPSULATE_RESERVE)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_ENCAPSULATE_RESERVE));
	memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= SAKKE_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
		x=mirvar_mem(_MIPP_ mem, num++);
        q=mirvar_mem(_MIPP_ mem, num++);
        a=mirvar_mem(_MIPP_ mem, num++);
        b=mirvar_mem(_MIPP_ mem, num++);
        r=mirvar_mem(_MIPP_ mem, num++);
        u=mirvar_mem(_MIPP_ mem, num++);
        v=mirvar_mem(_MIPP_ mem, num++);
        t=mirvar_mem(_MIPP_ mem, num++);		
        px=mirvar_mem(_MIPP_ mem, num++);
        py=mirvar_mem(_MIPP_ mem, num++);
        beta=mirvar_mem(_MIPP_ mem, num++);
		zzn4_alloc(_MIPP_ &g,mem,&num);

		flags=DOM->flags;

        bytes_to_big(_MIPP_ FS,DOM->X,x);
		mr_mip->TWIST=MR_SEXTIC_D;
		if ((flags&1)==1) mr_mip->TWIST=MR_SEXTIC_M;
		if ((flags&2)==2) negify(x,x); 

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Px,px);
        bytes_to_big(_MIPP_ FS,DOM->Py,py);
		bytes_to_big(_MIPP_ FS,DOM->Beta,beta);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        P=epoint_init_mem(_MIPP_ mem1,0);
        if (!epoint_set(_MIPP_ px,py,0,P)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		hash(SSV,-1,ID,&HT);	
		bytes_to_big(_MIPP_ HT.len,HT.val,u);
		HashToIntegerRange(_MIPP_ u,r,v);	// r = HashToIntegerRange( SSV || b, q, Hash )

		bytes_to_big(_MIPP_ FS,&(Z->val[1]),px);
		bytes_to_big(_MIPP_ FS,&(Z->val[FS+1]),py);
		ZS=epoint_init_mem(_MIPP_ mem1,1);
		if (!epoint_set(_MIPP_ px,py,0,ZS)) res=SAKKE_INVALID_PUBLIC_KEY; 
	}
	if (res==0)
	{
		bytes_to_big(_MIPP_ FS,DOM->G[0],g.a.a);
		bytes_to_big(_MIPP_ FS,DOM->G[1],g.a.b);
		bytes_to_big(_MIPP_ FS,DOM->G[2],g.b.a);
		bytes_to_big(_MIPP_ FS,DOM->G[3],g.b.b);

		xtr_pow(_MIPP_ &g,v,&g);	// g^v

/* hash g */

		W.len=4*FS;
		big_to_bytes(_MIPP_ FS,g.a.a,&(W.val[0]),TRUE);
		big_to_bytes(_MIPP_ FS,g.a.b,&(W.val[FS]),TRUE);
		big_to_bytes(_MIPP_ FS,g.b.a,&(W.val[2*FS]),TRUE);
		big_to_bytes(_MIPP_ FS,g.b.b,&(W.val[3*FS]),TRUE);

		hash(&W,-1,NULL,&HT);
		bytes_to_big(_MIPP_ HT.len,HT.val,t);
		expb2(_MIPP_ AS*8,u);
		HashToIntegerRange(_MIPP_ t,u,t);
		H->len=big_to_bytes(_MIPP_ AS,t,H->val,TRUE);
		OCTET_XOR(SSV,H);

		bytes_to_big(_MIPP_ ID->len,ID->val,b);

		G1_mult(_MIPP_ P,b,beta,r,x,P);	// [b]P
		ecurve_add(_MIPP_ ZS,P);		// [b]P + Z_S
		epoint_norm(_MIPP_ P);

		G1_mult(_MIPP_ P,v,beta,r,x,P);	// [r]([b]P + Z_S)
		epoint_norm(_MIPP_ P);
        epoint_get(_MIPP_ P,px,py);

		R->len=2*FS+1; R->val[0]=4;
		big_to_bytes(_MIPP_ FS,px,&(R->val[1]),TRUE);
		big_to_bytes(_MIPP_ FS,py,&(R->val[FS+1]),TRUE);
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,MR_SAKKE_BN_ENCAPSULATE_RESERVE);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_ENCAPSULATE_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;

    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return SAKKE_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return SAKKE_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

#define MR_SAKKE_BN_GET_SECRET_RESERVE 18

SAKKE_API int SAKKE_GET_USER_SECRET_KEY(sak_domain *DOM,octet* Z,octet *ID,octet *RSK)
{
	int flags,num=0;
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
    big x,q,a,b,r,s;
	zzn2 f,qx,qy; 
    ecn2 Q;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ MR_SAKKE_BN_GET_SECRET_RESERVE);
#else
    char mem[MR_BIG_RESERVE(MR_SAKKE_BN_GET_SECRET_RESERVE)];
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_GET_SECRET_RESERVE));
#endif
 
    if (mr_mip==NULL || mem==NULL) res= SAKKE_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
        x=mirvar_mem(_MIPP_ mem, num++);
        q=mirvar_mem(_MIPP_ mem, num++);
        a=mirvar_mem(_MIPP_ mem, num++);
        b=mirvar_mem(_MIPP_ mem, num++);
        r=mirvar_mem(_MIPP_ mem, num++);
        s=mirvar_mem(_MIPP_ mem, num++);
		zzn2_alloc(_MIPP_ &qx,mem,&num);
		zzn2_alloc(_MIPP_ &qy,mem,&num);
		zzn2_alloc(_MIPP_ &f,mem,&num);
		ecn2_alloc(_MIPP_ &Q,mem,&num);

 		flags=DOM->flags;

        bytes_to_big(_MIPP_ FS,DOM->X,x);
		mr_mip->TWIST=MR_SEXTIC_D;
		if ((flags&1)==1) mr_mip->TWIST=MR_SEXTIC_M;
		if ((flags&2)==2) negify(x,x); 

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Qxa,qx.a);
        bytes_to_big(_MIPP_ FS,DOM->Qxb,qx.b);
        bytes_to_big(_MIPP_ FS,DOM->Qya,qy.a);
        bytes_to_big(_MIPP_ FS,DOM->Qyb,qy.b);
        bytes_to_big(_MIPP_ FS,DOM->Fa,f.a);
        bytes_to_big(_MIPP_ FS,DOM->Fb,f.b);

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
		if (!ecn2_set(_MIPP_ &qx,&qy,&Q))  res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
        bytes_to_big(_MIPP_ ID->len,ID->val,b);
        bytes_to_big(_MIPP_ Z->len,Z->val,s);

		add(_MIPP_ b,s,s);
		xgcd(_MIPP_ s,r,s,s,s);

		/* G2_mult(_MIPP_ &Q,s,&f,r,x,&Q);  */
		ecn2_mul(_MIPP_ s, &Q); 
		ecn2_norm(_MIPP_ &Q);
        ecn2_getxy(&Q,&qx,&qy);

		RSK->len=4*FS;
		big_to_bytes(_MIPP_ FS,qx.a,&(RSK->val[0]),TRUE);
		big_to_bytes(_MIPP_ FS,qx.b,&(RSK->val[FS]),TRUE);
		big_to_bytes(_MIPP_ FS,qy.a,&(RSK->val[2*FS]),TRUE);
		big_to_bytes(_MIPP_ FS,qy.b,&(RSK->val[3*FS]),TRUE);
    }

#ifndef MR_STATIC
    memkill(_MIPP_ mem,MR_SAKKE_BN_GET_SECRET_RESERVE);
#else
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_GET_SECRET_RESERVE));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return SAKKE_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return SAKKE_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

#define MR_SAKKE_BN_GET_VALIDATE_RESERVE 36

SAKKE_API int SAKKE_SECRET_KEY_VALIDATE(sak_domain *DOM,octet *ID,octet *Z,octet *K)
{
	int flags,num=0;
#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
    big x,q,a,b,r,px,py,beta;
	epoint *P,*ZS;
    ecn2 Q;
	zzn2 f,qx,qy;
	zzn4 g;
	zzn12 g1;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ MR_SAKKE_BN_GET_VALIDATE_RESERVE);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 2);
#else
    char mem[MR_BIG_RESERVE(MR_SAKKE_BN_GET_VALIDATE_RESERVE)];
    char mem1[MR_ECP_RESERVE(2)];
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_GET_VALIDATE_RESERVE));
	memset(mem1,0,MR_ECP_RESERVE(2));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= SAKKE_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
		x=mirvar_mem(_MIPP_ mem, num++);
        q=mirvar_mem(_MIPP_ mem, num++);
        a=mirvar_mem(_MIPP_ mem, num++);
        b=mirvar_mem(_MIPP_ mem, num++);
        r=mirvar_mem(_MIPP_ mem, num++);
        px=mirvar_mem(_MIPP_ mem, num++);
        py=mirvar_mem(_MIPP_ mem, num++);
        beta=mirvar_mem(_MIPP_ mem, num++);

		zzn2_alloc(_MIPP_ &qx,mem,&num);
		zzn2_alloc(_MIPP_ &qy,mem,&num);
		zzn2_alloc(_MIPP_ &f,mem,&num);
		ecn2_alloc(_MIPP_ &Q,mem,&num);
		zzn4_alloc(_MIPP_ &g,mem,&num);
		zzn12_alloc(_MIPP_ &g1,mem,&num);

		flags=DOM->flags;

        bytes_to_big(_MIPP_ FS,DOM->X,x);
		mr_mip->TWIST=MR_SEXTIC_D;
		if ((flags&1)==1) mr_mip->TWIST=MR_SEXTIC_M;
		if ((flags&2)==2) negify(x,x); 

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Px,px);
        bytes_to_big(_MIPP_ FS,DOM->Py,py);
		bytes_to_big(_MIPP_ FS,DOM->Beta,beta);
        bytes_to_big(_MIPP_ FS,DOM->Qxa,qx.a);
        bytes_to_big(_MIPP_ FS,DOM->Qxb,qx.b);
        bytes_to_big(_MIPP_ FS,DOM->Qya,qy.a);
        bytes_to_big(_MIPP_ FS,DOM->Qyb,qy.b);
        bytes_to_big(_MIPP_ FS,DOM->Fa,f.a);
        bytes_to_big(_MIPP_ FS,DOM->Fb,f.b);		

		ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
        P=epoint_init_mem(_MIPP_ mem1,0);
        if (!epoint_set(_MIPP_ px,py,0,P)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		bytes_to_big(_MIPP_ ID->len,ID->val,b);
		bytes_to_big(_MIPP_ FS,&(Z->val[1]),px);
		bytes_to_big(_MIPP_ FS,&(Z->val[FS+1]),py);
        ZS=epoint_init_mem(_MIPP_ mem1,1);
        if (!epoint_set(_MIPP_ px,py,0,ZS)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		G1_mult(_MIPP_ P,b,beta,r,x,P);
		ecurve_add(_MIPP_ ZS,P);
		epoint_norm(_MIPP_ P);

        bytes_to_big(_MIPP_ FS,&(K->val[0]),qx.a);
        bytes_to_big(_MIPP_ FS,&(K->val[FS]),qx.b);
        bytes_to_big(_MIPP_ FS,&(K->val[2*FS]),qy.a);
        bytes_to_big(_MIPP_ FS,&(K->val[3*FS]),qy.b);
		if (!ecn2_set(_MIPP_ &qx,&qy,&Q)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		bytes_to_big(_MIPP_ FS,DOM->G[0],g.a.a);
		bytes_to_big(_MIPP_ FS,DOM->G[1],g.a.b);
		bytes_to_big(_MIPP_ FS,DOM->G[2],g.b.a);
		bytes_to_big(_MIPP_ FS,DOM->G[3],g.b.b);

		rate_miller(_MIPP_ &Q,P,x,&f,&g1);
		rate_fexp(_MIPP_ x,&f,&g1);

		trace(_MIPP_ &g1,&(g1.a));

		if (!zzn4_compare(&g,&(g1.a))) res=SAKKE_INVALID;
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,MR_SAKKE_BN_GET_VALIDATE_RESERVE);
    ecp_memkill(_MIPP_ mem1,2);
#else
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_GET_VALIDATE_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(2));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return SAKKE_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return SAKKE_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}

#define MR_SAKKE_BN_DECAPSULATE_RESERVE 34

SAKKE_API int SAKKE_KEY_DECAPSULATE(sak_domain *DOM,octet *R,octet *H,octet *Z,octet *ID,octet *K,octet *SSV)
{
	int flags,num=0;
	char ww[4*FS],ht[HASH_BYTES];
	octet W={0,sizeof(ww),ww};
	octet HT={0,sizeof(ht),ht};

#ifdef MR_GENERIC_AND_STATIC
	miracl instance;
	miracl *mr_mip=mirsys(&instance,DOM->nibbles,16);
#else
	miracl *mr_mip=mirsys(DOM->nibbles,16);
#endif
    big x,q,a,b,r,px,py,t,u,beta;
	epoint *P,*ZS,*RC;
    ecn2 Q;
	zzn2 f,qx,qy;
	zzn12 g;
    int err,res=0;
#ifndef MR_STATIC
    char *mem=(char *)memalloc(_MIPP_ MR_SAKKE_BN_DECAPSULATE_RESERVE);
    char *mem1=(char *)ecp_memalloc(_MIPP_ 3);
#else
    char mem[MR_BIG_RESERVE(MR_SAKKE_BN_DECAPSULATE_RESERVE)];
    char mem1[MR_ECP_RESERVE(3)];
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_DECAPSULATE_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(3));
#endif
 
    if (mr_mip==NULL || mem==NULL || mem1==NULL) res= SAKKE_OUT_OF_MEMORY;
    mr_mip->ERCON=TRUE;

    if (res==0)
    {
		x=mirvar_mem(_MIPP_ mem, num++);
        q=mirvar_mem(_MIPP_ mem, num++);
        a=mirvar_mem(_MIPP_ mem, num++);
        b=mirvar_mem(_MIPP_ mem, num++);
        r=mirvar_mem(_MIPP_ mem, num++);
        px=mirvar_mem(_MIPP_ mem, num++);
        py=mirvar_mem(_MIPP_ mem, num++);
        t=mirvar_mem(_MIPP_ mem, num++);
	    u=mirvar_mem(_MIPP_ mem, num++);
		beta=mirvar_mem(_MIPP_ mem,num++);
		zzn2_alloc(_MIPP_ &qx,mem,&num);
		zzn2_alloc(_MIPP_ &qy,mem,&num);
		zzn2_alloc(_MIPP_ &f,mem,&num);
		ecn2_alloc(_MIPP_ &Q,mem,&num);
		zzn12_alloc(_MIPP_ &g,mem,&num);

		flags=DOM->flags;

        bytes_to_big(_MIPP_ FS,DOM->X,x);
		mr_mip->TWIST=MR_SEXTIC_D;
		if ((flags&1)==1) mr_mip->TWIST=MR_SEXTIC_M;
		if ((flags&2)==2) negify(x,x); 

        bytes_to_big(_MIPP_ FS,DOM->Q,q);
        bytes_to_big(_MIPP_ FS,DOM->A,a);
        bytes_to_big(_MIPP_ FS,DOM->B,b);
        bytes_to_big(_MIPP_ FS,DOM->R,r);
        bytes_to_big(_MIPP_ FS,DOM->Fa,f.a);
        bytes_to_big(_MIPP_ FS,DOM->Fb,f.b);		

        ecurve_init(_MIPP_ a,b,q,MR_PROJECTIVE);
/* extract R into P */

		bytes_to_big(_MIPP_ FS,&(R->val[1]),px);
		bytes_to_big(_MIPP_ FS,&(R->val[FS+1]),py);
        RC=epoint_init_mem(_MIPP_ mem1,0);
        if (!epoint_set(_MIPP_ px,py,0,RC)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
/* extract K into P */
     
        bytes_to_big(_MIPP_ FS,&(K->val[0]),qx.a);
        bytes_to_big(_MIPP_ FS,&(K->val[FS]),qx.b);
        bytes_to_big(_MIPP_ FS,&(K->val[2*FS]),qy.a);
        bytes_to_big(_MIPP_ FS,&(K->val[3*FS]),qy.b);

		if (!ecn2_set(_MIPP_ &qx,&qy,&Q))  res=SAKKE_INVALID;
	}
	if (res==0)
	{
		rate_miller(_MIPP_ &Q,RC,x,&f,&g);
		rate_fexp(_MIPP_ x,&f,&g);

		trace(_MIPP_ &g,&(g.a));

		W.len=4*FS;
		big_to_bytes(_MIPP_ FS,g.a.a.a,&(W.val[0]),TRUE);
		big_to_bytes(_MIPP_ FS,g.a.a.b,&(W.val[FS]),TRUE);
		big_to_bytes(_MIPP_ FS,g.a.b.a,&(W.val[2*FS]),TRUE);
		big_to_bytes(_MIPP_ FS,g.a.b.b,&(W.val[3*FS]),TRUE);

		hash(&W,-1,NULL,&HT);
		bytes_to_big(_MIPP_ HT.len,HT.val,t);
		expb2(_MIPP_ AS*8,u);
		HashToIntegerRange(_MIPP_ t,u,t);
		SSV->len=big_to_bytes(_MIPP_ AS,t,SSV->val,TRUE);
		OCTET_XOR(H,SSV);

		bytes_to_big(_MIPP_ ID->len,ID->val,b);

		hash(SSV,-1,ID,&HT);	
		bytes_to_big(_MIPP_ HT.len,HT.val,u);
		HashToIntegerRange(_MIPP_ u,r,u);

        bytes_to_big(_MIPP_ FS,DOM->Px,px);
        bytes_to_big(_MIPP_ FS,DOM->Py,py);
		bytes_to_big(_MIPP_ FS,DOM->Beta,beta);

        P=epoint_init_mem(_MIPP_ mem1,1);
        if (!epoint_set(_MIPP_ px,py,0,P)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		bytes_to_big(_MIPP_ FS,&(Z->val[1]),px);
		bytes_to_big(_MIPP_ FS,&(Z->val[FS+1]),py);
		ZS=epoint_init_mem(_MIPP_ mem1,2);
        if (!epoint_set(_MIPP_ px,py,0,ZS)) res=SAKKE_INVALID_PUBLIC_KEY;
	}
	if (res==0)
	{
		G1_mult(_MIPP_ P,b,beta,r,x,P); 
		ecurve_add(_MIPP_ ZS,P);
		epoint_norm(_MIPP_ P);
		G1_mult(_MIPP_ P,u,beta,r,x,P);  
		epoint_norm(_MIPP_ P);

		if (!epoint_comp(_MIPP_ P,RC)) res=SAKKE_INVALID;
	}

#ifndef MR_STATIC
    memkill(_MIPP_ mem,MR_SAKKE_BN_DECAPSULATE_RESERVE);
    ecp_memkill(_MIPP_ mem1,3);
#else
    memset(mem,0,MR_BIG_RESERVE(MR_SAKKE_BN_DECAPSULATE_RESERVE));
    memset(mem1,0,MR_ECP_RESERVE(3));
#endif
    err=mr_mip->ERNUM;
    mirexit(_MIPPO_ );
    if (err==MR_ERR_OUT_OF_MEMORY) return SAKKE_OUT_OF_MEMORY;
    if (err==MR_ERR_DIV_BY_ZERO) return SAKKE_DIV_BY_ZERO;
    if (err!=0) return -(1000+err);
    return res;
}
