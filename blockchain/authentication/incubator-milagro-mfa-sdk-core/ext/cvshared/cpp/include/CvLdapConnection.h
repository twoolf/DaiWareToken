/* 
 * File:   CvLdapConnection.h
 * Author: mony
 *
 * Created on August 20, 2012, 2:53 PM
 */

#ifndef CVLDAPCONNECTION_H
#define	CVLDAPCONNECTION_H

#include <ldap.h>

#include "CvCommon.h"
#include "CvTime.h"

#include <list>
#include <map>
#include <string>

class CvLdapResult;

enum enLdapScope_t
{
	enLdapScope_Default = LDAP_SCOPE_DEFAULT,
	enLdapScope_Base = LDAP_SCOPE_BASE,
	enLdapScope_OneLevel = LDAP_SCOPE_ONELEVEL,
	enLdapScope_SubTree = LDAP_SCOPE_SUBTREE,
	enLdapScope_Children = LDAP_SCOPE_CHILDREN
};

typedef std::string	LdapString;
	
class CvLdapConnection
{
public:
	typedef CvShared::Millisecs	Millisecs;
	
	CvLdapConnection( const LdapString& aHostUri );
	virtual ~CvLdapConnection();
	
	LDAP*	GetHandle() const	{ return m_pLdapConnection; }
	
	bool	Bind( const LdapString& aUser, const LdapString& aPassword, OUT int& aErrCode, OUT LdapString& aErrDesc );
	
	bool	Search( const LdapString& aBaseDn, enLdapScope_t aScope, const LdapString& aFilter, const Millisecs& aTimeout,
					OUT CvLdapResult& aResult, OUT int& aErrCode, OUT LdapString& aErrDesc );
	
	inline bool	Search( const LdapString& aBaseDn, enLdapScope_t aScope, const Millisecs& aTimeout,
					OUT CvLdapResult& aResult, OUT int& aErrCode, OUT LdapString& aErrDesc );
	
private:
	CvLdapConnection(const CvLdapConnection& orig)	{}
	bool	Init( const LdapString& aHostUri );
	bool	Unbind();
	bool	Reconnect();
	
	LdapString	m_hostUri;
	int			m_port;
	LdapString	m_user;
	LdapString	m_password;
	
	LDAP*		m_pLdapConnection;
	bool		m_bBound;
};

bool CvLdapConnection::Search( const LdapString& aBaseDn, enLdapScope_t aScope, const Millisecs& aTimeout, OUT CvLdapResult& aResult, OUT int& aErrCode, OUT LdapString& aErrDesc )
{
	return Search( aBaseDn, aScope, "objectClass=*", aTimeout, aResult, aErrCode, aErrDesc );
}

class CvLdapResult
{
	friend class CvLdapConnection;
	
public:
	
	class CEntry
	{
		friend class CvLdapResult;

	public:
		typedef std::list<LdapString>				CListValues;
		typedef std::map<LdapString, CListValues>	CMapAttrs;

		virtual ~CEntry()	{}

		const LdapString&	GetDn() const		{ return m_dn; }
		const CMapAttrs&	GetAttrs() const	{ return m_mapAttrs; }

	protected:
		CEntry( const CvLdapConnection& aConnection, LDAPMessage* apLdapEntry );

		LdapString	m_dn;
		CMapAttrs	m_mapAttrs;
	};
	
	typedef std::list<CEntry*>	CListEntries;
	
	CvLdapResult()	{}
	virtual ~CvLdapResult();
	
	const CListEntries&	GetEntries() const		{ return m_listEntries; }

protected:
	CvLdapResult( const CvLdapConnection& aConnection, LDAPMessage* apLdapResult );
	
	bool Init( const CvLdapConnection& aConnection, LDAPMessage* apLdapResult );
	void Clear();
	
	CListEntries	m_listEntries;
};

#endif	/* CVLDAPCONNECTION_H */
