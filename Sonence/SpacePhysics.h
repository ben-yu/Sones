//
//  SpacePhysics.h
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-21.
//
//

#ifndef __Sonence__SpacePhysics__
#define __Sonence__SpacePhysics__

#include "cocos2d.h"
#include "Box2D.h"

USING_NS_CC;

class PhysicsSprite : public CCSprite
{
public:
    PhysicsSprite();
    void setPhysicsBody(b2Body * body);
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    b2Body* m_pBody;    // strong ref
};

class Box2DTestLayer : public CCLayer
{
    CCTexture2D* m_pSpriteTexture;    // weak ref
    b2World* world;
    //    GLESDebugDraw* m_debugDraw;
    
public:
    Box2DTestLayer();
    ~Box2DTestLayer();
    
    void initPhysics();
    void createResetButton();
    void reset(CCObject* sender);
    virtual void draw();
    
    void addNewSpriteAtPosition(CCPoint p);
    void update(float dt);
    virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);
    void startGameCallback(CCObject* pSender);

    
    //CREATE_NODE(Box2DTestLayer);
} ;

#endif /* defined(__Sonence__SpacePhysics__) */
