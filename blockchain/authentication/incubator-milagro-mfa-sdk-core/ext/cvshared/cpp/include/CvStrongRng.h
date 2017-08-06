/* 
 * File:   CvStrongRng.h
 * Author: mony
 *
 * Created on November 9, 2012, 4:44 PM
 */

#ifndef CVSTRONGRNG_H
#define	CVSTRONGRNG_H

#include "CvEntropyServer.h"

#include <string>
#include <memory>
#include <memory>

namespace CvShared
{

enum CSRNG_TYPE
{	OLD
};

enum CSRNG_MODE
{	NEWI
};

class CvStrongRng
{        
    //forbid copy and =
    CvStrongRng(const CvStrongRng& orig);
    void operator=(const CvStrongRng& orig);      
        
    csprng	m_csprng;
        
    class CDongleSource 
	{
		public:
			CDongleSource(){};            
			~CDongleSource(){strong_kill( &m_csprng );}

			csprng& dongle_slurp( bool abEnableEntropy, const String& aEntropyServerUrl = "", const String& aEntropyAlgorithm = "" );

		private:
			//forbid copy and =
			CDongleSource(const CDongleSource&);
			void operator=(const CDongleSource&);

			csprng	m_csprng;
	};
        
public:
        
	CvStrongRng(CSRNG_TYPE);
	CvStrongRng(CSRNG_MODE = NEWI);
        
	virtual ~CvStrongRng();
        
        //old interface
	static void Init( bool abEnableEntropy, const String& aEntropyServerUrl = "", const String& aEntropyAlgorithm = "" );
        csprng& Csprng()
        { return m_csprng; 
        }
        
	static bool m_bEnableEntropy;
        static String m_aEntropyServerUrl;
        static String m_aEntropyAlgorithm;
        
        //new interface
        const std::unique_ptr<SystemCSPRNG>		ISystemSource;
        const std::unique_ptr<CDongleSource>	IDongleSource;
        
};
}
#endif	/* CVSTRONGRNG_H */

