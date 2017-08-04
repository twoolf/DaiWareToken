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
/* certivox.c - basic BN curve stuff, EC curve support, and AES-GCM */
/* M. Scott November 2012 */

#include "aesGcm.h"


/* AES-GCM Encryption and Decryption of octets, K is key, H is header, P is plaintext, C is ciphertext, T is authentication tag */
void AES_GCM_ENCRYPT(octet *K,octet *IV,octet *H,octet *P,octet *C,octet *T)
{
	gcm g;
	gcm_init(&g,K->len,K->val,IV->len,IV->val);
	gcm_add_header(&g,H->val,H->len);
	gcm_add_cipher(&g,GCM_ENCRYPTING,P->val,P->len,C->val);
	C->len=P->len;
	gcm_finish(&g,T->val); 
	T->len=16;
}

void AES_GCM_DECRYPT(octet *K,octet *IV,octet *H,octet *C,octet *P,octet *T)
{
	gcm g;
	gcm_init(&g,K->len,K->val,IV->len,IV->val);
	gcm_add_header(&g,H->val,H->len);
	gcm_add_cipher(&g,GCM_DECRYPTING,P->val,C->len,C->val);
	P->len=C->len;
	gcm_finish(&g,T->val); 
	T->len=16;
}
