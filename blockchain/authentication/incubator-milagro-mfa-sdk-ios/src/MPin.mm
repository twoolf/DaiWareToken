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

#import "MPin.h"
#import "mpin_sdk.h"
// #import "def.h"
#import "Context.h"
#import <vector>
#import "User.h"

static MPinSDK mpin;
static BOOL isInitialized = false;

/// TEMPORARY FIX
static NSString * rpsURL;
static NSLock * lock = [[NSLock alloc] init];

typedef MPinSDK::UserPtr UserPtr;
typedef MPinSDK::Status Status;
typedef sdk_non_tee::Context Context;

@implementation MPin

/// TEMPORARY FIX
+ (NSString*) getRPSUrl {
    return rpsURL;
}

+ (void) initSDK {
    
    if (isInitialized) return;
    
    [lock lock];
    mpin.Init(StringMap(), sdk_non_tee::Context::Instance());
    isInitialized = true;
    [lock unlock];

}

+ (void) initSDKWithHeaders:(NSDictionary *)dictHeaders{
    
    if (isInitialized) return;

    StringMap sm_CustomHeaders;
    
    for( id headerName in dictHeaders)
    {
        sm_CustomHeaders.Put( [headerName UTF8String], [dictHeaders[headerName] UTF8String] );
    }
    
    [lock lock];
    mpin.Init(StringMap(), sdk_non_tee::Context::Instance(), sm_CustomHeaders);
    isInitialized = true;
    [lock unlock];
}

+ (void) Destroy {
    [lock lock];
    mpin.Destroy();
    isInitialized = false;
    [lock unlock];
}

+ (MpinStatus*) TestBackend:(const NSString * ) url {
    [lock lock];
    Status s = mpin.TestBackend((url == nil)?(""):([url UTF8String]));
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) SetBackend:(const NSString * ) url {
    [lock lock];
    Status s = mpin.SetBackend((url == nil)?(""):([url UTF8String]));
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) TestBackend:(const NSString * ) url rpsPrefix:(NSString *) rpsPrefix {
    if (rpsPrefix == nil || rpsPrefix.length == 0) {
        return [MPin TestBackend:url];
    }
    [lock lock];
    Status s = mpin.TestBackend([url UTF8String], [rpsPrefix UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}
+ (MpinStatus*) SetBackend:(const NSString * ) url rpsPrefix:(NSString *) rpsPrefix {
    if (rpsPrefix == nil || rpsPrefix.length == 0) {
        return [MPin SetBackend:url];
    }
    [lock lock];
    Status s = mpin.SetBackend([url UTF8String],[rpsPrefix UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (id<IUser>) MakeNewUser:(const NSString *) identity {
    [lock lock];
    UserPtr userPtr = mpin.MakeNewUser([identity UTF8String]);
    [lock unlock];
    return [[User alloc] initWith:userPtr];
}

+ (id<IUser>) MakeNewUser: (const NSString *) identity deviceName:(const NSString *) devName {
    [lock lock];
    UserPtr userPtr = mpin.MakeNewUser([identity UTF8String], [devName UTF8String]);
    [lock unlock];
    return [[User alloc] initWith:userPtr];
}

+ (MpinStatus*) StartRegistration:(const  id<IUser>) user {
    return [MPin StartRegistration:user activateCode:@"" userData:@""];
}

+ (MpinStatus*) RestartRegistration:(const id<IUser>) user {
    return [MPin RestartRegistration:user userData:@""];
}

+ (MpinStatus*) StartRegistration:(const id<IUser>)user activateCode:(NSString *) activateCode {
    return [MPin StartRegistration:user activateCode:activateCode userData:@""];
}

+ (MpinStatus*) StartRegistration:(const id<IUser>)user userData:(NSString *) userData {
    return [MPin StartRegistration:user activateCode:@"" userData:userData];
}

+ (MpinStatus*) StartRegistration:(const id<IUser>)user activateCode:(NSString *) activateCode userData:(NSString *) userData {
    [lock lock];
    Status s = mpin.StartRegistration([((User *) user) getUserPtr], [activateCode UTF8String], [userData UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) RestartRegistration:(const id<IUser>)user userData:(NSString *) userData {
    [lock lock];
    Status s = mpin.RestartRegistration([((User *) user) getUserPtr], [userData UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}


+ (MpinStatus*) ConfirmRegistration:(const id<IUser>)user {
    return [MPin ConfirmRegistration:user pushNotificationIdentifier:nil];
}

+ (MpinStatus*) ConfirmRegistration:(const id<IUser>)user  pushNotificationIdentifier:(NSString *) pushNotificationIdentifier {
    [lock lock];
    if(pushNotificationIdentifier == nil) pushNotificationIdentifier = @"";
    Status s = mpin.ConfirmRegistration([((User *) user) getUserPtr], [pushNotificationIdentifier UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) FinishRegistration:(const id<IUser>)user pin:(NSString *) pin {
    [lock lock];
    Status s = mpin.FinishRegistration([((User *) user) getUserPtr], [pin UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) StartAuthentication:(const id<IUser>)user {
    [lock lock];
    Status s = mpin.StartAuthentication([((User *) user) getUserPtr]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) StartAuthentication:(const id<IUser>)user accessCode:(NSString *) accessCode {
    [lock lock];
    Status s = mpin.StartAuthentication([((User *) user) getUserPtr], [accessCode UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) CheckAccessNumber:(NSString *)an {
    [lock lock];
    Status s = mpin.CheckAccessNumber([an UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) FinishAuthentication:(const id<IUser>) user pin:(NSString *) pin  {
    [lock lock];
    Status s = mpin.FinishAuthentication([((User *) user) getUserPtr], [pin UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*)FinishAuthentication:(const id<IUser>)user pin:(NSString *) pin authResultData:(NSString **)authResultData {
    MPinSDK::String c_authResultData;
    [lock lock];
    Status s = mpin.FinishAuthentication([((User *) user) getUserPtr], [pin UTF8String], c_authResultData);
    [lock unlock];
    *authResultData = [NSString stringWithUTF8String:c_authResultData.c_str()];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus*) FinishAuthenticationOTP:(id<IUser>) user pin:(NSString *) pin otp:(OTP **) otp {
    MPinSDK::OTP c_otp;
    [lock lock];
    Status s = mpin.FinishAuthenticationOTP([((User *) user) getUserPtr], [pin UTF8String], c_otp);
    [lock unlock];
    *otp = [[OTP alloc] initWith:[[MpinStatus alloc] initWith:(MPinStatus)c_otp.status.GetStatusCode() errorMessage:[NSString stringWithUTF8String:c_otp.status.GetErrorMessage().c_str()]]
                             otp:[NSString stringWithUTF8String:c_otp.otp.c_str()]
                      expireTime:c_otp.expireTime
                      ttlSeconds:c_otp.ttlSeconds
                         nowTime:c_otp.nowTime];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (MpinStatus *) FinishAuthenticationAN:(id<IUser>) user pin:(NSString *) pin  accessNumber:(NSString *) an {
    [lock lock];
     Status s = mpin.FinishAuthenticationAN([((User *) user) getUserPtr], [pin UTF8String], [an UTF8String]);
    [lock unlock];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (void) SetClientId:(NSString *) clientId {
    [lock lock];
    mpin.SetClientId([clientId UTF8String]);
    [lock unlock];
}

+ (MpinStatus*) FinishAuthenticationMFA:(id<IUser>)user pin:(NSString *) pin authzCode:(NSString **) authzCode {
    MPinSDK::String c_authzCode;
    [lock lock];
    Status s = mpin.FinishAuthenticationMFA( [((User *) user) getUserPtr], [pin UTF8String], c_authzCode);
    [lock unlock];
    *authzCode = [NSString stringWithUTF8String:c_authzCode.c_str()];
    return [[MpinStatus alloc] initWith:(MPinStatus)s.GetStatusCode() errorMessage:[NSString stringWithUTF8String:s.GetErrorMessage().c_str()]];
}

+ (Boolean) Logout:(const id<IUser>) user {
    [lock lock];
    Boolean b = mpin.Logout([((User *) user) getUserPtr]);
    [lock unlock];
    return b;
}

+ (Boolean) CanLogout:(const id<IUser>) user {
    [lock lock];
    Boolean b = mpin.CanLogout([((User *) user) getUserPtr]);
    [lock unlock];
    return b;
}

+(NSString*) GetClientParam:(const NSString *) key {
    [lock lock];
    String value = mpin.GetClientParam([key UTF8String]);
    [lock unlock];
    return [NSString stringWithUTF8String:value.c_str()];
}

+(NSMutableArray*) listUsers {
    NSMutableArray * users = [NSMutableArray array];
    std::vector<UserPtr> vUsers;
    mpin.ListUsers(vUsers);
    for (int i = 0; i<vUsers.size(); i++) {
        [users addObject:[[User alloc] initWith:vUsers[i]]];
    }
    return users;
}

+ (SessionDetails*) GetSessionDetails:(NSString *) accessCode {
    [lock lock];
    MPinSDK::SessionDetails sd;
    Status s = mpin.GetSessionDetails([accessCode UTF8String] , sd);
    [lock unlock];
    
    if (s.GetStatusCode() != Status::Code::OK)
        return nil;

    return  [[SessionDetails alloc] initWith:[NSString stringWithUTF8String:sd.prerollId.c_str()]
                                      appName:[NSString stringWithUTF8String:sd.appName.c_str()]
                                   appIconUrl:[NSString stringWithUTF8String:sd.appIconUrl.c_str()]];
}

+ (id<IUser>) getIUserById:(NSString *) userId {
    if( userId == nil ) return nil;
    if ([@"" isEqualToString:userId]) return nil;
    
    NSArray * users = [MPin listUsers];
    
    for (User * user in users)
        if ( [userId isEqualToString:[user getIdentity]] )
            return user;
    
    return nil;
}

+ (NSMutableArray*) listUsers:( NSString *) backendURL {
    if (backendURL == nil || backendURL.length == 0 ) return nil;
    
    NSMutableArray * users = [NSMutableArray array];
    std::vector<UserPtr> vUsers;
    mpin.ListUsers(vUsers, [backendURL UTF8String]);
    for (int i = 0; i<vUsers.size(); i++) {
        [users addObject:[[User alloc] initWith:vUsers[i]]];
    }
    return users;
}

+ (NSMutableArray*) listBackends {
    NSMutableArray * backends = [NSMutableArray array];
    std::vector<String> vBackends;
    mpin.ListBackends(vBackends);
    for (int i = 0; i<vBackends.size(); i++) {
        [backends addObject:[NSString stringWithUTF8String:vBackends[i].c_str()]];
    }
    return backends;
}

+ (void) DeleteUser:(const id<IUser>) user {
    [lock lock];
    mpin.DeleteUser([((User *) user) getUserPtr]);
    [lock unlock];
}

@end
