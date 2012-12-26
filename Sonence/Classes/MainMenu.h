//
//  MainMenu.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-12-22.
//
//

#ifndef __Sonence__MainMenu__
#define __Sonence__MainMenu__

#include <iostream>
#include "cocos2d.h"
#include "CCParallaxNodeExtras.h"
#include "ToneGeneratorHelper.h"
#include "DataStore.h"
#include "Ship.h"
#include "SpaceScene.h"

USING_NS_CC;
class MainMenuLayer : public CCLayer
{
public:
    virtual void onEnter();
    virtual void startGameCallback(CCObject* pSender);
    virtual void draw();
    
    CREATE_FUNC(MainMenuLayer);
private:
    
};

class MainMenu : public CCScene
{   
public:
    virtual void onEnter();
    virtual void runMainMenu();
    void initAudio();
    
    iOSBridge::ToneGeneratorHelper* getToneGenerator(void);
    void setToneGenerator(iOSBridge::ToneGeneratorHelper *);
private:
    CCMenu* m_pItemMenu;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
};

#endif /* defined(__Sonence__MainMenu__) */
