//
//  User.h
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-10.
//
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface User : NSManagedObject

@property (nonatomic, retain) NSNumber * playedTutorial;
@property (nonatomic, retain) NSString * uuid;

@end
