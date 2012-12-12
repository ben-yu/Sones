//
//  SimpleScatterPlot.m
//  MyToneGenerator
//
//  Created by Benjamin Yu on 12-09-05.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "SimpleScatterPlot.h"

@implementation SimpleScatterPlot
@synthesize hostingView = _hostingView;
@synthesize graph = _graph;
@synthesize graphData = _graphData;

// Initialise the scatter plot in the provided hosting view with the provided data.
// The data array should contain NSValue objects each representing a CGPoint.
-(id)initWithHostingView:(CPTGraphHostingView *)hostingView andData:(NSMutableArray *)data
{
    self = [super init];
    
    if ( self != nil ) {
        self.hostingView = hostingView;
        self.graphData = data;
        self.graph = nil;
    }
    
    return self;
}

// This does the actual work of creating the plot if we don't already have a graph object.
-(void)initialisePlot
{
    // Sanity checks
    if ( (self.hostingView == nil) || (self.graphData == nil) ) {
        NSLog(@"SimpleScatterPlot: Cannot initialise plot without hosting view or data.");
        return;
    }
    
    if ( self.graph != nil ) {
        NSLog(@"SimpleScatterPlot: Graph object already exists.");
        return;
    }
    
    // Create a graph object which we will use to host just one scatter plot.
    CGRect frame = [self.hostingView bounds];
    self.graph = [[CPTXYGraph alloc] initWithFrame:frame];
    
    // Add some padding to the graph, with more at the bottom for axis labels.
    self.graph.plotAreaFrame.paddingTop = 20.0f;
    self.graph.plotAreaFrame.paddingRight = 20.0f;
    self.graph.plotAreaFrame.paddingBottom = 50.0f;
    self.graph.plotAreaFrame.paddingLeft = 10.0f;
    
    // Tie the graph we've created with the hosting view.
    self.hostingView.hostedGraph = self.graph;
    
    // Apply theme
    [self.graph applyTheme:[CPTTheme themeNamed:kCPTDarkGradientTheme]];
    
    // Create a line style that we will apply to the axis and data line.
    CPTMutableLineStyle *lineStyle = [CPTMutableLineStyle lineStyle];
    lineStyle.lineColor = [CPTColor greenColor];
    lineStyle.lineWidth = 2.0f;
    
    // Create a text style that we will use for the axis labels.
    CPTMutableTextStyle *textStyle = [CPTMutableTextStyle textStyle];
    textStyle.fontName = @"Helvetica";
    textStyle.fontSize = 10;
    textStyle.color = [CPTColor whiteColor];
    
    // Create the plot symbol we're going to use.
    CPTPlotSymbol *plotSymbol = [CPTPlotSymbol crossPlotSymbol];
    plotSymbol.lineStyle = lineStyle;
    plotSymbol.size = CGSizeMake(8.0, 8.0);
    
    // Setup some floats that represent the min/max values on our axis.
    // FIXME - Audiograms have volume axis flipped
    float xAxisMin = -3000;
    float xAxisMax = 10000;
    float yAxisMin = -10;
    float yAxisMax = 100;
    
    // We modify the graph's plot space to setup the axis' min / max values.
    CPTXYPlotSpace *plotSpace = (CPTXYPlotSpace *)self.graph.defaultPlotSpace;
    plotSpace.xRange = [CPTPlotRange plotRangeWithLocation:CPTDecimalFromFloat(xAxisMin) length:CPTDecimalFromFloat(xAxisMax - xAxisMin)];
    plotSpace.yRange = [CPTPlotRange plotRangeWithLocation:CPTDecimalFromFloat(yAxisMin) length:CPTDecimalFromFloat(yAxisMax - yAxisMin)];
    
    // Modify the graph's axis with a label, line style, etc.
    CPTXYAxisSet *axisSet = (CPTXYAxisSet *)self.graph.axisSet;
    
    axisSet.xAxis.title = @"Frequency(Hz)";
    axisSet.xAxis.titleTextStyle = textStyle;
    axisSet.xAxis.titleOffset = 30.0f;
    axisSet.xAxis.labelOffset = 3.0f;
    axisSet.xAxis.majorIntervalLength = CPTDecimalFromFloat(2000.0f);
    axisSet.xAxis.minorTicksPerInterval = 1;
    axisSet.xAxis.minorTickLength = 5.0f;
    axisSet.xAxis.majorTickLength = 7.0f;
    
    axisSet.yAxis.title = @"Volume(dB)";
    axisSet.yAxis.titleTextStyle = textStyle;
    axisSet.yAxis.titleOffset = 40.0f;
    axisSet.yAxis.labelOffset = 3.0f;
    axisSet.yAxis.majorIntervalLength = CPTDecimalFromFloat(20.0f);
    axisSet.yAxis.minorTicksPerInterval = 1;
    axisSet.yAxis.minorTickLength = 5.0f;
    axisSet.yAxis.majorTickLength = 7.0f;
    
    // Add a plot to our graph and axis. We give it an identifier so that we
    // could add multiple plots (data lines) to the same graph if necessary.
    CPTScatterPlot *plot = [[CPTScatterPlot alloc] init];
    plot.dataSource = self;
    plot.identifier = @"mainplot";
    plot.dataLineStyle = lineStyle;
    //plot.areaFill = [CPTFill fillWithGradient:[CPTGradient aquaSelectedGradient]];
    //plot.areaBaseValue = [[NSDecimalNumber numberWithInt:0] decimalValue];
    [self.graph addPlot:plot];
}

// Delegate method that returns the number of points on the plot
-(NSUInteger)numberOfRecordsForPlot:(CPTPlot *)plot
{
    if ( [plot.identifier isEqual:@"mainplot"] )
    {
        return [self.graphData count];
    }
    
    return 0;
}

// Delegate method that returns a single X or Y value for a given plot.
-(NSNumber *)numberForPlot:(CPTPlot *)plot field:(NSUInteger)fieldEnum recordIndex:(NSUInteger)index
{
    if ( [plot.identifier isEqual:@"mainplot"] )
    {
        NSValue *value = [self.graphData objectAtIndex:index];
        CGPoint point = [value CGPointValue];
        
        // FieldEnum determines if we return an X or Y value.
        if ( fieldEnum == CPTScatterPlotFieldX )
        {
            return [NSNumber numberWithFloat:point.x];
        }
        else    // Y-Axis
        {
            return [NSNumber numberWithFloat:point.y];
        }
    }
    
    return [NSNumber numberWithFloat:0];
}

@end
