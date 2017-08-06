//
//  SessionDetails.h
//  MPinSDK
//
//  Created by Georgi Georgiev on 4/26/16.
//  Copyright © 2016 Certivox. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SessionDetails : NSObject
@property (nonatomic, retain) NSString * prerollId;
@property (nonatomic, retain) NSString * appName;
@property (nonatomic, retain) NSString * appIconUrl;
- (id) initWith:(NSString * ) prerollId appName:(NSString *) appName appIconUrl:(NSString *) appIconUrl;
@end