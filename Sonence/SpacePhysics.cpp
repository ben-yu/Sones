#include "SpacePhysics.h"
#include "SimpleAudioEngine.h"
#include "MainMenu.h"
#include "ToneGeneratorHelper.h"
#include "cocos2d.h"
#include "AudiogramScene.h"


#include <sstream>
#include <algorithm>

#define USE_CHUNKED 1

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

float SpacePhysicsLayer::sysVer = 0.0;
bool SpacePhysicsLayer::accelEnable;

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
    kTagMissileNode = 2,
};


EnemySprite::EnemySprite()
: m_pBody(NULL)
{
    
}

EnemySprite::~EnemySprite()
{
    m_pBody->GetWorld()->DestroyBody(m_pBody);
}

void EnemySprite::setPhysicsBody(b2Body * body)
{
    m_pBody = body;
}

b2Body* EnemySprite::getPhysicsBody()
{
    return m_pBody;
}

// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool EnemySprite::isDirty(void)
{
    return true;
}

CCSprite * EnemySprite::create(const char *pszFileName)
{
    return CCSprite::create(pszFileName);
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform EnemySprite::nodeToParentTransform(void)
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

SpacePhysicsLayer::SpacePhysicsLayer(void)
: m_pSpriteTexture(NULL)
{
    //////////////////////////////
    // 1. super init first
    
    // Get window sizes from director
    winWidth = CCDirector::sharedDirector()->getWinSize().height;
    winHeight = CCDirector::sharedDirector()->getWinSize().width;
    
    this->initPhysics();
    m_pointCount = 0;
    
    searchHelper = new MLSearch();
    searchHelper->setMode(1);
    
    //////////////////////////////
    //
    //  Labels & UI
    //
    //////////////////////////////
    
    score = 0;
    distLabel = CCLabelTTF::create("0m", "PressStart2P-Regular", 12.0);
    scoreLabel = CCLabelTTF::create("Score: 0", "PressStart2P-Regular", 12.0);
    tutorialText = CCLabelTTF::create("", "Audiowide-Regular", 30.0);
    tutorialText2 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    tutorialText3 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    tutorialText4 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    tutorialText5 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    
    
    distLabel->setPosition(ccp(winHeight*0.7,winWidth*0.95));
    this->addChild(distLabel, 1);
    distLabel->setVisible(false);
    
    scoreLabel->setPosition(ccp(winHeight*0.5, winWidth*0.95));
    this->addChild(scoreLabel, 1);
    scoreLabel->setVisible(false);
    
    tutorialText->setPosition(ccp(winHeight/2, winWidth/2));
    tutorialText2->setPosition(ccp(winHeight/2, winWidth/2));
    tutorialText3->setPosition(ccp(winHeight/2, winWidth/2));
    tutorialText4->setPosition(ccp(winHeight/2, winWidth/2));
    tutorialText5->setPosition(ccp(winHeight/2, winWidth/2));
    
    stringstream tempString;
    tempString<<"RED ALERT!\n Incoming Alien Fighters!";
    tutorialText->setString(tempString.str().c_str());
    tutorialText->setColor(ccc3(255, 0, 0));
    tutorialText->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText);
    tutorialText->setVisible(false);
    
    tempString.str("");
    tempString<<"As the aliens get closer to your ship,\n they will emit a pure-tone freq.\n Try to shoot them down before they damage your ship!";
    tutorialText2->setString(tempString.str().c_str());
    tutorialText2->setColor(ccc3(0, 255, 0));
    tutorialText2->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText2);
    tutorialText2->setVisible(false);
    
    tempString.str("");
    tempString<<"They will attack you from the following directions";
    tutorialText3->setString(tempString.str().c_str());
    tutorialText3->setColor(ccc3(0, 255, 0));
    tutorialText3->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText3);
    tutorialText3->setVisible(false);
    
    
    tempString.str("");
    tempString<<"Higher frequencies will come from your right.";
    tutorialText4->setString(tempString.str().c_str());
    tutorialText4->setColor(ccc3(0, 255, 0));
    tutorialText4->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText4);
    tutorialText4->setVisible(false);
    
    tempString.str("");
    tempString<<"Good Luck!";
    tutorialText5->setString(tempString.str().c_str());
    tutorialText5->setColor(ccc3(0, 255, 0));
    tutorialText5->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText5);
    tutorialText5->setVisible(false);
    
    //////////////////////////////
    //
    //  Sprites Animations
    //
    //////////////////////////////
    
    // set the appropriate resource directory for this device
    CCFileUtils::sharedFileUtils()->setResourceDirectory("");
    
    // load and cache the texture and sprite frames
    frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    animCache = CCAnimationCache::sharedAnimationCache();
    frameCache->addSpriteFramesWithFile("main_sprites.plist");
    
    CCArray* animFrames = CCArray::createWithCapacity(3);
    char str[100] = {0};
    for(int i = 0; i < 3; i++)
    {
        sprintf(str, "slice_0_%01d.png",i);
        CCSpriteFrame *frame = frameCache->spriteFrameByName(str);
        animFrames->addObject(frame);
    }
    CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
    
    // Add an animation to the Cache
    CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "ship");
    
    animFrames = CCArray::createWithCapacity(4);
    for(int i = 0; i < 3; i++)
    {
        sprintf(str, "slice_3_%01d.png",i);
        CCSpriteFrame *frame = frameCache->spriteFrameByName(str);
        animFrames->addObject(frame);
    }
    animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
    
    // Add an animation to the Cache
    CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "asteroid");
    
    animFrames = CCArray::createWithCapacity(3);
    for(int i = 2; i < 5; i++)
    {
        sprintf(str, "slice_4_%01d.png",i);
        CCSpriteFrame *frame = frameCache->spriteFrameByName(str);
        animFrames->addObject(frame);
    }
    animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
    
    // Add an animation to the Cache
    //CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "enemy");
    
    
    //////////////////////////////
    //
    //  Player
    //
    //////////////////////////////
    
    // create a sprite without texture
    playerShip = new EnemySprite();
    CCSpriteFrame *frame = frameCache->spriteFrameByName("slice_0_0.png");
    playerShip->initWithSpriteFrame(frame);
    playerShip->setDisplayFrame(frame);
    addChild(playerShip);
    
    // animate the player
    CCAnimation *normal = animCache->animationByName("ship");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    playerShip->runAction(seq);
    
    // Set ship sprite and pos
    playerPos = ccp(winHeight * 0.5, winWidth * 0.1);
    playerShip->setPosition(playerPos);
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set((playerShip->getPosition().x)/PTM_RATIO,(playerShip->getPosition().y)/PTM_RATIO);
    b2Body* body = world->CreateBody(&bd);
    
    b2PolygonShape polygon;
    polygon.SetAsBox(1.0, 0.1);
    body->CreateFixture(&polygon, 10000.0f);
    body->SetTransform(b2Vec2(playerShip->getPosition().x/PTM_RATIO,playerShip->getPosition().y/PTM_RATIO), 0.0f);
    b2Vec2 enemyPos = b2Vec2(playerShip->getPosition().x,playerShip->getPosition().y);
    b2Vec2 shipPos = b2Vec2(playerPos.x,playerPos.y);
    b2Vec2 vel = enemyPos - shipPos;
    vel.Normalize();
    body->SetLinearVelocity(-5*vel);
    playerShip->setPhysicsBody(body);
    body->SetUserData(playerShip);
        
    // Lasers
#define KNUMLASERS 2
    _shipLasers = new CCArray();
    for(int i = 0; i < KNUMLASERS; ++i) {
        CCSprite *shipLaser = CCSprite::create("missile_4_0.png");
        shipLaser->setVisible(false) ;
        addChild(shipLaser);
        _shipLasers->addObject(shipLaser);
    }
    
    CCSprite *hp = CCSprite::create("hp.png");
    hpBar = CCProgressTimer::create(hp);
    hpBar->setType(kCCProgressTimerTypeBar);
    hpBar->setMidpoint(ccp(0,0));
    //    Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
    hpBar->setBarChangeRate(ccp(1, 0));
    hpBar->setPosition(ccp(winHeight*0.13,winWidth*0.945));
    hpBar->setScaleX(0.9);
    hpBar->setScaleY(0.25);
    
    healthBar = CCSprite::create("health.png");
    healthBar->setPosition(ccp(winHeight*0.13,winWidth*0.95));
    addChild(healthBar);
    addChild(hpBar);
    
    hpBar->setPercentage(100.0);
    
    destroyedAsteroids = 0;
    
    //////////////////////////////
    //
    //  Parallax Background
    //
    //////////////////////////////
    
    // 1) Create the CCParallaxNode
    _backgroundNode = CCParallaxNodeExtras::node() ; //1
    this->addChild(_backgroundNode,-1) ;
    
    // 2) Create background sprites
    _spacedust1 = CCSprite::create("Starfield.jpg");
    _spacedust2 = CCSprite::create("Starfield.jpg");
    _planetsunrise = CCSprite::create("bg_planetsunrise.png");
    _galaxy = CCSprite::create("bg_galaxy.png");
    _spacialanomaly = CCSprite::create("bg_spacialanomaly.png");
    _spacialanomaly2 = CCSprite::create("bg_spacialanomaly2.png");
    
    // 3) Determine relative movement speeds for space dust and background
    CCPoint dustSpeed = ccp(0.1, 0.1);
    CCPoint bgSpeed = ccp(0.05, 0.05);
    
    // 4) Add children to CCParallaxNode
    _backgroundNode->addChild(_spacedust1, -2 , dustSpeed, ccp(0,0) ); // 2
    _backgroundNode->addChild(_spacedust2, -2 , dustSpeed, ccp(0,_spacedust2->getContentSize().height));
    
    _backgroundNode->addChild(_galaxy,-1, bgSpeed , ccp(winWidth * 0.7,0));
    _backgroundNode->addChild(_planetsunrise,-1 , bgSpeed,ccp(winWidth * 0.8,800));
    _backgroundNode->addChild(_spacialanomaly,-1, bgSpeed,ccp(winWidth * 0.3,1000));
    _backgroundNode->addChild(_spacialanomaly2,-1, bgSpeed,ccp(winWidth * 0.9,1200));
    
    _spacedust2->setFlipY(true);
    
    //////////////////////////////
    //
    //  NPC's
    //
    //////////////////////////////
    
    // Spawn enemies
#define KNUMENEMIES 1
    sineTones = (float *) calloc(sizeof(float),KNUMENEMIES); // Tone array
    timeTargets = (float *) calloc(sizeof(float),KNUMENEMIES);
    alphaTargets = (float *) calloc(sizeof(float),KNUMENEMIES);
    active_lanes = new CCArray();
    coins = new CCArray();
    homing_missiles = new CCArray();
    _AIEnemies = new CCArray();
    
    _batchNode = CCSpriteBatchNode::create("missile_1_0 copy.png", 100);
    _missleNode = CCSpriteBatchNode::create("missile.png", 100);
    m_pSpriteTexture = _batchNode->getTexture();
    missleTexture = _missleNode->getTexture();
    addChild(_batchNode, 0, kTagParentNode);
    addChild(_missleNode,0,kTagMissileNode);
    
    for(int i = 0; i < KNUMENEMIES; i++) {
        CCSprite *enemy = CCSprite::create("warrior1_0.png");
        enemy->setVisible(false);
        addChild(enemy);
        _AIEnemies->addObject(enemy);
    }
    
    
#define KNUMASTEROIDS 10
    _spawnQueue = new CCArray(KNUMASTEROIDS);
    for(int i = 0; i < KNUMASTEROIDS; ++i) {
        
        CCSprite *asteroid = CCSprite::create("coin_gold-4-0.png");
        asteroid->setVisible(false);
        addChild(asteroid);
        coins->addObject(asteroid);
        _spawnQueue->addObject(CCInteger::create(i));
        
    }

#define NUM_OF_LANES 5
    for (int i=0; i<NUM_OF_LANES; i++) {
        CCSprite *lane = CCSprite::create("MeagaLaser-1-1.png");
        active_lanes->addObject(lane);
        lane->setVisible(false);
        lane->setRotation(180.0f);
        lane->setScale(winHeight/lane->getContentSize().height);
        addChild(lane);
    }
    
}

SpacePhysicsLayer::~SpacePhysicsLayer(void)
{
}

void SpacePhysicsLayer::onEnter()
{
    CCLayer::onEnter();
    
    this->toneGenHelp = ((SpacePhysicsScene *)this->getParent())->getToneGenerator();
    this->toneGenHelp->playBackgroundMusic("main_background.wav");
    this->toneGenHelp->enableTones();
    this->dataStoreHandler = ((SpacePhysicsScene *)this->getParent())->getDataStore();
    this->searchHelper = new MLSearch();
    searchHelper->setMode(1);
    this->setTouchEnabled(true); // Enable Touch
    
    this->scheduleUpdate(); // Start updating
    seed_freq = floorf(randomValueBetween(20.0,4000.0));
    
    tutorialDuration = 10 * 1000.0;
    radarRadius = winWidth/2;
    enemyVelocity = winWidth/20.0;
    playedTutorial = true;
    if (!playedTutorial) {
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::playTutorial), 2.0);
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::endTutorial), 35.0);
    }
    
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawnAIEnemy), 2.0);
    
    _nextAsteroid = 0;
    _curAsteroidCount = 0;
    for (int i=0; i < 10; i++) {
        baseVolumes[i] = 0.3;
        volDiffs[i] = -0.1;
    }
}

void SpacePhysicsLayer::initPhysics()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    world = new b2World(gravity);
    
    m_destructionListener.test = this;
    world->SetDestructionListener(&m_destructionListener);
    world->SetContactListener(this);
    
    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);
    world->SetWarmStarting(false);
    world->SetContinuousPhysics(true);
    m_debugDraw = new GLESDebugDraw( PTM_RATIO );
    world->SetDebugDraw(m_debugDraw);
    
    uint32 flags = 0;
    //flags += b2Draw::e_shapeBit;
    //       flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    m_debugDraw->SetFlags(flags);
    
}


void SpacePhysicsLayer::update(float dt) {
    
    //float curTimeMillis = getTimeTick();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    //////////////////////////////
    //
    //  Box2D
    //
    //////////////////////////////
    
    //It is recommended that a fixed time step is used with Box2D for stability
    //of the simulation, however, we are using a variable time step here.
    //You need to make an informed choice, the following URL is useful
    //http://gafferongames.com/game-physics/fix-your-timestep/
    
    int velocityIterations = 8;
    int positionIterations = 3;
    
    // Instruct the world to perform a single step of simulation. It is
    // generally best to keep the time step and iterations fixed.
    world->Step(dt, velocityIterations, positionIterations);
    
    // We are going to destroy some bodies according to contact
    // points. We must buffer the bodies that should be destroyed
    // because they may belong to multiple contact points.
    const int32 k_maxNuke = 20;
    b2Body* nuke[k_maxNuke];
    int32 nukeCount = 0;
    bool hitFlag = false;
    
    // Traverse the contact results. Destroy bodies that
    // are touching heavier bodies.
    for (int32 i = 0; i < m_pointCount; ++i)
    {
        ContactPoint* point = m_points + i;
        
        b2Body* body1 = point->fixtureA->GetBody();
        b2Body* body2 = point->fixtureB->GetBody();
        EnemySprite* sprite1 = (EnemySprite*) body1->GetUserData();
        EnemySprite* sprite2 = (EnemySprite*) body2->GetUserData();
        float32 mass1 = body1->GetMass();
        float32 mass2 = body2->GetMass();
        
        
        if (mass1 > 0.0f && mass2 > 0.0f && !hitFlag)
        {
            if (sprite1 == playerShip && sprite2->isVisible()) {
                nuke[nukeCount++] = body2;
                playerShip->runAction( CCBlink::create(1.0, 5));
                _health -= 10;
                hpBar->setPercentage(_health);
                hitFlag = true;
            }
            else if (sprite2 == playerShip && sprite1->isVisible())
            {
                nuke[nukeCount++] = body1;
                playerShip->runAction( CCBlink::create(1.0, 5));
                _health -= 10;
                hpBar->setPercentage(_health);
                hitFlag = true;
            }
            
            if (nukeCount == k_maxNuke)
            {
                break;
            }
        }
    }
    
    // Sort the nuke array to group duplicates.
    std::sort(nuke, nuke + nukeCount);
    
    // Destroy the bodies, skipping duplicates.
    int32 i = 0;
    while (i < nukeCount)
    {
        b2Body* b = nuke[i++];
        while (i < nukeCount && nuke[i] == b)
        {
            ++i;
        }
        if (b != NULL) {
            b->SetActive(false);
            EnemySprite* shotPtr = (EnemySprite*) b->GetUserData();
            if (shotPtr != NULL) {
                shotPtr->setVisible(false);
                //shotPtr->setPhysicsBody(NULL);
                //removeChild(shotPtr,false);
                
                //delete shotPtr;
            }
            
            //b = NULL;

            //b->SetUserData(NULL);
            //world->DestroyBody(b);
            nuke[i] = NULL;

        }
    }
    
    //////////////////////////////
    //
    //  Accelerometer
    //
    //////////////////////////////
    
    float maxY = winSize.width - playerShip->getContentSize().width/2;
    float minY = playerShip->getContentSize().width/2;
    
    float diff = (_shipPointsPerSecY * dt) ;
    float newY = playerShip->getPosition().x + diff;
    newY = MIN(MAX(newY, minY), maxY);
    playerShip->setPosition(ccp(newY,playerShip->getPosition().y));
    playerShip->getPhysicsBody()->SetTransform(b2Vec2(playerShip->getPosition().x/PTM_RATIO,playerShip->getPosition().y/PTM_RATIO), 0.0f);
    
    
    //////////////////////////////
    //
    //  Background Scrolling
    //
    //////////////////////////////
    
    CCPoint backgroundScrollVert;
    backgroundScrollVert = ccp(0,-500 - floor(distance/10.0));
    _backgroundNode->setPosition(ccpAdd(_backgroundNode->getPosition(),ccpMult(backgroundScrollVert,dt)));
    
    CCArray *spaceDusts = CCArray::createWithCapacity(2) ;
    spaceDusts->addObject(_spacedust1);
    spaceDusts->addObject(_spacedust2);
    
    for ( int ii = 0  ; ii < spaceDusts->count() ; ii++ ) {
        CCSprite * spaceDust = (CCSprite *)(spaceDusts->objectAtIndex(ii)) ;
        float xPosition = _backgroundNode->convertToWorldSpace(spaceDust->getPosition()).y;
        float size = spaceDust->getContentSize().height ;
        if ( xPosition < -size) {
            _backgroundNode->incrementOffset(ccp(0,2*spaceDust->getContentSize().height),spaceDust) ;
        }
    }
    
    CCArray *backGrounds = CCArray::createWithCapacity(4);
    backGrounds->addObject(_galaxy);
    backGrounds->addObject(_planetsunrise);
    backGrounds->addObject(_spacialanomaly);
    backGrounds->addObject(_spacialanomaly2);
    
    for ( int ii = 0 ; ii <backGrounds->count() ; ii++ ) {
        CCSprite * background = (CCSprite *)(backGrounds->objectAtIndex(ii)) ;
        float xPosition = _backgroundNode->convertToWorldSpace(background->getPosition()).y ;
        float size = background->getContentSize().height;
        if ( xPosition < -size/2) {
            _backgroundNode->incrementOffset(ccp(0,2*winWidth + background->getContentSize().height),background);
        }
    }
    
    
    //////////////////////////////
    //
    //  NPC Spawning
    //
    //////////////////////////////
    
    if (!enemySpawned && playedTutorial) {
        enemySpawned = true;
        
        //float spawnRate = curTimeMillis/5000.0;
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawnRandomEnemy), 10.0);
    }
    
    if (_curAsteroidCount < KNUMASTEROIDS || _nextAsteroid != 0 ) {
         _curAsteroidCount++;
         float spawnRate = randomValueBetween(0.1,0.3);
         
         this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawnAsteroid), spawnRate);
     }
     
    
    //////////////////////////////
    //
    //  Collision Detection
    //
    //////////////////////////////
    
    int index = 0;

    // Enemy Collision
    CCObject *itEnemy;
    CCARRAY_FOREACH( homing_missiles,itEnemy) {
        EnemySprite *enemy = (EnemySprite *) itEnemy;
        if ( ! enemy->isVisible() )
            continue ;
        CCARRAY_FOREACH(_shipLasers,itEnemy) {
            CCSprite *shipLaser = (CCSprite *) itEnemy;
            if ( ! shipLaser->isVisible() )
                continue ;
            if (shipLaser->boundingBox().intersectsRect(enemy->boundingBox())) {
                
                touchTimerFlag = false;
                finalTime = getTimeTick() - initTime;
                
                shipLaser->setVisible(false);
                enemy->setVisible(false);
                setEnemyInvisible(enemy);
                _backgroundNode->removeChild(enemy, 1);
                float tmpVol = toneGenHelp->removeTone(index);
                score += 10;
                enemySpawned = false;
                //dataStoreHandler->saveData("Moving_Invaders",(double) sineTones[index], (double) tmpVol, earIndex, touchAttempts,initTime, finalTime);
                
                //char json[100];
                //sprintf(json, "{\"data_points\":[{\"uuid\":\"%d\",\"freq\":%f,\"game_type\":\"normal\",\"vol\":%f}]}",12345,(double) sineTones[index],(double) tmpVol);
                //jsonDoc.Parse<0>(json);
                //cout << json;
                
                m_emitter = CCParticleExplosion::create();
                m_emitter->retain();
                addChild(m_emitter, 10);
                m_emitter->setTexture( CCTextureCache::sharedTextureCache()->addImage("fire.png") );
                m_emitter->setAutoRemoveOnFinish(true);
                m_emitter->setPosition(enemy->getPosition());
                
                toneGenHelp->playExplosion();
                
                if (score % 50 == 0) {
                    nextLevel();
                }
                continue ;
            }
        }

        if (playerShip->boundingBox().intersectsRect(enemy->boundingBox())) {
            enemy->setVisible(false);
            setEnemyInvisible(enemy);
            _backgroundNode->removeChild(enemy, 1);
            toneGenHelp->removeTone(index);
            playerShip->runAction( CCBlink::create(1.0, 5));
            alphaTargets[index] = 0.0;
            enemySpawned = false;
            _health -= 10.0;
            hpBar->setPercentage(_health);
        }
        index++;
    }
    CCObject *itLane;
    CCARRAY_FOREACH( active_lanes,itLane) {
        CCSprite *lane = (CCSprite *) itLane;
        if (lane->isVisible() && lane->boundingBox().intersectsRect(playerShip->boundingBox())){
            CCObject *itLane2;
            CCARRAY_FOREACH( active_lanes,itLane2) {
                CCSprite *lane2 = (CCSprite *) itLane2;
                lane2->setVisible(false);
            }
            setEnemyInvisible(lane);
            //removeChild(active_lane, 1);
            toneGenHelp->removeTone(index);
            playerShip->runAction( CCBlink::create(1.0, 5));
            alphaTargets[index] = 0.0;
            enemySpawned = false;
            _health -= 10.0;
            laneScale = 0.0;
            hpBar->setPercentage(_health);
            toneGenHelp->removeTone(index);
            float tmpVol = toneGenHelp->getVolume();
            volDiffs[5*earIndex + enemyIndex] += (float) searchHelper->getNextTone(baseVolumes[5*earIndex + enemyIndex] + volDiffs[5*earIndex + enemyIndex],earIndex,false);
            CCLog("THRESH: %f",volDiffs[5*earIndex + enemyIndex]);
            dataStoreHandler->saveData("Moving_Invaders",(double) sineTones[index], (double) tmpVol, earIndex, 0,0,0);
            hitFlag = true;
        }
    }
    CCObject *itBoss;
    CCARRAY_FOREACH( _AIEnemies,itBoss) {
        CCSprite *boss = (CCSprite *) itBoss;
        CCObject *itLaser;
        CCARRAY_FOREACH(_shipLasers,itLaser) {
            CCSprite *shipLaser = (CCSprite *) itLaser;
            if ( ! shipLaser->isVisible() )
                continue ;
            if ( shipLaser->boundingBox().intersectsRect(boss->boundingBox()) ) {
                bossHealth--;
                bossBar->setPercentage(bossHealth/50.0);
                boss->runAction( CCBlink::create(1.0, 5));
                continue ;
            }
        }
    }
    
    //////////////////////////////
    //
    //  Game Stats
    //
    //////////////////////////////
    
    if (playedTutorial) {
        distance++;
    }
    
    stringstream tempString;
    tempString<<distance<<"m";
    distLabel->setString(tempString.str().c_str());
    
    tempString.str("");
    tempString<<"Score: "<< score;
    scoreLabel->setString(tempString.str().c_str());
    
    distLabel->setVisible(true);
    scoreLabel->setVisible(true);
    
    if (enemySpawned && laneScale < 2.0) {
        laneScale += 0.01;
        //active_lane->setScaleY(laneScale);
    }
    
    if (_health <= 0 && !gameOver) {
        gameOver = true;
        tutorialText = CCLabelTTF::create("GAME OVER!", "Audiowide-Regular", 30.0);
        tutorialText->setPosition(ccp(winHeight/2, winWidth/2));
        tutorialText->setVisible(true);
        tutorialText->setOpacity(255);
        addChild(tutorialText);
        
        runAction(CCWaves3D::create(5, 40, ccg(15,10), 10));
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::menuCloseCallback), 8.0);
    }
    
}

void SpacePhysicsLayer::draw(){
    // Draw Grid Lines
    
    glEnable(GL_BLEND); // Enable blending for alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glLineWidth(2);
    
    ccDrawSolidRect(ccp(0,0), ccp(winHeight/2,winWidth), ccc4f(0,1,0,0.1));
    ccDrawSolidRect( ccp(winHeight/2,0), ccp(winHeight,winWidth), ccc4f(0,0,1,0.1));
    
    CCLayer::draw();
    
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    
    kmGLPushMatrix();
    
    world->DrawDebugData();
    
    kmGLPopMatrix();
}

void SpacePhysicsLayer::playTutorial()
{
    distLabel->setVisible(false);
    scoreLabel->setVisible(false);
    
    CCLabelTTF* skipLabel = CCLabelTTF::create("Skip", "Ubuntu-Regular", floor(14 * winWidth/640));
    CCMenuItemLabel *tutorialSkipButtonItem = CCMenuItemLabel::create(skipLabel, this, menu_selector(SpacePhysicsLayer::endTutorial));
    tutorialSkipButton = CCMenu::create(tutorialSkipButtonItem,NULL);
    
    tutorialSkipButton->setPosition(ccp(winHeight*0.5,winWidth*0.2));
    tutorialSkipButton->setVisible(true);
    addChild(tutorialSkipButton);
    
    CCFiniteTimeAction *fadeIn = CCFadeIn::create(1.0);
    fadeIn->setDuration(1.0);
    CCDelayTime *waitDelay = CCDelayTime::create(1.5);
    CCFiniteTimeAction *fadeOut = CCFadeOut::create(1.0);
    fadeOut->setDuration(1.0);
    tutorialText->setOpacity(0);
    tutorialText->setVisible(true);
    tutorialText2->setOpacity(0);
    tutorialText2->setVisible(true);
    tutorialText3->setOpacity(0);
    tutorialText3->setVisible(true);
    tutorialText4->setOpacity(0);
    tutorialText4->setVisible(true);
    tutorialText5->setOpacity(0);
    tutorialText5->setVisible(true);
    
    tutorialText->runAction(CCSequence::create (fadeIn,waitDelay,fadeOut,fadeIn,waitDelay,fadeOut,NULL));
    CCFiniteTimeAction *initialDelay = CCDelayTime::create(5.0);
    fadeIn->setDuration(1.0);
    waitDelay = CCDelayTime::create(3.0);
    tutorialText2->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText2->runAction(CCSequence::create(CCDelayTime::create(10.0),fadeOut,NULL));
    initialDelay->setDuration(10.5);
    tutorialText3->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText3->runAction(CCSequence::create(CCDelayTime::create(14.5),fadeOut,NULL));
    initialDelay->setDuration(30.0);
    tutorialText5->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText5->runAction(CCSequence::create(CCDelayTime::create(32.0),fadeOut,NULL));
}

void SpacePhysicsLayer::endTutorial()
{
    playedTutorial = true;
    tutorialSkipButton->setVisible(false);
    removeChild(tutorialSkipButton, true);
    distLabel->setVisible(true);
    scoreLabel->setVisible(true);

    tutorialText->stopAllActions();
    tutorialText2->stopAllActions();
    tutorialText3->stopAllActions();
    tutorialText4->stopAllActions();
    tutorialText5->stopAllActions();
    tutorialText->setVisible(false);
    tutorialText2->setVisible(false);
    tutorialText3->setVisible(false);
    tutorialText4->setVisible(false);
    tutorialText5->setVisible(false);
    
}

void SpacePhysicsLayer::tutorialSpawn(int freqIndex, int earIndex)
{
    
    
}

void SpacePhysicsLayer::spawnRandomEnemy()
{
    enemyIndex = (int) floorf(randomValueBetweenD(0.0,6.0)); // Only generate harmonic pure-tones
    earIndex = (int)(randomValueBetweenD(0.0,2.0));
    spawnTones(enemyIndex,earIndex);
    touchTimerFlag = true;
    initTime = getTimeTick();
    touchAttempts = 0;
    
}

void SpacePhysicsLayer::spawnHomingMissle()
{
    CCSprite *aiEnemy = (CCSprite *) _AIEnemies->objectAtIndex(0);
    EnemySprite *enemy = new EnemySprite();
    enemy->initWithTexture(missleTexture);
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set((aiEnemy->getPosition().x)/PTM_RATIO,(aiEnemy->getPosition().y)/PTM_RATIO);
    b2Body* body = world->CreateBody(&bd);
    
    b2PolygonShape polygon;
    polygon.SetAsBox(1.0, 0.1);
    body->CreateFixture(&polygon, 1.0f);
    body->SetTransform(b2Vec2(aiEnemy->getPosition().x/PTM_RATIO,aiEnemy->getPosition().y/PTM_RATIO), -90.0);
    b2Vec2 enemyPos = b2Vec2(aiEnemy->getPosition().x,aiEnemy->getPosition().y);
    b2Vec2 shipPos = b2Vec2(playerPos.x,playerPos.y);
    b2Vec2 vel = enemyPos - shipPos;
    vel.Normalize();
    body->SetLinearVelocity(-5*vel);
    enemy->setPhysicsBody(body);

    addChild(enemy);
    body->SetUserData(enemy);
    

}

void SpacePhysicsLayer::spawnAIEnemy()
{
    CCSprite *aiEnemy = (CCSprite *) _AIEnemies->objectAtIndex(0);
    aiEnemy->stopAllActions();
    aiEnemy->setOpacity(255);
    aiEnemy->setVisible(true);
    aiEnemy->setRotation(90.0f);
    aiEnemy->setScale(0.5);
    
    CCSprite *hp = CCSprite::create("hp.png");
    bossBar = CCProgressTimer::create(hp);
    bossBar->setType(kCCProgressTimerTypeBar);
    bossBar->setMidpoint(ccp(0,0));
    //    Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
    bossBar->setBarChangeRate(ccp(1, 0));
    bossBar->setPosition(ccp(winHeight*0.13,winWidth*0.945));
    bossBar->setScaleX(0.9);
    bossBar->setScaleY(0.25);
    
    aiEnemy->addChild(bossBar);
    
    bossBar->setPercentage(100.0);
    
    aiEnemy->setPosition(ccp(winHeight/2,winWidth*0.75));
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(10.0,ccp(0,winWidth*0.75)),CCMoveTo::create(10.0,ccp(winHeight,winWidth*0.75)),NULL);
    
    CCRepeatForever *moveLoop = CCRepeatForever::create((CCActionInterval *)moveSeq);
    aiEnemy->runAction(moveLoop);
    
    this->schedule(schedule_selector(SpacePhysicsLayer::spawnAttackPatterns), 2.0f);
    this->schedule(schedule_selector(SpacePhysicsLayer::spawnHomingMissle), 3.0f);

}

void SpacePhysicsLayer::spawnAttackPatterns()
{
    CCSprite *aiEnemy = (CCSprite *) _AIEnemies->objectAtIndex(0);
    
    float degRot = 180.0;
    for (int i = 0; i < 10; i++) {
        
        EnemySprite *shot = new EnemySprite();
        shot->initWithTexture(m_pSpriteTexture);
        b2Vec2 vel;
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        //body->SetTransform(b2Vec2(aiEnemy->getPosition().x,aiEnemy->getPosition().y), 0.0);
        bd.position.Set((aiEnemy->getPosition().x + 50*cos(degRot))/PTM_RATIO,(aiEnemy->getPosition().y + 50*sin(degRot))/PTM_RATIO);
        b2Body* body = world->CreateBody(&bd);
        
        b2CircleShape circle;
        circle.m_radius = 0.1f;
        body->CreateFixture(&circle, 1.0f);
        vel = b2Vec2(cos(degRot),sin(degRot));
        vel.Normalize();
        body->SetLinearVelocity(vel);
        degRot += 18.0;
        shot->setPhysicsBody(body);
        shot->setVisible(true);
        addChild(shot);
        body->SetUserData(shot);
        
    }
}

void SpacePhysicsLayer::spawnTones(int freqIndex, int earIndex)
{
    double randDuration = randomValueBetweenD(2.0,5.0);
    float randFrequency = freqIndex * seed_freq + 250.0;    // Calculate frequency
    
    searchHelper->cur_freq = freqIndex;
    toneGenHelp->setVolume(baseVolumes[5*earIndex + freqIndex] + volDiffs[5*earIndex + freqIndex]);
    toneGenHelp->playConstantTone(randFrequency, baseVolumes[5*earIndex + freqIndex] + volDiffs[5*earIndex + freqIndex], earIndex);   // Play pure-tone
    this->earIndex = earIndex;
    
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawnBeam),randDuration);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::deSpawnBeam),randDuration + 2.0);
}

void SpacePhysicsLayer::spawnBeam()
{
    CCObject *laneObj;
    int index = 0;
    CCARRAY_FOREACH( active_lanes,laneObj) {
        CCSprite *lane = (CCSprite *) laneObj;
        lane->setVisible(true);
        if (earIndex)
            lane->setPosition(ccp((10 - index)*(winHeight/10),winWidth/2));
        else
            lane->setPosition(ccp(index*(winHeight/10),winWidth/2));
        index++;
    }
    enemySpawned = false;
    hitFlag = false;
}

void SpacePhysicsLayer::deSpawnBeam()
{
    CCObject *laneObj;
    CCARRAY_FOREACH( active_lanes,laneObj) {
        CCSprite *lane = (CCSprite *) laneObj;
        lane->setVisible(false);
    }
    enemySpawned = false;
    toneGenHelp->removeTone(0);
    float tmpVol = toneGenHelp->getVolume();
    if (!hitFlag) {
        volDiffs[5*earIndex + enemyIndex] += searchHelper->getNextTone(baseVolumes[5*earIndex + enemyIndex] + volDiffs[5*earIndex + enemyIndex],earIndex, true);
        CCLog("THRESH: %f",volDiffs[5*earIndex + enemyIndex]);
        dataStoreHandler->saveData("Moving_Invaders",(double) sineTones[0], (double) tmpVol, earIndex, 1,0,0);
    }
}

void SpacePhysicsLayer::spawnAsteroid()
{
    double randDuration = randomValueBetweenD(2.0,3.0);
    int multiple = (int) floorf(randomValueBetweenD(0.0,10.0)); // Only generate harmonic pure-tones
    float randY = winHeight/10.0 * multiple;
    
    CCInteger *randomIndex = (CCInteger *) _spawnQueue->randomObject();
    
    CCSprite *asteroid = (CCSprite *) coins->objectAtIndex(randomIndex->getValue());
    asteroid->stopAllActions();
    asteroid->setTag(randomIndex->getValue());
    //CCAnimation *normal = animCache->animationByName("asteroid");
    //normal->setRestoreOriginalFrame(true);
    //CCAnimate *animN = CCAnimate::create(normal);
    //CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                      ccp(randY,0)),CCCallFuncN::create(this,callfuncN_selector(SpacePhysicsLayer::setAsteroidInvisible)),NULL);
    asteroid->setPosition( ccp(randY,winWidth));
    asteroid->setRotation(-(randY/winHeight)*90.0f - 225.0f);
    asteroid->runAction (moveSeq);
    //asteroid->runAction(seq);
    
    asteroid->setVisible(true);
    _spawnQueue->removeObject(randomIndex);
}


float SpacePhysicsLayer::randomValueBetween( float low , float high ) {
    return (((float) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

double SpacePhysicsLayer::randomValueBetweenD( double low , double high ) {
    return (((double) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

float SpacePhysicsLayer::getTimeTick() {
    timeval time;
    gettimeofday(&time, NULL);
    unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (float) millisecs;
}

void SpacePhysicsLayer::setInvisible(CCNode * node) {
    node->setVisible(false) ;
}

void SpacePhysicsLayer::setAsteroidInvisible(CCNode * node) {
    node->setVisible(false);
    _spawnQueue->addObject(CCInteger::create(node->getTag()));
    _curAsteroidCount--;
}

void SpacePhysicsLayer::setEnemyInvisible(CCNode * node) {
    node->setVisible(false);
    enemySpawned = false;
}

void SpacePhysicsLayer::setFireInvisible(CCNode *emitter){
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
    asteroidSpawned = false;
    emitter->setVisible(false);
    _backgroundNode->removeChild(emitter, 1);
}

void SpacePhysicsLayer::stopTones(){
    toneGenHelp->stop();
}

void SpacePhysicsLayer::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    
}

void SpacePhysicsLayer::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    CCPoint mean_touch;
    CCSetIterator iter = touches->begin();
    for (; iter != touches->end(); iter++) // Iterate through touch list, only get first touch?
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        CCPoint location = pTouch->getLocation();
        
        mean_touch = location; // Take mean of multi-touch later if needed
        
    }
    CCPoint pos = playerShip->getPosition();
    
    //Calulate angle between ship and touch position
    float shot_angle = atan2f(mean_touch.y - pos.y,mean_touch.x - pos.x);
    
    pos.setPoint(pos.x - 15, pos.y);
    b2Body * bd = playerShip->getPhysicsBody();
    bd->SetTransform(b2Vec2(pos.x/PTM_RATIO,pos.y/PTM_RATIO), shot_angle * (-180/3.14159) + 90.0f);
    playerShip->setRotation(shot_angle * (-180/3.14159) + 90.0f);
}

void SpacePhysicsLayer::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    CCPoint mean_touch;
    CCSetIterator iter = touches->begin();
    for (; iter != touches->end(); iter++) // Iterate through touch list, only get first touch?
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        CCPoint location = pTouch->getLocation();
        
        mean_touch = location; // Take mean of multi-touch later if needed
        
    }
    CCPoint pos = playerShip->getPosition();
    
    //Calulate angle between ship and touch position
    float shot_angle = atan2f(mean_touch.y - pos.y,mean_touch.x - pos.x);
    
    pos.setPoint(pos.x - 15, pos.y);
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
    CCSprite *shipLaser = (CCSprite *) _shipLasers->objectAtIndex(_nextShipLaser++);
    shipLaser->setRotation(shot_angle * (-180/3.14159));
    playerShip->setRotation(shot_angle * (-180/3.14159) + 90.0f);
    b2Body * bd = playerShip->getPhysicsBody();
    bd->SetTransform(b2Vec2(pos.x/PTM_RATIO,pos.y/PTM_RATIO), shot_angle * (-180/3.14159) + 90.0f);
    playerShip->setRotation(shot_angle * (-180/3.14159) + 90.0f);
    if ( _nextShipLaser >= _shipLasers->count() )
        _nextShipLaser = 0;
    shipLaser->setPosition( ccpAdd( pos, ccp(shipLaser->getContentSize().width/2, 0)));
    shipLaser->setVisible(true) ;
    shipLaser->stopAllActions() ;
    shipLaser->runAction( CCSequence::create (
                                              CCMoveTo::create(0.5,ccp(mean_touch.x,mean_touch.y)),
                                              CCCallFuncN::create(this,callfuncN_selector(SpacePhysicsLayer::setInvisible)) ,
                                              NULL  // DO NOT FORGET TO TERMINATE WITH NULL
                                              ) ) ;
    if (touchTimerFlag) {
        touchTime = getTimeTick() - initTime; // Initial Reaction Time
        touchTimerFlag = false;
    }
    touchAttempts++;
}

void SpacePhysicsLayer::didAccelerate(CCAcceleration* pAccelerationValue) {
#define KFILTERINGFACTOR 0.1
#define KRESTACCELX -0.0
#define KSHIPMAXPOINTSPERSEC (winSize.height*0.5)
#define KMAXDIFFX 0.2
    
    double rollingX ;
    
    // Cocos2DX inverts X and Y accelerometer depending on device orientation
    // in landscape mode right x=-y and y=x !!! (Strange and confusing choice)
    pAccelerationValue->x = pAccelerationValue->x ;
    rollingX = (pAccelerationValue->x * KFILTERINGFACTOR) + (rollingX * (1.0 - KFILTERINGFACTOR));
    float accelX = pAccelerationValue->x - rollingX ;
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    float accelDiff = accelX - KRESTACCELX;
    float accelFraction = accelDiff / KMAXDIFFX;
    _shipPointsPerSecY = KSHIPMAXPOINTSPERSEC * accelFraction;
        
    /*EnemySprite *enemy = (EnemySprite *) homing_missiles->objectAtIndex(0);  // Grab enemy object

    b2Body *body = enemy->getPhysicsBody();
    b2Vec2 enemyPos = PTM_RATIO * body->GetPosition();
    b2Vec2 shipPos = b2Vec2(playerShip->getPosition().x,playerShip->getPosition().y);
    b2Vec2 velocity = enemyPos - shipPos;
    velocity = -1*velocity;
    velocity.Normalize();
    
    body->SetLinearVelocity(velocity);
     */
}


void SpacePhysicsLayer::menuCloseCallback(CCObject* pSender)
{
    CCScene* pScene = MainMenu::create();
    CCLayer* pLayer1 = new MainMenuLayer();
    CCLayer* pLayer2 = new OptionsLayer();
    CCLayer* pLayer3 = new AudiogramSceneLayer();
    CCLayer* pLayer4 = new CreditsLayer();
    
    
    CCLayerMultiplex* layer = CCLayerMultiplex::create(pLayer1, pLayer2, pLayer3, pLayer4, NULL);
    pScene->addChild(layer, 0);
    
    pLayer1->release();
    pLayer2->release();
    pLayer3->release();
    pLayer4->release();
    
    toneGenHelp->removeTone(0);
    toneGenHelp->playBackgroundMusic("echelon.wav");
    ((MainMenu *) pScene)->setToneGenerator(toneGenHelp);
    ((MainMenu *) pScene)->setDataStore(dataStoreHandler);
    CCDirector::sharedDirector()->replaceScene(pScene);
}

void SpacePhysicsLayer::nextLevel()
{
    if (radarRadius > winWidth/10) {
        radarRadius -= winWidth/10;
    } else {
        //enemyVelocity += 10;
    }
    
    stringstream tempString;
    tempString<<"Level " << level;
    tutorialText->setString(tempString.str().c_str());
    
    tutorialText->setOpacity(0);
    tutorialText->setVisible(true);
    CCFiniteTimeAction *fadeIn = CCFadeIn::create(1.0);
    fadeIn->setDuration(1.0);
    CCDelayTime *waitDelay = CCDelayTime::create(1.5);
    CCFiniteTimeAction *fadeOut = CCFadeOut::create(1.0);
    fadeOut->setDuration(1.0);
    
    tutorialText->setColor(ccc3(0,0,255));
    tutorialText->runAction(CCSequence::create (fadeIn,waitDelay,fadeOut,fadeIn,waitDelay,fadeOut,NULL));
    
    level++;
}

void SpacePhysicsLayer::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    const b2Manifold* manifold = contact->GetManifold();
    
    if (manifold->pointCount == 0)
    {
        return;
    }
    
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
    b2GetPointStates(state1, state2, oldManifold, manifold);
    
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    
    for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
    {
        ContactPoint* cp = m_points + m_pointCount;
        cp->fixtureA = fixtureA;
        cp->fixtureB = fixtureB;
        cp->position = worldManifold.points[i];
        cp->normal = worldManifold.normal;
        cp->state = state2[i];
        ++m_pointCount;
    }
}


iOSBridge::ToneGeneratorHelper* SpacePhysicsScene::getToneGenerator()
{
    return this->toneGenHelp;
}

void SpacePhysicsScene::setToneGenerator(iOSBridge::ToneGeneratorHelper * gen)
{
    this->toneGenHelp = gen;
}

iOSBridge::DataStore* SpacePhysicsScene::getDataStore()
{
    return this->dataStoreHandler;
}

void SpacePhysicsScene::setDataStore(iOSBridge::DataStore * gen)
{
    this->dataStoreHandler = gen;
}

void SpacePhysicsScene::runGame()
{
    CCLayer* pLayer = new SpacePhysicsLayer();
    
    switch (gameMode) {
        case 1:
            ((SpacePhysicsLayer *) pLayer)->setAccelerometerEnabled(true);
            break;
            
        default:
            break;
    }
    
    addChild(pLayer);
    pLayer->autorelease();
    
}

SpacePhysicsScene::SpacePhysicsScene()
{
    CCScene::init();
    
    pauseButton = CCMenuItemImage::create("glyphicons_174_pause.png", "glyphicons_174_pause.png", this, menu_selector(SpacePhysicsScene::pauseMenuCallback) );
    pauseButton->setScale(2.0);
    CCMenu *pMenu =CCMenu::create(pauseButton, NULL);
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    pMenu->setPosition(CCPointZero);
    pauseButton->setPosition( ccp(s.width*0.95,s.height*0.95) );
    
    addChild(pMenu, 1);
    
}


void SpacePhysicsScene::MainMenuCallback(CCObject* pSender)
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

void SpacePhysicsScene::pauseMenuCallback(CCObject* pSender)
{
    CCLayer* pauseLayer = new PauseSceneLayer2();
    addChild(pauseLayer);
    ((CCMenuItem *)pSender)->setEnabled(false);
    RecursivelyPauseAllChildren((CCNode *)this->getChildren()->objectAtIndex(0));
    ((SpacePhysicsScene *)(this->getChildren()->objectAtIndex(0)))->pauseSchedulerAndActions();
    this->getToneGenerator()->disableTones();
}

void SpacePhysicsScene::RecursivelyPauseAllChildren( CCNode * node ) {
    node->pauseSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyPauseAllChildren(n);
    }
}

void SpacePhysicsScene::RecursivelyResumeAllChildren( CCNode * node ) {
    node->resumeSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyResumeAllChildren(n);
    }
}

void DestructionListener::SayGoodbye(b2Joint* joint)
{
    test->JointDestroyed(joint);
}

PauseSceneLayer2::PauseSceneLayer2()
{
    // Label Item (CCLabelBMFont)
    CCMenuItemFont* item1 = CCMenuItemFont::create("Resume", this, menu_selector(PauseSceneLayer2::resumeCallback));
    
    
    CCMenuItemFont::setFontName("PressStart2P-Regular");
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont*title2 = CCMenuItemFont::create("Effects");
    title2->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle* item2 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(PauseSceneLayer2::fxCallback),
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
                                                                 menu_selector(PauseSceneLayer2::musicCallback),
                                                                 CCMenuItemFont::create( "On" ),
                                                                 CCMenuItemFont::create( "Off"),
                                                                 NULL );
    
    
    CCMenuItemFont* item4 = CCMenuItemFont::create("Quit", this, menu_selector(PauseSceneLayer2::MainMenuCallback));
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

PauseSceneLayer2::~PauseSceneLayer2()
{
    
}

void PauseSceneLayer2::resumeCallback(CCObject* pSender)
{
    ((SpaceScene *)(this->getParent()))->RecursivelyResumeAllChildren(this->getParent());
    ((SpaceScene *)(this->getParent()))->getToneGenerator()->enableTones();
    ((SpaceScene *)(this->getParent()))->pauseButton->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}

void PauseSceneLayer2::MainMenuCallback(CCObject* pSender)
{
    SpaceScene *parent = (SpaceScene *)this->getParent();
    
    //parent->sendData();
    
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

void PauseSceneLayer2::musicCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->disableBackground();
    } else {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->enableBackground();
    }
}

void PauseSceneLayer2::fxCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->disableTones();
    } else {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->enableTones();
    }
}


