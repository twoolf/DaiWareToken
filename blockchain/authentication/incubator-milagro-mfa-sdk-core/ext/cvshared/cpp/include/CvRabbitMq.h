/* 
 * File:   CvRabbitMq.h
 * Author: mony
 *
 * Created on August 31, 2012, 12:11 PM
 */

#ifndef CVRABBITMQ_H
#define	CVRABBITMQ_H

#include "CvCommon.h"
#include "CvString.h"
#include "CvTime.h"

#include <amqp.h>

#include <string>

class CvRabbitMq
{
public:
	typedef std::string				String;
	typedef CvShared::TimeValue_t	TimeValue_t;
	typedef CvShared::Millisecs		Millisecs;
	
	CvRabbitMq( const String& aQueueName );
	virtual ~CvRabbitMq();

	static const TimeValue_t TIMEOUT_INFINITE = -1;
	static const TimeValue_t TIMEOUT_NO_WAIT = 0;
	
	bool	Read( OUT uint8_t* apBuf, size_t aMaxLen, OUT size_t& aReadLen, const Millisecs& aTimeout = TIMEOUT_INFINITE );
	bool	Write( const uint8_t* apData, size_t aLen );	
	
	bool	Connect( const String& aHost, u_short aPort, const String& aUser, const String& aPassword );
	bool	Disconnect();
	
protected:
	CvRabbitMq( const CvRabbitMq& orig ) : m_channel(1)	{}
	
	static bool	CheckForError( int aError, const char* aContext );
	static bool	CheckForError( amqp_rpc_reply_t& aReply, const char* aContext );
	static bool	CheckForError( amqp_connection_state_t& aConn, const char* aContext );	
	
	String					m_host;
	u_short					m_port;
	String					m_user;
	String					m_password;
	
	String					m_name;
	amqp_connection_state_t	m_amqpConn;
	const amqp_channel_t	m_channel;
};

#endif	/* CVRABBITMQ_H */

