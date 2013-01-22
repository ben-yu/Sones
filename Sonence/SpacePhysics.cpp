//
//  SpacePhysics.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-21.
//
//

#include "SpacePhysics.h"
#include "SpaceScene.h"
#include "MainMenu.h"

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
};


PhysicsSprite::PhysicsSprite()
: m_pBody(NULL)
{
    
}

void PhysicsSprite::setPhysicsBody(b2Body * body)
{
    m_pBody = body;
}

// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool PhysicsSprite::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform PhysicsSprite::nodeToParentTransform(void)
{
    b2Vec2 pos  = m_pBody->GetPosition();
    
    float x = pos.x * PTM_RATIO;
    float y = pos.y * PTM_RATIO;
    
    if ( isIgnoreAnchorPointForPosition() ) {
        x += m_tAnchorPointInPoints.x;
        y += m_tAnchorPointInPoints.y;
    }
    
    // Make matrix
    float radians = m_pBody->GetAngle();
    float c = cosf(radians);
    float s = sinf(radians);
    
    if( ! m_tAnchorPointInPoints.equals(CCPointZero) ){
        x += c*-m_tAnchorPointInPoints.x + -s*-m_tAnchorPointInPoints.y;
        y += s*-m_tAnchorPointInPoints.x + c*-m_tAnchorPointInPoints.y;
    }
    
    // Rot, Translate Matrix
    m_tTransform = CCAffineTransformMake( c,  s,
                                         -s,    c,
                                         x,    y );
    
    return m_tTransform;
}

Box2DTestLayer::Box2DTestLayer()
: m_pSpriteTexture(NULL)
{
    setTouchEnabled( true );
    setAccelerometerEnabled( true );
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    // init physics
    this->initPhysics();
    // create reset button
    this->createResetButton();
    
    //Set up sprite
#if 1
    // Use batch node. Faster
    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("Images/blocks.png", 100);
    m_pSpriteTexture = parent->getTexture();
#else
    // doesn't use batch node. Slower
    m_pSpriteTexture = CCTextureCache::sharedTextureCache()->addImage("Images/blocks.png");
    CCNode *parent = CCNode::create();
#endif
    addChild(parent, 0, kTagParentNode);
    
    
    addNewSpriteAtPosition(ccp(s.width/2, s.height/2));
    
    CCLabelTTF *label = CCLabelTTF::create("Tap screen", "Marker Felt", 32);
    addChild(label, 0);
    label->setColor(ccc3(0,0,255));
    label->setPosition(ccp( s.width/2, s.height-50));
    
    scheduleUpdate();
}

Box2DTestLayer::~Box2DTestLayer()
{
    delete world;
    world = NULL;
    
    //delete m_debugDraw;
}

void Box2DTestLayer::initPhysics()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    //b2Vec2 gravity;
    //gravity.Set(0.0f, -10.0f);
    //world = new b2World(gravity);
    
    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);
    
    world->SetContinuousPhysics(true);
    
}

void Box2DTestLayer::createResetButton()
{
    CCMenuItemImage *reset = CCMenuItemImage::create("Images/r1.png", "Images/r2.png", this, menu_selector(Box2DTestLayer::reset));
    
    CCMenu *menu = CCMenu::create(reset, NULL);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    menu->setPosition(ccp(s.width/2, 30));
    this->addChild(menu, -1);
    
}

void Box2DTestLayer::reset(CCObject* sender)
{
}

void Box2DTestLayer::draw()
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    CCLayer::draw();
    
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    
    kmGLPushMatrix();
    
    world->DrawDebugData();
    
    kmGLPopMatrix();
}

void Box2DTestLayer::addNewSpriteAtPosition(CCPoint p)
{
    CCLOG("Add sprite %0.2f x %02.f",p.x,p.y);
    CCNode* parent = getChildByTag(kTagParentNode);
    
    //We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    //just randomly picking one of the images
    int idx = (CCRANDOM_0_1() > .5 ? 0:1);
    int idy = (CCRANDOM_0_1() > .5 ? 0:1);
    PhysicsSprite *sprite = new PhysicsSprite();
    sprite->initWithTexture(m_pSpriteTexture, CCRectMake(32 * idx,32 * idy,32,32));
    sprite->autorelease();
    
    parent->addChild(sprite);
    
    sprite->setPosition( CCPointMake( p.x, p.y) );
    
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
    
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    
    sprite->setPhysicsBody(body);
}


void Box2DTestLayer::update(float dt)
{
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    int velocityIterations = 8;
    int positionIterations = 1;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
}

void Box2DTestLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    //Add a new body/atlas sprite at the touched location
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint location = touch->getLocation();
        
        addNewSpriteAtPosition( location );
    }
}

void Box2DTestLayer::startGameCallback(CCObject* pSender)
{
    
    SpaceScene* pScene =  new SpaceScene();
    // fix bug #486, without setDepthTest(false), FlipX,Y will flickers
    CCDirector::sharedDirector()->setDepthTest(false);
    CCScene* tranScene = CCTransitionZoomFlipX::create(2.0, pScene);
    
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->runGame();
        pScene->rootVC = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->rootVC;
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
}
