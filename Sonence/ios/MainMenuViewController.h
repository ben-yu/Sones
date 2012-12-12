//
//  MainMenuViewController.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MPVolumeView.h>
#import "RootViewController.h"
#import "iCarousel.h"

@interface MainMenuViewController : UIViewController

@property RootViewController *controller;
@property (nonatomic, retain) NSManagedObjectContext *managedObjectContext;
@property (nonatomic, retain) IBOutlet iCarousel *carousel;

-(IBAction) goCocos2dx: (id) sender;
@end
