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
@property AUGraph processingGraph;
@property NSDate *date;
@property AudioUnit ioUnit;
//@property AudioUnit mixerUnit;
@property dispatch_source_t volTimer;
@property BOOL gameStarted;
@end

void audioCallback( Float32 * buffer, UInt32 framesize, void* userData )
{
    AudioData * data = (AudioData*) userData;
    
    for(int i=0; i<framesize; i++)
    {
        SAMPLE outz = 0.0;
        for (int j=0; j < data->numAsteroids; j++) {
            Float32 amp = data->myAsymp[j]->tick();
            outz += ((Float32) (pow(10.0,(amp - 98.0)/20.0))) * data->sineWaves[j]->tick();
            //if (amp > 60.9)
            //    NSLog(@"decibel level is %f", outz);
        }
        
        buffer[2*i] = buffer[2*i+1] = outz;
    }
}

@implementation ToneGenerator_objc

@synthesize processingGraph = _processingGraph;
@synthesize frequency = _frequency;
@synthesize sampleRate = _sampleRate;
@synthesize theta = _theta;
@synthesize amplitude = _amplitude;
@synthesize volume = _volume;
@synthesize date = _date;
@synthesize ioUnit = _ioUnit;
@synthesize mixerUnit = _mixerUnit;
@synthesize frequencies = _frequencies;
@synthesize amplitudes = _amplitudes;
@synthesize tmpbuffer = _tmpbuffer;
@synthesize sineBuffer = _sineBuffer;
@synthesize playing;

- (id)init:(int) numOfAsteroids
{
    self = [super init];
    if (self) {
        audioData.numAsteroids = numOfAsteroids;
        audioData.myMandolin = new Mandolin(400);
        audioData.myAsymp = (Envelope **) calloc(sizeof(void *), 5);
        audioData.sineWaves = (BlitSaw **) calloc(sizeof(void *), 5);
        for (int i=0; i<numOfAsteroids; i++) {
            audioData.sineWaves[i] = new BlitSaw();
            audioData.myAsymp[i] = new Envelope();
        }
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
        self.playing = YES;
    }
    
    return self;
}

- (void)dealloc {
    AUGraphStop (_processingGraph);
    AudioSessionSetActive(false);
    [super dealloc];
}


- (void) PlayRandomTone {
    if (self.isPlaying) {
        double timePassed_ms = [_date timeIntervalSinceNow] * -1000.0;
        OSStatus result = AUGraphStop (_processingGraph);
        NSAssert1(result == noErr, @"Error stopping AUGraph %ld", result);
        self.playing = NO;
        _amplitude = 0.0;
        NSLog(@"Time Passed: %f",timePassed_ms);
        dispatch_source_cancel(_volTimer);       
    } else {
        // Randomize frequency
        _frequency = floorf(((double)arc4random() / ARC4RANDOM_MAX) * 900.0f) + 500.0f;
        
        // Start volume timer
        //_volTimer = [self CreateDispatchTimer:1ull * NSEC_PER_MSEC withLeeway:1ull * NSEC_PER_MSEC  andQueue:dispatch_get_main_queue() andBlock:^{ [self increaseVolume]; }];
        
        OSStatus result = AUGraphStart(_processingGraph);
        NSAssert1(result == noErr, @"Error starting AUGraph %ld", result);
        self.playing = YES;
        _date = [NSDate date];
    }
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

- (void) AddTone:(int) frequency
       timeConst:(double) duration
        toneNum:(int) index
{
    audioData.sineWaves[index]->setFrequency(frequency);
    //audioData.sineWaves[index]->addPhaseOffset(index*15);
    audioData.myAsymp[index]->setTime(duration);
    audioData.myAsymp[index]->setTarget(98.0/audioData.numAsteroids);
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

@end
