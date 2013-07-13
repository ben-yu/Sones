//
//  Catapult.h
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-28.
//
//

#ifndef __Sonence__Catapult__
#define __Sonence__Catapult__

#include "cocos2d.h"
#include "Box2D.h"
#include "ToneGeneratorHelper.h"
#include "DataStore.h"
#include "CCLayerPanZoom.h"
#include "GLES-Render.h"

USING_NS_CC;

class CannonBallSprite : public CCSprite
{
public:
    CannonBallSprite();
    void setPhysicsBody(b2Body * body);
    b2Body* getPhysicsBody();
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    b2Body* m_pBody;    // strong ref
};

class CannonLayer : public CCLayerPanZoom
{
    b2World* world;
    GLESDebugDraw* m_debugDraw;
    
public:
    CannonLayer();
    ~CannonLayer();
    
    void onEnter();
    void initPhysics();
    void createResetButton();
    void reset(CCObject* sender);
    virtual void draw();
    
    void addNewSpriteAtPosition(CCPoint p);
    void spawnTarget();
    void update(float dt);
    void startGameCallback(CCObject* pSender);
    
    float randomValueBetween( float low , float high );
    
    iOSBridge::ToneGeneratorHelper *toneGenHelp;    // Sound Generator
    iOSBridge::DataStore *dataStoreHandler;         // Data Store Handler
    
    void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    
    CCTexture2D* m_pSpriteTexture;
    CCSprite* cannon;
    CCSprite *background;
    CCSprite *target;
    CannonBallSprite* cannonBall;
    float vol_meas;
    float curAngle = 0.0f;
    int countTimes;
    bool rotatingCannon = false;
    
    
    unsigned int m_gid;
    unsigned int m_gid2;
    
    int e_columnCount = 8;
    int e_rowCount = 10;
    
    //CREATE_NODE(Box2DTestLayer);

private:

} ;

class CannonScene : public cocos2d::CCScene
{
public:
    int gameMode;
    
    CannonScene();
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
    
    void shootCannon();
    
    void *rootVC;
    CCMenuItemImage *pauseButton;
    CCLabelTTF *powerLabel;
    CCScene* mainMenuPtr;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
    
private:

    //rapidjson::Document jsonDoc;
    
};

class PauseSceneLayer4 : public CCLayer
{
public:
    PauseSceneLayer4();
    ~PauseSceneLayer4();
    
    virtual void resumeCallback(CCObject* pSender);
    virtual void MainMenuCallback(CCObject* pSender);
    void musicCallback(CCObject *sender);
    void fxCallback(CCObject *sender);
    
};

#endif /* defined(__Sonence__Catapult__) */
