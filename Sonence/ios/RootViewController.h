//
//  SonenceAppController.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//  Copyright APL 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "cocos2d.h"
#include "SpaceScene.h"


@interface RootViewController : UIViewController {
    struct DataPoint {
        float freq;
        float vol;
        int channel;
    };
    
    iOSBridge::DataPoint *dataptr;
}


@property cocos2d::CCDirector *pDirector;
@property SpaceScene *sceneptr;
@property cocos2d::CCScene *scenePointers;
@property NSInteger gameType;
@property NSInteger numOfDataPoints;
@property (nonatomic, retain) NSManagedObjectContext *managedObjectContext;
@property (nonatomic, retain) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, retain) NSPersistentStoreCoordinator *persistentStoreCoordinator;

-(void)storeDataPoint:(NSString*) identifier
           freqThresh:(double) freq
            volThresh:(double) vol
         audioChannel:(int) channel
             attempts:(int) tapCounts
         reactionTime:(double) initTime
             lastTime:(double) finalTime;
-(iOSBridge::DataPoint *)getData;

@end
