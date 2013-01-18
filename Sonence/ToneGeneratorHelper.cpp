//
//  ToneGeneratorHelper.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//

#include "ToneGeneratorHelper.h"
#import "ToneGenerator_objc.h"

namespace iOSBridge{
    
    ToneGeneratorHelper::ToneGeneratorHelper()
    {
        toneGenerator_objc = [[ToneGenerator_objc alloc] initWithBackground:@"echelon.wav"];
    }
    
    ToneGeneratorHelper::ToneGeneratorHelper(int numOfAsteroids)
    {
        toneGenerator_objc = [[ToneGenerator_objc alloc] init:numOfAsteroids];
    }
    
    ToneGeneratorHelper::~ToneGeneratorHelper(void)
    {
    }
        
    void ToneGeneratorHelper::playRandomTone()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)  Play];
    }
    
    void ToneGeneratorHelper::addTone(float frequency, double duration, int index)
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   AddTone: (int) floorf(frequency) timeConst:(double) duration toneNum:index];
    }
    
    float ToneGeneratorHelper::removeTone(int index)
    {
        return [[((ToneGenerator_objc *)toneGenerator_objc)   RemoveTone:index] floatValue];
    }
    
    void ToneGeneratorHelper::play()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   Play];
    }
    
    void ToneGeneratorHelper::playBackgroundMusic(std::string fileName)
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   playBackgroundMusic:[NSString stringWithUTF8String:fileName.c_str()]];
    }
    
    void ToneGeneratorHelper::enableBackground()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   enableBackground];
    }
    
    void ToneGeneratorHelper::disableBackground()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   disableBackground];
    }
    
    void ToneGeneratorHelper::enableTones()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   ResumeTone];
    }
    
    void ToneGeneratorHelper::disableTones()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   PauseTone];
    }
    
    void ToneGeneratorHelper::stop()
    {
        [((ToneGenerator_objc *)toneGenerator_objc)   Stop];
    }
    
}