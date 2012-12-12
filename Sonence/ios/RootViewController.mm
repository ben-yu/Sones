//
//  SonenceAppController.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//  Copyright __MyCompanyName__ 2012. All rights reserved.
//

#import "RootViewController.h"

#include "cocos2d.h"
#import "EAGLView.h"
#include "CCEGLView.h"

#include "TestData.h"


@implementation RootViewController
@synthesize pDirector = _pDirector;
@synthesize sceneptr = _sceneptr;
@synthesize accelEnable;
@synthesize managedObjectContext = _managedObjectContext;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(void)loadOpenglScene:(CGRect)rect {
    cocos2d::CCApplication::sharedApplication()->run();
    
    EAGLView *glView = [EAGLView viewWithFrame: rect
                                   pixelFormat: kEAGLColorFormatRGBA8
                                   depthFormat: GL_DEPTH_COMPONENT16
                            preserveBackbuffer: NO
                                    sharegroup: nil
                                 multiSampling: NO
                               numberOfSamples: 0];
    
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        [self.view insertSubview:glView atIndex:0];
    } else {
        self.view = glView;
    }
    
    _pDirector = cocos2d::CCDirector::sharedDirector();
    
    _pDirector->setOpenGLView(cocos2d::CCEGLView::sharedOpenGLView());
    
	// enable High Resource Mode(2x, such as iphone4) and maintains low resource on other devices.
    // pDirector->enableRetinaDisplay(true);
    
	// turn on display FPS
	//pDirector->setDisplayFPS(true);
    
	//_pDirector->setDeviceOrientation(UIDeviceOrientationLandscapeLeft);
    
	// set FPS. the default value is 1.0/60 if you don't call this
	_pDirector->setAnimationInterval(1.0 / 60);
    
	// create a scene. it's an autorelease object
    cocos2d::CCScene *pScene = HelloWorld::scene([[UIDevice currentDevice].systemVersion floatValue],accelEnable,self);
    
	// run
	_pDirector->runWithScene(pScene);
    
    _sceneptr = (HelloWorld *) pScene->getChildren()->objectAtIndex(0);
    
    // to avoid BAD ACCESS when calling CCDirector::end()
    [[EAGLView sharedEGLView] retain];
}


- (void)viewDidLoad {
    [super viewDidLoad];
    
    float width, height;
    
    // add opengl view on the remaining space of the view
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        width = self.view.frame.size.width;
        height = self.view.frame.size.height;
        [self loadOpenglScene:CGRectMake(0, 0, width, height)];
        UIButton *closeButton = [[UIButton alloc] initWithFrame:CGRectMake( width - 20, height - 20, 20.0, 20.0)];
        [closeButton addTarget:self action:@selector(close) forControlEvents:UIControlEventTouchUpInside];
        UIImage *btnImage = [UIImage imageNamed:@"glyphicons_197_remove.png"];
        [closeButton setImage:btnImage forState:UIControlStateNormal];
        [self.view addSubview:closeButton];
    } else {
        width = self.view.frame.size.height;
        height = self.view.frame.size.width;
        [self loadOpenglScene:CGRectMake(0, 0, width, height)];
        UIButton *closeButton = [[UIButton alloc] initWithFrame:CGRectMake( width - 20, 0, 20.0, 20.0)];
        [closeButton addTarget:self action:@selector(close) forControlEvents:UIControlEventTouchUpInside];
        UIImage *btnImage = [UIImage imageNamed:@"glyphicons_197_remove.png"];
        [closeButton setImage:btnImage forState:UIControlStateNormal];
        [self.view addSubview:closeButton];
    }
}

-(void)viewDidAppear:(BOOL)animated {
    _pDirector->resume();
}

-(void)viewDidDisappear:(BOOL)animated {
    _pDirector = cocos2d::CCDirector::sharedDirector();
    _pDirector->pause();
}

- (void)viewDidUnload {
    [[EAGLView sharedEGLView] release];
    [super viewDidUnload];
}

// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead of shouldAutorotateToInterfaceOrientation
- (NSUInteger) supportedInterfaceOrientations{
        return UIInterfaceOrientationMaskLandscape;
}

- (BOOL) shouldAutorotate {
    return YES;
}

-(void)close {
    _pDirector = cocos2d::CCDirector::sharedDirector();
    _pDirector->pause();
    _sceneptr->stopTones();
    [self dismissModalViewControllerAnimated:YES];
}

-(void)storeDataPoint:(NSString*) identifier
           freqThresh:(double) freq
            volThresh:(double) vol
{
    TestData *dataPoint = (TestData *)[NSEntityDescription insertNewObjectForEntityForName:@"TestData" inManagedObjectContext:self.managedObjectContext];
    [dataPoint setFreq:[NSNumber numberWithDouble:freq]];
    [dataPoint setVolume:[NSNumber numberWithDouble:vol]];
    [dataPoint setSession:identifier];

    [self.managedObjectContext save:nil];
}


@end
