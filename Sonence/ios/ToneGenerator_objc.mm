//
//  ToneGenerator_objc.m
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//

#import "ToneGenerator_objc.h"
#import <AudioToolbox/AudioToolbox.h>
#import "mo_audio.h"

#define ARC4RANDOM_MAX      0x100000000
#define SRATE 44100
#define FRAMESIZE 128
#define NUMCHANNELS 2
#define DBOFFSET -74.0 

@interface ToneGenerator_objc()
@end

void audioCallback( Float32 * buffer, UInt32 framesize, void* userData )
{
    AudioData * data = (AudioData*) userData;
    
    for(int i=0; i<framesize; i++)
    {
        SAMPLE outz;
        if (data->backgroundEnabled) {
            outz = data->backgroundMusic->tick();
        } else {
            outz = 0.0;
        }
        for (int j=0; j < data->numAsteroids; j++) {
            Float32 amp = ((Float32) (pow(10.0,(98.0*data->myAsymp[j]->tick() - 98.0)/20.0)));
            //Float32 amp = data->myAsymp[j]->tick();
            outz += amp * data->sineWaves[j]->tick();
            //outz += data->myAsymp[j]->tick() * data->sineWaves[j]->tick();
            //if (amp >= 1.0)
            //    NSLog(@"decibel level is %f", outz);
        }
        
        buffer[2*i] = buffer[2*i+1] = outz;
    }
}

@implementation ToneGenerator_objc
@synthesize playing;

- (id)init:(int) numOfAsteroids
{
    self = [super init];
    if (self) {
        audioData.numAsteroids = numOfAsteroids;
        audioData.myMandolin = new Mandolin(400);
        NSBundle *mainBundle = [NSBundle mainBundle];
        NSString *myFile = [mainBundle pathForResource: @"echelon" ofType: @"wav"];
        audioData.backgroundMusic = new FileLoop(std::string([myFile UTF8String]));
        audioData.myAsymp = (Envelope **) calloc(sizeof(void *), numOfAsteroids);
        audioData.sineWaves = (BlitSaw **) calloc(sizeof(void *), numOfAsteroids);
        for (int i=0; i<numOfAsteroids; i++) {
            audioData.sineWaves[i] = new BlitSaw();
            audioData.myAsymp[i] = new Envelope();
        }
        if (!momuInitialized) {
            // init audio
            NSLog(@"Initializing Audio");
            // init the MoAudio layer
            bool result = MoAudio::init(SRATE, FRAMESIZE, NUMCHANNELS);
            
            if (!result)
            {
                NSLog(@"cannot initialize real-time audio!");
                return self;
            }
            
            // start the audio layer, registering a callback method
            result = MoAudio::start( audioCallback, &audioData);
            if (!result)
            {
                NSLog(@"cannot start real-time audio!");
                return self;
            }
            momuInitialized = true;
        }
        self.playing = YES;
    }
    
    return self;
}

- (id)initWithBackground:(NSString *) fileName {
    self = [super init];
    if (self) {
        audioData.backgroundMusic = new FileLoop("background-music-aac.wav");
        if (!momuInitialized) {
            // init audio
            NSLog(@"Initializing Audio");
            // init the MoAudio layer
            bool result = MoAudio::init(SRATE, FRAMESIZE, NUMCHANNELS);
            
            if (!result)
            {
                NSLog(@"cannot initialize real-time audio!");
                return self;
            }
            
            // start the audio layer, registering a callback method
            result = MoAudio::start( audioCallback, &audioData);
            if (!result)
            {
                NSLog(@"cannot start real-time audio!");
                return self;
            }
            momuInitialized = true;
        }
    }
    return self;
}

- (void) dealloc {
    AudioSessionSetActive(false);
    [super dealloc];
}


- (void) Play {
    // init audio
    NSLog(@"Initializing Audio");
    
    // init the MoAudio layer
    bool result = MoAudio::init(SRATE, FRAMESIZE, NUMCHANNELS);
    
    if (!result)
    {
        NSLog(@"cannot initialize real-time audio!");
        return;
    }
    
    // start the audio layer, registering a callback method
    result = MoAudio::start( audioCallback, &audioData);
    if (!result)
    {
        NSLog(@"cannot start real-time audio!");
        return;
    }
    self.playing = YES;
}

- (void) Stop {
    if (self.playing) {
        MoAudio::stop();
        self.playing = NO;
    }
}

- (void) playBackgroundMusic:(NSString *) fileName {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSArray *components = [fileName componentsSeparatedByString:@"."];
    NSString *myFile = [mainBundle pathForResource: [components objectAtIndex:0] ofType: [components objectAtIndex:1] ];
    audioData.backgroundMusic = new FileLoop(std::string([myFile UTF8String]));
}

- (void) enableBackground {
    audioData.backgroundEnabled = true;
}

- (void) disableBackground {
    audioData.backgroundEnabled = false;
}

- (void) AddTone:(int) frequency
         timeConst:(double) duration
         toneNum:(int) index
{
    audioData.sineWaves[index]->setFrequency(frequency);
    audioData.myAsymp[index]->setTarget(1.0);
    audioData.myAsymp[index]->setTime(duration);
}

- (NSNumber *) RemoveTone:(int) index
{
    audioData.sineWaves[index]->setFrequency(0.0);
    audioData.sineWaves[index]->reset();
    NSNumber *tmp = [[NSNumber alloc] initWithFloat: audioData.myAsymp[index]->tick()];
    audioData.myAsymp[index]->setTime(0.0);
    audioData.myAsymp[index]->setTarget(0.0);
    return tmp;
}

/*
- (dispatch_source_t) CreateDispatchTimer:(uint64_t) interval
                               withLeeway:(uint64_t) leeway
                                 andQueue:(dispatch_queue_t) queue
                                 andBlock:(dispatch_block_t) block
{
    dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,
                                                     0, 0, queue);
    if (timer)
    {
        dispatch_source_set_timer(timer, dispatch_walltime(NULL, 0), interval, leeway);
        dispatch_source_set_event_handler(timer, block);
        dispatch_resume(timer);
    }
    return timer;
}
*/
@end
