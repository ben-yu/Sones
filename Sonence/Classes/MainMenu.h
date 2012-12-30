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
    MainMenuLayer();
    virtual void startGameCallback(CCObject* pSender);
    virtual void draw();
    
    void optionsCallback(CCObject* pSender);
    
    CREATE_FUNC(MainMenuLayer);
private:
    
};

class OptionsLayer : public CCLayer
{
public:
    OptionsLayer();
    void backCallback(CCObject* sender);
    void menuCallback(CCObject* sender);
    void musicCallback(CCObject* sender);
    void fxCallback(CCObject* sender);
    void sensitivityCallback(CCObject* sender);
    void qualityCallback(CCObject* sender);
    
    
    CREATE_FUNC(OptionsLayer);
private:
    
};

class StatsLayer : public CCLayer
{
public:
    virtual void onEnter();

    CREATE_FUNC(StatsLayer);
private:
    
};

class CreditsLayer : public CCLayer
{
public:
    virtual void onEnter();
    
    CREATE_FUNC(CreditsLayer);
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
    
    iOSBridge::DataStore* getDataStore(void);
    void setDataStore(iOSBridge::DataStore *);
private:
    CCMenu* m_pItemMenu;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
};

#endif /* defined(__Sonence__MainMenu__) */
