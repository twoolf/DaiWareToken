/*******************************************************************************
 * Copyright (c) 2014 IBM Corporation and other Contributors.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   rajathr1 - Initial Contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTAsync.h"
#include <syslog.h>

#define DEBUG 1 //make this 1 for debugging
#ifdef DEBUG
#define ERROR 1
#define WARN 1
#define INFO 1
#define FINE 1
#else
#define ERROR 1
#define WARN 1
#define INFO 0
#define FINE 0
#endif

#define QOS 0
#define TRUSTSTORE "/opt/iot/IoTFoundation.pem"

/* this maintains the status of connection
 *  0 - Not connected
 *  1 - Connected
 * -1 - Connection Failed, retry
 */
int connected = 0;
volatile MQTTAsync_token deliveredtoken;

//for parsing the json
int getDelay(char *text);

/*
 * This function is called when the message is successfully published
 */
void onSend(void* context __attribute__((unused)),
	MQTTAsync_successData* response) {

#ifdef FINE
	syslog(LOG_DEBUG, "Event with token value %d delivery confirmed\n",
			response->token);
#endif
}

/*
 * This function is called when the subscription succeeds
 */
void onSubscription(void* context __attribute__((unused)),
	MQTTAsync_successData* response __attribute__((unused))) {

#ifdef FINE
	syslog(LOG_INFO, "Subscription succeeded\n");
#endif
}

/*
 * Called when the connection is successful. Update the connected variable
 */
void onConnectSuccess(void* context __attribute__((unused)),
	MQTTAsync_successData* response __attribute__((unused))) {

#ifdef FINE
	syslog(LOG_INFO, "Connection was successful\n");
#endif
	// The connection is successful. update it to 1
	connected = 1;

}

/*
 * After sending the message disconnect from IoT/mqtt server
 */
int disconnect_mqtt_client(MQTTAsync* client) {
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc = MQTTASYNC_SUCCESS;

	opts.context = client;

	if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS) {
#ifdef ERROR
		syslog(LOG_ERR, "Failed to start sendMessage, return code %d\n", rc);
#endif
	}
	MQTTAsync_destroy(client);
	return rc;
}

/* 
 * On failure of connection to server, print the response code 
 */
void onConnectFailure(void* context __attribute__((unused)),
	MQTTAsync_failureData* response) {
#ifdef ERROR
	syslog(LOG_ERR, "Connect failed ");
	if (response) {
		syslog(LOG_ERR, "with response code : %d and with message : %s", response->code, response->message);
	}

#endif
	connected = -1; // connection has failed
}

/*
 * Function to process the subscribed messages
 */
int subscribeMessage(void *context __attribute__((unused)),
		char *topicName, int topicLen __attribute__((unused)),
		MQTTAsync_message *message) {
	/* int i; */
	char* payloadptr;
	char command[100];
	int time_delay = 0;

	payloadptr = message->payload;

	time_delay = getDelay(payloadptr);
	if(time_delay != -1) {
		sprintf(command,"sudo /sbin/shutdown -r %d", time_delay);
		syslog(LOG_INFO, "Received command to restart in %d minutes.",time_delay);
		system(command);
	} else
		syslog(LOG_ERR, "Invalid command received.");

	MQTTAsync_freeMessage(&message);
	MQTTAsync_free(topicName);
	return 1;
}

/*
 * Try to reconnect if the connection is lost
 */
void connlost(void *context, char *cause) {

	MQTTAsync client = (MQTTAsync)context;
	int rc;
#ifdef ERROR
	syslog(LOG_ERR, "Connection lost\n");
	syslog(LOG_ERR, " cause: %s\n", cause);
#endif

	syslog(LOG_INFO, "Retrying the connection\n");
		MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

		conn_opts.keepAliveInterval = 20;
		conn_opts.cleansession = 1;
		conn_opts.onSuccess = onConnectSuccess;
		conn_opts.onFailure = onConnectFailure;
		conn_opts.context = &client;
		syslog(LOG_INFO, "Retrying the connection -1 \n");
		if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
	#ifdef ERROR
			syslog(LOG_ERR, "Failed to start connect from connlost, return code %d\n", rc);
	#endif
		}
}

/* 
 * Function is used to initialize the MQTT connection handle "client"
 */
int init_mqtt_connection(MQTTAsync* client, char *address, int isRegistered,
		char* client_id, char* username, char* passwd) {

	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;

	MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;

	int rc = MQTTASYNC_SUCCESS;
	MQTTAsync_create(client, address, client_id, MQTTCLIENT_PERSISTENCE_NONE,
	NULL);

	MQTTAsync_setCallbacks(*client, NULL, NULL, subscribeMessage, NULL);

#ifdef INFO
	syslog(LOG_INFO, "Connecting to %s with client Id: %s \n", address,
			client_id);
#endif
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = onConnectSuccess;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	//only when in registered mode, set the username/passwd and enable TLS
	if (isRegistered) {
		//currently only supported mech is token based. Need to change this in future.
		conn_opts.username = username;
		conn_opts.password = passwd;
		sslopts.trustStore = TRUSTSTORE;
		sslopts.enableServerCertAuth = 1;

		conn_opts.ssl = &sslopts;
	}

	if ((rc = MQTTAsync_connect(*client, &conn_opts)) != MQTTASYNC_SUCCESS) {
#ifdef ERROR
		syslog(LOG_ERR, "Failed to start connect, return code %d\n", rc);
#endif
	}
	return rc;
}

int reconnect(MQTTAsync* client, int isRegistered,
		char* username, char* passwd) {

	syslog(LOG_INFO, "Retrying the connection\n");
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	MQTTAsync_SSLOptions sslopts = MQTTAsync_SSLOptions_initializer;

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = onConnectSuccess;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	int rc;

	//only when in registered mode, set the username/passwd and enable TLS
		if (isRegistered) {
			//currently only supported mech is token based. Need to change this in future.
			syslog(LOG_INFO, "with SSL properties\n");
			conn_opts.username = username;
			conn_opts.password = passwd;
			sslopts.trustStore = TRUSTSTORE;
			sslopts.enableServerCertAuth = 0;

			conn_opts.ssl = &sslopts;
	}

	if ((rc = MQTTAsync_connect(*client, &conn_opts)) != MQTTASYNC_SUCCESS) {
#ifdef ERROR
		syslog(LOG_ERR, "Failed to start connect, return code %d\n", rc);
#endif
	}
	return rc;
}

int subscribe(MQTTAsync* client, char *topic) {

	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc = MQTTASYNC_SUCCESS;

	opts.onSuccess = onSubscription;
	opts.context = *client;

	if ((rc = MQTTAsync_subscribe(*client, topic, QOS, &opts))
			!= MQTTASYNC_SUCCESS) {
#ifdef ERROR
		syslog(LOG_ERR, "Failed to subscribe, return code %d\n", rc);
#endif
		return rc;
	}

#ifdef INFO
	syslog(LOG_DEBUG, "Waiting for subscription "
			"on topic %s\n", topic);
#endif

	return rc;
}
/* 
 * Function is used to publish events to the IoT MQTT reciver. 
 * This reuses the "client"  
 */
int publishMQTTMessage(MQTTAsync* client, char *topic, char *payload) {

	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc = MQTTASYNC_SUCCESS;

	opts.onSuccess = onSend;
	opts.context = *client;

	pubmsg.payload = payload;
	pubmsg.payloadlen = strlen(payload);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;

	if ((rc = MQTTAsync_sendMessage(*client, topic, &pubmsg, &opts))
			!= MQTTASYNC_SUCCESS) {
#ifdef ERROR
		syslog(LOG_ERR, "Failed to start sendMessage, return code %d\n", rc);
#endif
		return rc;
	}

#ifdef INFO
	syslog(LOG_DEBUG, "Waiting for publication of %s on topic %s\n", payload,
			topic);
#endif

	return rc;
}
