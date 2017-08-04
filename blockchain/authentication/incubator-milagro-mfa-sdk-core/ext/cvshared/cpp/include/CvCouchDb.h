/* 
 * File:   CvCouchDb.h
 * Author: mony
 *
 * Created on August 28, 2012, 12:23 PM
 */

#ifndef CVCOUCHDB_H
#define	CVCOUCHDB_H

#include "CvString.h"
#include "CvCommon.h"
#include "json/elements.h"

class CvCouchDb
{
public:
	CvCouchDb( const CvString& aHost, u_short aPort, const CvString& aUser, const CvString& aPassword );

	bool		CreateDatabase( const CvString& aDbName ) const;
	inline bool	AssureDatabase( const CvString& aDbName ) const;
	bool		IsDatabaseExists( const CvString& aDbName ) const;
	
	bool		PutDocument( const CvString& aDbName, const CvString& aId, const json::Object aDoc, OUT bool& abExists ) const;
	bool		GetDocument( const CvString& aDbName, const CvString& aId, json::Object& aDoc ) const;
	
	virtual ~CvCouchDb();
	
private:
	CvCouchDb(const CvCouchDb& orig)	{}
	
	CvString	m_host;
	u_short		m_port;
	CvString	m_user;
	CvString	m_password;
	
	CvString	m_url;
};

bool CvCouchDb::AssureDatabase( const CvString& aDbName ) const
{
	if ( IsDatabaseExists( aDbName) )
		return true;
	
	return CreateDatabase( aDbName );
}
	
#endif	/* CVCOUCHDB_H */

