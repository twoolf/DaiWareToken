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
/*! \file  common_c.h
    \brief MIRACL Common SAKKE/ECCSI/ECDH header file

*-  Project     : SkyKey SDK
*-  Authors     : M. Scott, modified by Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 10, 2013, 5:10 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

*/

#ifndef COMMON_C_H
#define	COMMON_C_H

#ifdef	__cplusplus
extern "C"
{
#endif

#define AS				16	/* Secret size - 128 bits */
#define FS				2*AS	/* Field size fixed at compile time - 32 bytes for 256 bit field size */
#define GS				FS	/* Group Size */
#define HASH_BYTES                      FS

#if HASH_BYTES == 20
	#define HASHFUNC	sha
	#define SHS_INIT	shs_init
	#define SHS_PROCESS	shs_process
	#define SHS_HASH	shs_hash
	#define HASH_BLOCK	64
#endif

#if HASH_BYTES==32
	#define HASHFUNC	sha256
	#define SHS_INIT	shs256_init
	#define SHS_PROCESS	shs256_process
	#define SHS_HASH	shs256_hash
	#define HASH_BLOCK	64
#endif

#if HASH_BYTES==48
	#define HASHFUNC	sha384
	#define SHS_INIT	shs384_init
	#define SHS_PROCESS	shs384_process
	#define SHS_HASH	shs384_hash
	#define HASH_BLOCK	128
#endif

#if HASH_BYTES==64
	#define HASHFUNC	sha512
	#define SHS_INIT	shs512_init
	#define SHS_PROCESS	shs512_process
	#define SHS_HASH	shs512_hash
	#define HASH_BLOCK	128
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_C_H */

