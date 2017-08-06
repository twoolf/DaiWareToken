//
//  SessionDetails.m
//  MPinSDK
//
//  Created by Georgi Georgiev on 4/26/16.
//  Copyright © 2016 Certivox. All rights reserved.
//

#import "SessionDetails.h"

@implementation SessionDetails

- (id) initWith:(NSString * ) prerollId appName:(NSString *) appName appIconUrl:(NSString *) appIconUrl {
    self = [super init];
    if (self) {
        self.prerollId = prerollId;
        self.appName = appName;
        self.appIconUrl = appIconUrl;
    }
    return self;
}

@end