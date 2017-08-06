/* 
 * File:   main.cpp
 * Author: mony
 *
 * Created on January 10, 2013, 4:48 PM
 */

#include <cstdlib>

#define TEST_AESGCM		1
#define TEST_SAKKE		1
#define TEST_ECCSI		1
#define TEST_MIKEY		1

#if TEST_AESGCM == 1
	#include "CvAesGcm.h"
#endif

#if TEST_SAKKE == 1
	#include "CvSakke.h"
#endif

#if TEST_ECCSI == 1
	#include "CvEccsi.h"
#endif

#if TEST_MIKEY == 1
	#include "CvMikey.h"
#endif

#include "CvXcode.h"

#include "csprng_c.h"

using namespace std;
using namespace CvShared;

/*
 * 
 */
int main( int argc, char** argv )
{
	mr_init_threading();
	
	/* Crypto Strong RNG */
	char raw[100]; 
	octet octetRAW = { 0, sizeof(raw), raw };

	unsigned long ran;
	time((time_t *)&ran);

	octetRAW.val[0] = ran;
	octetRAW.val[1] = ran>>8;
	octetRAW.val[2] = ran>>16;
	octetRAW.val[3] = ran>>24;
	for ( int i = 4; i < 100; i++ )
		octetRAW.val[i] = i+1;
	octetRAW.len = 100;

	csprng rng;		
	CREATE_CSPRNG( &rng, &octetRAW ); 
	
#if TEST_AESGCM == 1
	{
		CvAesGcm aes( &rng );
		
		string cipher;
		if ( aes.Encrypt( "olfI2QIfssQInb7dicPvpQ==", "hello", cipher ) )
		{
			printf( "Cipher: %s\n", cipher.c_str() );
			
			string plain;
			if ( aes.Decrypt( "olfI2QIfssQInb7dicPvpQ==", cipher, plain ) )
			{
				printf( "Plain: %s\n", plain.c_str() );				
			}
			else
			{
				printf( "AESGCM decryption FAILED.\n" );
			}
		}
		else
		{
			printf( "AESGCM encryption FAILED.\n" );
		}
	}
#endif

#if TEST_SAKKE == 1
	{
		CvSakke sakke;

		string plainData;
		string encapsulatedData;
		
		uint8_t data[AS] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		CvBase64::Encode( data, sizeof(data), plainData );
		
		if ( sakke.Encapsulate( plainData, 
				"[Er++AvSwHYPfPrDSLu0e4ynZnwXSz0WRtVh4f2RzSc8=,DrpxYl8+jEDRUDIeoMZaOfwaXWI0XqeDqhRLb6tWZBA=]",
				"user-b@certivox.com/SKY/1012/000",
				encapsulatedData,
				"[HZj8uD8vbHOaUUeHsjr6cai4INxNeT4vn3sSdkEwDIw=,EjE8VXHa8hlAngk965KIW1Ki9lfnx+fatJ47XjYWGJA=]#[CHKdbkka2fNosLisNOhbRowJElMuUfmxGCG8crb0ba0=,B8IEjcUQ0n/BnoqE4nmi84EPGdkpwRk7JD3RWZq2Mhc=,BLujWWb9/xLiHP2t9jNJ4rzkM9WlyJWNTM1JNa4HClg=,A7cIic6Wv9l8MgO1+EaXw6L7wRr50aGBtz3Tl+zr1R0=]" ) )
		{
			printf( "Encapsulated Data: %s\n", encapsulatedData.c_str() );			
		}
		else
		{
			printf( "Encapsulate error\n" );
		}
		
		plainData.clear();
		
		if ( sakke.Decapsulate( encapsulatedData,
				"[Er++AvSwHYPfPrDSLu0e4ynZnwXSz0WRtVh4f2RzSc8=,DrpxYl8+jEDRUDIeoMZaOfwaXWI0XqeDqhRLb6tWZBA=]",
				"user-b@certivox.com/SKY/1012/000",
				"[CesWMbdsa6rs/0Tl8HIY6ummE45alKjALtia59SA6ek=,BTD8+Mmy8ymPpUPznV46u92FMYeOLrNRf0Pt89aJgYQ=,CFgu8ccRgYa9GNa8vbXLZVj4DM5mwzSUJixVdvNpZJg=,H0HGIaLUrvxfeGHuctVtgFfrVTJu2uv3cQ0EJNLV9lA=]",
				plainData,
				"[HZj8uD8vbHOaUUeHsjr6cai4INxNeT4vn3sSdkEwDIw=,EjE8VXHa8hlAngk965KIW1Ki9lfnx+fatJ47XjYWGJA=]#[CHKdbkka2fNosLisNOhbRowJElMuUfmxGCG8crb0ba0=,B8IEjcUQ0n/BnoqE4nmi84EPGdkpwRk7JD3RWZq2Mhc=,BLujWWb9/xLiHP2t9jNJ4rzkM9WlyJWNTM1JNa4HClg=,A7cIic6Wv9l8MgO1+EaXw6L7wRr50aGBtz3Tl+zr1R0=]" ) )
		{
			printf( "Plain Data: %s\n", plainData.c_str() );			
		}
		else
		{
			printf( "Decapsulate error\n" );			
		}
	}
#endif
	
#if TEST_ECCSI == 1
	{
		
		CvEccsi eccsi( &rng );
		
		string message = "top secret";
		
		if ( eccsi.Verify( message.c_str(), message.length(), "user@certivox.com", 
				"[o01KHX6grAomcjfycPLvuITpcGDgN14zV3vcnycHbUI=,/zc2uVFvbo87WM3zZRozD46EPmibQKC/x+fN1lwWqTU=]",
				"GF1ObVTI2pIUQeikp7ZDJ+4GncTwL4KQg3lCMJCGhrxr1VgWwV25hSMZd4DGzV11BZHNs/3s1OLKz97qVNvecwSvlgGAE5i1O1Di/nPCaw7JdP+e2w1VMCnE5tPFQPMPpr13TdtcZS8CSaWhAdxtcEqKiWI9DFjT/Fg85ARgyvFY" ) )
		{
			printf( "Signature verified\n" );
		}
		else
		{
			printf( "Signature NOT verified\n" );
		}
		
		string signature;
		
		if ( eccsi.Sign( message.c_str(), message.length(), "user@certivox.com", 
				"[o01KHX6grAomcjfycPLvuITpcGDgN14zV3vcnycHbUI=,/zc2uVFvbo87WM3zZRozD46EPmibQKC/x+fN1lwWqTU=]",
				"q9VUAWSgGf6oAukMFXz5P5d8TsIHKSx4cPFsslOVtXA=",
				"BK+WAYATmLU7UOL+c8JrDsl0/57bDVUwKcTm08VA8w+mvXdN21xlLwJJpaEB3G1wSoqJYj0MWNP8WDzkBGDK8Vg=",
				signature ) )
		{
			printf( "Signature: %s\n", signature.c_str() );
		}
		else
		{
			printf( "Signature FAILED\n" );
		}
		
		if ( eccsi.ValidateSecret( "user@certivox.com",
				"[o01KHX6grAomcjfycPLvuITpcGDgN14zV3vcnycHbUI=,/zc2uVFvbo87WM3zZRozD46EPmibQKC/x+fN1lwWqTU=]",
				"q9VUAWSgGf6oAukMFXz5P5d8TsIHKSx4cPFsslOVtXA=",
				"BK+WAYATmLU7UOL+c8JrDsl0/57bDVUwKcTm08VA8w+mvXdN21xlLwJJpaEB3G1wSoqJYj0MWNP8WDzkBGDK8Vg=") )
		{
			printf( "Secret is valid\n" );
		}
		else
		{
			printf( "Secret is NOT valid\n" );
		}
	}
#endif
	
#if TEST_MIKEY == 1
	{
		string payload = "ELsBK44roKnFCHAk8yJcUQ==";
		
		string senderId = "user-a@certivox.com/SKY/1012/000";
		string senderEccsiSecret = "bThXA29iARXnbjDRhflFw3a95i/wZNhANQ+DgC+AzEM=";
		string senderEccsiPrivateKey = "BEQvsN3iq+dL1EUYGmLC3g2QZcb8SyQ9FqaWK1LE/HYrFj6R5SscptKz5g7h9NT4s4l1WH75kHN3Afv27AHeppo=";
		
		string kms = "server@certivox.com";
		string kmsEccsiPublicKey = "[cGzEoFPdpVZ3V6DyRwHTBH+esl3zIiNEIksiRrBidMY=,nJZoN8pLlmurHrhUGSaK1P9t1lHRzqWpfHx66nJrs3E=]";
		
		string receiverId = "user-b@certivox.com/SKY/1012/000";
		string receiverSakkePrivateKey = "[CesWMbdsa6rs/0Tl8HIY6ummE45alKjALtia59SA6ek=,BTD8+Mmy8ymPpUPznV46u92FMYeOLrNRf0Pt89aJgYQ=,CFgu8ccRgYa9GNa8vbXLZVj4DM5mwzSUJixVdvNpZJg=,H0HGIaLUrvxfeGHuctVtgFfrVTJu2uv3cQ0EJNLV9lA=]";
		
		string kmsSakkePublicKey = "[Er++AvSwHYPfPrDSLu0e4ynZnwXSz0WRtVh4f2RzSc8=,DrpxYl8+jEDRUDIeoMZaOfwaXWI0XqeDqhRLb6tWZBA=]";
		string kmsSakkePublicParams = "[HZj8uD8vbHOaUUeHsjr6cai4INxNeT4vn3sSdkEwDIw=,EjE8VXHa8hlAngk965KIW1Ki9lfnx+fatJ47XjYWGJA=]#[CHKdbkka2fNosLisNOhbRowJElMuUfmxGCG8crb0ba0=,B8IEjcUQ0n/BnoqE4nmi84EPGdkpwRk7JD3RWZq2Mhc=,BLujWWb9/xLiHP2t9jNJ4rzkM9WlyJWNTM1JNa4HClg=,A7cIic6Wv9l8MgO1+EaXw6L7wRr50aGBtz3Tl+zr1R0=]";
		
		string message;		// = "AQ4OAQIAIHVzZXItYUBjZXJ0aXZveC5jb20vU0tZLzEwMTIvMDAwDgICACB1c2VyLWJAY2VydGl2b3guY29tL1NLWS8xMDEyLzAwMP4DAgATc2VydmVyQGNlcnRpdm94LmNvbQQCAgBRgcAGTHxJXhZbAU5OSMRFPAQbv2diNEqEUcM8F8EU2CReBWsEVZyYzUsS9fbjozo8ViQ1VOF6e010euATGhLlS0htU5j73SzWhCROwEWiQiU8ACCBJTCJ0+Al3L5/2DO4MB7xND6YIi3i8lC/d9AgBu4E5XxiO8vPS9yhzdNBJdw35UJg9Gp/zJCflX63cc7riUG2EQREL7Dd4qvnS9RFGBpiwt4NkGXG/EskPRamlitSxPx2KxY+keUrHKbSs+YO4fTU+LOJdVh++ZBzdwH79uwB3qaa";
		
		CvMikey mikey(&rng);
		
		mikey.CreateMessage( payload, senderId, receiverId, kms, kms, kmsSakkePublicParams, kmsSakkePublicKey,
							senderEccsiSecret, senderEccsiPrivateKey, kmsEccsiPublicKey, message );
		
		printf( "MIKEY message: %s\n", message.c_str() );
		
		payload.clear();
		
		if ( mikey.ProcessMessage( message, receiverId, kms, receiverSakkePrivateKey, kmsSakkePublicParams, kmsSakkePublicKey,
									kmsEccsiPublicKey, payload ) )
		{
			printf( "Message: %s\n", payload.c_str() );
		}
		else
		{
			printf( "Message processing FAILED.\n" );
		}
	}
#endif
	
	KILL_CSPRNG( &rng );
	
	return 0;
}

