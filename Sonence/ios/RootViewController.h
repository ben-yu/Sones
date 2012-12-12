//
//  SonenceAppController.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#import <UIKit/UIKit.h>

#include "cocos2d.h"
#include "HelloWorldScene.h"

@interface RootViewController : UIViewController {

}

@property cocos2d::CCDirector *pDirector;
@property HelloWorld *sceneptr;
@property bool accelEnable;
@property (nonatomic, retain) NSManagedObjectContext *managedObjectContext;
@property (nonatomic, retain) NSManagedObjectModel *managedObjectModel;
@property (nonatomic, retain) NSPersistentStoreCoordinator *persistentStoreCoordinator;

@end
