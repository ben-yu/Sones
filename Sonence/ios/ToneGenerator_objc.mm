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
#define DBOFFSET 98.0 

@interface ToneGenerator_objc()
@end

// Render callback function called from MoMu
void audioCallback( Float32 * buffer, UInt32 framesize, void* userData )
{
    AudioData * data = (AudioData*) userData;
    
    for(int i=0; i<framesize; i++)
    {
        SAMPLE outz, fx;
        Float32 amp ;
        
        // Background Music
        if (data->backgroundEnabled && data->oscillateBackground) {
            amp = data->myAsymp[0]->tick();
            outz = amp * data->backgroundMusic->tick();
        } else if (data->backgroundEnabled  && !data->oscillateBackground) {
            outz = data->backgroundMusic->tick();
        } else {
            outz = 0.0;
        }
        
        if (data->fxEnabled) {
            // Pure-Tones
            if (!data->maxVol) {
                amp = ((Float32) (pow(10.0,(98.0*data->myAsymp[0]->tick() - 98.0)/20.0)));
                fx = amp * data->sineWaves[0]->tick();
            } else {
                fx = data->sineWaves[0]->tick();
            }
            
            // Explosion
            if (data->playExplosion)
                outz += data->explosion->tick();
            if (data->explosion->isFinished()) {
                data->playExplosion = false;
                data->explosion->reset();
            }
        }
        
        // Channel Output
        switch (data->toneIndex) {
            case 0:
                buffer[2*i] = outz + fx;
                buffer[2*i+1] = outz;
                break;
            case 1:
                buffer[2*i] = outz;
                buffer[2*i+1] = outz + fx;
                break;
            default:
                buffer[2*i] = buffer[2*i+1] = outz + fx;
                break;
        }
        
        if (data->oscillate){
            if (amp >= data->upperBound)
                data->myAsymp[0]->setTarget(data->lowerBound);
            else if (amp <= data->lowerBound)
                data->myAsymp[0]->setTarget(data->upperBound);
        }
    }
}

@implementation ToneGenerator_objc
@synthesize playing;


- (id)init:(int) numOfAsteroids
{
    self = [super init];
    if (self) {
        
        NSBundle *mainBundle = [NSBundle mainBundle]; // Get Bundle
        NSString *myFile = [mainBundle pathForResource: @"echelon" ofType: @"wav"]; // Open background music file
        
        audioData.numAsteroids = numOfAsteroids;
        audioData.myMandolin = new Mandolin(400);
        audioData.backgroundMusic = new FileLoop(std::string([myFile UTF8String]));
        audioData.myAsymp = (Envelope **) calloc(sizeof(void *), numOfAsteroids);
        audioData.sineWaves = (SineWave **) calloc(sizeof(void *), numOfAsteroids);
        
        myFile = [mainBundle pathForResource: @"explosion" ofType: @"wav"]; // Open explosion music file
        audioData.explosion = new FileWvIn(std::string([myFile UTF8String]));
        
        for (int i=0; i<numOfAsteroids; i++) {
            audioData.sineWaves[i] = new SineWave();
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

- (BOOL) getBackgroundStatus {
    return audioData.backgroundEnabled;
}

- (void) AddTone:(int) frequency
         timeConst:(double) duration
         toneNum:(int) index
{
    audioData.sineWaves[0]->setFrequency(frequency);
    audioData.myAsymp[0]->setTarget(1.0);
    audioData.myAsymp[0]->setTime(duration);
    audioData.toneIndex = index;
    audioData.maxVol = false;
}

- (void) playOscillatingTone:(int) frequency
       timeConst:(double) duration
         toneNum:(int) index
{
    audioData.sineWaves[0]->setFrequency(frequency);
    audioData.myAsymp[0]->setTarget(100.0);
    audioData.myAsymp[0]->setTime(duration);
    audioData.toneIndex = index;
    audioData.maxVol = true;
    audioData.oscillate = true;
    audioData.upperBound = 100.0;
    audioData.oscillateBackground = true;
}

- (void) playDecreasingTone:(int) frequency
                   timeConst:(double) duration
                     toneNum:(int) index
{
    audioData.sineWaves[0]->setFrequency(frequency);
    audioData.myAsymp[0]->setTarget(0.0);
    audioData.myAsymp[0]->setTime(duration);
    audioData.toneIndex = index;
    audioData.maxVol = false;
    audioData.oscillate = false;
}

- (void) PauseTone
{
    audioData.fxEnabled = false;
}

- (void) oscillateBackground
{
    audioData.oscillateBackground = true;
    audioData.maxVol = true;
}

- (void) MaxTone
{
    audioData.maxVol = true;
}

- (BOOL) getToneStatus
{
    return audioData.fxEnabled;
}

- (NSNumber *) getAmplitude
{
    return [[NSNumber alloc] initWithFloat: 98.0 * audioData.myAsymp[0]->tick()];
}

- (void) ResumeTone
{
    audioData.fxEnabled = true;
}

- (NSNumber *) RemoveTone:(int) index
{
    audioData.sineWaves[0]->setFrequency(0.0);
    audioData.sineWaves[0]->reset();
    NSNumber *tmp = [[NSNumber alloc] initWithFloat: 98.0 * audioData.myAsymp[index]->tick()];
    audioData.myAsymp[0]->setTime(0.0);
    audioData.myAsymp[0]->setTarget(0.0);
    return tmp;
}

- (void) playExplosion
{
    audioData.playExplosion = true;
}

- (Float32) getVolume
{
    return MoAudio::getVolume();
}
@end
