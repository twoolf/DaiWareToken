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

#ifndef MPINCLIENT_H
#define	MPINCLIENT_H

#include "mpin_sdk.h"

#include "CvThread.h"
#include "CvQueue.h"

typedef MPinSDK::String String;
typedef MPinSDK::StringMap StringMap;
	
class CStorage;
class CContext;

class CMpinClient
{
public:

	CMpinClient( int aClientId, const String& aBackendUrl, const String& aUserId, const String& aPinGood, const String& aPinBad, const String& aRegOTC = "" );
	CMpinClient( int aClientId, const String& aBackendUrl, const String& aUserId );

	virtual ~CMpinClient();
	
	uint32_t		GetId() const { return m_id; }
	const String&	GetUserId() const { return m_userId; }
	
	void Register()			{ m_queue.Push( enEvent_Register ); }
	void AuthenticateGood()	{ m_queue.Push( enEvent_AuthenticateGood ); }
	void AuthenticateBad()	{ m_queue.Push( enEvent_AuthenticateBad ); }
	bool Done() const		{ return m_bIdle; }
	
	struct sStats_t
	{
		sStats_t() :
			m_numOfReg(0), m_minRegMsec(0), m_maxRegMsec(0), m_avgRegMsec(0),
			m_numOfAuth(0), m_minAuthMsec(0), m_maxAuthMsec(0), m_avgAuthMsec(0),
			m_numOfErrors(0) {}
		
		uint32_t	m_numOfAuth;
		uint32_t	m_numOfReg;
		uint32_t	m_minRegMsec;
		uint32_t	m_maxRegMsec;
		uint32_t	m_avgRegMsec;
		uint32_t	m_minAuthMsec;
		uint32_t	m_maxAuthMsec;
		uint32_t	m_avgAuthMsec;
		int			m_numOfErrors;
	};
	
	void EnableStats(bool abEnable = true) { m_bStatsEnabled = abEnable; }
	const sStats_t& GetStats() const	{ return m_stats; }
	
private:
	friend class CThread;
	
	class CStorage : public MPinSDK::IStorage
	{
	public:
		CStorage(const String& aFileNameSuffix);
		virtual ~CStorage() {}
		
		virtual bool SetData(const String& data);
		virtual bool GetData(OUT String &data);
		virtual const String& GetErrorMessage() const { return m_errorMsg; }
	private:
		String	m_fileName;
		String	m_errorMsg;
	};

	class CContext : public MPinSDK::IContext
	{
	public:
		CContext( const String& aId, CStorage* apStorageSecure, CStorage* apStorageNonSecure ) :
			m_id(aId), m_pStorageSecure(apStorageSecure), m_pStorageNonSecure(apStorageNonSecure)
		{}		

		virtual ~CContext() {}

		virtual MPinSDK::IHttpRequest* CreateHttpRequest() const;
		virtual void ReleaseHttpRequest( IN MPinSDK::IHttpRequest *request ) const	{ delete request; }
		virtual MPinSDK::IStorage* GetStorage( MPinSDK::IStorage::Type type ) const	{ return (type == MPinSDK::IStorage::SECURE) ? m_pStorageSecure : m_pStorageNonSecure; }
		virtual MPinSDK::CryptoType GetMPinCryptoType() const						{ return MPinSDK::CRYPTO_NON_TEE; }

	private:
		String		m_id;
		CStorage*	m_pStorageSecure;
		CStorage*	m_pStorageNonSecure;
	};

	CMpinClient(const CMpinClient& orig);
	bool _Init(const String& aBackendUrl);
	bool _Authenticate( const String& aPin );
	bool _Register();
	bool _AuthenticateGood();
	bool _AuthenticateBad();
	
	uint32_t	m_id;
	
	MPinSDK		m_sdk;
	CStorage	m_storageSecure;
	CStorage	m_storageNonSecure;
	CContext	m_context;
	
	bool		m_bInitialized;
	
	String		m_userId;
	String		m_pinGood;
	String		m_pinBad;
	
	String		m_regOTC;
	
	enum enEvent_t
	{
		enEvent_Register,
		enEvent_AuthenticateGood,
		enEvent_AuthenticateBad,
		enEvent_Exit
	};

	typedef CvShared::CvThread				CvThread;
	typedef CvShared::CvQueue<enEvent_t>	CQueueEvents;
	
	class CThread : public CvThread
	{
	public:
		CThread( const String& aName ) : CvThread(aName.c_str()) {}
	private:
		virtual long Body( void* apArgs );
	};
		
	CThread			m_thread;
	CQueueEvents	m_queue;
	bool			m_bIdle;
	
	sStats_t		m_stats;
	bool			m_bStatsEnabled;
};

#endif	/* MPINCLIENT_H */

