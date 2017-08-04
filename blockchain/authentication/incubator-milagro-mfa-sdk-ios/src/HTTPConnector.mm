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

#include "HTTPConnector.h"
#import "MPin.h"
#import <UIKit/UIKit.h>

static NSInteger constIntTimeoutInterval = 30;
static NSString *constStrConnectionTimeoutNotification = @"ConnectionTimeoutNotification";

namespace net {
    static const String HTTP_GET = "GET";
    static const String HTTP_POST = "POST";
    static const String HTTP_PUT = "PUT";
    static const String HTTP_DELETE = "DELETE";
    static const String HTTP_OPTIONS = "OPTIONS";
    static const String HTTP_PATHCH = "PATCH";
    
    const String&  getHTTPMethod(Method method) {
        if(method == 0)
            return HTTP_GET;
        else if(method == 1)
            return HTTP_POST;
        else if (method == 2)
            return HTTP_PUT;
        else if (method == 3)
            return HTTP_DELETE;
        else if (method == 4)
            return HTTP_OPTIONS;
        else
            return HTTP_PATHCH;

    }
    
	void HTTPConnector::SetHeaders(const StringMap& headers) {
		m_requestHeaders = headers;
	}

	void HTTPConnector::SetQueryParams(const StringMap& queryParams){
        m_queryParams = queryParams;
	}

	void HTTPConnector::SetContent(const String& data) {
        m_bodyData = data;
	}

	void HTTPConnector::SetTimeout(int seconds) {
        if(seconds <=0) throw IllegalArgumentException("Timeout is negative or 0");
        timeout = seconds;
	}
    
	bool HTTPConnector::Execute(Method method, const String& url){
        NSString * strURL = [NSString stringWithUTF8String:url.c_str()];
        strURL = [strURL stringByReplacingOccurrencesOfString:@"wss://" withString:@"https://"];
        strURL = [strURL stringByReplacingOccurrencesOfString:@"ws://" withString:@"http://"];
    
        if ( [strURL hasPrefix:@"/"] ) {
             strURL = [[MPin getRPSUrl] stringByAppendingString:strURL];
        }
        
        if(!m_queryParams.empty()) {
            NSString *queryString = @"";
            for (StringMap::const_iterator it=m_queryParams.begin(); it!=m_queryParams.end(); ++it) {
                queryString = [queryString stringByAppendingString:[NSString stringWithUTF8String:it->first.c_str()]];
                queryString = [queryString stringByAppendingString:@"="];
                queryString = [queryString stringByAppendingString:[NSString stringWithUTF8String:it->second.c_str()]];
                queryString = [queryString stringByAppendingString:@"&"];
            }
            
            queryString = [queryString substringToIndex:[queryString length] -1];
            strURL = [strURL stringByAppendingString:@"?"];
            strURL = [strURL stringByAppendingString:queryString];
        }
        
        NSURL * theUrl = [NSURL URLWithString:strURL];
        NSMutableURLRequest * request = [NSMutableURLRequest requestWithURL:theUrl cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:timeout];
        request.HTTPMethod = [NSString stringWithUTF8String:(getHTTPMethod(method)).c_str()];

        [request setTimeoutInterval:constIntTimeoutInterval];
        
        NSDictionary *dictionary = [request allHTTPHeaderFields];
        
        if(!m_requestHeaders.empty())
        {
            for (StringMap::const_iterator it=m_requestHeaders.begin(); it!=m_requestHeaders.end(); ++it)
            {
                NSString *strValue = [NSString stringWithUTF8String:it->second.c_str()];
                NSString *strKey   = [NSString stringWithUTF8String:it->first.c_str()];
                
                if ([[dictionary allKeys] containsObject:strKey])
                {
                    [request setValue:strValue forHTTPHeaderField:strKey];
                }
                else
                {
                    [request addValue:strValue forHTTPHeaderField:strKey];
                }
            }
        }
        
        [request addValue:@"ios" forHTTPHeaderField:@"X-MIRACL-OS-Class"];
        
        /*
        // Deprecated Code Starting Here
        NSDictionary *dictInfo = [[NSBundle mainBundle] infoDictionary];
        NSString *strBundleID       = dictInfo[@"CFBundleIdentifier"];
        NSString *strAppVersion     = dictInfo[@"CFBundleShortVersionString"];
        NSString *strOSVersion      = [[UIDevice currentDevice] systemVersion];
        NSString *strBuildNumber    = dictInfo[@"CFBundleVersion"];
        
        NSString *strUserAgent = [NSString stringWithFormat:@"%@/%@ (ios/%@) build/%@",strBundleID,strAppVersion,strOSVersion, strBuildNumber];
        
        [request setValue:strUserAgent forHTTPHeaderField:@"User-Agent"];
        // Deprecated Code Ends Here
         */
        
        if(!m_bodyData.empty()) {
            request.HTTPBody =  [[NSString stringWithUTF8String:m_bodyData.c_str()] dataUsingEncoding:NSUTF8StringEncoding];
        }
        
        NSHTTPURLResponse * response = nil;
        NSError * error = nil;
        NSData * data = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
        
        if(error != nil) {
            //TODO: IMPORTANT FIX THIS IN LATER COMMITS
           
            switch (error.code) {
                case -1001: //Connection timeout
                    m_statusCode = 408;
                    m_errorMessage += "Connection timeout!";
                    return false;
                    break;
                case -1012:
                    m_statusCode = 401;
                    m_errorMessage += "Unauthorized Access! Please check your e-mail and confirm the activation link!";
                    return true;
                    break;
            }
            m_errorMessage += [error.localizedDescription UTF8String];
            return false;
        }
        
        if(response != nil) {
            m_statusCode = (int)response.statusCode;
            for(NSString * key in response.allHeaderFields) {
                NSString * value = [response.allHeaderFields objectForKey:key];
                m_responseHeaders[([key UTF8String])] = [value UTF8String];
            }
        }
        
        if(data != nil) {
            NSString * dataStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
            m_response += [dataStr UTF8String];
        }

		return true;
	}

	const String& HTTPConnector::GetExecuteErrorMessage() const { return m_errorMessage; }

	int HTTPConnector::GetHttpStatusCode() const { return m_statusCode; }

	const StringMap& HTTPConnector::GetResponseHeaders() const { return m_responseHeaders; }

	const String& HTTPConnector::GetResponseData() const {	return m_response; }

	HTTPConnector :: ~HTTPConnector () { }
}


