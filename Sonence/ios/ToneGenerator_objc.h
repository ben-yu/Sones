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
    SineWave **sineWaves;
    FileLoop *backgroundMusic;
    FileWvIn *explosion;
    float lowerBound = 0.001;
    float upperBound = 1.0;
    int numAsteroids;
    int toneIndex; // 0 - L , 1 - R, 2 - Both
    BOOL backgroundEnabled = true;
    BOOL fxEnabled = true;
    BOOL playExplosion = false;
    BOOL maxVol = false;
    BOOL oscillate = false;
    BOOL oscillateBackground = false;
};

@interface ToneGenerator_objc : NSObject {
    BOOL                            playing;
    BOOL                            momuInitialized;
    struct AudioData audioData;
}


@property (getter = isPlaying) BOOL playing;

- (id)init: (int) numOfAsteroids;
- (id)initWithBackground:(NSString *) fileName;
- (void) Play;
- (void) Stop;
- (void) playBackgroundMusic:(NSString *) fileName;
- (void) enableBackground;
- (void) disableBackground;
- (void) AddTone:(int) frequency
       timeConst:(double) duration
         toneNum:(int) index;
- (void) playConstantTone:(int) frequency
                   volume:(double) level
                  toneNum:(int) index;
- (void) playOscillatingTone:(int) frequency
                   timeConst:(double) duration
                     toneNum:(int) index;
- (void) playDecreasingTone:(int) frequency
                   timeConst:(double) duration
                     toneNum:(int) index;
- (NSNumber *) RemoveTone:(int) index;
- (void) ResumeTone;
- (void) PauseTone;
- (void) MaxTone;
- (void) playExplosion;
- (NSNumber *) getAmplitude;
- (Float32) getVolume;
- (void) setVolume:(float) level;
- (void) oscillateBackground;

@end
