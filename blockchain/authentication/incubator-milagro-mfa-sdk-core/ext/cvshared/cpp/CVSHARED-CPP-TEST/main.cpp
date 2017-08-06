/* 
 * File:   main.cpp
 * Author: mony
 *
 * Created on August 20, 2012, 2:53 PM
 */

#include "CvLdapConnection.h"

#include <cstdlib>
#include <list>

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <fstream>

#include "stdio.h"
#include "CvThread.h"
#include "CvMutex.h"
#include "CvLogger.h"
#include "CvString.h"
#include "CvHttpRequest.h"
#include "CvHttpServerMg.h"
#include "CvHttpServerUv.h"
#include "json/reader.h"
#include "json/elements.h"
#include "json/writer.h"
#include "CvCouchDb.h"
#include "CvXcode.h"
#include "CvRabbitMq.h"
#include "CvXml.h"
#include "CvStateMachine.h"
#include "CvQueue.h"
#include "CvFileSystem.h"

using namespace std;
using namespace CvShared;

#define TEST_THREADS_AND_MUTEXES	0
#define TEST_HTTP_SERVER			0
#define TEST_HTTP_SERVER_UV			0
#define TEST_RABBIT_MQ				0
#define TEST_JSON					0
#define TEST_XML					0
#define TEST_XCODING				0
#define TEST_STATE_MACHINE			1
#define TEST_QUEUE					1
#define TEST_HTTP_REQUEST			1
#define TEST_FILE_SYSTEM			1

CvMutex mutex("mutex-test");

class CMyThread : public CvThread
{
public:
	CMyThread( const char* apName ) : CvThread( apName )	{}
	
protected:
	virtual long Body( void* apArgs );
};

long CMyThread::Body(void* apArgs)
{
	LogMessage( enLogLevel_Info, "Thread [%s] is running with arguments [%p]", m_name.c_str(), apArgs );
	
	CvMutexLock lock( mutex, 5800 );
	
	if ( lock.IsLocked() )
		LogMessage( enLogLevel_Info, "Thread [%s] got the mutex", m_name.c_str() );
	else
		LogMessage( enLogLevel_Info, "Thread [%s] ain't got the mutex", m_name.c_str() );		
	
	SleepFor(2500);
	
	return (long)apArgs;
}

void LogCurrentTime()
{
	DateTime dt;
	GetCurrentDateTime( dt );
	
	LogMessage( enLogLevel_Info, "Current time: %s %d-%s-%d %02d:%02d:%02d",
			dt.GetDayOfWeekName().c_str(), dt.GetDay(), dt.GetMonthName(false).c_str(), dt.GetYear(),
			dt.GetHour(), dt.GetMinute(), dt.GetSecond() );
}

class CMyHttpServer : public CvHttpServerMg
{
public:
	CMyHttpServer( CMapOptions& aOptions ) : CvHttpServerMg(aOptions)	{}
	
protected:
	virtual bool OnReceiveRequest( INOUT CvContext& aContext );
};

class CMyHttpServerUv : public CvHttpServerUv
{
public:
	CMyHttpServerUv( CMapOptions& aOptions ) : CvHttpServerUv(aOptions)	{}
	
protected:
	virtual bool OnReceiveRequest( INOUT CvContext& aContext );
};

#if TEST_HTTP_SERVER == 1
bool CMyHttpServer::OnReceiveRequest( INOUT CvContext& aContext )
#else
bool CMyHttpServerUv::OnReceiveRequest( INOUT CvContext& aContext )
#endif
{
	const CvRequest& request = aContext.GetRequest();
	CvResponse& response = aContext.GetResponse();
	
	vector<CvString> tokens;
	request.GetUri().Tokenize( "/", tokens );

	if ( tokens.back() != "ldap" )
		return false;
	
	CvString base;
	CvString filter;
	CvString scopeStr;
	enLdapScope_t scope = enLdapScope_Base;
	CvString id;

	if ( request.GetMethod() == enHttpMethod_POST )
	{
		if ( request.GetHeaderValue("Content-Length").Ulong() == 0 )
			return false;

		istringstream iss( (const char*)request.GetContent() );
		json::Object joRoot;

		try
		{
			json::Reader::Read( joRoot, iss );
		}
		catch( const json::Exception& e )
		{
			LogMessage( enLogLevel_Error, "ERROR while parsing JSON data: %s", e.what() );
			return false;
		}

		base = ((json::String&)joRoot["params"]["baseDN"]).Value();
		filter = ((json::String&)joRoot["params"]["filter"]).Value();

		scopeStr = ((json::String&)joRoot["params"]["scope"]).Value();
		if ( scopeStr == "BASE" )
			scope = enLdapScope_Base;
		else
		if ( scopeStr == "SUB" )
			scope = enLdapScope_SubTree;
		else
		if ( scopeStr == "ONE" )
			scope = enLdapScope_OneLevel;
		else
		if ( scopeStr == "SUBORDINATE_SUBTREE" )
			scope = enLdapScope_Children;

		id = ((json::String&)joRoot["id"]).Value();
	}

	CvLdapResult result;

	{
		CvMutexLock lock(mutex);

		CvLdapConnection ldapConn( "ldap://localhost:389" );

		int err = 0;
		string errMsg;

		if ( !ldapConn.Bind( "cn=root", "lite1268", err, errMsg) )
		{
			LogMessage( enLogLevel_Info, "LDAP Bind failed: %s (%d)", errMsg.c_str(), err );
			return -1;
		}

		if ( !ldapConn.Search( base, scope, filter, Seconds(10), result, err, errMsg ) )
		{
			LogMessage( enLogLevel_Info, "LDAP Search failed: %s (%d)", errMsg.c_str(), err );
			return -1;
		}
	}

	stringstream reply;

	json::Object joReply;

	joReply["id"] = json::String(id);

	joReply["matches"] = json::Array();

	json::Array& jarrMatches = joReply["matches"];

	for ( CvLdapResult::CListEntries::const_iterator itr = result.GetEntries().begin();
			itr != result.GetEntries().end();
			++itr )
	{
		const CvLdapResult::CEntry* pEntry = (*itr);			

		json::Object joEntry;
		joEntry["DN"] = json::String(pEntry->GetDn());

		for ( CvLdapResult::CEntry::CMapAttrs::const_iterator itrAttrs = pEntry->GetAttrs().begin();
				itrAttrs != pEntry->GetAttrs().end();
				++itrAttrs )
		{
			const string& attrName = itrAttrs->first;
			const CvLdapResult::CEntry::CListValues& attrValues = itrAttrs->second;

			joEntry[attrName] = json::Array();
			json::Array& jarrValues = joEntry[attrName];

			for ( CvLdapResult::CEntry::CListValues::const_iterator itrVals = attrValues.begin();
					itrVals != attrValues.end();
					++itrVals )
			{
				jarrValues.Insert( json::String(*itrVals) );
			}
		}

		jarrMatches.Insert( joEntry );			
	}

	json::Writer::Write( joReply, reply );

	response.SetHttpVersion("1.1");
	response.SetStatusCode(200);
	response.SetStatusMessage("OK");
	response.SetHeaderValue( HTTP_HEADER_CONTENT_TYPE, "text/plain" );
	response.SetContent( reply.str().c_str(), reply.str().length() );

	aContext.SendResponse();
	
	CvCouchDb couch( "localhost", 5984, "local", "12345678" );

	couch.AssureDatabase( "test" );

	DateTime time;
	GetCurrentDateTime( time );

	CvString docName;
	docName.Format( "%02d-%s-%04d-%02d-%02d-%02d", time.GetDay(), time.GetMonthName().c_str(), time.GetYear(),
			time.GetHour(), time.GetMinute(), time.GetSecond() );

	json::Object jsonDoc;

	if ( couch.GetDocument( "test", docName, jsonDoc ) )
	{
		ostringstream oss;
		json::Writer::Write( jsonDoc, oss );

		LogMessage( enLogLevel_Info, "Existing document [%s]: %s", docName.c_str(), oss.str().c_str() );
	}

	bool bExists;
	bool bOk = couch.PutDocument( "test", docName, joReply, bExists );

	return true;
}

void TestXcoding()
{
	LogMessage( enLogLevel_Info, "Testing Base64 Xcoding" );
	
	string email = "simeon.aladjem@certivox.com";
	string base64Enc;
	CvBase64::Encode( (const uint8_t*)email.c_str(), email.length(), base64Enc );
	
	string base64Dec;	
	CvBase64::Decode( base64Enc, base64Dec );
	
	LogMessage( enLogLevel_Info, "Base64 Encode: [%s] -> [%s]", email.c_str(), base64Enc.c_str() );
	LogMessage( enLogLevel_Info, "Base64 Decode: [%s] -> [%s]", base64Enc.c_str(), base64Dec.c_str() );	

	LogMessage( enLogLevel_Info, "Testing Hex Xcoding" );
	
	string hexEnc;
	CvHex::Encode( (const uint8_t*)email.c_str(), email.length(), hexEnc );
	
	string hexDec;	
	CvHex::Decode( hexEnc, hexDec );
	
	LogMessage( enLogLevel_Info, "Hex Encode: [%s] -> [%s]", email.c_str(), hexEnc.c_str() );
	LogMessage( enLogLevel_Info, "Hex Decode: [%s] -> [%s]", hexEnc.c_str(), hexDec.c_str() );	
}

void TestJson()
{
	LogMessage( enLogLevel_Info, "Testing JSON document" );
	
	json::Object joRoot;
	joRoot["Boolean-1"] = json::Boolean(true);
	joRoot["Boolean-2"] = json::Boolean(false);
	joRoot["Number-1"] = json::Number(123);
	joRoot["Number-2"] = json::Number(456.78);
	
	ostringstream oss;
	json::Writer::Write( joRoot, oss );
	
	LogMessage( enLogLevel_Info, "JSON: %s", oss.str().c_str() );
}

void TestRabbitMq()
{
	LogMessage( enLogLevel_Info, "Testing RabbitMQ" );
	
	CvRabbitMq rabbitMq("mony-test");
	
	if ( !rabbitMq.Connect( "localhost", 5672, "guest", "guest" ) )
	{
		LogMessage( enLogLevel_Error, "Failed to connect to RabbitMQ" );
	}
	
	for ( int i = 0; i < 20; ++i )
	{
		CvString message;
		
		DateTime dt;
		GetCurrentDateTime( dt );
		message.Format( "Test message [%d] [0x%04X] [%02d:%02d:%02d]", i+1, i+1, dt.GetHour(), dt.GetMinute(), dt.GetSecond() );
		
		if ( rabbitMq.Write( (const uint8_t*)message.c_str(), message.length() ) )
			LogMessage( enLogLevel_Info, "[%d] Written [%d] bytes to rabbit-mq: %s", i+1, message.length(), message.c_str() );
		else
			LogMessage( enLogLevel_Info, "[%d] ERROR while writing [%d] bytes to rabbit-mq: %s", i+1, message.length(), message.c_str() );			
		
		SleepFor( 100 );
	}
	
	SleepFor( Seconds(10) );
		
	for ( int i = 0; i < 50; ++i )
	{
		char buff[256] = {'\0'};
		
		size_t readLen = 0;
		if ( rabbitMq.Read( (uint8_t*)buff, sizeof(buff), readLen, Millisecs(1500) ) )
			LogMessage( enLogLevel_Info, "[%d] Read [%d] bytes from rabbit-mq: %s", i+1, readLen, buff );
		else
			LogMessage( enLogLevel_Info, "[%d] Failed to reading bytes from rabbit-mq: Actually read [%d]", i+1, readLen );
		
//		Sleep( Seconds(1) );		
	}
}

void TestXml()
{
	LogMessage( enLogLevel_Info, "Testing XML document" );
	
	CvXmlDoc xmlDoc;
	
	CvXmlNode* pXmlNode = xmlDoc.allocate_node( rapidxml::node_declaration );

	CvXmlAttr* pAttr = xmlDoc.allocate_attribute( "version", "1.0" );

	pXmlNode->append_attribute( pAttr );

	xmlDoc.append_node( pXmlNode );

	pXmlNode = xmlDoc.allocate_node( rapidxml::node_element, "one" );
	pAttr = xmlDoc.allocate_attribute( "attribute-one", "1 (One)" );

	pXmlNode->append_attribute( pAttr );

	xmlDoc.append_node( pXmlNode );
	
	pXmlNode->append_node( xmlDoc.allocate_node( rapidxml::node_element, "two" ) );
	
	CvString xmlMsg;
	
	rapidxml::print( back_inserter(xmlMsg), xmlDoc, 0 );
	
	LogMessage( enLogLevel_Info, "XML message: %s", xmlMsg.c_str() );
}

void TestThreadsAndMutexes()
{
	LogMessage( enLogLevel_Info, "Testing Threads and Mutexes" );
	
	mutex.Create();
	
	list<CMyThread*> threads;
	for ( int t = 0; t < 10; ++t )
	{
		CvString name;
		name.Format( "cv-thread-%d", t+1 );
		
		threads.push_back( new CMyThread( name.c_str() ) );
		
		threads.back()->Create( (void*)(0xaa5500 + t) );
		
		SleepFor( Seconds(1) );
	}
	
	while ( !threads.empty() )
	{
		delete threads.front();
		threads.pop_front();
	}
}

class CvMySmHandler : public CvSmEventHandler
{
public:
	CvMySmHandler();
	
	void Start();
	
	bool Action1( void* apData )
	{
		LogMessage( enLogLevel_Info, "Action-1 called with data [%s]", ((CvString*)apData)->c_str() );
		return true;
	}
	
	bool Action2( void* apData )
	{
		LogMessage( enLogLevel_Info, "Action-2 called with data [%s]", ((CvString*)apData)->c_str() );
		return true;
	}
	
	bool Action3( void* apData )
	{
		LogMessage( enLogLevel_Info, "Action-3 called with data [%s]", ((CvString*)apData)->c_str() );
		return !((CvString*)apData)->empty();
	}
	
	bool Condition( void* apData ) const
	{
		CvString* pData = (CvString*)apData;
		LogMessage( enLogLevel_Info, "Condition returns [%s]", pData->empty() ? "false" : "true" );
		return !pData->empty();
	}

protected:
	
	enum enStates_t
	{
		enStateInit = 0,
		enStateStarted,
		enStateWaitingForDone
	};
	
	enum enEvents_t
	{
		enEventStart = 0,
		enEventProcess,
		enEventDone
	};
	
	CvStateMachine	m_sm;
};

CvMySmHandler::CvMySmHandler() :
	m_sm("testing")
{
	m_sm.AddState( enStateInit, "Init" );
	m_sm.AddState( enStateStarted, "Started" );	
	m_sm.AddState( enStateWaitingForDone, "WaitingForDone" );
	
	m_sm.AddEvent( enEventStart, "Start" );
	m_sm.AddEvent( enEventProcess, "Process" );
	m_sm.AddEvent( enEventDone, "Done" );
	
	CvSmTransitionSimple tr1(this);
	tr1.AppendAction( (Action_t)&CvMySmHandler::Action1 );
	tr1.AppendAction( (Action_t)&CvMySmHandler::Action2 );
	
	tr1.SetNewState( enStateStarted );
	
	m_sm.AddTransition( enStateInit, enEventStart, tr1 );
	
	CvSmTransitionConditional tr2( this, (Condition_t)&CvMySmHandler::Condition );
	CvSmTransitionSimple& tr2true = tr2.GetTransitionOnTrue();
	CvSmTransitionSimple& tr2false = tr2.GetTransitionOnFalse();
	
	tr2true.AppendAction( (Action_t)&CvMySmHandler::Action2 );
	tr2true.AppendAction( (Action_t)&CvMySmHandler::Action3 );
	tr2true.SetNewState( enStateWaitingForDone );
	
	tr2false.AppendAction( (Action_t)&CvMySmHandler::Action3 );
	tr2false.AppendAction( (Action_t)&CvMySmHandler::Action1 );
	tr2false.SetNewState( enStateInit );
	tr2false.SetErrorState( enStateWaitingForDone );	
	
	m_sm.AddTransition( enStateStarted, enEventProcess, tr2 );
	
	CvSmTransitionSimple tr3(this);
	
	tr3.AppendAction( (Action_t)&CvMySmHandler::Action3 );
	tr3.AppendAction( (Action_t)&CvMySmHandler::Action2 );
	tr3.SetNewState( enStateInit );
	
	m_sm.AddTransition( enStateWaitingForDone, enEventDone, tr3 );
	
	CvSmTransitionSimple tr4(this);
	tr4.AppendAction( (Action_t)&CvMySmHandler::Action3 );
	tr4.SetNewState( enStateWaitingForDone );
	
	m_sm.AddTransition( enStateWaitingForDone, enEventProcess, tr4 );
}

void CvMySmHandler::Start()
{
	m_sm.Start( enStateInit );
	
	CvString data("Run-1");
	
	m_sm.Signal( enEventDone, &data );
	
	m_sm.Signal( enEventStart, &data );
	
	m_sm.Signal( enEventProcess, &data );
	
	m_sm.Signal( enEventProcess, &data );
	
	m_sm.Signal( enEventDone, &data );
	
	data.clear();
	
	m_sm.Signal( enEventStart, &data );
	
	m_sm.Signal( enEventProcess, &data );
	
	m_sm.Signal( enEventProcess, &data );
	
	m_sm.Signal( enEventDone, &data );	
}

//////////////////////////////////////////////////////////////////////////////

void TestStateMachine()
{
	CvMySmHandler myHandler;
	
	myHandler.Start();
}

//////////////////////////////////////////////////////////////////////////////

typedef CvQueue<string>	CMyQueue;

class CQueueProducer : public CvThread
{
public:
	CQueueProducer() : CvThread("thread-producer")	{}
	
protected:
	virtual long Body( void* apArgs );
};

long CQueueProducer::Body(void* apArgs)
{
	CMyQueue& queue = *(CMyQueue*)apArgs;
	
	for( int i = 0; i < 5; ++i )
	{
		for ( int j = 0; j < 6; ++j )
		{
			CvString message;

			DateTime dt;
			GetCurrentDateTime( dt );
			message.Format( "Test message [%d,%d] [%02d:%02d:%02d]", i+1, j+1, dt.GetHour(), dt.GetMinute(), dt.GetSecond() );
			
			LogMessage( enLogLevel_Info, "[%d,%d] Written [%d] bytes to queue: %s", i+1, j+1, message.length(), message.c_str() );
			
			queue.Push( message );
			
			SleepFor( Millisecs(500) );
		}
		
		SleepFor( Seconds(10) );		
	}
}

class CQueueConsumer : public CvThread
{
public:
	CQueueConsumer() : CvThread("thread-consumer")	{}
	
protected:
	virtual long Body( void* apArgs );
};

long CQueueConsumer::Body(void* apArgs)
{
	CMyQueue& queue = *(CMyQueue*)apArgs;
	
	for( int i = 0; i < 40; ++i )
	{
		CvString message;

		if ( !queue.Pop( message, CMyQueue::TIMEOUT_INFINITE ) )
		{
			LogMessage( enLogLevel_Info, "[%d] Couldn't pop element from queue", i+1 );
			continue;
		}
		
		LogMessage( enLogLevel_Info, "[%d] Read [%d] bytes from queue: %s", i+1, message.length(), message.c_str() );		
	}
}

//////////////////////////////////////////////////////////////////////////////

void TestQueue()
{
	CMyQueue queue("my-queue");
	
	CQueueProducer producer;
	
	producer.Create( &queue );
	
	CQueueConsumer consumer;
	
	consumer.Create( &queue );
	
	SleepFor( Minutes(1) );
}

//////////////////////////////////////////////////////////////////////////////

void TestHttpRequest()
{
	CMapHttpHeaders headers;
	headers["Content-Length"] = "0";
	headers["Content-Type"] = "text/plain";
	headers["Custom-Header"] = "test custom header";
	
	CvHttpRequest httpRequest(enHttpMethod_GET);
	
	httpRequest.SetUrl( "http://www.dispostable.com" );
	httpRequest.SetHeaders( headers );
	
	CvHttpRequest::enStatus_t s = httpRequest.Execute();
	
	if ( s != CvHttpRequest::enStatus_Ok )
	{
		printf( "ERROR in http request to [%s]: %d\n", httpRequest.GetUrl().c_str(), s );
		return;
	}
	
	printf( "Http response code [%ld]\n", httpRequest.GetResponseCode() );
	printf( "Http response headers:\n" );
	
	for( CMapHttpHeaders::const_iterator itr = httpRequest.GetResponseHeaders().begin();
			itr != httpRequest.GetResponseHeaders().end();
			++itr )
	{
		printf( "\t%s: %s\n", itr->first.c_str(), itr->second.c_str() );
	}
	
	printf( "Http response data length [%d]:\n", (int)httpRequest.GetResponse().length() );
	printf( "\t%s\n", httpRequest.GetResponse().c_str() );
}

//////////////////////////////////////////////////////////////////////////////

void ListDirectoryRecursively( const string& aPath, const string& aIndentation = "" )
{
	CvFileAbs::CvList list;
	
	CvDirectory dir(aPath);
	
	if ( !dir.List(list) )
		printf( "%sFailed to list directory [%s]\n", aIndentation.c_str(), dir.GetPath().c_str() );
	
	if ( list.empty() )
	{
		printf( "%s(empty)\n", aIndentation.c_str() );
		return;
	}
	
	for ( CvFileAbs::CvList::iterator itr = list.begin(); itr != list.end(); ++itr )
	{
		if ( (*itr)->GetType() == CvFileAbs::enType_Directory )
		{
			printf( "%s%s/\n", aIndentation.c_str(), (*itr)->GetName().c_str() );
			ListDirectoryRecursively( (*itr)->GetPath(), aIndentation + "    " );
		}
		else
			printf( "%s%s\n", aIndentation.c_str(), (*itr)->GetName().c_str() );
	}
}

void TestFileSystem()
{
	{
		CvFile file111( "/home/mony/dir-1/dir-11/file-111" );

		if ( file111.Exists() )
			printf( "File [%s] exists\n", file111.GetName().c_str() );
		else
			printf( "File [%s] doesn't exist\n", file111.GetName().c_str() );

		if ( file111.Create() )
			printf( "Successfully created file [%s]\n", file111.GetPath().c_str() );
		else
			printf( "Failed to create file [%s]\n", file111.GetPath().c_str() );

		if ( file111.Exists() )
			printf( "File [%s] exists\n", file111.GetName().c_str() );
		else
			printf( "File [%s] doesn't exist\n", file111.GetName().c_str() );
	}
	//-----------------------------------------------------------------------
	{
		CvDirectory dir112( "/home/mony/dir-1/dir-11/dir-112/" );

		if ( dir112.Exists() )
			printf( "Directory [%s] exists\n", dir112.GetPath().c_str() );
		else
			printf( "Directory [%s] doesn't exist\n", dir112.GetPath().c_str() );

		if ( dir112.Create() )
			printf( "Successfully created directory [%s]\n", dir112.GetPath().c_str() );
		else
			printf( "Failed to create directory [%s]\n", dir112.GetPath().c_str() );
	}
	//-----------------------------------------------------------------------	
	{
		CvFile file12( "/home/mony/dir-1/file-12.txt" );

		if ( file12.Exists() )
			printf( "File [%s] exists\n", file12.GetName().c_str() );
		else
			printf( "File [%s] doesn't exist\n", file12.GetName().c_str() );

		if ( file12.Create() )
			printf( "Successfully created file [%s]\n", file12.GetPath().c_str() );
		else
			printf( "Failed to create file [%s]\n", file12.GetPath().c_str() );

		ofstream ofs( file12.GetPath().c_str() );
		ofs << "Some very VERY VeRy vErY very VERY VeRy vErY very VERY VeRy vErY "
				"very VERY VeRy vErY very VERY VeRy vErY very VERY VeRy vErY long string\r\n";
		ofs.close();

		printf( "File [%s] size is [%lld]\n", file12.GetPath().c_str(), file12.GetSize() );
	}
	//-----------------------------------------------------------------------
	{
		CvDirectory dir13( "/home/mony/dir-1/dir-13" );

		if ( dir13.Exists() )
			printf( "Directory [%s] exists\n", dir13.GetPath().c_str() );
		else
			printf( "Directory [%s] doesn't exist\n", dir13.GetPath().c_str() );

		if ( dir13.Create() )
			printf( "Successfully created directory [%s]\n", dir13.GetPath().c_str() );
		else
			printf( "Failed to create directory [%s]\n", dir13.GetPath().c_str() );
	}
	//-----------------------------------------------------------------------
	
	ListDirectoryRecursively( "/home/mony/Desktop" );
	
	//-----------------------------------------------------------------------
	{
		CvDirectory dir1( "/home/mony/dir-1" );

		if ( dir1.Delete() )
			printf( "Successfully deleted directory [%s]\n", dir1.GetPath().c_str() );
		else
			printf( "Failed to delete directory [%s]\n", dir1.GetPath().c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////////

/*
 * 
 */
int main(int argc, char** argv)
{
	InitLogger( "my-test", enLogLevel_Debug3 );
	
	LogCurrentTime();
	
#if TEST_XCODING == 1
	TestXcoding();
#endif
	
#if TEST_JSON == 1
	TestJson();
#endif
	
#if TEST_RABBIT_MQ == 1
	TestRabbitMq();
#endif
	
#if TEST_XML == 1
	TestXml();
#endif
	
#if TEST_THREADS_AND_MUTEXES == 1
	TestThreadsAndMutexes();
#endif
	
#if TEST_HTTP_SERVER == 1
	CvHttpServer::CMapOptions options;
	options[HTTP_SERVER_OPTION_PORT] = "8081";
	options[HTTP_SERVER_OPTION_NUM_THREADS] = "16";

	CMyHttpServer server(options);
	
	server.Start();
	
	SleepFor( Hours(1) );
#endif

#if TEST_HTTP_SERVER_UV == 1
	CvHttpServer::CMapOptions options;
	options[HTTP_SERVER_OPTION_PORT] = "8081";
	options[HTTP_SERVER_OPTION_MAX_CONNECTIONS_NUM] = "128";
	
	CMyHttpServerUv server(options);
	
	server.Start();
	
	SleepFor( Hours(1) );
#endif
	
#if TEST_STATE_MACHINE == 1
	TestStateMachine();
#endif

#if TEST_QUEUE == 1
	TestQueue();
#endif

#if TEST_HTTP_REQUEST == 1
	TestHttpRequest();
#endif
	
#if TEST_FILE_SYSTEM == 1
	TestFileSystem();
#endif
	
    return 0;
}
