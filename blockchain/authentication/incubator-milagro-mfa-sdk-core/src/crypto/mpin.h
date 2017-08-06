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

/*
 *  CLINT MPIN header file
 *  Author: M. Scott 2014
 */

#ifndef MPIN_H
#define MPIN_H

#include "clint.h"

/* Field size is assumed to be greater than or equal to group size */

#define PGS 32  /* MPIN Group Size */
#define PFS 32  /* MPIN Field Size */
#define PAS 16  /* MPIN Symmetric Key Size */

#define MPIN_OK                     0
#define MPIN_DOMAIN_ERROR          -11
#define MPIN_INVALID_PUBLIC_KEY    -12
#define MPIN_ERROR                 -13
#define MPIN_INVALID_POINT         -14
#define MPIN_DOMAIN_NOT_FOUND      -15
#define MPIN_OUT_OF_MEMORY         -16
#define MPIN_DIV_BY_ZERO           -17
#define MPIN_WRONG_ORDER           -18
#define MPIN_BAD_PIN               -19


/* Configure your PIN here */

#define MAXPIN 10000
#define PBLEN 14   /* max length of PIN in bits */

#define TIME_SLOT_MINUTES 1440 /* Time Slot = 1 day */
#define HASH_BYTES 32

/* MPIN support functions */

/* MPIN primitives */

DLL_EXPORT void MPIN_HASH_ID(octet *,octet *);
DLL_EXPORT int MPIN_EXTRACT_PIN(octet *,int,octet *); 
DLL_EXPORT int MPIN_CLIENT_1(int,octet *,csprng *,octet *,int,octet *,octet *,octet *,octet *,octet *);
DLL_EXPORT int MPIN_RANDOM_GENERATE(csprng *,octet *);
DLL_EXPORT int MPIN_CLIENT_2(octet *,octet *,octet *);
DLL_EXPORT void	MPIN_SERVER_1(int,octet *,octet *,octet *);
DLL_EXPORT int MPIN_SERVER_2(int,octet *,octet *,octet *,octet *,octet *,octet *,octet *,octet *,octet *);
DLL_EXPORT int MPIN_SERVER(int,int,octet *,octet *,octet *,octet *,octet *,octet *,octet *,octet *);
DLL_EXPORT int MPIN_RECOMBINE_G1(octet *,octet *,octet *);
DLL_EXPORT int MPIN_RECOMBINE_G2(octet *,octet *,octet *);
DLL_EXPORT int MPIN_KANGAROO(octet *,octet *);

DLL_EXPORT int MPIN_ENCODING(csprng *,octet *);
DLL_EXPORT int MPIN_DECODING(octet *);

DLL_EXPORT unsign32 today(void);
DLL_EXPORT void CREATE_CSPRNG(csprng *,octet *);
DLL_EXPORT void KILL_CSPRNG(csprng *);

DLL_EXPORT int MPIN_GET_G1_MULTIPLE(csprng *,int,octet *,octet *,octet *);
DLL_EXPORT int MPIN_GET_CLIENT_SECRET(octet *,octet *,octet *); 
DLL_EXPORT int MPIN_GET_CLIENT_PERMIT(int,octet *,octet *,octet *); 
DLL_EXPORT int MPIN_GET_SERVER_SECRET(octet *,octet *); 
DLL_EXPORT int MPIN_TEST_PAIRING(octet *,octet *);

/* For M-Pin Full */

DLL_EXPORT int MPIN_PRECOMPUTE(octet *,octet *,octet *,octet *);
DLL_EXPORT int MPIN_SERVER_KEY(octet *,octet *,octet *,octet *,octet *,octet *);
DLL_EXPORT int MPIN_CLIENT_KEY(octet *,octet *,int ,octet *,octet *,octet *,octet *);

#endif

