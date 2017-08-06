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

/*
 * Code for JSON operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "cJSON.h"

/*
 * Function to get time delay for reboot from the
 * JSON message sent from the application
 */
int getDelay(char *text) {
	cJSON *json;
	int delay = -1;

	json = cJSON_Parse(text);
	if (!json) {
		syslog(LOG_ERR, "JSON Parsing error : [%s]\n", cJSON_GetErrorPtr());
	} else {
		delay = cJSON_GetObjectItem(json, "delay")->valueint;
		cJSON_Delete(json);
	}

	return delay;
}
