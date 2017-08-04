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
/*! \file  csprng_c.h
    \brief MIRACL Strong Pseudo-random Number Generator header file

*-  Project     : SkyKey SDK
*-  Authors     : M. Scott, modified by Mony Aladjem
*-  Company     : Certivox
*-  Created     : January 10, 2013, 5:10 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : MIRACL library

*/

#ifndef CSPRNG_C_H
#define	CSPRNG_C_H

#include "miracl.h"
#include "octet_c.h"

#ifdef mr_compare
	#undef mr_compare
#endif

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

SKYKEY_API void CREATE_CSPRNG(csprng *,octet *);
SKYKEY_API void KILL_CSPRNG(csprng *);

#ifdef	__cplusplus
}
#endif

#endif	/* CSPRNG_C_H */

