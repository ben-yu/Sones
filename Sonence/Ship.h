//
//  Ship.h
//  SoundInvaders
//
//  Created by Benjamin Yu on 2012-09-30.
//
//

#ifndef __SoundInvaders__Ship__
#define __SoundInvaders__Ship__

#include "cocos2d.h"

USING_NS_CC;

typedef enum tagShipState
{
    kShipStateGrabbed,
    kShipStateUngrabbed
} ShipState;

class Ship : public CCSprite, public CCTargetedTouchDelegate
{
    ShipState        m_state;
    
public:
    Ship(void);
    virtual ~Ship(void);
    
    CCRect rect();
    CCPoint currentPosition;
    bool initWithTexture(CCTexture2D* aTexture);
    virtual void onEnter();
    virtual void onExit();
    bool containsTouchLocation(CCTouch* touch);
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    virtual CCObject* copyWithZone(CCZone *pZone);
    
    virtual void touchDelegateRetain();
    virtual void touchDelegateRelease();
    
    static Ship* ShipWithTexture(CCTexture2D* aTexture);
    static Ship* ShipWithSpriteFrameName(CCSpriteFrame* frameName);
    static Ship* ShipWithFileName(const char* fileName);
};

#endif
