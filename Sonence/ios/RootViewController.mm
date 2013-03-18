//
//  SonenceAppController.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//  Copyright APL 2012. All rights reserved.
//

#import "RootViewController.h"
#import "EAGLView.h"

#include "cocos2d.h"
#include "CCEGLView.h"
#include "TestData.h"
#include "User.h"


@implementation RootViewController
@synthesize pDirector = _pDirector;
@synthesize sceneptr = _sceneptr;
@synthesize gameType;
@synthesize numOfDataPoints = _numOfDataPoints;
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
    //_pDirector->enableRetinaDisplay(true);
    
	// turn on display FPS
	//_pDirector->setDisplayStats(true);
    
	//_pDirector->setDeviceOrientation(UIDeviceOrientationLandscapeLeft);
    
	// set FPS. the default value is 1.0/60 if you don't call this
	_pDirector->setAnimationInterval(1.0 / 60);
    /*
    if (_pDirector->getRunningScene() == NULL) {
    
        // create a scene. it's an autorelease object
        cocos2d::CCScene *pScene = SpaceSceneLayer::scene([[UIDevice currentDevice].systemVersion floatValue],gameType,self);
        _pDirector->runWithScene(pScene);
        _sceneptr = (SpaceScene *) pScene->getChildren()->objectAtIndex(0);
    } else {
        cocos2d::CCScene *pScene = SpaceSceneLayer::scene([[UIDevice currentDevice].systemVersion floatValue],gameType,self);
        _pDirector->replaceScene(pScene);
        _sceneptr = (SpaceScene *) pScene->getChildren()->objectAtIndex(0);
    }
    */
    // to avoid BAD ACCESS when calling CCDirector::end()
    [[EAGLView sharedEGLView] retain];
}

/*
- (void)viewDidLoad {
    [super viewDidLoad];
    
    float width, height;
    // add opengl view on the remaining space of the view
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0) {
        width = self.view.frame.size.width;
        height = self.view.frame.size.height;
        [self loadOpenglScene:CGRectMake(0, 0, height, width)];
        UIButton *closeButton = [[UIButton alloc] initWithFrame:CGRectMake( height - 20, 0, 20.0, 20.0)];
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
*/
/*
-(void)viewDidAppear:(BOOL)animated {
    //_pDirector->resume();
}

-(void)viewDidDisappear:(BOOL)animated {
    _pDirector = cocos2d::CCDirector::sharedDirector();
    _pDirector->pause();
}

- (void)viewDidUnload {
    [[EAGLView sharedEGLView] release];
    [self.view release];
    _pDirector->end();
    [super viewDidUnload];
}
*/
 
// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead of shouldAutorotateToInterfaceOrientation
- (NSUInteger) supportedInterfaceOrientations{
        return UIInterfaceOrientationMaskLandscape;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return  (interfaceOrientation == UIInterfaceOrientationLandscapeLeft || interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

-(void)playedTutorial
{
    User *curUser = (User *)[NSEntityDescription insertNewObjectForEntityForName:@"User" inManagedObjectContext:self.managedObjectContext];
    [curUser setPlayedTutorial:[NSNumber numberWithBool:YES]];
}

-(void)setUUID:(NSString *)uuid
{
    User *curUser = (User *)[NSEntityDescription insertNewObjectForEntityForName:@"User" inManagedObjectContext:self.managedObjectContext];
    [curUser setUuid:uuid];
}

-(void)generateUUID
{
    User *curUser = (User *)[NSEntityDescription insertNewObjectForEntityForName:@"User" inManagedObjectContext:self.managedObjectContext];
    CFUUIDRef cfuuid = CFUUIDCreate(NULL);
    NSString *uuid =  (NSString *)CFUUIDCreateString(NULL, cfuuid);
    CFRelease(cfuuid);
    [curUser setUuid:uuid];
}

-(void)storeDataPoint:(NSString*) identifier
           freqThresh:(double) freq
            volThresh:(double) vol
         audioChannel:(int) channel
             attempts:(int) tapCounts
         reactionTime:(double) initTime
             lastTime:(double) finalTime
{
    TestData *dataPoint = (TestData *)[NSEntityDescription insertNewObjectForEntityForName:@"TestData" inManagedObjectContext:self.managedObjectContext];
    
    [dataPoint setFreq:[NSNumber numberWithDouble:freq]];
    [dataPoint setVolume:[NSNumber numberWithDouble:vol]];
    [dataPoint setChannel:[NSNumber numberWithDouble:channel]];
    [dataPoint setAttempts:[NSNumber numberWithInt:tapCounts]];
    [dataPoint setInit_time:[NSNumber numberWithDouble:initTime]];
    [dataPoint setFinal_time:[NSNumber numberWithDouble:finalTime]];
    [dataPoint setSession:identifier];

    [self.managedObjectContext save:nil];
}

-(void)storeCalibrationPoint:(NSString*) identifier
           freqThresh:(double) freq
            volThresh:(double) vol
         audioChannel:(int) channel
{
    TestData *dataPoint = (TestData *)[NSEntityDescription insertNewObjectForEntityForName:@"TestData" inManagedObjectContext:self.managedObjectContext];
    
    [dataPoint setFreq:[NSNumber numberWithDouble:freq]];
    [dataPoint setVolume:[NSNumber numberWithDouble:vol]];
    [dataPoint setChannel:[NSNumber numberWithDouble:channel]];
    [dataPoint setSession:identifier];
    
    [self.managedObjectContext save:nil];
}

-(iOSBridge::DataPoint *)getData
{
    NSManagedObjectContext *moc = [self managedObjectContext];
    NSEntityDescription *entityDescription = [NSEntityDescription
                                              entityForName:@"TestData" inManagedObjectContext:moc];
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    [request setEntity:entityDescription];
    request.sortDescriptors = [NSArray arrayWithObject:[NSSortDescriptor sortDescriptorWithKey:@"freq"
                                                                                     ascending:YES
                                                                                      selector:@selector(localizedCaseInsensitiveCompare:)]];
    NSError *error;
    NSArray *reqData = [moc executeFetchRequest:request error:&error];
    NSMutableArray *data = [NSMutableArray array];
    _numOfDataPoints = [reqData count];
    
    if (dataptr != NULL)
        free(dataptr);
    
    dataptr = (iOSBridge::DataPoint *)malloc(sizeof(iOSBridge::DataPoint)*_numOfDataPoints);
    
    for (int i = 0; i < _numOfDataPoints; i++){
        dataptr[i].freq = [((TestData *) [reqData objectAtIndex:i]).freq floatValue];
        dataptr[i].vol = [((TestData *) [reqData objectAtIndex:i]).volume floatValue];
        dataptr[i].channel = [((TestData *) [reqData objectAtIndex:i]).channel integerValue];
        [data addObject:[NSValue value:&dataptr[i] withObjCType:@encode(iOSBridge::DataPoint)]];
    }
    
    return dataptr;
}


@end
