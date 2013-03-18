#ifndef __SpacePhysics_SCENE_H__
#define __SpacePhysics_SCENE_H__

#include "cocos2d.h"
#include "CCParallaxNodeExtras.h"
#include "ToneGeneratorHelper.h"
#include "DataStore.h"
#include "document.h"
#include "rapidjson.h"
#include "Box2D.h"
#include "GLES-Render.h"
#include "MLSearch.h"


USING_NS_CC;

class SpacePhysicsLayer;

const int32 k_maxContactPoints = 2048;

struct ContactPoint
{
    b2Fixture* fixtureA;
    b2Fixture* fixtureB;
    b2Vec2 normal;
    b2Vec2 position;
    b2PointState state;
};

class EnemySprite : public CCSprite
{
public:
    EnemySprite();
    ~EnemySprite();
    void setPhysicsBody(b2Body * body);
    b2Body* getPhysicsBody();
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
    static CCSprite * create(const char *pszFileName);
private:
    b2Body* m_pBody;    // strong ref
};


class DestructionListener : public b2DestructionListener
{
public:
    void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
    void SayGoodbye(b2Joint* joint);
    
    SpacePhysicsLayer* test;
};

class SpacePhysicsLayer : public CCLayer, b2ContactListener
{
    
    b2World* world;
    GLESDebugDraw* m_debugDraw;
    b2Body* m_bomb;
    ContactPoint m_points[k_maxContactPoints];
    int32 m_pointCount;
    

public:
    DestructionListener m_destructionListener;
    
    const float TWOPI = 3.1415926535 * 2.0;
    
    SpacePhysicsLayer(void);
    ~SpacePhysicsLayer(void);
    
    static float sysVer;
    
    static bool accelEnable;
    
    virtual void onEnter();
    void initPhysics();
    
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
    void spawnBeam();
    void deSpawnBeam();
    void moveEnemy();
    void spawnHomingMissle();
    void spawnLowFreqEnemy();
    void spawnHighFreqEnemy();
    void spawnAsteroid();
    void spawnAIEnemy();
    void spawnAttackPatterns();
    void spawnTones(int freqIndex, int earIndex);
    void nextLevel();
    
    void spawn1_L(); void spawn2_L(); void spawn3_L(); void spawn4_L(); void spawn5_L();
    void spawn1_R(); void spawn2_R(); void spawn3_R(); void spawn4_R(); void spawn5_R();
    
    // Let derived tests know that a joint was destroyed.
    virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }
    
    // Callbacks for derived classes.
    virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
    virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse)
    {
        B2_NOT_USED(contact);
        B2_NOT_USED(impulse);
    }
    
    friend class DestructionListener;
    friend class BoundaryListener;
    friend class ContactListener;
    
    CREATE_FUNC(SpacePhysicsLayer);
    
private:
    // Game Resources - Sprites
    cocos2d::CCSpriteBatchNode * _batchNode; // Background stuff
    cocos2d::CCSpriteBatchNode * _missleNode;

    CCParallaxNodeExtras *_backgroundNode;
    CCSprite *_spacedust1;
    CCSprite *_spacedust2;
    CCSprite *_planetsunrise;
    CCSprite *_galaxy;
    CCSprite *_spacialanomaly;
    CCSprite *_spacialanomaly2;
    CCSprite *txtWindow;            // UI Stuff
    CCSprite *healthBar;
    CCProgressTimer *hpBar;
    CCProgressTimer *bossBar;
    CCTexture2D* m_pSpriteTexture;
    CCTexture2D* missleTexture;
    
    
    // Text Labels
    CCLabelTTF *distLabel;
    CCLabelTTF *scoreLabel;
    CCLabelTTF *tutorialText;
    CCLabelTTF *tutorialText2;
    CCLabelTTF *tutorialText3;
    CCLabelTTF *tutorialText4;
    CCLabelTTF *tutorialText5;
    CCMenu* tutorialSkipButton;
    
    // NPC Sprites
    EnemySprite *playerShip;
    CCSprite *boss;
    
    CCArray *active_lanes;
    CCArray  *coins;
    CCArray *_spawnQueue;
    CCArray  *homing_missiles;
    CCArray *_AIEnemies;
    CCArray * _shipLasers;
    CCParticleSystem*    m_emitter;
    
    CCSpriteFrameCache *frameCache;
    CCAnimationCache *animCache;
        
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
    int bossHealth = 100;
    
    float *alphaTargets;
    float *timeTargets;
    
    // Game State Variables
    bool playedTutorial = false;
    bool enemySpawned = false;
    bool asteroidSpawned = false;
    bool gameOver = false;
    bool spawnRegularEnemies;
    bool touchTimerFlag = false;
    bool hitFlag = false;
    
    int score = 0;
    int distance = 0;
    int level = 2;
    int touchAttempts = 0;
    
    float radarRadius;
    float enemyVelocity;
    float seed_freq;
    float baseVolumes[10];
    float volDiffs[10];
    float tutorialDuration;
    float initTime;
    float touchTime;
    float finalTime;
    float laneScale;
    
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

class SpacePhysicsScene : public cocos2d::CCScene
{
public:
    int gameMode;
    
    SpacePhysicsScene();
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
    
private:
    CCScene* mainMenuPtr;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
    rapidjson::Document jsonDoc;
    
};

class PauseSceneLayer2 : public CCLayer
{
public:
    PauseSceneLayer2();
    ~PauseSceneLayer2();
    
    virtual void resumeCallback(CCObject* pSender);
    virtual void MainMenuCallback(CCObject* pSender);
    void musicCallback(CCObject *sender);
    void fxCallback(CCObject *sender);
    
};

#endif // __SpacePhysics_SCENE_H__
