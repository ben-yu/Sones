//
//  CalibrationData.h
//  Sonence
//
//  Created by Benjamin Yu on 2013-04-12.
//
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface CalibrationData : NSManagedObject

@property (nonatomic, retain) NSNumber * channel;
@property (nonatomic, retain) NSNumber * freq;
@property (nonatomic, retain) NSNumber * vol;

@end
