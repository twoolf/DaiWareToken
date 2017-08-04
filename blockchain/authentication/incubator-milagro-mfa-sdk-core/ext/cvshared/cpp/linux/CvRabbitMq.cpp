/* 
 * File:   CvRabbitMq.cpp
 * Author: mony
 * 
 * Created on August 31, 2012, 12:11 PM
 */

#include "CvRabbitMq.h"

#include "CvLogger.h"

#include <amqp_framing.h>
#include <stdlib.h>
#include <memory.h>

const amqp_table_t amqp_empty_table = { 0, NULL };
const amqp_bytes_t amqp_empty_bytes = { 0, NULL };

using namespace CvShared;

CvRabbitMq::CvRabbitMq( const String& aQueueName ) :
	 m_name(aQueueName), m_port(0), m_amqpConn(NULL), m_channel(1)
{
}

CvRabbitMq::~CvRabbitMq()
{
	Disconnect();
}

bool CvRabbitMq::Connect( const String& aHost, u_short aPort, const String& aUser, const String& aPassword )
{
	if ( m_amqpConn != NULL || !m_host.empty() )
		Disconnect();
	
	m_host = aHost;
	m_port = aPort;
	m_user = aUser;
	m_password = aPassword;
	
	m_amqpConn = amqp_new_connection();

	int sockfd = amqp_open_socket( m_host.c_str(), m_port );
	
	if ( CheckForError( sockfd, "Opening socket" ) )
		return false;

	amqp_set_sockfd( m_amqpConn, sockfd );

	amqp_rpc_reply_t reply;
	
	reply = amqp_login( m_amqpConn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, m_user.c_str(), m_password.c_str() );
	
	if ( CheckForError( reply, "Logging in" ) )
		return false;

	amqp_channel_open( m_amqpConn, m_channel );
	
	if ( CheckForError( m_amqpConn, "Opening channel" ) )
		return false;
	
	return true;
}

bool CvRabbitMq::Disconnect()
{
	if ( m_amqpConn == NULL )
		return true;
	
	amqp_rpc_reply_t reply;	
	
	reply = amqp_channel_close( m_amqpConn, m_channel, AMQP_REPLY_SUCCESS );
	
	if ( CheckForError( reply, "Closing channel" ) )
		return false;

	reply = amqp_connection_close( m_amqpConn, AMQP_REPLY_SUCCESS );
	
	if ( CheckForError( reply, "Closing connection" ) )
		return false;
	
	int result = amqp_destroy_connection( m_amqpConn );
	
	if ( CheckForError( result, "Ending connection" ) )
		return false;
	
	m_amqpConn = NULL;
	
	m_host.clear();
	m_port = 0;
	m_user.clear();
	m_password.clear();
	
	return true;
}

bool CvRabbitMq::Read( OUT uint8_t* apBuf, size_t aMaxLen, OUT size_t& aReadLen, const Millisecs& aTimeout )
{
	aReadLen = 0;
	
	if ( m_amqpConn == NULL )
	{
		LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - Read: Connection is NULL" );
		return false;
	}
	
	bool bNoSpace = false;
	bool bDone = false;
	
	time_t timeout = aTimeout.Value();
	
	while( !bDone )
	{
		amqp_frame_t frame;
		int result;

		amqp_maybe_release_buffers( m_amqpConn );
		
		amqp_rpc_reply_t reply = amqp_basic_get( m_amqpConn, m_channel, amqp_cstring_bytes( m_name.c_str() ), TRUE );
		if( CheckForError( reply, "Basic get" ) )
			return false;

		if( reply.reply.id == AMQP_BASIC_GET_EMPTY_METHOD )
		{
			//No message was present
			if ( timeout == TIMEOUT_NO_WAIT )
				return false;
			
			if ( timeout == TIMEOUT_INFINITE )
			{
				//Infinite timeout
				SleepFor( Seconds(5) );
			}
			else
			{
				time_t timeToSleep = Seconds(1).ToMillisecs();

				if ( timeout < timeToSleep )
					timeToSleep = timeout;

				SleepFor( timeToSleep );

				timeout -= timeToSleep;
			}
			
			continue;
		}

		result = amqp_simple_wait_frame( m_amqpConn, &frame );
		if( CheckForError( result, "Simple wait (Header)" ) )
			return false;

		if( frame.frame_type != AMQP_FRAME_HEADER )
		{
			LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - Read: Expected header!" );
			return false;
		}

		size_t bodySize = frame.payload.properties.body_size;
		size_t bodyReceived = 0;
				
		while( bodyReceived < bodySize )
		{
			result = amqp_simple_wait_frame( m_amqpConn, &frame );
			if( CheckForError( result, "Simple wait (Body)" ) )
				return false;

			if (frame.frame_type != AMQP_FRAME_BODY)
			{
				LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - Read: Expected body!" );				
				return false;
			}

			size_t fragmentSize = frame.payload.body_fragment.len;
			size_t readSize = ( aReadLen + fragmentSize < aMaxLen ) ? fragmentSize : aMaxLen - aReadLen;
			
			bNoSpace = ( readSize < fragmentSize );
			
			memcpy( &apBuf[aReadLen], frame.payload.body_fragment.bytes, readSize );
			
			bodyReceived += fragmentSize;
			aReadLen += readSize;
		}
		
		bDone = true;
	}
	
	return !bNoSpace;
}

bool CvRabbitMq::Write( const uint8_t* apData, size_t aLen )
{
	amqp_basic_properties_t props;
	memset(&props, 0, sizeof props);
	
	props._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.delivery_mode = 2;	// persistent delivery mode
//	props._flags |= AMQP_BASIC_CONTENT_TYPE_FLAG;
//	props.content_type = amqp_cstring_bytes("text/plain");	
	
	amqp_bytes_t message_bytes = { aLen, (void*)apData };
	
	int result = amqp_basic_publish( m_amqpConn, m_channel,
									amqp_cstring_bytes(""),
									amqp_cstring_bytes( m_name.c_str() ),
									FALSE,
									FALSE,
									&props,
									message_bytes );
   
	if ( CheckForError( result, "Publishing" ) )
		return false;
   
	return true;
}
	
bool CvRabbitMq::CheckForError( int aRetVal, const char* aContext )
{
	if ( aRetVal >= 0 )
		return false;
	
	char* errstr = amqp_error_string(-aRetVal);
	LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - %s: %s", aContext, errstr );
	free(errstr);
	
	return true;
}

bool CvRabbitMq::CheckForError( amqp_rpc_reply_t& aReply, const char* aContext )
{
	switch( aReply.reply_type )
	{
	case AMQP_RESPONSE_NORMAL:
		return false;
		
	case AMQP_RESPONSE_NONE:
		LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - %s: missing RPC reply type!", aContext );
		break;

	case AMQP_RESPONSE_LIBRARY_EXCEPTION:
		{
			char* errstr = amqp_error_string( aReply.library_error );
			LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - %s: %s", aContext, errstr );
			free( errstr );
		}
		break;

	case AMQP_RESPONSE_SERVER_EXCEPTION:
		
		switch( aReply.reply.id )
		{
		case AMQP_CONNECTION_CLOSE_METHOD:
			{
				amqp_connection_close_t* m = (amqp_connection_close_t*)aReply.reply.decoded;
				LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - %s: server connection error %d, message: %.*s",
						aContext, m->reply_code, (int)m->reply_text.len, (char*)m->reply_text.bytes );
			}
			break;
			
		case AMQP_CHANNEL_CLOSE_METHOD:
			{
				amqp_channel_close_t* m = (amqp_channel_close_t*)aReply.reply.decoded;
				LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - %s: server channel error %d, message: %.*s",
						aContext, m->reply_code, (int)m->reply_text.len, (char*)m->reply_text.bytes);
			}				
			break;

		default:
			LogMessage( enLogLevel_Error, "ERROR in RabbitMQ - %s: unknown server error, method id 0x%08X",
					aContext, aReply.reply.id );
			break;
		}
		
		break;
	}
	
	return true;
}

bool CvRabbitMq::CheckForError( amqp_connection_state_t& aConn, const char* aContext )
{
	amqp_rpc_reply_t reply = amqp_get_rpc_reply( aConn );
	
	return CheckForError( reply, aContext );
}