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
#include "SpaceScene.h"
#include "AudiogramScene.h"

USING_NS_CC;
class MainMenuLayer : public CCLayer
{
public:
    MainMenuLayer();
    virtual void startGameCallback(CCObject* pSender);
    virtual void draw();
    
    void optionsCallback(CCObject* pSender);
    void levelsCallback(CCObject* pSender);
    void statsCallback(CCObject* pSender);
    void exitCallback(CCObject* pSender);
    void creditsCallback(CCObject* pSender);
    
    CREATE_FUNC(MainMenuLayer);
private:
    
};

class LevelLayer : public CCLayer
{
public:
    LevelLayer();
    void backCallback(CCObject* sender);
    void menuCallback(CCObject* sender);
    void musicCallback(CCObject* sender);
    void fxCallback(CCObject* sender);
    void sensitivityCallback(CCObject* sender);
    void qualityCallback(CCObject* sender);
    
    void startGameCallback(CCObject* sender);
    void startAccelCallback(CCObject* sender);
    void startCannonCallback(CCObject* pSender);
    void startCountingCallback(CCObject* pSender);

    
    
    CREATE_FUNC(LevelLayer);
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
    void calibrateCallback(CCObject* sender);
        
    CREATE_FUNC(OptionsLayer);
private:
    
};

class CreditsLayer : public CCLayer
{
public:
    CreditsLayer();
    virtual void onEnter();
    void backCallback(CCObject* pSender);
    
    CREATE_FUNC(CreditsLayer);
private:
    
};

class CalibrateLayer : public CCLayer
{
public:
    CalibrateLayer();
    virtual void onEnter();
    void backCallback(CCObject* pSender);
    void calibrateCallback(CCObject* pSender);
    void startCallback(CCObject* sender);
    void playSingleTone();
    void stopAndMeasureTone();

    CREATE_FUNC(CalibrateLayer);
private:
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
    
    float decibel_levels[100][100];
    int volIndex = 0;
    int freqIndex = 0;
};

class MainMenu : public CCScene
{   
public:
    void* rootVC;
    
    virtual void onEnter();
    void runMainMenu();
    void initAudio();
    
    iOSBridge::ToneGeneratorHelper* getToneGenerator(void);
    void setToneGenerator(iOSBridge::ToneGeneratorHelper *);
    
    iOSBridge::DataStore* getDataStore(void);
    void setDataStore(iOSBridge::DataStore *);
    
    CREATE_FUNC(MainMenu);
private:
    CCMenu* m_pItemMenu;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
};

#endif /* defined(__Sonence__MainMenu__) */
