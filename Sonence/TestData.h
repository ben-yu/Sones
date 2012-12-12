//
//  TestData.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-11-13.
//
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface TestData : NSManagedObject

@property (nonatomic, retain) NSNumber * freq;
@property (nonatomic, retain) NSNumber * volume;
@property (nonatomic, retain) NSString * session;

@end
