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

#include "MpinClient.h"

#include "HttpRequest.h"

#include "CvLogger.h"
#include "CvTime.h"

#include <fstream>

using namespace std;
using CvShared::SleepFor;
using CvShared::Millisecs;
using CvShared::Seconds;
using CvShared::TimeSpec;
using CvShared::GetCurrentTime;
using CvShared::LogMessage;
using CvShared::enLogLevel_Info;
using CvShared::enLogLevel_Error;
using CvShared::enLogLevel_Warning;
using CvShared::enLogLevel_Debug1;
using CvShared::enLogLevel_Debug2;
using CvShared::enLogLevel_Debug3;

MPinSDK::IHttpRequest* CMpinClient::CContext::CreateHttpRequest() const
{
	return new CHttpRequest(30);
}
		
CMpinClient::CStorage::CStorage(const String& aFileNameSuffix)
{
	m_fileName = "client-storage-";
	m_fileName += aFileNameSuffix;
}

bool CMpinClient::CStorage::SetData(const String& data)
{
	std::ofstream file( m_fileName.c_str() );
	file << data;
	file.close();
	
	LogMessage( enLogLevel_Debug3, "Writing data to [%s]:\n%s", m_fileName.c_str(), data.c_str() );
	
	return true;
}

bool CMpinClient::CStorage::GetData(OUT String &data)
{
	std::ifstream file( m_fileName.c_str() );
	std::stringstream buffer;
	buffer << file.rdbuf();	
	file.close();
	
	data = buffer.str();
	
	LogMessage( enLogLevel_Debug3, "Reading data from [%s]:\n%s", m_fileName.c_str(), data.c_str() );
	
	return true;
}
		
CMpinClient::CMpinClient( int aClientId, const String& aBackendUrl, const String& aUserId ) :
	m_bInitialized(false), m_id(aClientId), m_userId(aUserId),
	m_storageSecure( String().Format("sec-%d", aClientId) ), m_storageNonSecure( String().Format("%d", aClientId) ),
	m_context( String().Format("%d",aClientId), &m_storageSecure, &m_storageNonSecure ),
	m_thread(aUserId), m_queue(aUserId.c_str()), m_bIdle(false), m_bStatsEnabled(true)
{
	std::ifstream filePin( String().Format("pin-%d", m_id).c_str() );
	filePin >> m_pinGood;
	filePin >> m_pinBad;
	
	_Init(aBackendUrl);	
}

CMpinClient::CMpinClient( int aClientId, const String& aBackendUrl, const String& aUserId, const String& aPinGood, const String& aPinBad, const String& aRegOTC ) :
	m_bInitialized(false), m_id(aClientId), m_userId(aUserId), m_pinGood(aPinGood), m_pinBad(aPinBad), m_regOTC(aRegOTC),
	m_storageSecure( String().Format("sec-%d", aClientId) ), m_storageNonSecure( String().Format("%d", aClientId) ),
	m_context( String().Format("%d",aClientId), &m_storageSecure, &m_storageNonSecure ),
	m_thread(aUserId), m_queue(aUserId.c_str()), m_bIdle(false), m_bStatsEnabled(true)
{
	std::ofstream filePin( String().Format("pin-%d", m_id).c_str() );
	filePin << m_pinGood << " " << m_pinBad;
	
	_Init(aBackendUrl);
}

CMpinClient::~CMpinClient()
{
	m_queue.PushFront(enEvent_Exit);
	
	SleepFor( Millisecs(100) );
}

bool CMpinClient::_Init(const String& aBackendUrl)
{
	LogMessage( enLogLevel_Info, "Initializing client #%d for [%s] with PIN [%s] and BAD PIN [%s]", m_id, m_userId.c_str(), m_pinGood.c_str(), m_pinBad.c_str() );
	
	StringMap config;
	config["backend"] = aBackendUrl;
	
	MPinSDK::Status status = m_sdk.Init( config, &m_context );
	
	if ( status != MPinSDK::Status::OK )
	{
		LogMessage( enLogLevel_Error, "Client #%d for user [%s] couldn't be initialized: %s", m_id, m_userId.c_str(), status.GetErrorMessage().c_str() );
		if ( m_bStatsEnabled )
		{
			++m_stats.m_numOfErrors;
		}
		m_bIdle = true;
		
		return false;
	}
	
	m_thread.Create(this);
	
	m_bInitialized = true;
	
	return true;
}
	
bool CMpinClient::_AuthenticateGood()
{
	return _Authenticate( m_pinGood );
}

bool CMpinClient::_AuthenticateBad()
{
	return _Authenticate( m_pinBad );
}

bool CMpinClient::_Register()
{
	if (!m_bInitialized)
	{
		LogMessage( enLogLevel_Error, "Client #%d for user [%s] was not initialized", m_id, m_userId.c_str() );
		return false;
	}
	
	LogMessage( enLogLevel_Info, "Registering user [%s]...", m_userId.c_str() );
			
	std::vector<MPinSDK::UserPtr> listUsers;
	m_sdk.ListUsers( listUsers );
	
	std::vector<MPinSDK::UserPtr>::iterator itr = listUsers.begin();
	for ( ;itr != listUsers.end(); ++itr )
	{
		if ( (*itr)->GetId() == m_userId )
		{
			m_sdk.DeleteUser( *itr );
			break;
		}
	}

	MPinSDK::UserPtr user = m_sdk.MakeNewUser( m_userId, String().Format( "M-Pin Test Client #%d", m_id ) );
	
	TimeSpec now;
	GetCurrentTime(now);
	Millisecs startTime = now.ToMillisecs();

	MPinSDK::Status status = m_sdk.StartRegistration( user, m_regOTC, "{ \"data\": \"test\" }" );
	
	if ( status != MPinSDK::Status::OK )
	{
		LogMessage( enLogLevel_Error, "Failed in StartRegistration(): %s [%d]", status.GetErrorMessage().c_str(), status.GetStatusCode() );
		if ( m_bStatsEnabled )
		{
			++m_stats.m_numOfErrors;
		}
		return false;
	}
	
	if ( user->GetState() != MPinSDK::User::ACTIVATED )
	{
		while ( user->GetState() != MPinSDK::User::REGISTERED )
		{
			LogMessage( enLogLevel_Info, "User [%s] has NOT been activated yet", user->GetId().c_str() );

			CvShared::SleepFor( CvShared::Seconds(10) );

			status = m_sdk.ConfirmRegistration( user );
			
			if ( status == MPinSDK::Status::OK )
			{
				LogMessage( enLogLevel_Info, "User [%s] has been activated", user->GetId().c_str() );
				continue;
			}
				
			if ( status != MPinSDK::Status::IDENTITY_NOT_VERIFIED )
			{
				LogMessage( enLogLevel_Error, "Failed in ConfirmRegistration(): %s [%d]", status.GetErrorMessage().c_str(), status.GetStatusCode() );
				if ( m_bStatsEnabled )
				{
					++m_stats.m_numOfErrors;
				}
				return false;
			}
		}
	}
	else
	{
		LogMessage( enLogLevel_Info, "User [%s] has been force-activated", user->GetId().c_str() );
		
		status = m_sdk.ConfirmRegistration( user );

		if ( status != MPinSDK::Status::OK )
		{
			LogMessage( enLogLevel_Error, "Failed in ConfirmRegistration(): %s [%d]", status.GetErrorMessage().c_str(), status.GetStatusCode() );
			if ( m_bStatsEnabled )
			{
				++m_stats.m_numOfErrors;
			}
			return false;
		}
	}
	
	status = m_sdk.FinishRegistration( user, m_pinGood );
	
	if ( status != MPinSDK::Status::OK )
	{
		LogMessage( enLogLevel_Error, "Failed in FinishRegistration(): %s [%d]", status.GetErrorMessage().c_str(), status.GetStatusCode() );
		if ( m_bStatsEnabled )
		{
			++m_stats.m_numOfErrors;
		}
		return false;
	}
	
	GetCurrentTime(now);
	
	if ( m_bStatsEnabled )
	{
		uint32_t currMsec = now.ToMillisecs() - startTime.Value();
		m_stats.m_avgRegMsec = ( m_stats.m_avgRegMsec*m_stats.m_numOfReg + currMsec ) / ( m_stats.m_numOfReg + 1 );
		++m_stats.m_numOfReg;
		
		if ( currMsec < m_stats.m_minRegMsec || m_stats.m_minRegMsec == 0 )
		{
			m_stats.m_minRegMsec = currMsec;
		}
		if ( currMsec > m_stats.m_maxRegMsec )
		{
			m_stats.m_maxRegMsec = currMsec;
		}
	}
	
	return true;
}

bool CMpinClient::_Authenticate( const String& aPin )
{
	if (!m_bInitialized)
	{
		LogMessage( enLogLevel_Error, "Client #%d for user [%s] was not initialized", m_id, m_userId.c_str() );
		return false;
	}
	
	std::vector<MPinSDK::UserPtr> listUsers;
	m_sdk.ListUsers( listUsers );
	
	std::vector<MPinSDK::UserPtr>::iterator itr = listUsers.begin();
	for ( ;itr != listUsers.end(); ++itr )
	{
		if ( (*itr)->GetId() == m_userId )
			break;
	}
	
	if ( itr == listUsers.end() )
	{
		LogMessage( enLogLevel_Warning, "User [%s] not found in the list", m_userId.c_str() );
		if ( m_bStatsEnabled )
		{
			++m_stats.m_numOfErrors;
		}
		return false;
	}
	
	MPinSDK::UserPtr user = *itr;
	
	if ( aPin == m_pinGood )
	{
		LogMessage( enLogLevel_Info, "Authenticating user [%s] with correct PIN...", user->GetId().c_str() );
	}
	else
	{
		LogMessage( enLogLevel_Info, "Authenticating user [%s] with incorrect PIN...", user->GetId().c_str() );		
	}
	
	TimeSpec now;
	GetCurrentTime(now);
	Millisecs startTime = now.ToMillisecs();

	MPinSDK::Status status = m_sdk.StartAuthentication( user );
	
	if ( status != MPinSDK::Status::OK )
	{
		LogMessage( enLogLevel_Error, "Failed in StartAuthentication(): %s [%d]", status.GetErrorMessage().c_str(), status.GetStatusCode() );
		if ( m_bStatsEnabled )
		{
			++m_stats.m_numOfErrors;
		}
		return false;
	}
	
	status = m_sdk.FinishAuthentication( user, aPin );
	
	if ( aPin == m_pinGood )
	{
		if ( status != MPinSDK::Status::OK && user->GetState() != MPinSDK::User::BLOCKED )
		{
			LogMessage( enLogLevel_Error, "ERROR: Authentication for user [%s] failed: %s [%d]", user->GetId().c_str(), status.GetErrorMessage().c_str(), status.GetStatusCode() );
			if ( m_bStatsEnabled )
			{
				++m_stats.m_numOfErrors;
			}
			return false;
		}

		if ( user->GetState() == MPinSDK::User::BLOCKED )
		{
			LogMessage( enLogLevel_Error, "Authentication for user [%s] has failed because the user has been BLOCKED previously.", user->GetId().c_str() );
		}
		else
		{
			LogMessage( enLogLevel_Info, "Authentication for user [%s] succeeded", user->GetId().c_str() );
		}
	}
	else
	{
		if ( status == MPinSDK::Status::OK )
		{
			LogMessage( enLogLevel_Error, "ERROR: Authentication for user [%s] succeeded ?!", user->GetId().c_str() );
			if ( m_bStatsEnabled )
			{
				++m_stats.m_numOfErrors;
			}
			return false;
		}
		else if ( status != MPinSDK::Status::INCORRECT_PIN )
		{
			LogMessage( enLogLevel_Error, "ERROR: Authentication for user [%s] failed: %s [%d]", user->GetId().c_str(), status.GetErrorMessage().c_str(), status.GetStatusCode() );
			if ( m_bStatsEnabled )
			{
				++m_stats.m_numOfErrors;
			}
			return false;
		}
		
		LogMessage( enLogLevel_Info, "Authentication for user [%s] not successful (OK): %s [%d]", user->GetId().c_str(), status.GetErrorMessage().c_str(), status.GetStatusCode() );		
	}

	GetCurrentTime(now);
	
	if ( m_bStatsEnabled )
	{
		uint32_t currMsec = now.ToMillisecs() - startTime.Value();
		m_stats.m_avgAuthMsec = ( m_stats.m_avgAuthMsec*m_stats.m_numOfAuth + currMsec ) / ( m_stats.m_numOfAuth + 1 );
		++m_stats.m_numOfAuth;
		
		if ( currMsec < m_stats.m_minAuthMsec || m_stats.m_minAuthMsec == 0 )
		{
			m_stats.m_minAuthMsec = currMsec;
		}
		if ( currMsec > m_stats.m_maxAuthMsec )
		{
			m_stats.m_maxAuthMsec = currMsec;
		}
	}
	
	return true;
}

long CMpinClient::CThread::Body( void* apArgs )
{
	CMpinClient* pClient = (CMpinClient*)apArgs;
	uint32_t id = pClient->m_id;
	
	bool bExit = false;
	
	while (!bExit)
	{
		enEvent_t event;
			
		if ( !pClient->m_queue.Pop( event, 0 ) )
		{
			pClient->m_bIdle = true;
			
			if ( !pClient->m_queue.Pop( event ) )
			{
				LogMessage( enLogLevel_Error, "Client #%d: Error popping from the event queue. Thread [%s]", pClient->m_id, m_name.c_str() );
				SleepFor( Millisecs(500) );
				continue;
			}
		}
		
		pClient->m_bIdle = false;
		
		switch (event)
		{
			case enEvent_Register:
				pClient->_Register();
				break;
			case enEvent_AuthenticateGood:
				pClient->_AuthenticateGood();
				break;
			case enEvent_AuthenticateBad:
				pClient->_AuthenticateBad();
				break;
			case enEvent_Exit:
				pClient->m_bIdle = true;				
				bExit = true;
				break;
		}
	}
	
	LogMessage( enLogLevel_Debug1, "Client thread #%d is exiting...", id );
	
	return 0;
}
