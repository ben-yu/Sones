#ifndef __SpaceScene_SCENE_H__
#define __SpaceScene_SCENE_H__

#include "cocos2d.h"
#include "CCParallaxNodeExtras.h"
#include "ToneGeneratorHelper.h"
#include "DataStore.h"
#include "document.h"
#include "rapidjson.h"
#include <curl/curl.h>


USING_NS_CC;

class SpaceSceneLayer : public CCLayer
{
public:
    
    const float TWOPI = 3.1415926535 * 2.0;
    
    SpaceSceneLayer(void);
    ~SpaceSceneLayer(void);
    
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
    void tutorialSpawn(int freqIndex, int earIndex);
    void spawnEnemy();
    void spawnRandomEnemy();
    void moveEnemy();
    void spawnEnemyAtLoc(int freqIndex, int earIndex);
    void spawnLowFreqEnemy();
    void spawnHighFreqEnemy();
    void spawnAsteroid();
    void nextLevel();
    
    void spawn1_L(); void spawn2_L(); void spawn3_L(); void spawn4_L(); void spawn5_L();
    void spawn1_R(); void spawn2_R(); void spawn3_R(); void spawn4_R(); void spawn5_R();
    
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
    CCArray *_AIEnemies;
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
    
    iOSBridge::ToneGeneratorHelper *toneGenHelp;    // Sound Generator
    iOSBridge::DataStore *dataStoreHandler;         // Data Store Handler
    rapidjson::Document jsonDoc;
    void *rootVCPtr;
    
    float winWidth;
    float winHeight;
    
    // Player Data
    CCPoint playerPos;
    int _health = 100;
    
    float *alphaTargets;
    float *timeTargets;
    
    // Game State Variables
    bool playedTutorial = false;
    bool enemySpawned = false;
    bool asteroidSpawned = false;
    bool gameOver = false;
    bool spawnRegularEnemies;
    int score = 0;
    int distance = 0;
    int level = 1;
    float radarRadius;
    float enemyVelocity;
    float seed_freq;
    float tutorialDuration;
    float initTime;
    
    int _nextAsteroid;
    int _curAsteroidCount;
    int asteroidIndex;
    int enemyIndex = 0;
    int earIndex = 0;
    int destroyedAsteroids;
    float _nextAsteroidSpawn = 0.0;
    int _nextShipLaser = 0;
    float win_height;
    float *sineTones;
    float _shipPointsPerSecY;
    
};

class PauseSceneLayer : public CCLayer
{
public:
    PauseSceneLayer();
    ~PauseSceneLayer();
    
    virtual void resumeCallback(CCObject* pSender);
    virtual void MainMenuCallback(CCObject* pSender);
        
};

class SpaceScene : public cocos2d::CCScene
{
public:
    int gameMode;
    
    SpaceScene();
    virtual void runGame();
    
    // The CallBack for back to the main menu scene
    virtual void MainMenuCallback(CCObject* pSender);
    virtual void pauseMenuCallback(CCObject* pSender);
    
    iOSBridge::ToneGeneratorHelper* getToneGenerator(void);
    void setToneGenerator(iOSBridge::ToneGeneratorHelper *);
    
    iOSBridge::DataStore* getDataStore(void);
    void setDataStore(iOSBridge::DataStore *);
    int sendData();
    
    void RecursivelyPauseAllChildren(CCNode* node);
    void RecursivelyResumeAllChildren(CCNode* node);
    
    void *rootVC;
    CCMenuItemImage *pauseButton;
        
private:
    CCScene* mainMenuPtr;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
    rapidjson::Document jsonDoc;

};

struct WriteThis {
    const char *readptr;
    long sizeleft;
};

#endif // __SpaceScene_SCENE_H__
