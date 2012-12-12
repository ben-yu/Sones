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
    
    ToneGeneratorHelper::ToneGeneratorHelper(int numOfAsteroids)
    {
        toneGenerator_objc = [[ToneGenerator_objc alloc] init:numOfAsteroids];
    }
    
    ToneGeneratorHelper::~ToneGeneratorHelper(void)
    {
    }
    
    void ToneGeneratorHelper::ShowAlert()
    {
        [ToneGenerator_objc  ShowAlert];
    }
    
    void ToneGeneratorHelper::playRandomTone()
    {
        [toneGenerator_objc  Play];
    }
    
    void ToneGeneratorHelper::addTone(float frequency, double duration, int index)
    {
        [toneGenerator_objc  AddTone: (int) floorf(frequency) timeConst:(double) duration toneNum:index];
    }
    
    float ToneGeneratorHelper::removeTone(int index)
    {
        return [[toneGenerator_objc  RemoveTone:index] floatValue];
    }
    
    void ToneGeneratorHelper::play()
    {
        [toneGenerator_objc  Play];
    }
    
    void ToneGeneratorHelper::stop()
    {
        [toneGenerator_objc  Stop];
    }
    
}