/***************************************************************************************************************************************************************************************************************************
																																																						   *
This file is part of CertiVox M-Pin Client and Server Libraries.																																						   *
The CertiVox M-Pin Client and Server Libraries provide developers with an extensive and efficient set of strong authentication and cryptographic functions.																   *
For further information about its features and functionalities please refer to http://www.certivox.com																													   *
The CertiVox M-Pin Client and Server Libraries are free software: you can redistribute it and/or modify it under the terms of the BSD 3-Clause License http://opensource.org/licenses/BSD-3-Clause as stated below.		   *
The CertiVox M-Pin Client and Server Libraries are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   *
Note that CertiVox Ltd issues a patent grant for use of this software under specific terms and conditions, which you can find here: http://certivox.com/about-certivox/patents/											   * 	
Copyright (c) 2013, CertiVox UK Ltd																																														   *	
All rights reserved.																																																	   *
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:																			   *
�	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.																						   *	
�	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.			   *	
�	Neither the name of CertiVox UK Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.								   *
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,																		   *
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS																	   *
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE																	   *	
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,														   *
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.																		   *	
																																																						   *
***************************************************************************************************************************************************************************************************************************/
/*! \file  CvHttpRequest.h
    \brief C++ class providing portable HTTP request functionality

*-  Project     : SkyKey SDK
*-  Authors     : Mony Aladjem
*-  Company     : Certivox
*-  Created     : August 28, 2012, 4:30 PM
*-  Last update : February 15, 2013
*-  Platform    : Windows XP SP3 - Windows 7
*-  Dependency  : WinHttp.lib (Windows) libcurl.so (Linux)

 C++ class providing portable HTTP request functionality

*/

#ifndef CVHTTPREQUEST_H
#define	CVHTTPREQUEST_H

#include "CvHttpCommon.h"
#include "CvMutex.h"
#include "CvTime.h"
#include "CvCommon.h"

#include <sstream>
#include <iostream>


#if defined(_WIN32)

	#include "Winhttp.h"

#elif defined(__linux__) || defined(__MACH__)

	#include <curl/curl.h>
	#include <curl/easy.h>

#else

    #error "Unsupported OS"

#endif

class CvHttpRequest
{
public:

	typedef std::string				String;
	typedef CvShared::TimeValue_t	TimeValue_t;
	typedef CvShared::Seconds		Seconds;

#if defined(__linux__)
	// OpenSSL Multi-treading support
	class COpenSslMt
	{
	public:
		COpenSslMt();
		~COpenSslMt();
	private:
		static void LockCallback( int mode, int type, const char* file, int line );
		static unsigned long ThreadId();
		
		static CvShared::CvMutex* m_lockArray;
	};
#endif
	
	CvHttpRequest( enHttpMethod_t aMethod = enHttpMethod_GET );
	~CvHttpRequest();

	enum enStatus_t
	{
		enStatus_Ok = 0,
		enStatus_ClientError,
		enStatus_ServerError,
		enStatus_NetworkError
	};
	
	static const TimeValue_t TIMEOUT_INFINITE = -1;
	
	enStatus_t	Execute( const Seconds& aTimeout = TIMEOUT_INFINITE, bool abProgress = false );
	void		SetHeaders( const CMapHttpHeaders &aHeaders );

	void		SetContent( const char* apData, int64_t aSize )	{ m_req.data = apData; m_req.data_size = aSize; }
	void		SetMethod( enHttpMethod_t aMethod )	{ m_req.method = aMethod; }
	void		SetUrl( const String& aUrl )			{ m_req.url = aUrl; }
	void		SetFileName( const String& aFileName )	{ m_req.fname = aFileName; };
	void		SetResumePos( int64_t aPos )			{ m_req.resume_from = aPos; }
	void		SetProxy( const String& aProxy )		{ m_req.proxy = aProxy; }
	
	const String&	GetUrl() const		{ return m_req.url; }
	enHttpMethod_t	GetMethod() const	{ return m_req.method; }
	
	/////////////////////////////////////////////////////////////////////////////////////
	
	void			SetCancel()							{ CvShared::CvMutexLock lock(m_mutex); m_bCancel = true; }
	const String&	GetResponse() const					{ return m_response; }
	long			GetResponseCode() const				{ return m_responseCode; }
	const String&	GetResponseHeader( const String& aKey ) const;
	const CMapHttpHeaders& GetResponseHeaders() const	{ return m_responseHeaders; }
	int64_t			GetContentSize() const				{ return m_contentSize; }
	int64_t			GetProgressUp() const				{ return m_progressUp; }
	int64_t			GetProgressDown() const				{ return m_progressDown; }
	
	void			Clear();
	
	static bool		EncodeURL( const String& aUrl, OUT String& aEncodedUrl );
	
protected:
	
	typedef CvShared::CvMutex	CvMutex;
	
	struct sRequestData_t
	{
		sRequestData_t()	{ Clear(); }
		void	Clear();
		
#if defined (_WIN32)
		String				header_list;
#elif defined (__linux__) || defined(__MACH__)
		struct curl_slist*	header_list;
#endif
		const char*			data;
		int64_t				data_size;

		enHttpMethod_t		method;
		String				url;
		long				no_progress;
		int64_t				resume_from;
		String				fname;
		long long			resp_size;	// Used only for Windows implementation
		String				proxy; // <host:port>
		time_t				timeout;	//in seconds
	};
	
	void			Start();
	
#if defined (_WIN32)
	
	bool			SendRequestData( HINTERNET ahRequest );
	bool			ReadResponseCode( HINTERNET ahRequest );
	bool			ReadResponseHeaders( HINTERNET ahRequest );
	bool			ReadResponseData( HINTERNET ahRequest );
	
#elif defined (__linux__) || defined(__MACH__)
	
	static size_t	WriteData(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t	WriteToFile(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static size_t	ReadResponse(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t	HeaderCallback(void *ptr, size_t size, size_t nmemb, void *userdata);
	static int		SetProgress(void *data, double total, double dlnow, double ultotal, double ulnow);
	
	bool			m_bResponseDataStarted;
	
#endif

	sRequestData_t	m_req;

	bool			m_bCancel;
	String			m_response;
	long			m_responseCode;
	CMapHttpHeaders m_responseHeaders;
	int64_t			m_contentSize;
	int64_t			m_progressUp;
	int64_t			m_progressDown;
	CvMutex			m_mutex;
};

#endif	// CVHTTPREQUEST_H