//
//  MainMenuViewController.m
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//

#import "MainMenuViewController.h"
#import "RootViewController.h"

@interface MainMenuViewController ()
@property (nonatomic, retain) NSMutableArray *items;
@end

@implementation MainMenuViewController
@synthesize controller = _controller;
@synthesize managedObjectContext = _managedObjectContext;
@synthesize carousel;
@synthesize items;

- (void)awakeFromNib
{
    //set up data
    //your carousel should always be driven by an array of
    //data of some kind - don't store data in your item views
    //or the recycling mechanism will destroy your data once
    //your item views move off-screen
    self.items = [NSMutableArray array];
    for (int i = 0; i < 1; i++)
    {
        [items addObject:[NSNumber numberWithInt:i]];
    }
}


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {

    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    carousel.type = iCarouselTypeRotary;
    
    // Volume Slider
    MPVolumeView *myVolumeView = [[MPVolumeView alloc] initWithFrame:CGRectMake(10,250,self.view.frame.size.width,self.view.frame.size.height)];
    [self.view addSubview: myVolumeView];
    [myVolumeView release];
    
    UIFont *myFont = [UIFont fontWithName:@"Audiowide-Regular" size:40];
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(20,20, self.view.frame.size.height, self.view.frame.size.width/7)];
    titleLabel.font = myFont;
    titleLabel.text = @"Sonic Intelligence";
    titleLabel.backgroundColor = [UIColor clearColor];
    titleLabel.textColor = [UIColor whiteColor];
    [self.view addSubview:titleLabel];
    [titleLabel release];
    
    self.view.backgroundColor = [UIColor colorWithPatternImage: [UIImage imageNamed:@"hexabump_@2X.png"]];
    
    // Create game controller
    _controller = [[RootViewController alloc] init];
    _controller.managedObjectContext = self->_managedObjectContext;

	// Do any additional setup after loading the view.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    self.carousel = nil;
    // Release any retained subviews of the main view.
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return  (interfaceOrientation == UIInterfaceOrientationLandscapeLeft || interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

-(IBAction) goCocos2dx: (id) sender {
    //get item index for button
	NSInteger index = [carousel indexOfItemViewOrSubview:sender];
    if (index == 1) {
        _controller.gameType = 1;
    } else {
        _controller.gameType = 0;
    }
    [self presentViewController:_controller animated:YES completion:NULL];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
	if ([segue.identifier isEqualToString:@"Segue To Graph"])
	{
        NSLog(@"Audiogram View");
        
        NSLog(@"Current User: %@", self.managedObjectContext);
	}
}

#pragma mark -
#pragma mark iCarousel methods

- (NSUInteger)numberOfItemsInCarousel:(iCarousel *)carousel
{
    //return the total number of items in the carousel
    return [items count];
}

- (UIView *)carousel:(iCarousel *)carousel viewForItemAtIndex:(NSUInteger)index reusingView:(UIView *)view
{
	UIButton *button = (UIButton *)view;
	if (button == nil)
	{
        UIImage *image, *scaledImage;
		//no button available to recycle, so create new one
        if (index != 2) {
            image = [UIImage imageNamed:@"button.png"];
            scaledImage =
            [UIImage imageWithCGImage:[image CGImage]
                                scale:3.0 orientation:UIImageOrientationUp];
        } else {
            image = [UIImage imageNamed:@"Icon-72.png"];
            scaledImage =
            [UIImage imageWithCGImage:[image CGImage]
                                scale:1.0 orientation:UIImageOrientationUp];
        }
		button = [UIButton buttonWithType:UIButtonTypeCustom];
		button.frame = CGRectMake(0.0f, 0.0f, scaledImage.size.width, scaledImage.size.height);
		[button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
		[button setBackgroundImage:scaledImage forState:UIControlStateNormal];
		button.titleLabel.font = [button.titleLabel.font fontWithSize:20];
		[button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchUpInside];
	}
    
	//set button label
	[button setTitle:@"Start" forState:UIControlStateNormal];
    
	return button;
}

#pragma mark -
#pragma mark Button tap event

- (void)buttonTapped:(UIButton *)sender
{
    [self goCocos2dx:sender];
}

@end
