//
//  ToneGenerator_objc.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//

#import <Foundation/Foundation.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <dispatch/dispatch.h>
#import <QuartzCore/QuartzCore.h>

#include "Stk.h"
#include "SineWave.h"
#include "Mandolin.h"
#include "Envelope.h"
#include "Asymp.h"
#include "BlitSaw.h"
#include "FileLoop.h"

using namespace stk;
struct AudioData{
    Mandolin *myMandolin;
    Envelope **myAsymp;
    BlitSaw **sineWaves;
    FileLoop *backgroundMusic;
    int numAsteroids;
};

@interface ToneGenerator_objc : NSObject {

    Float64                         graphSampleRate;
    
    // Before using an AudioStreamBasicDescription struct you must initialize it to 0. However, because these ASBDs
    // are declared in external storage, they are automatically initialized to 0.
    AudioStreamBasicDescription     stereoStreamFormat;
    AudioStreamBasicDescription     monoStreamFormat;
    AUGraph                         processingGraph;
    BOOL                            playing;
    BOOL                            momuInitialized;
    BOOL                            backgroundEnabled;
    BOOL                            fxEnabled;
    BOOL                            interruptedDuringPlayback;
    AudioUnit                       mixerUnit;
    
@public
    Float32            *sinBuffer;
    struct AudioData audioData;
}


@property (getter = isPlaying) BOOL playing;
@property                       BOOL                        interruptedDuringPlayback;
@property                       AudioUnit                   mixerUnit;
@property (nonatomic, retain) NSMutableArray *frequencies;
@property (nonatomic, retain) NSMutableArray *amplitudes;
@property float *tmpbuffer;
@property double frequency;
@property double sampleRate;
@property double theta;
@property double volume;
@property double amplitude;
@property AudioUnitSampleType             *sineBuffer;

- (FLOAT32) RemoveTone:(int) index;

@end
