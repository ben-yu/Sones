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

#define PTM_RATIO 20
#define PI 3.14159265

enum {
    kTagParentNode = 1,
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
    setTouchEnabled(true);
    setAccelerometerEnabled(true);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    // init physics
    this->initPhysics();
    // create reset button
    this->createResetButton();
    
    //Set up sprite
#if 1
    // Use batch node. Faster
    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("ironball.png", 100);
    m_pSpriteTexture = parent->getTexture();
#else
    // doesn't use batch node. Slower
    m_pSpriteTexture = CCTextureCache::sharedTextureCache()->addImage("slice_3_0.png");
    CCNode *parent = CCNode::create();
#endif
    addChild(parent, 0, kTagParentNode);
    
            
    for (int32 j = 0; j < e_columnCount; ++j)
    {        
        for (int i = 0; i < e_rowCount; ++i)
        {
            addNewSpriteAtPosition(ccp((j*s.width/10) + s.width, (3*s.height/10 + (16.0f * i))));
        }
    }

    cannon = CCSprite::create("cannon2.png");
    addChild(cannon,0);
    cannon->setPosition(ccp(s.width/10, s.height/2));
    
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
    CCLayer::onEnter();
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCNode* parent = getChildByTag(kTagParentNode);
    cannonBall = new CannonBallSprite();
    
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
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    
    cannonBall->initWithTexture(m_pSpriteTexture);
    cannonBall->autorelease();
    //cannonBall->setRotation(180.0);
    //cannonBall->setPosition(ccp(0.9*s.width/PTM_RATIO,0.5*s.height/PTM_RATIO));
    cannonBall->setPhysicsBody(body);
    cannonBall->setVisible(true);
    
    parent->addChild(cannonBall);
    this->runAction(CCFollow::create(cannonBall, CCRectMake(0, 0, s.width*2 + 100, s.height)));
    
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
    //     m_debugDraw = new GLESDebugDraw( PTM_RATIO );
    //     world->SetDebugDraw(m_debugDraw);
    
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    //m_debugDraw->SetFlags(flags);
    
    
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
    
    groundBox.Set(b2Vec2(0,0), b2Vec2(s.width*3/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);
    
    // top
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(s.width/PTM_RATIO,s.height/PTM_RATIO));
    groundBody->CreateFixture(&groundBox,0);
    
    // left
    groundBox.Set(b2Vec2(0,s.height/PTM_RATIO), b2Vec2(0,0));
    groundBody->CreateFixture(&groundBox,0);
    
    // right
    groundBox.Set(b2Vec2(s.width*3/PTM_RATIO,s.height/PTM_RATIO), b2Vec2(s.width*3/PTM_RATIO,0));
    groundBody->CreateFixture(&groundBox,0);    
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


void CannonLayer::update(float dt)
{
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
}

void CannonLayer::ccTouchesBegan(CCSet* touches, CCEvent* event)
{

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
    CCPoint pos = cannon->getPosition();
    
    //Calulate angle between ship and touch position
    curAngle = atan2f(mean_touch.y - pos.y,mean_touch.x - pos.x) * (-180/3.14159) + 90.0f;
    
    pos.setPoint(pos.x - 15, pos.y);
    cannon->setRotation(curAngle);
}

void CannonLayer::ccTouchesEnded(CCSet* touches, CCEvent* event)
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
        
        //addNewSpriteAtPosition( location );
    }
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
    addChild(menu, -1);
    
    addChild(pMenu, 1);
    addChild(powerLabel);
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
    CCLayer* pauseLayer = new PauseSceneLayer();
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


