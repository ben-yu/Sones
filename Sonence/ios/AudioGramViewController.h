//
//  AudioGramViewController.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-11-13.
//
//

#import <UIKit/UIKit.h>
#import "CorePlot-CocoaTouch.h"
#import "SimpleScatterPlot.h"

@class AudiogramViewController;

@interface AudiogramViewController : UIViewController {
    IBOutlet CPTGraphHostingView *_graphingHostingView;
    SimpleScatterPlot *_scatterPlot;
}
@property (nonatomic, retain) NSManagedObjectContext *managedObjectContext;

@end
