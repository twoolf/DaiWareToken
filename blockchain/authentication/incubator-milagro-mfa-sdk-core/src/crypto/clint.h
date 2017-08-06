/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* CLINT header file */
/* Designed for AES-128 security, 254-256 bit elliptic curves and BN curves for pairings */
/* Each "limb" of a big number occupies at most (n-3) bits of an n-bit computer word. The most significant word must have at least 4 extra unused bits */ 
/* For n=64, use 5 words, use 56 bits per limb, leaving at least 24 unused MSBs 5*56-256  */
/* For n=32, use 9 words, use 29 bits per limb, leaving at least 5 unused MSBs  9*29-256  */
/* For n=16, use 20 words, use 13 bits per limb, leaving at least 4 unused MSBs 20*13-256 */

/* NOTE: There is only one user configurable section in this header - see below */

#ifndef CLINT_H
#define CLINT_H

#include <stdio.h>
#include <stdlib.h>
#include "DLLDefines.h"

/* Support for C99?  Note for GCC need to explicitly include -std=c99 in command line */

#if __STDC_VERSION__ >= 199901L
/* C99 code */
#define C99
#else
/* Not C99 code */
#endif

#ifndef C99  /* You are on your own! These are for Microsoft C */
#define sign32 __int32
#define sign8 signed char
#define unsign32 unsigned __int32
#else
#include <stdint.h>
#define sign8 int8_t
#define sign32 int32_t
#define unsign32 uint32_t
#endif

/* modulus types */

#define NOT_SPECIAL 0
#define PSEUDO_MERSENNE 1
#define MONTGOMERY_FRIENDLY 3

/* curve types */

#define WEIERSTRASS 0
#define EDWARDS 1
#define MONTGOMERY 2

/* Elliptic curves are defined over prime fields */
/* Here are some popular EC prime fields for which I have prepared curves. Feel free to specify your own. */

#define NIST 0 /* For the NIST 256-bit standard curve		- WEIERSTRASS only */
#define C25519 1  /* Bernstein's Modulus 2^255-19			- EDWARDS or MONTGOMERY only */
#define BRAINPOOL 2 /* For Brainpool 256-bit curve			- WEIERSTRASS only */
#define ANSSI 3 /* For French 256-bit standard curve		- WEIERSTRASS only */
#define MF254 4 /* For NUMS curves from Bos et al - 254-bit Montgomery friendly modulus		- WEIERSTRASS or EDWARDS or MONTGOMERY */
#define MS255 5 /* For NUMS curve - 255-bit pseudo-mersenne modulus							- WEIERSTRASS or EDWARDS or MONTGOMERY */
#define MF256 6 /* For NUMS curve - 256-bit Montgomery friendly modulus						- WEIERSTRASS or EDWARDS or MONTGOMERY */
#define MS256 7 /* For NUMS curve - 256-bit pseudo-merseene modulus							- WEIERSTRASS or EDWARDS or MONTGOMERY */

#define BN 100    /* Standard Nogami BN curve - fastest. Modulus built from  t=-0x4080000000000001	- WEIERSTRASS only */
#define BNCX 101  /* Our CertiVox BN curve. Modulus built from t=-0x4000000003C012B1				- WEIERSTRASS only */
#define BNT 102   /* GT_Strong BN curve. Modulus built from t=-0x4000806000004081  					- WEIERSTRASS only */
#define BNT2 103  /* G2 and GT-Strong BN curve.  Modulus built from t=-0x4000020100608205 			- WEIERSTRASS only */


/*** START OF USER CONFIGURABLE SECTION - set architecture and choose modulus and curve  ***/

#define CLINT_VERSION_MAJOR 1
#define CLINT_VERSION_MINOR 0
#define CLINT_VERSION_PATCH 0
#define FIELD_CHOICE BNCX
#define CURVE_TYPE WEIERSTRASS

#include "platform.h"

#define CHUNK WORD_LENGTH /* size of chunk in bits = wordlength of computer = 16, 32 or 64. Note not all curve options are supported on 16-bit processors - see rom.c */
#define CHOICE  FIELD_CHOICE   /* Current choice of Field */
/* For some moduli only WEIERSTRASS curves are supported. For others there is a choice of WEIERSTRASS, EDWARDS or MONTGOMERY curves. See above. */
#define CURVETYPE CURVE_TYPE /* Note that not all curve types are supported - see above */
/* Actual curve parameters associated with these choices can be found in rom.c */

/* These next options only apply for pairings */
#define USE_GLV		/* Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2	/* Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT   /* Not patented, so probably always use this */

/* Finite field support - for RSA, DH etc. */
#define FF_BITS 2048 /* Finite Field Size in bits - must be 256.2^n */

/* For debugging Only.
#define DEBUG_REDUCE 
#define DEBUG_NORM
#define GET_STATS
*/

/*** END OF USER CONFIGURABLE SECTION ***/


#if CHOICE>=BN     /* Its a BN curve */
#define MBITS 254 /* Number of bits in Modulus */
#define MOD8 3 /* Modulus mod 8  */
#define MODTYPE  NOT_SPECIAL 
#endif

#if CHOICE>BN
#define GT_STRONG   /* Using a GT-Strong BN curve */
#endif

#if CHOICE==NIST    /* The NIST256 Curve */
#define MBITS 256 
#define MOD8 7 
#define MODTYPE  NOT_SPECIAL 
#endif

#if CHOICE==C25519  /* ED25519 Edwards curve or Curve25519 Montgomery Curve */
#define MBITS 255 /* Number of bits in Modulus */
#define MOD8 5 /* Modulus mod 8  */
#define MODTYPE PSEUDO_MERSENNE 
#endif

#if CHOICE==BRAINPOOL    /* The BRAINPOOL 256-bit Curve */
#define MBITS 256 /* Number of bits in Modulus */
#define MOD8 7 /* Modulus mod 8  */
#define MODTYPE  NOT_SPECIAL 
#endif

#if CHOICE==ANSSI
#define MBITS 256
#define MOD8 3
#define MODTYPE  NOT_SPECIAL 
#endif

#if CHOICE==MF254   /* NUMS curve from Bos et al. paper */
#define MBITS 254   /* Number of bits in Modulus */
#define MOD8 7      /* Modulus mod 8  */
#define MODTYPE MONTGOMERY_FRIENDLY  
#endif

#if CHOICE==MF256   /* NUMS curve from Bos et al. paper */
#define MBITS 256   /* Number of bits in Modulus */
#define MOD8 7      /* Modulus mod 8  */
#define MODTYPE MONTGOMERY_FRIENDLY  
#endif

#if CHOICE==MS255
#define MBITS 255
#define MOD8 3
#define MODTYPE PSEUDO_MERSENNE 
#endif

#if CHOICE==MS256
#define MBITS 256
#define MOD8 3
#define MODTYPE PSEUDO_MERSENNE 
#endif


#define FFLEN (FF_BITS/256)
#define HFLEN (FFLEN/2)  /* Useful for half-size RSA private key operations */

/* This next is probably OK, but may need changing for non-C99-standard environments */

#if CHUNK==16
#define NLEN 20				/* Number of words in BIG. */
#define BASEBITS 13			/* Numbers represented to base 2*BASEBITS */
#ifndef C99
#define chunk __int16		/* C type corresponding to word length */
#define dchunk __int32		/* Always define double length chunk type if available */
#else
#define chunk int16_t
#define dchunk int32_t
#endif
#endif

#if CHUNK == 32
#define NLEN 9				/* Number of words in BIG. */
#define BASEBITS 29			/* Numbers represented to base 2*BASEBITS */
#ifndef C99
#define chunk __int32		/* C type corresponding to word length */
#define dchunk __int64		/* Always define double length chunk type if available */
#else
#define chunk int32_t
#define dchunk int64_t
#endif
#endif

#if CHUNK == 64
#define NLEN 5				/* Number of words in BIG. */
#define BASEBITS 56			/* Numbers represented to base 2*BASEBITS */
#ifndef C99
#define chunk __int64		/* C type corresponding to word length */						
							/* Note - no 128-bit type available    */
#else
#define chunk int64_t
#ifdef __GNUC__
#define dchunk __int128		/* Always define double length chunk type if available - GCC supports 128 bit type  ??? */
#endif
#endif
#endif

/* Don't mess with anything below this line */

#ifdef GET_STATS
extern int tsqr,rsqr,tmul,rmul;
extern int tadd,radd,tneg,rneg;
extern int tdadd,rdadd,tdneg,rdneg;
#endif

#define DCHUNK 2*CHUNK
#define DNLEN 2*NLEN  /* double length required for products of BIGs */

#ifdef dchunk
#define COMBA      /* Use COMBA method for faster BN muls, sqrs and reductions */
#endif

#define CHUNK_BITS 8*sizeof(chunk)

#ifdef DEBUG_NORM    /* Add an extra location to track chunk extension */
typedef chunk BIG[NLEN+1];
typedef chunk DBIG[DNLEN+1];
#else
typedef chunk BIG[NLEN];
typedef chunk DBIG[DNLEN];
#endif

#define HBITS (BASEBITS/2)
#define HBITS1 ((BASEBITS+1)/2)
#define HDIFF (HBITS1-HBITS)

#define MASK (((chunk)1<<BASEBITS)-1)
#define HMASK (((chunk)1<<HBITS)-1)
#define HMASK1 (((chunk)1<<HBITS1)-1)

#define MODBITS MBITS
#define MODBYTES 32
#define MB (MBITS%BASEBITS)
#define TBITS (MBITS%BASEBITS) /* Number of active bits in top word */
#define TMASK (((chunk)1<<(MBITS%BASEBITS))-1)
#define NEXCESS (1<<(CHUNK-BASEBITS-1)) /* 2^(CHUNK-BASEBITS-1) - digit cannot be multiplied by more than this before normalisation */
#define FEXCESS ((chunk)1<<(BASEBITS*NLEN-MBITS)) /* 2^(BASEBITS*NLEN-MODBITS) - normalised BIG can be multiplied by more than this before reduction */
#define OMASK ((chunk)(-1)<<(MBITS%BASEBITS))

/* catch field excesses */
#define EXCESS(a) ((a[NLEN-1]&OMASK)>>(MB))

/* Field Params - see rom.c */
extern const BIG Modulus;  /* Actual Modulus set in rom.c */
extern const chunk MConst; /* Montgomery only - 1/p mod 2^BASEBITS */

/* Curve Params - see rom.c */
extern const int CURVE_A;
extern const BIG CURVE_B;
extern const BIG CURVE_Order;

/* Generator point on G1 */
extern const BIG CURVE_Gx;
extern const BIG CURVE_Gy;

/* For Pairings only */

/* Generator point on G2 */
extern const BIG CURVE_Pxa;
extern const BIG CURVE_Pxb;
extern const BIG CURVE_Pya;
extern const BIG CURVE_Pyb;
/* BN curve x parameter */
extern const BIG CURVE_Bnx;
/* BN curve Cube Root of Unity */
extern const BIG CURVE_Cru;
/* BN curve Frobenius Constant */
extern const BIG CURVE_Fra;
extern const BIG CURVE_Frb; 

/* BN curve constants for GLV and GS decomposition */
extern const BIG CURVE_W[2];
extern const BIG CURVE_SB[2][2];
extern const BIG CURVE_WB[4];
extern const BIG CURVE_BB[4][4];

/* Structures */

typedef struct {
#if CURVETYPE!=EDWARDS
int inf;
#endif
BIG x;
#if CURVETYPE!=MONTGOMERY
BIG y;
#endif
BIG z;
} ECP;

typedef struct {
BIG a;
BIG b;
} FP2;

typedef struct {
FP2 a;
FP2 b;
} FP4;

typedef struct {
FP4 a;
FP4 b;
FP4 c;
} FP12;

typedef struct {
int inf; 
FP2 x;
FP2 y;
FP2 z;
} ECP2;

/* SHA256 structure */

typedef struct {
unsign32 length[2];
unsign32 h[8];
unsign32 w[64];
} hash;

/* Symmetric Encryption AES structure */

#define ECB   0
#define CBC   1
#define CFB1  2
#define CFB2  3
#define CFB4  5
#define OFB1  14
#define OFB2  15
#define OFB4  17
#define OFB8  21
#define OFB16 29

#define uchar unsigned char

typedef struct {
int mode;
unsign32 fkey[44];
unsign32 rkey[44];
char f[16];
} aes;

/* AES-GCM suppport.  */

#define GCM_ACCEPTING_HEADER 0
#define GCM_ACCEPTING_CIPHER 1
#define GCM_NOT_ACCEPTING_MORE 2
#define GCM_FINISHED 3
#define GCM_ENCRYPTING 0
#define GCM_DECRYPTING 1

typedef struct {
unsign32 table[128][4]; /* 2k bytes */
uchar stateX[16];
uchar Y_0[16];
unsign32 counter;
unsign32 lenA[2],lenC[2];
int status;
aes a;
} gcm;

/* Marsaglia & Zaman Random number generator constants */

#define NK   21 
#define NJ   6
#define NV   8

/* Cryptographically strong pseudo-random number generator */

typedef struct {
unsign32 ira[NK];  /* random number...   */
int      rndptr;   /* ...array & pointer */
unsign32 borrow;
int pool_ptr;
char pool[32];    /* random pool */
} csprng;


/* portable representation of a big positive number */

typedef struct
{
    int len;
    int max;
    char *val;
} octet;

/* IF Public Key */

typedef struct
{
    sign32 e;
    BIG n[FFLEN];
} rsa_public_key;

/* IF Private Key */

typedef struct
{
    BIG p[FFLEN/2];
    BIG q[FFLEN/2];
    BIG dp[FFLEN/2];
    BIG dq[FFLEN/2];
    BIG c[FFLEN/2];
} rsa_private_key;

/*

Note that a normalised BIG consists of digits mod 2^BASEBITS
However BIG digits may be "extended" up to 2^(WORDLENGTH-1).

BIGs in extended form may need to be normalised before certain 
operations.

A BIG may be "reduced" to be less that the Modulus, or it 
may be "unreduced" and allowed to grow greater than the 
Modulus.

Normalisation is quite fast. Reduction involves conditional branches, 
which can be regarded as significant "speed bumps". We try to 
delay reductions as much as possible. Reductions may also involve 
side channel leakage, so delaying and batching them
hopefully disguises internal operations.

*/

/* BIG number prototypes */
extern chunk muladd(chunk,chunk,chunk,chunk *);
extern int BIG_iszilch(BIG);
extern int BIG_diszilch(DBIG);
extern void BIG_output(BIG);
extern void BIG_rawoutput(BIG);
extern void BIG_cswap(BIG,BIG,int);
extern void BIG_cmove(BIG,BIG,int);
extern void BIG_toBytes(char *,BIG );
extern void BIG_fromBytes(BIG,char *);
extern void BIG_doutput(DBIG);
extern void BIG_rcopy(BIG,const BIG);
extern void BIG_copy(BIG,BIG);
extern void BIG_dcopy(DBIG,DBIG);
extern void BIG_dsucopy(DBIG,BIG);
extern void BIG_dscopy(DBIG,BIG);
extern void BIG_sdcopy(BIG,DBIG);
extern void BIG_sducopy(BIG,DBIG);
extern void BIG_zero(BIG);
extern void BIG_dzero(DBIG);
extern void BIG_one(BIG);
extern void BIG_invmod2m(BIG);
extern void BIG_add(BIG,BIG,BIG);
extern void BIG_inc(BIG,int);
extern void BIG_sub(BIG,BIG,BIG);
extern void BIG_dsub(DBIG,DBIG,DBIG);
extern void BIG_dec(BIG,int);
extern void BIG_imul(BIG,BIG,int);
extern chunk BIG_pmul(BIG,BIG,int);
extern int BIG_div3(BIG);
extern void BIG_pxmul(DBIG,BIG,int);
extern void BIG_mul(DBIG,BIG,BIG);
extern void BIG_smul(BIG,BIG,BIG);
extern void BIG_sqr(DBIG,BIG);
extern void BIG_shl(BIG,int);
extern chunk BIG_fshl(BIG,int);
extern void BIG_dshl(DBIG,int);
extern void BIG_shr(BIG,int);
extern chunk BIG_fshr(BIG,int);
extern void BIG_dshr(DBIG,int);
extern void BIG_split(BIG,BIG,DBIG,int);
extern chunk BIG_norm(BIG);
extern void BIG_dnorm(DBIG);
extern int BIG_comp(BIG,BIG);
extern int BIG_dcomp(DBIG,DBIG);
extern int BIG_nbits(BIG);
extern int BIG_dnbits(DBIG);
extern void BIG_mod(BIG,BIG);
extern void BIG_sdiv(BIG,BIG);
extern void BIG_dmod(DBIG,DBIG,BIG);
extern void BIG_ddiv(BIG,DBIG,BIG);
extern int BIG_parity(BIG);
extern int BIG_bit(BIG,int);
extern int BIG_lastbits(BIG,int);
extern void BIG_random(BIG,csprng *);
extern void BIG_randomnum(BIG,BIG,csprng *);
extern int BIG_nafbits(BIG,BIG,int,int *,int *);
extern void BIG_modmul(BIG,BIG,BIG,BIG);
extern void BIG_moddiv(BIG,BIG,BIG,BIG);
extern void BIG_modsqr(BIG,BIG,BIG);
extern void BIG_modneg(BIG,BIG,BIG);
extern int BIG_jacobi(BIG,BIG);
extern void BIG_invmodp(BIG,BIG,BIG);

/* FP prototypes */
extern int FP_iszilch(BIG);
extern void FP_nres(BIG);
extern void FP_redc(BIG);
extern void FP_one(BIG);
extern void FP_mod(BIG,BIG);
extern void FP_mul(BIG,BIG,BIG);
extern void FP_imul(BIG,BIG,int);
extern void FP_sqr(BIG,BIG);
extern void FP_add(BIG,BIG,BIG);
extern void FP_sub(BIG,BIG,BIG);
extern void FP_div2(BIG,BIG);
extern void FP_pow(BIG,BIG,BIG);
extern void FP_sqrt(BIG,BIG);
extern void FP_neg(BIG,BIG);
extern void FP_output(BIG);
extern void FP_rawoutput(BIG);
extern void FP_reduce(BIG);
extern int FP_qr(BIG);
extern void FP_inv(BIG,BIG);

/* FP2 prototypes */
extern int FP2_iszilch(FP2 *);
extern void FP2_cmove(FP2 *,FP2 *,int);
extern int FP2_isunity(FP2 *);
extern int FP2_equals(FP2 *,FP2 *);
extern void FP2_from_zps(FP2 *,BIG,BIG);
extern void FP2_from_BIGs(FP2 *,BIG,BIG);
extern void FP2_from_zp(FP2 *,BIG);
extern void FP2_from_BIG(FP2 *,BIG);
extern void FP2_copy(FP2 *,FP2 *);
extern void FP2_zero(FP2 *);
extern void FP2_one(FP2 *);
extern void FP2_neg(FP2*,FP2 *);
extern void FP2_conj(FP2*,FP2 *);
extern void FP2_add(FP2 *,FP2 *,FP2 *);
extern void FP2_sub(FP2 *,FP2 *,FP2 *);
extern void FP2_pmul(FP2 *,FP2 *,BIG);
extern void FP2_imul(FP2 *,FP2 *,int);
extern void FP2_sqr(FP2 *,FP2 *);
extern void FP2_mul(FP2 *,FP2 *,FP2 *);
extern void FP2_output(FP2 *);
extern void FP2_rawoutput(FP2 *);
extern void FP2_inv(FP2 *,FP2 *);
extern void FP2_div2(FP2 *,FP2 *);
extern void FP2_mul_ip(FP2 *);
extern void FP2_div_ip(FP2 *);
extern void FP2_norm(FP2 *);
extern void FP2_reduce(FP2 *);
extern void FP2_pow(FP2 *,FP2 *,BIG);
extern int FP2_sqrt(FP2 *,FP2 *);

/* ECP E(Fp) prototypes */
extern int ECP_isinf(ECP *);
extern int ECP_equals(ECP *,ECP *);
extern void ECP_copy(ECP *,ECP *);
extern void ECP_neg(ECP *);
extern void ECP_inf(ECP *);
extern void ECP_rhs(BIG,BIG);
extern int ECP_setx(ECP *,BIG,int);

#if CURVETYPE==MONTGOMERY
extern int ECP_set(ECP *,BIG);
extern void ECP_get(BIG,ECP *);
#else
extern int ECP_set(ECP *,BIG,BIG);
extern int ECP_get(BIG,BIG,ECP *);
#endif
extern void ECP_affine(ECP *P);
extern void ECP_outputz(ECP *);
extern void ECP_output(ECP *);
extern void ECP_toOctet(octet *,ECP *);
extern int ECP_fromOctet(ECP *,octet *);
extern void ECP_dbl(ECP *);

#if CURVETYPE==MONTGOMERY
extern void ECP_add(ECP *,ECP *,ECP *);
#else
extern void ECP_add(ECP *,ECP *);
#endif
extern void ECP_sub(ECP *,ECP *);
extern void ECP_pinmul(ECP *,int,int);
extern void ECP_mul(ECP *,BIG);
extern void ECP_mul2(ECP *,ECP *,BIG,BIG);

/* ECP2 E(Fp2) prototypes */
extern int ECP2_isinf(ECP2 *);
extern void ECP2_copy(ECP2 *,ECP2 *);
extern void ECP2_inf(ECP2 *);
extern int ECP2_equals(ECP2 *,ECP2 *);
extern void ECP2_affine(ECP2 *);
extern void ECP2_get(FP2 *,FP2 *,ECP2 *);
extern void ECP2_output(ECP2 *);
extern void ECP2_outputxyz(ECP2 *);
extern void ECP2_toOctet(octet *,ECP2 *);
extern int ECP2_fromOctet(ECP2 *,octet *);
extern void ECP2_rhs(FP2 *,FP2 *);
extern int ECP2_set(ECP2 *,FP2 *,FP2 *);
extern int ECP2_setx(ECP2 *,FP2 *);
extern void ECP2_neg(ECP2 *);
extern int ECP2_dbl(ECP2 *);
extern int ECP2_add(ECP2 *,ECP2 *);
extern void ECP2_sub(ECP2 *,ECP2 *);
extern void ECP2_mul(ECP2 *,BIG);
extern void ECP2_frob(ECP2 *,FP2 *);
extern void ECP2_mul4(ECP2 *,ECP2 *,BIG *);

/* FP4 prototypes */
extern int FP4_iszilch(FP4 *);
extern int FP4_isunity(FP4 *);
extern int FP4_equals(FP4 *,FP4 *);
extern int FP4_isreal(FP4 *);
extern void FP4_from_FP2s(FP4 *,FP2 *,FP2 *);
extern void FP4_from_FP2(FP4 *,FP2 *);
extern void FP4_copy(FP4*,FP4*);
extern void FP4_zero(FP4*);
extern void FP4_one(FP4*);
extern void FP4_neg(FP4*,FP4*);
extern void FP4_conj(FP4*,FP4*);
extern void FP4_nconj(FP4*,FP4*);
extern void FP4_mod(FP4 *,DBIG,DBIG,DBIG,DBIG);
extern void FP4_add(FP4*,FP4*,FP4*);
extern void FP4_sub(FP4*,FP4*,FP4*);
extern void FP4_pmul(FP4*,FP4*,FP2*);
extern void FP4_imul(FP4*,FP4*,int);
extern void FP4_sqr(FP4*,FP4*);
extern void FP4_mul(FP4*,FP4*,FP4*);
extern void FP4_inv(FP4*,FP4*);
extern void FP4_output(FP4*);
extern void FP4_rawoutput(FP4*);
extern void FP4_times_i(FP4*);
extern void FP4_norm(FP4*);
extern void FP4_reduce(FP4*);
extern void FP4_pow(FP4*,FP4*,BIG);
extern void FP4_frob(FP4* ,FP2* );
extern void FP4_xtr_A(FP4 *,FP4 *,FP4 *,FP4 *,FP4 *);
extern void FP4_xtr_D(FP4 *,FP4 *);
extern void FP4_xtr_pow(FP4 *,FP4 *,BIG);
extern void FP4_xtr_pow2(FP4 *,FP4 *,FP4 *,FP4 *,FP4 *,BIG,BIG);

/* FP12 prototypes */
extern int FP12_iszilch(FP12 *);
extern int FP12_isunity(FP12 *);
extern void FP12_copy(FP12*,FP12*);
extern void FP12_one(FP12*);
extern int FP12_equals(FP12 *,FP12 *);
extern void FP12_conj(FP12 *,FP12 *);
extern void FP12_from_FP4(FP12 *,FP4 *);
extern void FP12_from_FP4s(FP12 *,FP4 *,FP4*,FP4*);
extern void FP12_usqr(FP12 *,FP12 *);
extern void FP12_sqr(FP12 *,FP12 *);
extern void FP12_smul(FP12 *,FP12 *);
extern void FP12_mul(FP12 *,FP12*);
extern void FP12_inv(FP12 *,FP12 *);
extern void FP12_pow(FP12*,FP12*,BIG);
extern void FP12_pinpow(FP12*,int,int);
extern void FP12_pow4(FP12*,FP12*,BIG *);
extern void FP12_frob(FP12*,FP2*);
extern void FP12_reduce(FP12 *);
extern void FP12_norm(FP12 *);
extern void FP12_output(FP12 *);
extern void FP12_toOctet(octet *,FP12 *);
extern void FP12_fromOctet(FP12 *,octet *);
extern void FP12_trace(FP4 *,FP12 *);

/* Pairing function prototypes */
extern void PAIR_ate(FP12 *,ECP2 *,ECP *);
extern void PAIR_double_ate(FP12 *,ECP2 *,ECP *,ECP2 *,ECP *);
extern void PAIR_fexp(FP12 *);
extern void PAIR_G1mul(ECP *,BIG);
extern void PAIR_G2mul(ECP2 *,BIG);
extern void PAIR_GTpow(FP12 *,BIG);
extern int PAIR_GTmember(FP12 *);

/* Finite Field Prototypes */
extern void BIG_invmod2m(BIG);
extern void FF_rcopy(BIG *,const BIG *,int);
extern void FF_copy(BIG *,BIG *,int);
extern void FF_init(BIG *,sign32,int);
extern void FF_dsucopy(BIG *,BIG *,int);
extern void FF_dscopy(BIG *,BIG *,int);
extern void FF_sducopy(BIG *,BIG *,int);
extern void FF_zero(BIG *,int);
extern int FF_iszilch(BIG *,int);
extern void FF_shrw(BIG *,int);
extern void FF_shlw(BIG *,int);
extern int FF_parity(BIG *);
extern int FF_lastbits(BIG *,int);
extern void FF_one(BIG *,int);
extern int FF_comp(BIG *,BIG *,int);
extern void FF_radd(BIG *,int,BIG *,int,BIG *,int,int);
extern void FF_rsub(BIG *,int,BIG *,int,BIG *,int,int);
extern void FF_rinc(BIG *,int,BIG *,int,int);
extern void FF_rdec(BIG *,int,BIG *,int,int);
extern void FF_add(BIG *,BIG *,BIG *,int);
extern void FF_sub(BIG *,BIG *,BIG *,int);
extern void FF_inc(BIG *,int,int);
extern void FF_dec(BIG *,int,int);
extern void FF_rnorm(BIG *,int,int);
extern void FF_norm(BIG *,int);
extern void FF_shl(BIG *,int);
extern void FF_shr(BIG *,int);
extern void FF_output(BIG *,int);
extern void FF_toOctet(octet *,BIG *,int);
extern void FF_fromOctet(BIG *,octet *,int);
extern void FF_cswap(BIG *,BIG *,int,int);
extern void FF_karmul(BIG *,int,BIG *,int,BIG *,int,BIG *,int,int);
extern void FF_karsqr(BIG *,int,BIG *,int,BIG *,int,int);
extern void FF_karmul_lower(BIG *,int,BIG *,int,BIG *,int,BIG *,int,int);
extern void FF_karmul_upper(BIG *,BIG *,BIG *,BIG *,int);
extern void FF_mul(BIG *,BIG *,BIG *,int); 
extern void FF_lmul(BIG *,BIG *,BIG *,int);
extern void FF_mod(BIG *,BIG *,int);
extern void FF_sqr(BIG *,BIG *,int);
extern void FF_reduce(BIG *,BIG *,BIG *,BIG *,int);
extern void FF_dmod(BIG *,BIG *,BIG *,int);
extern void FF_invmodp(BIG *,BIG *,BIG *,int);
extern void FF_nres(BIG *,BIG *,int);
extern void FF_redc(BIG *,BIG *,BIG *,int);
extern void FF_invmod2m(BIG *,BIG *,int);
extern void FF_random(BIG *,csprng *,int);
extern void FF_randomnum(BIG *,BIG *,csprng *,int);
extern void FF_modmul(BIG *,BIG *,BIG *,BIG *,BIG *,int);
extern void FF_modsqr(BIG *,BIG *,BIG *,BIG *,int);
extern void FF_skpow(BIG *,BIG *,BIG *,BIG *,int);
extern void FF_skspow(BIG *,BIG *,BIG,BIG *,int);
extern void FF_power(BIG *,BIG *,int,BIG *,int);
extern void FF_pow(BIG *,BIG *,BIG *,BIG *,int);
extern int FF_cfactor(BIG *,sign32,int);
extern int FF_prime(BIG *,csprng *,int);
extern void FF_pow2(BIG *,BIG *,BIG,BIG *,BIG,BIG *,int);

/* Octet string handlers */
extern void OCT_output(octet *);
extern void OCT_output_string(octet *);
extern void OCT_clear(octet *);
extern int  OCT_comp(octet *,octet *);
extern int  OCT_ncomp(octet *,octet *,int);
extern void OCT_jstring(octet *,char *);
extern void OCT_jbytes(octet *,char *,int);
extern void OCT_jbyte(octet *,int,int);
extern void OCT_joctet(octet *,octet *);
extern void OCT_xor(octet *,octet *);
extern void OCT_empty(octet *);
extern int OCT_pad(octet *,int);
extern void OCT_tobase64(char *,octet *);
extern void OCT_frombase64(octet *,char *);
extern void OCT_copy(octet *,octet *);
extern void OCT_xorbyte(octet *,int);
extern void OCT_chop(octet *,octet *,int);
extern void OCT_jint(octet *,int,int);
extern void OCT_rand(octet *,csprng *,int);
extern void OCT_shl(octet *,int);

/* Hash function */
extern void HASH_init(hash *);
extern void HASH_process(hash *,int);
extern void HASH_hash(hash *,char *);


/* AES functions */
extern void AES_reset(aes *,int,char *);
extern void AES_getreg(aes *,char *);
extern void AES_init(aes* ,int,char *,char *);
extern void AES_ecb_encrypt(aes *,uchar *);
extern void AES_ecb_decrypt(aes *,uchar *);
extern unsign32 AES_encrypt(aes* ,char *);
extern unsign32 AES_decrypt(aes *,char *);
extern void AES_end(aes *);

/* AES-GCM functions */
extern void GCM_init(gcm*,char *,int,char *);
extern int GCM_add_header(gcm* ,char *,int );
extern int GCM_add_plain(gcm *,char *,char *,int);
extern int GCM_add_cipher(gcm *,char *,char *,int);
extern void GCM_finish(gcm *,char *);

/* random numbers */
extern void RAND_seed(csprng *,int,char *);
extern void RAND_clean(csprng *);
extern int RAND_byte(csprng *);

#endif
