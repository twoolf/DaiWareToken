/* 
 * File:   CvMiraclDefs.h
 * Author: mony
 *
 * Created on November 16, 2012, 4:42 PM
 */

#ifndef CVMIRACLDEFS_H
#define	CVMIRACLDEFS_H

#define AES_SECURITY	128
#define BN_BYTES		(((2 * AES_SECURITY / 8) + 2 - ((2 * AES_SECURITY / 8 + 2) % 3)) / 3 * 4)
#define KEY_BYTES		(((AES_SECURITY / 8) + 2 - ((AES_SECURITY / 8 + 2) % 3)) / 3 * 4)
#define G1_BYTES		(2 * BN_BYTES + 2 + 1)
#define G2_BYTES		(4 * BN_BYTES + 2 + 3)
#define ECN_BYTES		(2 * BN_BYTES + 2 + 1)
#define HASH_LEN		32
//#define FS				32
#define HASH_BYTES		FS

#endif	/* CVMIRACLDEFS_H */

