//
//  Catapult.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-28.
//
//

#include "Catapult.h"
#include "SpaceScene.h"
#include "MainMenu.h"
#include <math.h>

#define PTM_RATIO 20
#define PI 3.14159265

enum {
    kTagParentNode = 1,
    kTagTileMap = 2,
};


CannonBallSprite::CannonBallSprite()
: m_pBody(NULL)
{
    
}

void CannonBallSprite::setPhysicsBody(b2Body * body)
{
    m_pBody = body;
}


b2Body* CannonBallSprite::getPhysicsBody()
{
    return this->m_pBody;
}

// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool CannonBallSprite::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform CannonBallSprite::nodeToParentTransform(void)
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

CannonLayer::CannonLayer()
: m_pSpriteTexture(NULL)
{
    CCLayerPanZoom::init();
    
    // init physics
    this->initPhysics();
    // create reset button
    this->createResetButton();
    
    
    setTouchEnabled(true);
    setAccelerometerEnabled(true);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    //Set up sprite
    // Use batch node. Faster
    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("ironball.png", 100);
    m_pSpriteTexture = parent->getTexture();

    addChild(parent, 0, kTagParentNode);
    
    /*
    for (int32 j = 0; j < e_columnCount; ++j)
    {        
        for (int i = 0; i < e_rowCount; ++i)
        {
            addNewSpriteAtPosition(ccp((j*s.width/10) + s.width, (3*s.height/10 + (16.0f * i))));
        }
    }
     */
    
    target = CCSprite::create("target.png");
    addChild(target);
    
    cannon = CCSprite::create("cannon2.png");
    addChild(cannon,0);
    cannon->setPosition(ccp(-0.5*s.width, s.height*0.25));
    
    // Define the dynamic body.
    //Set up a 1m squared box in the physics world
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(cannon->getPosition().x/PTM_RATIO,cannon->getPosition().y/PTM_RATIO);
    
    b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2CircleShape circle;
    circle.m_radius = 0.4f;
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 2.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    
    cannonBall = new CannonBallSprite();
    cannonBall->initWithTexture(m_pSpriteTexture);
    cannonBall->autorelease();
    cannonBall->setPhysicsBody(body);
    cannonBall->setVisible(true);
    
    parent->addChild(cannonBall);
    //this->runAction(CCFollow::create(cannonBall, CCRectMake(0, 0, s.width*2 + 100, s.height)));
    
    CCLabelTTF *label = CCLabelTTF::create("Press the Button when you can't hear the tone", "Ubuntu", 20);
    addChild(label, 0);
    label->setColor(ccc3(0,0,255));
    label->setPosition(ccp( s.width/2, s.height-50));
    
    scheduleUpdate();
}

CannonLayer::~CannonLayer()
{
    delete world;
    world = NULL;
    
    //delete m_debugDraw;
}

void CannonLayer::onEnter()
{
    //CCLayerPanZoom::onEnter();
    CCLayer::onEnter();
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCNode* parent = getChildByTag(kTagParentNode);
    
    background = CCSprite::create("sky1.png");
    addChild(background,-1000);
    background->setScale(2.0);
    
    m_gid = 0;
    
    CCTMXTiledMap* map = CCTMXTiledMap::create("orthogonal-test2.tmx");
    addChild(map, -10, kTagTileMap);
    
    CCSize mapsize = map->getContentSize();
    ////----UXLOG("ContentSize: %f, %f", s.width,s.height);
    
    CCTMXLayer* layer = map->layerNamed("Layer 0");
    layer->getTexture()->setAntiAliasTexParameters();
    
    map->setScale(0.75);
    map->setPosition(-s.width/2,-mapsize.height*0.175);
    
    float32 a = 0.6f;
    b2BodyDef bd;
    bd.position.y = -a;
    b2Body* ground = world->CreateBody(&bd);
    /*
    int32 N = 34;
    int32 M = 13;
    b2Vec2 position;
    position.y = 0.0f;
    for (int32 j = 0; j < M; ++j)
    {
        position.x = cannon->getPosition().x/PTM_RATIO;
        for (int32 i = 0; i < N; ++i)
        {
            b2PolygonShape shape;
            shape.SetAsBox(a, a, position, 0.0f);
            ground->CreateFixture(&shape, 0.0f);
            position.x += 2.0f * a;
        }
        position.y -= 2.0f * a;
    }
    
    N = 6;
    M = 20;
    
    position.y = 6.25f;
    for (int32 j = 0; j < M; ++j)
    {
        position.x = s.width*0.78/PTM_RATIO;
        for (int32 i = 0; i < N; ++i)
        {
            b2PolygonShape shape;
            shape.SetAsBox(a, a, position, 0.0f);
            ground->CreateFixture(&shape, 0.0f);
            position.x += 2.0f * a;
        }
        position.y -= 2.0f * a;
    }
    
    N = 12;
    M = 13;
    
    position.y = 0.0f;
    for (int32 j = 0; j < M; ++j)
    {
        position.x = s.width*1.25/PTM_RATIO;
        for (int32 i = 0; i < N; ++i)
        {
            b2PolygonShape shape;
            shape.SetAsBox(a, a, position, 0.0f);
            ground->CreateFixture(&shape, 0.0f);
            position.x += 2.0f * a;
        }
        position.y -= 2.0f * a;
    }
    */
    
    
    b2Vec2 position = b2Vec2(-s.width/2/PTM_RATIO,0);

    for (int i=63; i >= 0; i--)
    {
        for (int j=63; j >= 0; j--)
        {
            if (layer->tileGIDAt(ccp(i,j)) != 0){
                b2PolygonShape shape;
                shape.SetAsBox(a, a, position, 0.0f);
                ground->CreateFixture(&shape, 0.0f);
            }
            position.y = (s.height*1.55/PTM_RATIO) + (-2.0f * a * j);
        }
        position.x = (-s.width/2/PTM_RATIO) + (2.0f * a * i);
    }
    
    
    b2Body* b1;
    {
        b2EdgeShape shape;
        shape.Set(b2Vec2(-1, 0.0f), b2Vec2(1.0f, 0.0f));
        
        b2BodyDef bd;
        b1 = world->CreateBody(&bd);
        b1->CreateFixture(&shape, 0.0f);
    }
    
    {
        b2PolygonShape shape;
        shape.SetAsBox(6.0f, 0.25f);
        
        b2BodyDef bd;
        bd.position.Set(-1.5f, 10.0f);
        b2Body* ground = world->CreateBody(&bd);
        ground->CreateFixture(&shape, 0.0f);
    }
    
    {
        b2PolygonShape shape;
        shape.SetAsBox(0.1f, 1.0f);
        
        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = 20.0f;
        fd.friction = 0.1f;
        
        for (int i = 0; i < 10; ++i)
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(-6.0f + 1.0f * i, 11.25f);
            b2Body* body = world->CreateBody(&bd);
            body->CreateFixture(&fd);
        }
    }
    /*
    {
        b2PolygonShape shape;
        shape.SetAsBox(7.0f, 0.25f, b2Vec2_zero, 0.3f);
        
        b2BodyDef bd;
        bd.position.Set(1.0f, 6.0f);
        b2Body* ground = world->CreateBody(&bd);
        ground->CreateFixture(&shape, 0.0f);
    }
    
    b2Body* b2;
    {
        b2PolygonShape shape;
        shape.SetAsBox(0.25f, 1.5f);
        
        b2BodyDef bd;
        bd.position.Set(-7.0f, 4.0f);
        b2 = world->CreateBody(&bd);
        b2->CreateFixture(&shape, 0.0f);
    }
    
    b2Body* b3;
    {
        b2PolygonShape shape;
        shape.SetAsBox(6.0f, 0.125f);
        
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(-0.9f, 1.0f);
        bd.angle = -0.15f;
        
        b3 = world->CreateBody(&bd);
        b3->CreateFixture(&shape, 10.0f);
    }
    
    b2RevoluteJointDef jd;
    b2Vec2 anchor;
    
    anchor.Set(-2.0f, 1.0f);
    jd.Initialize(b1, b3, anchor);
    jd.collideConnected = true;
    world->CreateJoint(&jd);
    /*
    b2Body* b4;
    {
        b2PolygonShape shape;
        shape.SetAsBox(0.25f, 0.25f);
        
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(-10.0f, 15.0f);
        b4 = world->CreateBody(&bd);
        b4->CreateFixture(&shape, 10.0f);
    }
    
    
    anchor.Set(-7.0f, 15.0f);
    jd.Initialize(b2, b4, anchor);
    world->CreateJoint(&jd);
    */
    /*
    b2Body* b5;
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(6.5f, 3.0f);
        b5 = world->CreateBody(&bd);
        
        b2PolygonShape shape;
        b2FixtureDef fd;
        
        fd.shape = &shape;
        fd.density = 10.0f;
        fd.friction = 0.1f;
        
        shape.SetAsBox(1.0f, 0.1f, b2Vec2(0.0f, -0.9f), 0.0f);
        b5->CreateFixture(&fd);
        
        shape.SetAsBox(0.1f, 1.0f, b2Vec2(-0.9f, 0.0f), 0.0f);
        b5->CreateFixture(&fd);
        
        shape.SetAsBox(0.1f, 1.0f, b2Vec2(0.9f, 0.0f), 0.0f);
        b5->CreateFixture(&fd);
    }
    
    anchor.Set(6.0f, 2.0f);
    jd.Initialize(b1, b5, anchor);
    world->CreateJoint(&jd);
    
    b2Body* b6;
    {
        b2PolygonShape shape;
        shape.SetAsBox(1.0f, 0.1f);
        
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(6.5f, 4.1f);
        b6 = world->CreateBody(&bd);
        b6->CreateFixture(&shape, 30.0f);
    }
    
    anchor.Set(7.5f, 4.0f);
    jd.Initialize(b5, b6, anchor);
    world->CreateJoint(&jd);
    
    b2Body* b7;
    {
        b2PolygonShape shape;
        shape.SetAsBox(0.1f, 1.0f);
        
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(7.4f, 1.0f);
        
        b7 = world->CreateBody(&bd);
        b7->CreateFixture(&shape, 10.0f);
    }
    */
    /*
    b2DistanceJointDef djd;
    djd.bodyA = b3;
    djd.bodyB = b7;
    djd.localAnchorA.Set(6.0f, 0.0f);
    djd.localAnchorB.Set(0.0f, -1.0f);
    b2Vec2 d = djd.bodyB->GetWorldPoint(djd.localAnchorB) - djd.bodyA->GetWorldPoint(djd.localAnchorA);
    djd.length = d.Length();
    world->CreateJoint(&djd);
    
    {
        float32 radius = 0.2f;
        
        b2CircleShape shape;
        shape.m_radius = radius;
        
        for (int32 i = 0; i < 4; ++i)
        {
            b2BodyDef bd;
            bd.type = b2_dynamicBody;
            bd.position.Set(5.9f + 2.0f * radius * i, 2.4f);
            b2Body* body = world->CreateBody(&bd);
            body->CreateFixture(&shape, 10.0f);
        }
    }
    */
    spawnTarget();
}

void CannonLayer::initPhysics()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    world = new b2World(gravity);
    
    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);
    
    world->SetContinuousPhysics(true);
     m_debugDraw = new GLESDebugDraw( PTM_RATIO );
     world->SetDebugDraw(m_debugDraw);
    
    uint32 flags = 0;
    // flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    m_debugDraw->SetFlags(flags);
    
    
    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 2); // bottom-left corner
    
    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    
    // Define the ground box shape.
    b2EdgeShape groundBox;
    
    // bottom
    
    //groundBox.Set(b2Vec2(0,0), b2Vec2(s.width*3/PTM_RATIO,0));
    //groundBody->CreateFixture(&groundBox,0);
    
    // top
    //groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
    //groundBody->CreateFixture(&groundBox,0);
    
    // left
    //groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0,0));
    //groundBody->CreateFixture(&groundBox,0);
    
    // right
    //groundBox.Set(b2Vec2(s.width*3/PTM_RATIO,s.height/PTM_RATIO), b2Vec2(s.width*3/PTM_RATIO,0));
    //groundBody->CreateFixture(&groundBox,0);
    
    this->setPanBoundsRect(CCRectMake(0, 0, s.width/PTM_RATIO, s.height/PTM_RATIO));
}

void CannonLayer::createResetButton()
{

}

void CannonLayer::reset(CCObject* sender)
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    CCNode* parent = getChildByTag(kTagParentNode);

    vol_meas = toneGenHelp->getAmplitude();
    countTimes++;
    if (countTimes == 5)
    {
        this->toneGenHelp->playOscillatingTone(randomValueBetween(20.0,2000.0), 5.0, 2);
    }
    
    b2Body* body = cannonBall->getPhysicsBody();
    b2Vec2 vel = b2Vec2(sin(curAngle*PI/180),cos(curAngle*PI/180));
    vel =  50*vel;
    body->SetTransform(b2Vec2(cannon->getPosition().x/PTM_RATIO,cannon->getPosition().y/PTM_RATIO), 0.0);
    body->SetLinearVelocity(vel);
    cannonBall->setVisible(true);
    
}

void CannonLayer::draw()
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

void CannonLayer::addNewSpriteAtPosition(CCPoint p)
{
    CCLOG("Add sprite %0.2f x %02.f",p.x,p.y);
    CCNode* parent = getChildByTag(kTagParentNode);
    
    //We have a 64x64 sprite sheet with 4 different 32x32 images.  The following code is
    //just randomly picking one of the images
    CannonBallSprite *sprite = new CannonBallSprite();
    sprite->initWithTexture(m_pSpriteTexture);
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

void CannonLayer::spawnTarget()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    float height = randomValueBetween(s.height/4, s.height);
    target->setPosition(ccp(s.width/2, height));
}

void CannonLayer::update(float dt)
{
    
    CCLayerPanZoom::update(dt);
    
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    int velocityIterations = 8;
    int positionIterations = 1;
    b2Body* body = cannonBall->getPhysicsBody();
    cannonBall->setPosition(ccp(body->GetPosition().x*PTM_RATIO,body->GetPosition().y*PTM_RATIO));
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
    
    if (target->boundingBox().containsPoint(cannonBall->getPosition())) {
        CCLog("HIT!");
        spawnTarget();
    }
    
}

void CannonLayer::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    
    CCPoint mean_touch;
    CCSetIterator iter = touches->begin();
    for (; iter != touches->end(); iter++) // Iterate through touch list, only get first touch?
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        CCPoint location = pTouch->getLocation();
        
        mean_touch = location; // Take mean of multi-touch later if needed
        
    }
    
    CCSize s = cannon->getContentSize();
    CCRect bound_rect = CCRectMake(0,0,s.width, s.height);
    
    if (bound_rect.containsPoint(cannon->convertToNodeSpace(mean_touch)))
    {
        CCPoint pos = cannon->convertToWorldSpace(cannon->getPosition());
        //pos.y = -pos.y;
        
        //Calulate angle between ship and touch position
        curAngle = atan((mean_touch.y - pos.y)/(mean_touch.x + pos.x)) * (180/PI);
        
        //pos.setPoint(pos.x - 15, pos.y);
        cannon->setRotation(curAngle);
        rotatingCannon = true;
        CCLog("Anglez: %f, %f, %f, %f",mean_touch.y,mean_touch.x,pos.y,pos.x);
    } else {
        CCLayerPanZoom::ccTouchesBegan(touches, event);
    }

}

void CannonLayer::ccTouchesMoved(CCSet* touches, CCEvent* event)
{

    CCPoint mean_touch;
    CCSetIterator iter = touches->begin();
    for (; iter != touches->end(); iter++) // Iterate through touch list, only get first touch?
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        CCPoint location = pTouch->getLocation();
        
        mean_touch = location; // Take mean of multi-touch later if needed
        
    }
    CCSize s = cannon->getContentSize();
    CCRect bound_rect = CCRectMake(0,0,s.width, s.height);
    
    if (rotatingCannon)
    {
        CCPoint pos = cannon->convertToWorldSpace(CCPointZero);
        //pos.y = -pos.y;
        //pos = ccp(pos.x,CCDirector::sharedDirector()->getWinSize().height - pos.y);
        
        //Calulate angle between ship and touch position
        curAngle = atan2((pos.y - mean_touch.y),(mean_touch.x - pos.x)) * (180.0/PI) + 90.0;
        
        //pos.setPoint(pos.x - 15, pos.y);
        cannon->setRotation(curAngle);
        CCLog("Angle: %f",curAngle);
    } else {
        CCLayerPanZoom::ccTouchesMoved(touches, event);
    }
}

void CannonLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    CCLayerPanZoom::ccTouchesEnded(touches, event);
    
    //Add a new body/atlas sprite at the touched location
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint location = touch->getLocation();
        
        //addNewSpriteAtPosition( location );
    }
    rotatingCannon = false;
}

void CannonLayer::startGameCallback(CCObject* pSender)
{
    
    CannonScene* pScene =  new CannonScene();
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

float CannonLayer::randomValueBetween( float low , float high ) {
    return (((float) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

iOSBridge::ToneGeneratorHelper* CannonScene::getToneGenerator()
{
    return this->toneGenHelp;
}

void CannonScene::setToneGenerator(iOSBridge::ToneGeneratorHelper * gen)
{
    this->toneGenHelp = gen;
}

iOSBridge::DataStore* CannonScene::getDataStore()
{
    return this->dataStoreHandler;
}

void CannonScene::setDataStore(iOSBridge::DataStore * gen)
{
    this->dataStoreHandler = gen;
}

void CannonScene::runGame()
{
    CCLayer* pLayer = new CannonLayer();
    
    switch (gameMode) {
        case 1:
            ((CannonLayer *) pLayer)->setAccelerometerEnabled(true);
            break;
            
        default:
            break;
    }
    
    addChild(pLayer,0,0);
    pLayer->autorelease();
    
}

CannonScene::CannonScene()
{
    CCScene::init();
    
    pauseButton = CCMenuItemImage::create("glyphicons_174_pause.png", "glyphicons_174_pause.png", this, menu_selector(CannonScene::pauseMenuCallback) );
    pauseButton->setScale(2.0);
    CCMenu *pMenu =CCMenu::create(pauseButton, NULL);
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    pMenu->setPosition(CCPointZero);
    pauseButton->setPosition( ccp(s.width*0.95,s.height*0.95) );
    
    CCMenuItemImage *reset = CCMenuItemImage::create("launchBtn.png", "launchBtnWhenPressed.png", this, menu_selector(CannonScene::shootCannon));
    
    powerLabel = CCLabelTTF::create("0", "PressStart2P-Regular", 12.0);
    powerLabel->setPosition(ccp(s.width/10,s.height*0.9));
    
    reset->setScale(0.1);
    
    CCMenu *menu = CCMenu::create(reset, NULL);
    
    menu->setPosition(ccp(s.width/2, 30));
    addChild(menu, 5);
    
    addChild(pMenu, 1);
    addChild(powerLabel,5);
}

void CannonScene::MainMenuCallback(CCObject* pSender)
{    
    CCScene* pScene = MainMenu::create();
    CCLayer* pLayer1 = new MainMenuLayer();
    CCLayer* pLayer2 = new OptionsLayer();
    CCLayer* pLayer3 = new AudiogramSceneLayer();
    CCLayer* pLayer4 = new CreditsLayer();
    CCLayer* pLayer5 = new LevelLayer();
    
    
    CCLayerMultiplex* layer = CCLayerMultiplex::create(pLayer1, pLayer2, pLayer3, pLayer4, pLayer5, NULL);
    pScene->addChild(layer, 0);
    
    pLayer1->release();
    pLayer2->release();
    pLayer3->release();
    pLayer4->release();
    pLayer5->release();
    
    ((MainMenu *) pScene)->setToneGenerator(toneGenHelp);
    ((MainMenu *) pScene)->setDataStore(dataStoreHandler);
    CCDirector::sharedDirector()->replaceScene(pScene);
}

void CannonScene::pauseMenuCallback(CCObject* pSender)
{
    CCLayer* pauseLayer = new PauseSceneLayer4();
    addChild(pauseLayer);
    ((CCMenuItem *)pSender)->setEnabled(false);
    RecursivelyPauseAllChildren((CCNode *)this->getChildren()->objectAtIndex(0));
    ((SpaceScene *)(this->getChildren()->objectAtIndex(0)))->pauseSchedulerAndActions();
    this->getToneGenerator()->disableTones();
}

void CannonScene::RecursivelyPauseAllChildren( CCNode * node ) {
    node->pauseSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyPauseAllChildren(n);
    }
}

void CannonScene::RecursivelyResumeAllChildren( CCNode * node ) {
    node->resumeSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyResumeAllChildren(n);
    }
}

void CannonScene::shootCannon()
{

    CannonLayer* layer = ((CannonLayer *)(this->getChildByTag(0)));
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    layer->vol_meas = toneGenHelp->getAmplitude()/98.0;
    iOSBridge::stringstream tempString;
    tempString.str("");
    tempString<<layer->vol_meas;
    powerLabel->setString(tempString.str().c_str());
    
    layer->countTimes++;
    if (layer->countTimes == 5)
    {
        this->toneGenHelp->playOscillatingTone(layer->randomValueBetween(20.0,2000.0), 5.0, 2);
    }
    
    b2Body* body = layer->cannonBall->getPhysicsBody();
    b2Vec2 vel = b2Vec2(sin(layer->curAngle*PI/180),cos(layer->curAngle*PI/180));
    vel =  50*layer->vol_meas*vel;
    body->SetTransform(b2Vec2(layer->cannon->getPosition().x/PTM_RATIO,layer->cannon->getPosition().y/PTM_RATIO), 0.0);
    body->SetLinearVelocity(vel);
    layer->cannonBall->setVisible(true);
}

PauseSceneLayer4::PauseSceneLayer4()
{
    // Label Item (CCLabelBMFont)
    CCMenuItemFont* item1 = CCMenuItemFont::create("Resume", this, menu_selector(PauseSceneLayer4::resumeCallback));
    
    
    CCMenuItemFont::setFontName("PressStart2P-Regular");
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont*title2 = CCMenuItemFont::create("Effects");
    title2->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle* item2 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(PauseSceneLayer4::fxCallback),
                                                                 CCMenuItemFont::create( "On" ),
                                                                 CCMenuItemFont::create( "Off"),
                                                                 NULL );
    
    CCMenuItemFont::setFontName( "PressStart2P-Regular" );
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont* title3 = CCMenuItemFont::create( "Music" );
    title3->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle *item3 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(PauseSceneLayer4::musicCallback),
                                                                 CCMenuItemFont::create( "On" ),
                                                                 CCMenuItemFont::create( "Off"),
                                                                 NULL );
    
    
    CCMenuItemFont* item4 = CCMenuItemFont::create("Quit", this, menu_selector(PauseSceneLayer4::MainMenuCallback));
    CCMenu* menu = CCMenu::create( item1, title2, item2, title3, item3, item4, NULL);
    menu->alignItemsInColumns(1,2,2,1,NULL);
    
    // elastic effect
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    int i=0;
    CCNode* child;
    CCArray * pArray = menu->getChildren();
    CCObject* pObject = NULL;
    CCARRAY_FOREACH(pArray, pObject)
    {
        if(pObject == NULL)
            break;
        
        child = (CCNode*)pObject;
        
        CCPoint dstPoint = child->getPosition();
        int offset = (int) (s.width/2 + 50);
        
        child->setPosition( CCPointMake( dstPoint.x + offset, dstPoint.y) );
        child->runAction(CCEaseElasticOut::create(CCMoveBy::create(2, CCPointMake(dstPoint.x - offset,0)), 0.35f));
        i++;
    }
    addChild(menu);
    menu->setPosition(ccp(s.width/2, s.height/2));
}


PauseSceneLayer4::~PauseSceneLayer4()
{
    
}

void PauseSceneLayer4::resumeCallback(CCObject* pSender)
{
    ((CannonScene *)(this->getParent()))->RecursivelyResumeAllChildren(this->getParent());
    ((CannonScene *)(this->getParent()))->getToneGenerator()->enableTones();
    ((CannonScene *)(this->getParent()))->pauseButton->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}

void PauseSceneLayer4::MainMenuCallback(CCObject* pSender)
{
    CannonScene *parent = (CannonScene *)this->getParent();
        
    CCScene* pScene = MainMenu::create();
    CCLayer* pLayer1 = new MainMenuLayer();
    CCLayer* pLayer2 = new OptionsLayer();
    CCLayer* pLayer3 = new AudiogramSceneLayer();
    CCLayer* pLayer4 = new CreditsLayer();
    CCLayer* pLayer5 = new LevelLayer();
    
    
    CCLayerMultiplex* layer = CCLayerMultiplex::create(pLayer1, pLayer2, pLayer3, pLayer4, pLayer5, NULL);
    pScene->addChild(layer, 0);
    
    ((MainMenu *)(pScene))->rootVC = parent->rootVC;
    
    pLayer1->release();
    pLayer2->release();
    pLayer3->release();
    pLayer4->release();
    pLayer5->release();
    
    ((MainMenu *) pScene)->setToneGenerator(parent->getToneGenerator());
    ((MainMenu *) pScene)->setDataStore(parent->getDataStore());
    CCDirector::sharedDirector()->replaceScene(pScene);
}

void PauseSceneLayer4::musicCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((CannonScene *)(this->getParent()))->getToneGenerator()->disableBackground();
    } else {
        ((CannonScene *)(this->getParent()))->getToneGenerator()->enableBackground();
    }
}

void PauseSceneLayer4::fxCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((CannonScene *)(this->getParent()))->getToneGenerator()->disableTones();
    } else {
        ((CannonScene *)(this->getParent()))->getToneGenerator()->enableTones();
    }
}

