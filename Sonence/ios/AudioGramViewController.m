//
//  AudiogramViewController.m
//  MyToneGenerator
//
//  Created by Benjamin Yu on 12-09-04.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AudiogramViewController.h"
#import <CoreData/CoreData.h>
#import "TestData.h"

@interface AudiogramViewController ()

@end

@implementation AudiogramViewController
- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)viewDidUnload
{
    [_graphingHostingView release];
    _graphingHostingView = nil;
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return  (interfaceOrientation == UIInterfaceOrientationLandscapeLeft || interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
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
    
    for (TestData *a in reqData){
        [data addObject:[NSValue valueWithCGPoint:CGPointMake([a.freq floatValue], [a.volume floatValue])]];
    }
    
    self->_scatterPlot = [[SimpleScatterPlot alloc] initWithHostingView:_graphingHostingView andData:data];
    [self->_scatterPlot initialisePlot];
}

- (void)dealloc {
    [_graphingHostingView release];
    [super dealloc];
}
@end
