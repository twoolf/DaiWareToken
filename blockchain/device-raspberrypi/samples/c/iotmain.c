/*******************************************************************************
 * Copyright (c) 2014 IBM Corporation and other Contributors.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *   Jeffrey Dare - Initial Contribution
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include "iot.h"
#include "MQTTAsync.h"
#include <syslog.h>

char configFile[50] = "/etc/iotsample-raspberrypi/device.cfg";
float PI = 3.1415926;
float MIN_VALUE = -1.0;
float MAX_VALUE = 1.0;

char clientId[MAXBUF];
char publishTopic[MAXBUF] = "iot-2/evt/status/fmt/json";
char subscribeTopic[MAXBUF] = "iot-2/cmd/reboot/fmt/json";

//flag to check if running in registered mode or quickstart mode
// registered mode = 1
// quickstart mode = 0
int isRegistered = 0;

MQTTAsync client;

//config file structure
struct config {
	char org[MAXBUF];
	char type[MAXBUF];
	char id[MAXBUF];
	char authmethod[MAXBUF];
	char authtoken[MAXBUF];
};

int get_config(char* filename, struct config * configstr);
void getClientId(struct config * configstr, char* mac_address);
float sineVal(float minValue, float maxValue, float duration, float count);
void sig_handler(int signo);
int reconnect_delay(int i);

//cpustat.c
float getCPUTemp();
float GetCPULoad();
//mac.c
char *getmac(char *iface);
//jsonator.c
char * generateJSON(JsonMessage passedrpi);

//mqttPublisher.c
int init_mqtt_connection(MQTTAsync* client, char *address, int isRegistered,
		char* client_id, char* username, char* passwd);
int publishMQTTMessage(MQTTAsync* client, char *topic, char *payload);
int subscribe(MQTTAsync* client, char *topic);
int disconnect_mqtt_client(MQTTAsync* client);
int reconnect(MQTTAsync* client, int isRegistered,
		char* username, char* passwd);

int main(int argc __attribute__((unused)),
	 char **argv __attribute__((unused))) {

	char* json;

	/* int lckStatus; */
	int res;
	int sleepTimeout;
	struct config configstr;

	char *passwd;
	char *username;
	char msproxyUrl[MAXBUF];

	//setup the syslog logging
	setlogmask(LOG_UPTO(LOGLEVEL));
	openlog("iot", LOG_PID | LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "**** IoT Raspberry Pi Sample has started ****");

	// register the signal handler for USR1-user defined signal 1
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		syslog(LOG_CRIT, "Not able to register the signal handler\n");
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		syslog(LOG_CRIT, "Not able to register the signal handler\n");

	//read the config file, to decide whether to goto quickstart or registered mode of operation
	isRegistered = get_config(configFile, &configstr);

	if (isRegistered) {
		syslog(LOG_INFO, "Running in Registered mode\n");
		sprintf(msproxyUrl, "ssl://%s.messaging.internetofthings.ibmcloud.com:8883", configstr.org);
		if(strcmp(configstr.authmethod ,"token") != 0) {
			syslog(LOG_ERR, "Detected that auth-method is not token. Currently other authentication mechanisms are not supported, IoT process will exit.");
			syslog(LOG_INFO, "**** IoT Raspberry Pi Sample has ended ****");
				closelog();
				exit(1);
		} else {
			username = "use-token-auth";
			passwd = configstr.authtoken;
		}
	} else {
		syslog(LOG_INFO, "Running in Quickstart mode\n");
		strcpy(msproxyUrl,"tcp://quickstart.messaging.internetofthings.ibmcloud.com:1883");
	}

	// read the events
	char* mac_address = getmac("eth0");
	getClientId(&configstr, mac_address);
	//the timeout between the connection retry
	int connDelayTimeout = 1;	// default sleep for 1 sec
	int retryAttempt = 0;

	// initialize the MQTT connection
	init_mqtt_connection(&client, msproxyUrl, isRegistered, clientId, username, passwd);
	// Wait till we get a successful connection to IoT MQTT server
	while (!MQTTAsync_isConnected(client)) {
		connDelayTimeout = 1; // add extra delay(3,60,600) only when reconnecting
		if (connected == -1) {
			connDelayTimeout = reconnect_delay(++retryAttempt);	//Try to reconnect after the retry delay
			syslog(LOG_ERR,
					"Failed connection attempt #%d. Will try to reconnect "
							"in %d seconds\n", retryAttempt, connDelayTimeout);
			connected = 0;
			init_mqtt_connection(&client, msproxyUrl, isRegistered, clientId, username,
					passwd);
		}
		fflush(stdout);
		sleep(connDelayTimeout);
	}
	// resetting the counters
	connDelayTimeout = 1;
	retryAttempt = 0;

	// count for the sine wave
	int count = 1;
	sleepTimeout = EVENTS_INTERVAL;

	//subscribe for commands - only on registered mode
	if (isRegistered) {
		subscribe(&client, subscribeTopic);
	}
	while (1) {
		JsonMessage json_message = { DEVICE_NAME, getCPUTemp(), sineVal(
				MIN_VALUE, MAX_VALUE, 16, count), GetCPULoad() };
		json = generateJSON(json_message);
		res = publishMQTTMessage(&client, publishTopic, json);
		syslog(LOG_DEBUG, "Posted the message with result code = %d\n", res);
		if (res == -3) {
			//update the connected to connection failed
			connected = -1;
			while (!MQTTAsync_isConnected(client)) {
				if (connected == -1) {
					connDelayTimeout = reconnect_delay(++retryAttempt); //Try to reconnect after the retry delay
					syslog(LOG_ERR, "Failed connection attempt #%d. "
							"Will try to reconnect in %d "
							"seconds\n", retryAttempt, connDelayTimeout);
					sleep(connDelayTimeout);
					connected = 0;
					reconnect(&client, isRegistered, username,passwd);
				}
				fflush(stdout);
				sleep(1);
			}
			// resetting the counters
			connDelayTimeout = 1;
			retryAttempt = 0;
		}
		fflush(stdout);
		free(json);
		count++;
		sleep(sleepTimeout);
	}

	return 0;
}

//This generates the clientID based on the tenant_prefix and mac_address(external Id)
void getClientId(struct config * configstr, char* mac_address) {

	char *orgId;
	char *typeId;
	char *deviceId;

	if (isRegistered) {

		orgId = configstr->org;
		typeId = configstr->type;
		deviceId = configstr->id;

	} else {

		orgId = "quickstart";
		typeId = "iotsample-raspberrypi";
		deviceId = mac_address;

	}
	sprintf(clientId, "d:%s:%s:%s", orgId, typeId, deviceId);
//	sprintf(clientId, "%s:%s", TENANT_PREFIX,mac_address);
}

//This function generates the sine value based on the interval specified and the duration
float sineVal(float minValue, float maxValue, float duration, float count) {
	float sineValue;
	sineValue = sin(2.0 * PI * count / duration) * (maxValue - minValue) / 2.0;
	return sineValue;
}

// Signal handler to handle when the user tries to kill this process. Try to close down gracefully
void sig_handler(int signo __attribute__((unused))) {
	syslog(LOG_INFO, "Received the signal to terminate the IoT process. \n");
	syslog(LOG_INFO,
			"Trying to end the process gracefully. Closing the MQTT connection. \n");
	int res = disconnect_mqtt_client(&client);

	syslog(LOG_INFO, "Disconnect finished with result code : %d\n", res);
	syslog(LOG_INFO, "Shutdown of the IoT process is complete. \n");
	syslog(LOG_INFO, "**** IoT Raspberry Pi Sample has ended ****");
	closelog();
	exit(1);
}
/* Reconnect delay time 
 * depends on the number of failed attempts
 */
int reconnect_delay(int i) {
	if (i < 10) {
		return 3; // first 10 attempts try within 3 seconds
	}
	if (i < 20)
		return 60; // next 10 attempts retry after every 1 minute

	return 600;	// after 20 attempts, retry every 10 minutes
}
//Trimming characters
char *trim(char *str) {
	size_t len = 0;
	char *frontp = str - 1;
	char *endp = NULL;

	if (str == NULL)
		return NULL;

	if (str[0] == '\0')
		return str;

	len = strlen(str);
	endp = str + len;

	while (isspace(*(++frontp)))
		;
	while (isspace(*(--endp)) && endp != frontp)
		;

	if (str + len - 1 != endp)
		*(endp + 1) = '\0';
	else if (frontp != str && endp == frontp)
		*str = '\0';

	endp = str;
	if (frontp != str) {
		while (*frontp)
			*endp++ = *frontp++;
		*endp = '\0';
	}

	return str;
}


// This is the function to read the config from the device.cfg file
int get_config(char * filename, struct config * configstr) {

	FILE* prop;
	/* char str1[10], str2[10]; */
	prop = fopen(filename, "r");
	if (prop == NULL) {
		syslog(LOG_INFO,"Config file not found. Going to Quickstart mode\n");
		return 0; // as the file is not present, it must be quickstart mode
	}
	char line[256];
	int linenum = 0;
	while (fgets(line, 256, prop) != NULL) {
		char* prop;
		char* value;

		linenum++;
		if (line[0] == '#')
			continue;

		prop = strtok(line, "=");
		prop = trim(prop);
		value = strtok(NULL, "=");
		value = trim(value);
		if (strcmp(prop, "org") == 0)
			strncpy(configstr->org, value, MAXBUF);
		else if (strcmp(prop, "type") == 0)
			strncpy(configstr->type, value, MAXBUF);
		else if (strcmp(prop, "id") == 0)
			strncpy(configstr->id, value, MAXBUF);
		else if (strcmp(prop, "auth-token") == 0)
			strncpy(configstr->authtoken, value, MAXBUF);
		else if (strcmp(prop, "auth-method") == 0)
					strncpy(configstr->authmethod, value, MAXBUF);
	}

	return 1;
}
