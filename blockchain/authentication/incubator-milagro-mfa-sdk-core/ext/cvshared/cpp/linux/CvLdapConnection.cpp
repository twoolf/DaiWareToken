/* 
 * File:   CvLdapConnection.cpp
 * Author: mony
 * 
 * Created on August 20, 2012, 2:53 PM
 */

#include "CvLdapConnection.h"

#include <iostream>

#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <list>

using namespace CvShared;

CvLdapConnection::CvLdapConnection( const LdapString& aHostUri ) :
	m_hostUri( aHostUri ), m_pLdapConnection(NULL), m_bBound(false)
{
	Init( aHostUri );
}

bool CvLdapConnection::Init( const LdapString& aHostUri )
{
	m_hostUri = aHostUri;
	
	int rc = ldap_initialize( &m_pLdapConnection, m_hostUri.c_str() );
	if ( rc != LDAP_SUCCESS )
	{
		printf( "Failed to open LDAP session with [%s]: %s (%d)", m_hostUri.c_str(), ldap_err2string(rc), rc );
		return false;
	}
	
	int opt = 3;
    ldap_set_option( m_pLdapConnection, LDAP_OPT_PROTOCOL_VERSION, &opt );
	
	return true;
}

bool CvLdapConnection::Bind( const LdapString& aUser, const LdapString& aPassword, int& aErrCode, LdapString& aErrDesc )
{
	if ( m_bBound )
	{
		aErrCode = -1;
		aErrDesc = "Already bound";
		return false;
	}
	
	m_user = aUser;
	m_password = aPassword;
	
	struct berval creds;
	creds.bv_len = aPassword.length();
	creds.bv_val = (char*)aPassword.c_str();

	struct berval* pServerCreds = NULL;
	
	aErrCode = ldap_sasl_bind_s( m_pLdapConnection, aUser.c_str(), LDAP_SASL_SIMPLE, &creds, NULL, NULL, &pServerCreds );
	if ( aErrCode != LDAP_SUCCESS )
	{
		aErrDesc = ldap_err2string(aErrCode);
		return false;
	}
	
	m_bBound = true;
	
	return true;
}

bool CvLdapConnection::Unbind()
{
	if ( !m_bBound )
		return true;
	
	int rc = ldap_unbind_ext_s( m_pLdapConnection, NULL, NULL );
	
	m_pLdapConnection = NULL;
	
	m_bBound = false;
	
	return (rc == LDAP_SUCCESS);
}

bool CvLdapConnection::Reconnect()
{
	int err;
	LdapString errMsg;
	
	return ( Unbind() &&
			Init( m_hostUri ) &&
			Bind( m_user, m_password, err, errMsg ) );
}

bool CvLdapConnection::Search( const LdapString& aBaseDn, enLdapScope_t aScope, const LdapString& aFilter, const Millisecs& aTimeout,
								OUT CvLdapResult& aResult, OUT int& aErrCode, OUT LdapString& aErrDesc )
{
	if ( m_pLdapConnection == NULL )
	{
		aErrCode = -1;
		aErrDesc = "Connection was not initialized";
		return false;
	}
	
	if ( !m_bBound )
	{
		aErrCode = -1;
		aErrDesc = "Connection was not bound";
		return false;
	}
	
	LDAPMessage* pLdapResult;
	TimeVal timeout = aTimeout.ToTimeVal();
	
	aErrCode = ldap_search_ext_s( m_pLdapConnection, aBaseDn.c_str(), aScope, aFilter.c_str(), NULL, 0, NULL, NULL, &timeout, INT_MAX, &pLdapResult );
	
	if ( aErrCode != LDAP_SUCCESS )
	{
		aErrDesc = ldap_err2string(aErrCode);
		
		if ( aErrCode != LDAP_SERVER_DOWN )
			return false;			

		if ( !Reconnect() )
			return false;

		if ( ldap_search_ext_s( m_pLdapConnection, aBaseDn.c_str(), aScope, aFilter.c_str(), NULL, 0, NULL, NULL, &timeout, INT_MAX, &pLdapResult )
				!= LDAP_SUCCESS )
			return false;
	}
	
	aResult.Init( *this, pLdapResult );
	
	ldap_msgfree( pLdapResult );
	
	aErrCode = 0;
	aErrDesc.clear();
	
	return true;
}
	
CvLdapConnection::~CvLdapConnection()
{
	Unbind();
}

CvLdapResult::CvLdapResult( const CvLdapConnection& aConnection, LDAPMessage* apLdapResult )
{
	Init( aConnection, apLdapResult );
}

bool CvLdapResult::Init( const CvLdapConnection& aConnection, LDAPMessage* apLdapResult )
{
	Clear();
	
	if ( apLdapResult == NULL )
	{
		printf( "CvLdapResult::CvLdapResult() - apLdapResult is NULL " );
		return false;
	}
	
	for ( LDAPMessage* pLdapEntry = ldap_first_entry( aConnection.GetHandle(), apLdapResult );
			pLdapEntry != NULL;
			pLdapEntry = ldap_next_entry( aConnection.GetHandle(), pLdapEntry ) )
	{
		CEntry* pEntry = new CEntry( aConnection, pLdapEntry );
		
		m_listEntries.push_back( pEntry );
	}
	
	return true;
}

CvLdapResult::~CvLdapResult()
{
	Clear();
}

void CvLdapResult::Clear()
{
	while( !m_listEntries.empty() )
	{
		CEntry* pEntry = m_listEntries.front();
		
		m_listEntries.pop_front();		
		
		delete pEntry;
	}
}

CvLdapResult::CEntry::CEntry( const CvLdapConnection& aConnection, LDAPMessage* apLdapEntry )
{
	if ( apLdapEntry == NULL )
	{
		printf( "CvLdapResult::CEntry::CEntry() - apEntry is NULL " );
		return;
	}
	
	BerElement* pBer = NULL;
	BerValue berVal = { 0, NULL };
	BerValue* pBerVals;

	int rc = ldap_get_dn_ber( aConnection.GetHandle(), apLdapEntry, &pBer, &berVal );

	if ( rc != LDAP_SUCCESS )
	{
		printf( "CvLdapResult::CEntry::CEntry() - failed to get entry dn" );
		return;
	}
	
	m_dn.assign( berVal.bv_val, berVal.bv_len );

	for ( rc = ldap_get_attribute_ber( aConnection.GetHandle(), apLdapEntry, pBer, &berVal, &pBerVals );
			rc == LDAP_SUCCESS;
			rc = ldap_get_attribute_ber( aConnection.GetHandle(), apLdapEntry, pBer, &berVal, &pBerVals ) )
	{
		if ( berVal.bv_val == NULL )
			break;

		if ( pBerVals == NULL )
			continue;

		CListValues& listValues = m_mapAttrs[berVal.bv_val];
		
		for ( int i = 0; pBerVals[i].bv_val != NULL; ++i )
		{
			listValues.push_back( pBerVals[i].bv_val );
		}
		
		ber_memfree( pBerVals );
	}

	if ( pBer != NULL )
		ber_free( pBer, 0 );
}
