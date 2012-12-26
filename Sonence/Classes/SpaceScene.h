#ifndef __SpaceScene_SCENE_H__
#define __SpaceScene_SCENE_H__

#include "cocos2d.h"
#include "CCParallaxNodeExtras.h"
#include "ToneGeneratorHelper.h"
#include "DataStore.h"
#include "Ship.h"

USING_NS_CC;

class SpaceSceneLayer : public CCLayer
{
public:
    
    SpaceSceneLayer(void);
    ~SpaceSceneLayer(void);
    
    static float sysVer;
    
    static bool accelEnable;
    
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual void onEnter();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    //static cocos2d::CCScene* scene(float ver, bool accelEnabled, void *rootVC);
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    void runGame();
    
    // scheduled Update
    void update(float dt);
    
    // Touch methods
    virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void didAccelerate(CCAcceleration* pAccelerationValue);
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
    void spawnEnemy();
    void spawnEnemyAtLoc(float y);
    void spawnLowFreqEnemy();
    void spawnHighFreqEnemy();
    void spawnAsteroid();
    
    CREATE_FUNC(SpaceSceneLayer);
        
private:
    // Game Resources - Sprites
    cocos2d::CCSpriteBatchNode * _batchNode; // Background stuff
    CCParallaxNodeExtras *_backgroundNode;
    CCSprite *_spacedust1;
    CCSprite *_spacedust2;
    CCSprite *_planetsunrise;
    CCSprite *_galaxy;
    CCSprite *_spacialanomaly;
    CCSprite *_spacialanomaly2;
    CCSprite *playerShip;
    CCSprite *txtWindow;
    CCSprite *healthBar;
    CCProgressTimer *hpBar;
    
    CCArray  *_asteroids;
    CCArray *_spawnQueue;
    CCArray  *_enemies;
    CCArray * _shipLasers;
    CCParticleSystem*    m_emitter;
    
    CCSpriteFrameCache *frameCache;
    CCAnimationCache *animCache;
    
    // Game Resources - Labels
    CCLabelTTF *distLabel;
    CCLabelTTF *scoreLabel;
    CCLabelTTF *tutorialText;
    CCLabelTTF *tutorialText2;
    CCLabelTTF *tutorialText3;
    CCLabelTTF *tutorialText4;
    CCLabelTTF *tutorialText5;
    
    //
    CCPoint playerPos;
    
    // Sound Generator
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    
    // Data Store Handler
    iOSBridge::DataStore *dataStoreHandler;
    void *rootVCPtr;
    
    float winWidth;
    float winHeight;
    
    // Player Stats
    int score = 0;
    int distance = 0;
    int _health = 100;
    
    float *alphaTargets;
    float *timeTargets;
    
    bool playedTutorial = false;
    bool enemySpawned = false;
    bool asteroidSpawned = false;
    bool gameOver = false;
    float tutorialDuration;
    float initTime;
    
    int _nextAsteroid;
    int _curAsteroidCount;
    int asteroidIndex;
    int destroyedAsteroids;
    float _nextAsteroidSpawn = 0.0;
    int _nextShipLaser = 0;
    float seed_freq;
    float win_height;
    float *sineTones;
    float _shipPointsPerSecY;
    
};

class SpaceScene : public cocos2d::CCScene
{
public:
    SpaceScene();
    virtual void runGame();
    virtual void onEnter();
    
    // The CallBack for back to the main menu scene
    virtual void MainMenuCallback(CCObject* pSender);
    
    iOSBridge::ToneGeneratorHelper* getToneGenerator(void);
    void setToneGenerator(iOSBridge::ToneGeneratorHelper *);
private:
    CCScene* mainMenuPtr;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
};

#endif // __SpaceScene_SCENE_H__
