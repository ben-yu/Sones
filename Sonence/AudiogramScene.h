//
//  Audiogram.h
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-10.
//
//

#ifndef __Sonence__AudiogramScene__
#define __Sonence__AudiogramScene__

#include <iostream>

#include "cocos2d.h"
#include "DataStore.h"

USING_NS_CC;

class AudiogramSceneLayer : public CCLayer
{
public:
    AudiogramSceneLayer();
    virtual void draw();
    virtual void onEnter();
    void backCallback(CCObject* sender);
    
    iOSBridge::DataPoint *displayData;
    iOSBridge::DataStore *dsPtr;
    void* rootVC;
    CREATE_FUNC(AudiogramSceneLayer);
private:
    
    float winWidth;
    float winHeight;
    
};

#endif /* defined(__Sonence__AudiogramScene__) */