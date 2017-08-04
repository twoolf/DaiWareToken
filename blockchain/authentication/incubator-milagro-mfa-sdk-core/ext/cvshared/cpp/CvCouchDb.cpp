/* 
 * File:   CvCouchDb.cpp
 * Author: mony
 * 
 * Created on August 28, 2012, 12:23 PM
 */

#include "CvHttpCommon.h"


#include "CvCouchDb.h"

#include "CvHttpRequest.h"
#include "CvLogger.h"
#include "json/writer.h"

using namespace std;
using namespace CvShared;

CvCouchDb::CvCouchDb( const CvString& aHost, u_short aPort, const CvString& aUser, const CvString& aPassword ) :
	m_host(aHost), m_port(aPort), m_user(aUser), m_password(aPassword)
{
	if ( m_user.empty() )
		m_url.Format( "http://%s:%d", m_host.c_str(), m_port );
	else
		m_url.Format( "http://%s:%s@%s:%d", m_user.c_str(), m_password.c_str(), m_host.c_str(), m_port );
}

CvCouchDb::~CvCouchDb()
{
}

bool CvCouchDb::CreateDatabase( const CvString& aDbName ) const
{
	CvString dbName;
	CvHttpRequest::EncodeURL( aDbName, dbName );
	
	CvHttpRequest httpRequest;
	
	httpRequest.SetUrl( m_url + "/" + dbName );
	httpRequest.SetMethod( enHttpMethod_PUT );
	
	if ( httpRequest.Execute() != CvHttpRequest::enStatus_Ok )
		return false;
	
	json::Object joResponse;
	
	try
	{
		istringstream iss( httpRequest.GetResponse() );
		json::Reader::Read( joResponse, iss );
	}
	catch ( exception& e )
	{
		LogMessage( enLogLevel_Error, "Exception [%s] while parsing json response: %s", e.what(), httpRequest.GetResponse().c_str() );
		return false;
	}
	catch ( ... )
	{
		LogMessage( enLogLevel_Error, "Unexpected exception while parsing json response: %s", httpRequest.GetResponse().c_str() );
		return false;	
	}
	
	if ( httpRequest.GetResponseCode() >= 200 &&
		httpRequest.GetResponseCode() < 300 &&
		(json::Boolean&)joResponse["ok"] )
		return true;
		
	const string& error = (json::String&)joResponse["error"];
	const string& reason = (json::String&)joResponse["reason"];
	
	LogMessage( enLogLevel_Error, "ERROR while trying to create database [%s] on CouchDB [%s]: %s (%s)",
			aDbName.c_str(), m_url.c_str(), reason.c_str(), error.c_str() );

	return false;
}

bool CvCouchDb::IsDatabaseExists( const CvString& aDbName ) const
{
	CvString dbName;
	CvHttpRequest::EncodeURL( aDbName, dbName );
	
	CvHttpRequest httpRequest;
	
	httpRequest.SetUrl( m_url + "/" + dbName );
	httpRequest.SetMethod( enHttpMethod_GET );
	
	if ( httpRequest.Execute() != CvHttpRequest::enStatus_Ok )
		return false;
	
	json::Object joResponse;
	
	try
	{
		istringstream iss( httpRequest.GetResponse() );
		json::Reader::Read( joResponse, iss );
	}
	catch ( exception& e )
	{
		LogMessage( enLogLevel_Error, "Exception [%s] while parsing json response: %s", e.what(), httpRequest.GetResponse().c_str() );
		return false;
	}
	catch ( ... )
	{
		LogMessage( enLogLevel_Error, "Unexpected exception while parsing json response: %s", httpRequest.GetResponse().c_str() );
		return false;	
	}
	
	return ( httpRequest.GetResponseCode() >= 200 &&
		httpRequest.GetResponseCode() < 300 &&
		((json::String&)joResponse["db_name"]).Value() == aDbName );
}

bool CvCouchDb::PutDocument( const CvString& aDbName, const CvString& aId, const json::Object aDoc, OUT bool& abExists ) const
{
	abExists = false;
	
	CvString dbName;
	CvHttpRequest::EncodeURL( aDbName, dbName );

	CvString docId;
	CvHttpRequest::EncodeURL( aId, docId );

	CvHttpRequest httpRequest;
	
	httpRequest.SetUrl( m_url + "/" + dbName + "/" + docId );
	httpRequest.SetMethod( enHttpMethod_PUT );
	
	ostringstream oss;
	
	json::Writer::Write( aDoc, oss );
	
	CvString content = oss.str();
	
	httpRequest.SetContent( content.data(), content.length() );
	
	CMapHttpHeaders mapHeaders;	
			
	httpRequest.SetHeaders( mapHeaders );
	
	if ( httpRequest.Execute() != CvHttpRequest::enStatus_Ok )
		return false;
	
	json::Object joResponse;
		
	try
	{
		istringstream iss( httpRequest.GetResponse() );
		json::Reader::Read( joResponse, iss );
	}
	catch ( exception& e )
	{
		LogMessage( enLogLevel_Error, "Exception [%s] while parsing json response: %s", e.what(), httpRequest.GetResponse().c_str() );
		return false;
	}
	catch ( ... )
	{
		LogMessage( enLogLevel_Error, "Unexpected exception while parsing json response: %s", httpRequest.GetResponse().c_str() );
		return false;	
	}
	
	if ( httpRequest.GetResponseCode() >= 200 &&
		httpRequest.GetResponseCode() < 300 &&
		(json::Boolean&)joResponse["ok"] )
		return true;
		
	const string& error = (json::String&)joResponse["error"];
	const string& reason = (json::String&)joResponse["reason"];
	
	abExists = ( error == "conflict" && reason == "Document update conflict." );
			
	LogMessage( enLogLevel_Error, "ERROR while trying to create document [%s/%s] on CouchDB [%s]: %s (%s)",
			aDbName.c_str(), aId.c_str(), m_url.c_str(), reason.c_str(), error.c_str() );

	return false;
}

bool CvCouchDb::GetDocument( const CvString& aDbName, const CvString& aId, json::Object& aDoc ) const
{
	CvString dbName;
	CvHttpRequest::EncodeURL( aDbName, dbName );
	
	CvString docId;
	CvHttpRequest::EncodeURL( aId, docId );

	CvHttpRequest httpRequest;
	
	httpRequest.SetUrl( m_url + "/" + dbName + "/" + docId );
	httpRequest.SetMethod( enHttpMethod_GET );
	
	if ( httpRequest.Execute() != CvHttpRequest::enStatus_Ok )
		return false;
	
	try
	{
		istringstream iss( httpRequest.GetResponse() );
		json::Reader::Read( aDoc, iss );
	}
	catch ( exception& e )
	{
		LogMessage( enLogLevel_Error, "Exception [%s] while parsing json response: %s", e.what(), httpRequest.GetResponse().c_str() );
		return false;
	}
	catch ( ... )
	{
		LogMessage( enLogLevel_Error, "Unexpected exception while parsing json response: %s", httpRequest.GetResponse().c_str() );
		return false;	
	}
	
	if ( httpRequest.GetResponseCode() >= 200 &&
		httpRequest.GetResponseCode() < 300 &&
		((json::String&)aDoc["_id"]).Value() == aId )
		return true;
		
	const string& error = (json::String&)aDoc["error"];
	const string& reason = (json::String&)aDoc["reason"];
	
	LogMessage( enLogLevel_Error, "ERROR while trying to get document [%s/%s] from CouchDB [%s]: %s (%s)",
			aDbName.c_str(), aId.c_str(), m_url.c_str(), reason.c_str(), error.c_str() );

	return false;
}

