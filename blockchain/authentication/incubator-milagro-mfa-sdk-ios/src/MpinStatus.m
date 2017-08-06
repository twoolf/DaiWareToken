/*
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
 */

#import "MpinStatus.h"

@implementation MpinStatus

-(id) initWith:(MPinStatus)status errorMessage:(NSString*) error {
    self = [super init];
    if (self) {
        self.status = status;
        self.errorMessage = error;
    }
    return self;

}

- (NSString *) getStatusCodeAsString {
    NSString * result = @"";
    switch (self.status) {
        case OK:
            result = @"KEY_BTNOK";
            break;
        case PIN_INPUT_CANCELED:
            result = @"PIN_INPUT_CANCELED";
            break;
        case CRYPTO_ERROR:
            result = @"CRYPTO_ERROR";
            break;
        case STORAGE_ERROR:
            result = @"STORAGE_ERROR";
            break;
        case NETWORK_ERROR:
            result = @"NETWORK_ERROR";
            break;
        case RESPONSE_PARSE_ERROR:
            result = @"RESPONSE_PARSE_ERROR";
            break;
        case FLOW_ERROR:
            result = @"FLOW_ERROR";
            break;
        case IDENTITY_NOT_AUTHORIZED:
            result = @"IDENTITY_NOT_AUTHORIZED";
            break;
        case IDENTITY_NOT_VERIFIED:
            result = @"IDENTITY_NOT_VERIFIED";
            break;
        case REQUEST_EXPIRED:
            result = @"REQUEST_EXPIRED";
            break;
        case REVOKED:
            result = @"REVOKED";
            break;
        case INCORRECT_PIN:
            result = @"INCORRECT_PIN";
            break;
        case INCORRECT_ACCESS_NUMBER:
            result = @"INCORRECT_ACCESS_NUMBER";
            break;
        case HTTP_SERVER_ERROR:
            result = @"HTTP_SERVER_ERROR";
            break;
        case HTTP_REQUEST_ERROR:
            result = @"HTTP_REQUEST_ERROR";
            break;
        case BAD_USER_AGENT:
            result = @"BAD_USER_AGENT";
            break;
        case CLIENT_SECRET_EXPIRED:
            result = @"CLIENT_SECRET_EXPIRED";
            break;
        default:
            break;
    }
    return result;
}

@end
