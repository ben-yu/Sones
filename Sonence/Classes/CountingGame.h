#ifndef __CountingGame_SCENE_H__
#define __CountingGame_SCENE_H__

#include "cocos2d.h"
#include "CCParallaxNodeExtras.h"
#include "ToneGeneratorHelper.h"
#include "DataStore.h"
#include "document.h"
#include "rapidjson.h"
#include "MLSearch.h"
#include <curl/curl.h>


USING_NS_CC;

class CountingGameLayer : public CCLayer
{
public:
    
    const float TWOPI = 3.1415926535 * 2.0;
    
    CountingGameLayer(void);
    ~CountingGameLayer(void);
    
    static float sysVer;
    
    static bool accelEnable;
    
    virtual void onEnter();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    void runGame();
    
    // scheduled Update
    void update(float dt);
    
    // Touch methods
    virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void draw();
    
    float randomValueBetween( float low , float high );
    double randomValueBetweenD( double low , double high );
    void setInvisible(CCNode * node);
    void setAsteroidInvisible(CCNode * node);
    void setEnemyInvisible(CCNode * node);
    void setFireInvisible(CCNode *node);
    float getTimeTick();
    
    void startTones();
    void stopTones();
    
    void playTutorial();
    void endTutorial();
    void playTones();
    void playSingleTone();
    void scheduleTone();
    
    void pressedOne();
    void pressedTwo();
    
    
    CREATE_FUNC(CountingGameLayer);
    
private:
    // Game Resources - Sprites
    CCSprite *happyEmoticon;
    CCSprite *sadEmoticon;
    CCMenuItemImage *oneButton;
    CCMenuItemImage *twoButton;
    
    // Game Resources - Labels
    CCLabelTTF *oneLabel;
    CCLabelTTF *twoLabel;
    CCLabelTTF *livesLabel;
    CCLabelTTF *tutorialText;
    CCLabelTTF *tutorialText2;
    CCLabelTTF *tutorialText3;
    CCLabelTTF *tutorialText4;
    CCLabelTTF *tutorialText5;
    
    CCMenu* tutorialSkipButton;
    
    iOSBridge::ToneGeneratorHelper *toneGenHelp;    // Sound Generator
    iOSBridge::DataStore *dataStoreHandler;         // Data Store Handler
    rapidjson::Document jsonDoc;
    MLSearch *searchHelper;
    void *rootVCPtr;
    
    float winWidth;
    float winHeight;
    
    // Player Data
    CCPoint playerPos;
    int _health = 100;
    int lives = 3;
    
    float *alphaTargets;
    float *timeTargets;
    
    // Game State Variables
    bool playedTutorial = false;
    bool enemySpawned = false;
    bool asteroidSpawned = false;
    bool gameOver = false;
    bool spawnRegularEnemies;
    bool touchTimerFlag = false;
    bool reloading = false;
    
    int numTones = 1;
    
    float radarRadius;
    float enemyVelocity;
    float reloadTime = 2.0;
    float seed_freq;
    float tutorialDuration;
    float initTime;
    float touchTime;
    float finalTime;
    float baseVol;
    float volDelta;
    
    float baseVolumes[10];
    float volDiffs[10];
    
    int _nextAsteroid;
    int _curAsteroidCount;
    int asteroidIndex;
    int freqIndex = 0;
    int earIndex = 0;
    int destroyedAsteroids;
    float _nextAsteroidSpawn = 0.0;
    int _nextShipLaser = 0;
    float win_height;
    float *sineTones;
    float _shipPointsPerSecY;
    
};

class PauseSceneLayer3 : public CCLayer
{
public:
    PauseSceneLayer3();
    ~PauseSceneLayer3();
    
    virtual void resumeCallback(CCObject* pSender);
    virtual void MainMenuCallback(CCObject* pSender);
    virtual void onEnter();
    void musicCallback(CCObject *sender);
    void fxCallback(CCObject *sender);
private:
    CCMenuItemToggle *item2;
    CCMenuItemToggle *item3;
    
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    
};

class CountingGame : public cocos2d::CCScene
{
public:
    int gameMode;
    
    CountingGame();
    virtual void runGame();
    
    // The CallBack for back to the main menu scene
    virtual void MainMenuCallback(CCObject* pSender);
    virtual void pauseMenuCallback(CCObject* pSender);
    
    iOSBridge::ToneGeneratorHelper* getToneGenerator(void);
    void setToneGenerator(iOSBridge::ToneGeneratorHelper *);
    
    iOSBridge::DataStore* getDataStore(void);
    void setDataStore(iOSBridge::DataStore *);
    
    void RecursivelyPauseAllChildren(CCNode* node);
    void RecursivelyResumeAllChildren(CCNode* node);
    
    void *rootVC;
    CCMenuItemImage *pauseButton;
    CCScene* mainMenuPtr;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
    rapidjson::Document jsonDoc;
    
private:
    
};

#endif // __CountingGame_SCENE_H__
