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

USING_NS_CC;

class CannonBallSprite : public CCSprite
{
public:
    CannonBallSprite();
    void setPhysicsBody(b2Body * body);
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    b2Body* m_pBody;    // strong ref
};

class CannonLayer : public CCLayer
{
    b2World* world;
    //GLESDebugDraw* m_debugDraw;
    
public:
    CannonLayer();
    ~CannonLayer();
    
    void onEnter();
    void initPhysics();
    void createResetButton();
    void reset(CCObject* sender);
    virtual void draw();
    
    void addNewSpriteAtPosition(CCPoint p);
    void update(float dt);
    virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);
    void startGameCallback(CCObject* pSender);
    
    float randomValueBetween( float low , float high );
    
    iOSBridge::ToneGeneratorHelper *toneGenHelp;    // Sound Generator
    iOSBridge::DataStore *dataStoreHandler;         // Data Store Handler
    
    //CREATE_NODE(Box2DTestLayer);

private:
    CCTexture2D* m_pSpriteTexture;
    float vol_meas;
    int countTimes;
    
    int e_columnCount = 5;
    int e_rowCount = 6;
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
    
    void *rootVC;
    CCMenuItemImage *pauseButton;
    
private:
    CCScene* mainMenuPtr;
    iOSBridge::ToneGeneratorHelper *toneGenHelp;
    iOSBridge::DataStore *dataStoreHandler;
    //rapidjson::Document jsonDoc;
    
};

#endif /* defined(__Sonence__Catapult__) */
