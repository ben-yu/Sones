//
//  Ship.cpp
//  SoundInvaders
//
//  Created by Benjamin Yu on 2012-09-30.
//
//

#include "Ship.h"

Ship::Ship(void)
{
}

Ship::~Ship(void)
{
}

CCRect Ship::rect()
{
    CCSize s = getTexture()->getContentSize();
    return CCRectMake(-s.width / 2, -s.height / 2, s.width, s.height);
}

Ship* Ship::ShipWithTexture(CCTexture2D* aTexture)
{
    Ship* pShip = new Ship();
    pShip->initWithTexture( aTexture );
    //pShip->autorelease();
    
    return pShip;
}

Ship* Ship::ShipWithSpriteFrameName(CCSpriteFrame* frameName)
{
    Ship* pShip = new Ship();
    pShip->spriteWithSpriteFrame(frameName);
    
    return pShip;
}

Ship* Ship::ShipWithFileName(const char *fileName)
{
    Ship* pShip = new Ship();
    pShip->create(fileName);
    
    return pShip;
}

bool Ship::initWithTexture(CCTexture2D* aTexture)
{
    if( CCSprite::initWithTexture(aTexture) )
    {
        m_state = kShipStateUngrabbed;
    }
    
    return true;
}

void Ship::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}

void Ship::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}

bool Ship::containsTouchLocation(CCTouch* touch)
{
    return rect().containsPoint(convertTouchToNodeSpaceAR(touch));
}

bool Ship::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    if (m_state != kShipStateUngrabbed) return false;
    if ( !containsTouchLocation(touch) ) return false;
    
    m_state = kShipStateGrabbed;
    return true;
}

void Ship::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    // If it weren't for the TouchDispatcher, you would need to keep a reference
    // to the touch from touchBegan and check that the current touch is the same
    // as that one.
    // Actually, it would be even more complicated since in the Cocos dispatcher
    // you get CCSets instead of 1 UITouch, so you'd need to loop through the set
    // in each touchXXX method.
    
    CCAssert(m_state == kShipStateGrabbed, L"Ship - Unexpected state!");
    
    CCPoint touchPoint = touch->getLocation();
    //setPosition( CCPointMake(getPosition().x,touchPoint.y) );
    
}

CCObject* Ship::copyWithZone(CCZone *pZone)
{
    this->retain();
    return this;
}

void Ship::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    CCAssert(m_state == kShipStateGrabbed, L"Ship - Unexpected state!");
    
    m_state = kShipStateUngrabbed;
}

void Ship::touchDelegateRetain()
{
    this->retain();
}

void Ship::touchDelegateRelease()
{
    this->release();
}