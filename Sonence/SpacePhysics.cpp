#include "SpacePhysics.h"
#include "SimpleAudioEngine.h"
#include "MainMenu.h"
#include "ToneGeneratorHelper.h"
#include "cocos2d.h"
#include "AudiogramScene.h"

#include <sstream>

#define USE_CHUNKED 1

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

float SpacePhysicsLayer::sysVer = 0.0;
bool SpacePhysicsLayer::accelEnable;

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
};


EnemySprite::EnemySprite()
: m_pBody(NULL)
{
    
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
    
    tutorialText->setPosition(ccp(winHeight/2, winWidth/2 ));
    tutorialText2->setPosition(ccp(winHeight/2, winWidth/2 ));
    tutorialText3->setPosition(ccp(winHeight/2, winWidth/2 ));
    tutorialText4->setPosition(ccp(winHeight/2, winWidth/2 ));
    tutorialText5->setPosition(ccp(winHeight/2, winWidth/2 ));
    
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
    playerShip = CCSprite::create();
    CCSpriteFrame *frame = frameCache->spriteFrameByName("slice_0_0.png");
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
    _asteroids = new CCArray();
    _enemies = new CCArray();
    _AIEnemies = new CCArray();
    
    //Set up sprite
#if 1
    // Use batch node. Faster
    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("slice_4_4.png", 100);
    m_pSpriteTexture = parent->getTexture();
#else
    // doesn't use batch node. Slower
    m_pSpriteTexture = CCTextureCache::sharedTextureCache()->addImage("slice_4_4.png");
    CCNode *parent = CCNode::create();
#endif
    addChild(parent, 0, kTagParentNode);
    
    for(int i = 0; i < KNUMENEMIES; i++) {
        EnemySprite *enemy = new EnemySprite();
        frame = frameCache->spriteFrameByName("slice_4_2.png");
        enemy->initWithTexture(m_pSpriteTexture);
        enemy->autorelease();
        parent->addChild(enemy);
        _enemies->addObject(enemy);
        // Define the dynamic body.
        //Set up a 1m squared box in the physics world
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(0.9*winHeight/PTM_RATIO,0.5*winWidth/PTM_RATIO);
        
        
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
        body->SetLinearVelocity(b2Vec2(-50.0f,0.0f));
        
        enemy->setPhysicsBody(body);
    }
    
    
    for(int i = 0; i < KNUMENEMIES; i++) {
        CCSprite *enemy = CCSprite::create("warrior1_0.png");
        enemy->setVisible(false);
        addChild(enemy);
        _AIEnemies->addObject(enemy);
    }
    
#define KNUMASTEROIDS 10
    _spawnQueue = new CCArray(KNUMASTEROIDS);
    for(int i = 0; i < KNUMASTEROIDS; ++i) {
        
        CCSprite *asteroid = CCSprite::create("slice_3_0.png");
        asteroid->setVisible(false);
        addChild(asteroid);
        _asteroids->addObject(asteroid);
        _spawnQueue->addObject(CCInteger::create(i));
        
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
    this->setTouchEnabled(true); // Enable Touch
    
    this->scheduleUpdate(); // Start updating
    seed_freq = floorf(randomValueBetween(20.0,4000.0));
    
    tutorialDuration = 10 * 1000.0;
    radarRadius = winWidth/2;
    enemyVelocity = winWidth/20.0;
    playedTutorial = false;
    if (!playedTutorial) {
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::playTutorial), 2.0);
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::endTutorial), 35.0);
    }
    
    _nextAsteroid = 0;
    _curAsteroidCount = 0;
}

void SpacePhysicsLayer::initPhysics()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    world = new b2World(gravity);
    
    
    // Do we want to let bodies sleep?
    world->SetAllowSleeping(true);
    
    world->SetContinuousPhysics(true);
         //m_debugDraw = new GLESDebugDraw( PTM_RATIO );
         //world->SetDebugDraw(m_debugDraw);
    
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    //m_debugDraw->SetFlags(flags);
    
}


void SpacePhysicsLayer::update(float dt) {
    
    float curTimeMillis = getTimeTick();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
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
        
        float spawnRate = curTimeMillis/5000.0;
        
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawnRandomEnemy), 2.0 - spawnRate);
        this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawnAIEnemy), 2.0 - spawnRate);
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
    CCObject *itAster, *itLaser;
    CCARRAY_FOREACH( _asteroids,itAster) {
        CCSprite *asteroid = (CCSprite *) itAster;
        if ( ! asteroid->isVisible() )
            continue ;
        CCARRAY_FOREACH(_shipLasers,itLaser) {
            CCSprite *shipLaser = (CCSprite *) itLaser;
            if ( ! shipLaser->isVisible() )
                continue ;
            if ( shipLaser->boundingBox().intersectsRect(asteroid->boundingBox()) ) {
                shipLaser->setVisible(false);
                asteroid->setVisible(false);
                setAsteroidInvisible(asteroid);
                _backgroundNode->removeChild(asteroid, 1);
                score -= 10;
                continue ;
            }
        }
        if ( playerShip->boundingBox().intersectsRect(asteroid->boundingBox()) ) {
            asteroid->setVisible(false);
            setAsteroidInvisible(asteroid);
            score += 1;
        }
    }
    
    // Enemy Collision
    CCObject *itEnemy;
    CCARRAY_FOREACH( _enemies,itEnemy) {
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
                dataStoreHandler->saveData("Moving_Invaders",(double) sineTones[index], (double) tmpVol, earIndex, touchAttempts,initTime, finalTime);
                
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

void SpacePhysicsLayer::draw(){
    // Draw Grid Lines
    
    glEnable(GL_BLEND); // Enable blending for alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glLineWidth(2);
    ccDrawColor4F(0.0, 1.0, 0.0, 0.3);
    for (int i = 0; i < 10; i++) {
        ccDrawCircle( playerShip->getPosition(), winWidth/10 * i, 0, 50, false);
    }
    
    float tmp = TWOPI / 40.0;
    int i, n = 120;
    float da, ga, a, b;
    int dt = (int)(getTimeTick()/10) % n;
    ccDrawColor4F(0.0, 1.0, 1.0, 0.3);
    for (int i = 4; i < 17; i++)  {
        a = (float)i * tmp;
        ccDrawLine(playerShip->getPosition(), ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(winWidth * cos(a), winWidth * sin(a))));
    }
    
    ccDrawColor4F(1.0, 0.0, 0.0, 1.0);
    ccDrawCircle(playerShip->getPosition(), radarRadius, 0, 50, false);
    
    // Radar Animation
    da = TWOPI / (float)n;
    
    for (i = 0 + dt; i < n/5 + dt; i++)  {
        a = (float)i * da;
        b = a + da;
        ga = 0.8*((float)(i-dt)/(float)(n/5));
        CCPoint filledVertices[] = { playerShip->getPosition(), ccpAdd(playerShip->getPosition(),ccp(radarRadius * cos(a), radarRadius * sin(a))), ccpAdd(playerShip->getPosition(),ccp(radarRadius * cos(b), radarRadius * sin(b)))};
        ccDrawSolidPoly(filledVertices, 3, ccc4f(0.0,ga, 0.0, ga));
    }
    
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
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn1_L), 15.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn2_L), 16.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn3_L), 17.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn4_L), 18.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn5_L), 19.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn1_R), 20.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn2_R), 21.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn3_R), 22.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn4_R), 23.0f);
    this->scheduleOnce(schedule_selector(SpacePhysicsLayer::spawn5_R), 24.0f);
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
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn1_L));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn2_L));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn3_L));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn4_L));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn5_L));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn1_R));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn2_R));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn3_R));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn4_R));
    this->unschedule(schedule_selector(SpacePhysicsLayer::spawn5_R));
    this->unschedule(schedule_selector(SpacePhysicsLayer::endTutorial));
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

void SpacePhysicsLayer::moveEnemy() {
    
    
    // Init enemy
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);
    enemy->setOpacity(255);
    enemy->stopAllActions();
    
    float visibleDelay = (ccpDistance(playerShip->getPosition(), enemy->getPosition()) - radarRadius)/enemyVelocity;
    
    // Animation seq
    CCAnimation *normal = animCache->animationByName("enemy");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    // Time appearance
    CCDelayTime *fadeDelay = CCDelayTime::create(visibleDelay);
    CCFadeIn *fade = CCFadeIn::create(0.01);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);
    
    double randDuration = ccpDistance(playerShip->getPosition(), enemy->getPosition())/enemyVelocity;
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,playerShip->getPosition()),NULL);
    
    enemy->runAction(moveSeq);
    enemy->setRotation(-45.0 + (enemyIndex/10.0)*90.0f);
    enemy->runAction(seq);
    enemy->runAction(fadeAnim);
    enemy->setVisible(true);
}

void SpacePhysicsLayer::spawnEnemy()
{
    spawnEnemyAtLoc(enemyIndex,earIndex);
}

void SpacePhysicsLayer::tutorialSpawn(int freqIndex, int earIndex)
{
    
    double randDuration = winWidth/enemyVelocity;   // Total travel duration
    float randFrequency = freqIndex * seed_freq + 250.0;    // Calculate frequency
    float visibleDelay = (winWidth - radarRadius)/enemyVelocity;
    
    float tmp = TWOPI / 40.0;
    EnemySprite *enemy = (EnemySprite *) _enemies->objectAtIndex(0);  // Grab enemy object
    enemy->stopAllActions();
    enemy->setOpacity(255);
    enemy->setVisible(true);
    if (earIndex) {
        enemy->setPosition(ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(winWidth/2 * cos(tmp*(freqIndex + 4)), winWidth/2 * sin(tmp*(freqIndex + 4)))));
        enemy->setRotation(45.0f - (freqIndex/10.0)*90.0f);
    } else {
        enemy->setPosition(ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(-winWidth/2 * cos(tmp*(freqIndex + 4)), winWidth/2 * sin(tmp*(freqIndex + 4)))));
        enemy->setRotation(-45.0f + (freqIndex/10.0)*90.0f);
    }
    sineTones[_nextAsteroid] = randFrequency;
    timeTargets[_nextAsteroid] = randDuration;
    toneGenHelp->addTone(randFrequency, visibleDelay, earIndex);   // Play pure-tone
    toneGenHelp->maxToneVolume();
    
    CCAnimation *normal = animCache->animationByName("enemy"); // Animation
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCDelayTime *fadeDelay = CCDelayTime::create(visibleDelay); // Invisibilty timer
    CCFadeIn *fade = CCFadeIn::create(0.01);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,playerShip->getPosition()),NULL); // Movement Action
    
    // Run All Actions
    enemy->runAction (moveSeq);
    enemy->runAction(seq);
    enemy->runAction(fadeAnim);
    
}

void SpacePhysicsLayer::spawnRandomEnemy()
{
    enemyIndex = (int) floorf(randomValueBetweenD(0.0,6.0)); // Only generate harmonic pure-tones
    earIndex = (int)(randomValueBetweenD(0.0,2.0));
    spawnEnemyAtLoc(enemyIndex,earIndex);
    touchTimerFlag = true;
    initTime = getTimeTick();
    touchAttempts = 0;
    
}

void SpacePhysicsLayer::spawn1_L()
{
    tutorialSpawn(1,0);
}
void SpacePhysicsLayer::spawn2_L()
{
    tutorialSpawn(2,0);
}
void SpacePhysicsLayer::spawn3_L()
{
    tutorialSpawn(3,0);
}
void SpacePhysicsLayer::spawn4_L()
{
    tutorialSpawn(4,0);
}
void SpacePhysicsLayer::spawn5_L()
{
    tutorialSpawn(5,0);
}
void SpacePhysicsLayer::spawn1_R()
{
    tutorialSpawn(1,1);
}
void SpacePhysicsLayer::spawn2_R()
{
    tutorialSpawn(2,1);
}
void SpacePhysicsLayer::spawn3_R()
{
    tutorialSpawn(3,1);
}
void SpacePhysicsLayer::spawn4_R()
{
    tutorialSpawn(4,1);
}
void SpacePhysicsLayer::spawn5_R()
{
    tutorialSpawn(5,1);
}

void SpacePhysicsLayer::spawnLowFreqEnemy()
{
    spawnEnemyAtLoc(1,0);
}

void SpacePhysicsLayer::spawnHighFreqEnemy()
{
    spawnEnemyAtLoc(5,1);
}

void SpacePhysicsLayer::spawnEnemyAtLoc(int freqIndex, int earIndex)
{
    double randDuration = winWidth/enemyVelocity;   // Total travel duration
    float randFrequency = freqIndex * seed_freq + 250.0;    // Calculate frequency
    float visibleDelay = (winWidth - radarRadius)/enemyVelocity;
    
    float tmp = TWOPI / 40.0;
    EnemySprite *enemy = (EnemySprite *) _enemies->objectAtIndex(0);  // Grab enemy object
    //enemy->initWithTexture(m_pSpriteTexture);
    //enemy->autorelease();
    enemy->stopAllActions();
    enemy->setOpacity(255);
    enemy->setVisible(true);
    
    //b2BodyDef bodyDef;
    //bodyDef.type = b2_dynamicBody;
    if (earIndex) {
        //bodyDef.position.Set((winHeight * 0.5+winWidth * cos(tmp*(freqIndex + 4)))/PTM_RATIO,(winWidth * 0.1 + winWidth * sin(tmp*(freqIndex + 4)))/PTM_RATIO);
        enemy->getPhysicsBody()->SetTransform(b2Vec2((winHeight * 0.5+winWidth * cos(tmp*(freqIndex + 4)))/PTM_RATIO,(winWidth * 0.1 + winWidth * sin(tmp*(freqIndex + 4)))/PTM_RATIO), 0.0);
        enemy->setPosition(ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(winWidth * cos(tmp*(freqIndex + 4)), winWidth * sin(tmp*(freqIndex + 4)))));
        enemy->setRotation(45.0f - (freqIndex/10.0)*90.0f);
    } else {
        //bodyDef.position.Set((winHeight * 0.5-winWidth * cos(tmp*(freqIndex + 4)))/PTM_RATIO,(winWidth * 0.1 + winWidth * sin(tmp*(freqIndex + 4)))/PTM_RATIO);
        enemy->getPhysicsBody()->SetTransform(b2Vec2((winHeight * 0.5-winWidth * cos(tmp*(freqIndex + 4)))/PTM_RATIO,(winWidth * 0.1 + winWidth * sin(tmp*(freqIndex + 4)))/PTM_RATIO), 0.0);
        enemy->setPosition(ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(-winWidth * cos(tmp*(freqIndex + 4)), winWidth * sin(tmp*(freqIndex + 4)))));
        enemy->setRotation(-45.0 + (freqIndex/10.0)*90.0f);
    }
    sineTones[_nextAsteroid] = randFrequency;
    timeTargets[_nextAsteroid] = randDuration;
    toneGenHelp->addTone(randFrequency, visibleDelay, earIndex);   // Play pure-tone
    
    //CCAnimation *normal = animCache->animationByName("enemy"); // Animation
    //normal->setRestoreOriginalFrame(true);
    //CCAnimate *animN = CCAnimate::create(normal);
    //CCRepeatForever *seq = CCRepeatForever::create(animN);
    /*
    CCDelayTime *fadeDelay = CCDelayTime::create(visibleDelay); // Invisibilty timer
    CCFadeIn *fade = CCFadeIn::create(0.01);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,playerShip->getPosition()),NULL); // Movement Action
    */
    
    //b2Body *body = world->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    //b2PolygonShape dynamicBox;
    //dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box
    
    // Define the dynamic body fixture.
    //b2FixtureDef fixtureDef;
    //fixtureDef.shape = &dynamicBox;
    //fixtureDef.density = 1.0f;
    //fixtureDef.friction = 0.3f;
    //body->CreateFixture(&fixtureDef);
    //body->SetLinearVelocity(b2Vec2(-50.0f,0.0f));
    //body->SetTransform(b2Vec2(winHeight/2,winWidth/2), 0.0);
    
    b2Vec2 enemyPos = b2Vec2(enemy->getPosition().x,enemy->getPosition().y);
    b2Vec2 shipPos = b2Vec2(playerPos.x,playerPos.y);
    b2Vec2 velocity = enemyPos - shipPos;
    velocity.Normalize();
    b2Body *body = enemy->getPhysicsBody();
    body->SetLinearVelocity(-1*velocity);
    enemy->setPhysicsBody(body);
    
    // Run All Actions
    //enemy->runAction (moveSeq);
    //enemy->runAction(seq);
    //enemy->runAction(fadeAnim);
    //parent->addChild(enemy);
}

void SpacePhysicsLayer::spawnAIEnemy()
{
    CCSprite *aiEnemy = (CCSprite *) _AIEnemies->objectAtIndex(0);
    aiEnemy->stopAllActions();
    aiEnemy->setOpacity(255);
    aiEnemy->setVisible(true);
    aiEnemy->setRotation(90.0f);
    
    aiEnemy->setPosition(ccp(winHeight/2,winWidth/2));
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(10.0,ccp(0,winHeight/2)),CCMoveTo::create(10.0,ccp(winWidth,winHeight/2)),NULL);
    
    CCRepeatForever *moveLoop = CCRepeatForever::create((CCActionInterval *)moveSeq);
    aiEnemy->runAction(moveLoop);
}

void SpacePhysicsLayer::spawnAsteroid()
{
    double randDuration = randomValueBetweenD(2.0,3.0);
    int multiple = (int) floorf(randomValueBetweenD(3.0,11.0)); // Only generate harmonic pure-tones
    float randY = winHeight/10.0 * multiple;
    
    CCInteger *randomIndex = (CCInteger *) _spawnQueue->randomObject();
    
    CCSprite *asteroid = (CCSprite *) _asteroids->objectAtIndex(randomIndex->getValue());
    asteroid->stopAllActions();
    asteroid->setTag(randomIndex->getValue());
    CCAnimation *normal = animCache->animationByName("asteroid");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                      ccp(randY,0)),CCCallFuncN::create(this,callfuncN_selector(SpacePhysicsLayer::setAsteroidInvisible)),NULL);
    asteroid->setPosition( ccp(randY,winWidth));
    asteroid->setRotation(-(randY/winHeight)*90.0f - 225.0f);
    asteroid->runAction (moveSeq);
    asteroid->runAction(seq);
    
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
    //enemySpawned = false;
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
    
    // Flame Collision
    CCObject *itFlame;
    CCARRAY_FOREACH( _enemies,itFlame) {
        CCParticleSystem *emitter = (CCParticleSystem *) itFlame;
        if ( ! emitter->isVisible() )
            continue ;
        emitter->stopAllActions();
        emitter->runAction(CCMoveTo::create(8,playerShip->getPosition()));
    }
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
    CCLayer* pauseLayer = new PauseSceneLayer();
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
