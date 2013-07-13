#include "SpaceScene.h"
#include "MainMenu.h"
#include "SimpleAudioEngine.h"
#include "ToneGeneratorHelper.h"
#include "Ship.h"
#include "cocos2d.h"
#include "AudiogramScene.h"
#include <sstream>

#define USE_CHUNKED 1

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

float SpaceSceneLayer::sysVer = 0.0;
bool SpaceSceneLayer::accelEnable;

SpaceSceneLayer::SpaceSceneLayer(void)
{
    //////////////////////////////
    // 1. super init first
    
    // Get window sizes from director
    winWidth = CCDirector::sharedDirector()->getWinSize().height;
    winHeight = CCDirector::sharedDirector()->getWinSize().width;
    
    //////////////////////////////
    //
    //  Labels & UI
    //
    //////////////////////////////

    score = 0;
    distLabel = CCLabelTTF::create("0m", "PressStart2P-Regular", 12.0);
    scoreLabel = CCLabelTTF::create("Score: 0", "PressStart2P-Regular", 12.0);
    livesLabel = CCLabelTTF::create("x 3", "PressStart2P-Regular", 12.0);
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
    
    livesLabel->setPosition(ccp(winHeight*0.2, winWidth*0.9));
    this->addChild(livesLabel, 1);
    livesLabel->setVisible(false);
    
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
    CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "enemy");
    
    
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
    
    CCSprite *lifeSprite = CCSprite::create();
    lifeSprite->setDisplayFrame(frame);
    lifeSprite->setPosition(ccp(winHeight*0.15, winWidth*0.9));
    lifeSprite->setScale(0.8);
    addChild(lifeSprite);
    lifeSprite->setVisible(true);
    
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
    hpBar->setPosition(ccp(winHeight*0.13,winWidth*0.96));
    hpBar->setScaleX(0.9);
    hpBar->setScaleY(0.25);
    
    healthBar = CCSprite::create("health.png");
    healthBar->setPosition(ccp(winHeight*0.13,winWidth*0.95));
    healthBar->setFlipY(true);
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
    
    for(int i = 0; i < KNUMENEMIES; i++) {
        CCSprite *enemy = CCSprite::create();
        frame = frameCache->spriteFrameByName("slice_4_2.png");
        enemy->setDisplayFrame(frame);
        enemy->setVisible(false);
        addChild(enemy);
        _enemies->addObject(enemy);
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

SpaceSceneLayer::~SpaceSceneLayer(void)
{
}

void SpaceSceneLayer::onEnter()
{
    CCLayer::onEnter();
    
    this->toneGenHelp = ((SpaceScene *)this->getParent())->getToneGenerator();
    this->toneGenHelp->playBackgroundMusic("OutThere_0.aiff");
    this->toneGenHelp->enableTones();
    this->dataStoreHandler = ((SpaceScene *)this->getParent())->getDataStore();
    this->setTouchEnabled(true); // Enable Touch
    
    this->scheduleUpdate(); // Start updating
    seed_freq = floorf(randomValueBetween(0.0,1550.0));
    
    tutorialDuration = 10 * 1000.0;
    radarRadius = winWidth/2;
    enemyVelocity = winWidth/20.0;
    playedTutorial = false;
    if (!playedTutorial) {
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::playTutorial), 2.0);
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::endTutorial), 35.0);
    }
    
    _nextAsteroid = 0;
    _curAsteroidCount = 0;
}

void SpaceSceneLayer::update(float dt) {
    
    float curTimeMillis = getTimeTick();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    if (lives <= 0 && !gameOver) {
        gameOver = true;
        tutorialText = CCLabelTTF::create("GAME OVER!", "Audiowide-Regular", 30.0);
        tutorialText->setPosition(ccp(winHeight/2, winWidth/2));
        tutorialText->setVisible(true);
        tutorialText->setOpacity(255);
        addChild(tutorialText);
        
        runAction(CCWaves3D::create(5, 40, ccg(15,10), 10));
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::menuCloseCallback), 8.0);
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
        
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawnRandomEnemy), 2.0 - spawnRate);
    }
    /*
    if ((_curAsteroidCount < KNUMASTEROIDS || _nextAsteroid != 0 )&& playedTutorial) {
        _curAsteroidCount++;
        float spawnRate = randomValueBetween(0.1,0.3);
        
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawnAsteroid), spawnRate);
    }
    */
    
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
        CCSprite *enemy = (CCSprite *) itEnemy;
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
                
                dataStoreHandler->saveData(1,(double) sineTones[index],  (double) (toneGenHelp->getVolume() * tmpVol), earIndex, touchAttempts,touchTime, finalTime);
                
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
            lives -= 1;
            //hpBar->setPercentage(_health);
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
    
    if (_health <= 0 && !reloading) {
        reloading = true;
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::reload),reloadTime);
        hpBar->runAction(CCProgressFromTo::create(reloadTime, 0.0, 100.0));
    }
    if (_health == 100) {
        reloading = false;
    }
        
    stringstream tempString;
    tempString<<distance<<"m";
    distLabel->setString(tempString.str().c_str());

    tempString.str("");
    tempString<<"Score: "<< score;
    scoreLabel->setString(tempString.str().c_str());
    
    tempString.str("");
    tempString<<" x "<< lives;
    livesLabel->setString(tempString.str().c_str());
    
    distLabel->setVisible(true);
    scoreLabel->setVisible(true);
    livesLabel->setVisible(true);
}

void SpaceSceneLayer::draw(){
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
    int dt = (int)(getTimeTick()/60.0) % n;
    ccDrawColor4F(0.0, 1.0, 1.0, 0.3);
    for (int i = 4; i < 17; i++)  {
        a = (float)i * tmp;
        ccDrawLine(playerShip->getPosition(), ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(winWidth * cos(a), winWidth * sin(a))));
    }
    
    ccDrawColor4F(1.0, 0.0, 0.0, 1.0);
    ccDrawCircle( playerShip->getPosition(), radarRadius, 0, 50, false );
    
    // Radar Animation
    da = TWOPI / (float)n;
    
    for (i = 0 + dt; i < n/5 + dt; i++)  {
        a = (float)i * da;
        b = a + da;
        ga = 0.8*((float)(i-dt)/(float)(n/5));
        CCPoint filledVertices[] = { playerShip->getPosition(), ccpAdd(playerShip->getPosition(),ccp(radarRadius * cos(a), radarRadius * sin(a))), ccpAdd(playerShip->getPosition(),ccp(radarRadius * cos(b), radarRadius * sin(b)))};
        ccDrawSolidPoly(filledVertices, 3, ccc4f(0.0,ga, 0.0, ga));
    }
}

void SpaceSceneLayer::playTutorial()
{
    distLabel->setVisible(false);
    scoreLabel->setVisible(false);
    
    CCLabelTTF* skipLabel = CCLabelTTF::create("Skip", "Ubuntu-Regular", 12);
    CCMenuItemLabel *tutorialSkipButtonItem = CCMenuItemLabel::create(skipLabel, this, menu_selector(SpaceSceneLayer::endTutorial));
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
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn1_L), 15.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn2_L), 16.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn3_L), 17.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn4_L), 18.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn5_L), 19.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn1_R), 20.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn2_R), 21.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn3_R), 22.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn4_R), 23.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn5_R), 24.0f);
    tutorialText3->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText3->runAction(CCSequence::create(CCDelayTime::create(14.5),fadeOut,NULL));
    initialDelay->setDuration(30.0);
    tutorialText5->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText5->runAction(CCSequence::create(CCDelayTime::create(32.0),fadeOut,NULL));
}

void SpaceSceneLayer::endTutorial()
{
    playedTutorial = true;
    tutorialSkipButton->setVisible(false);
    removeChild(tutorialSkipButton, true);
    distLabel->setVisible(true);
    scoreLabel->setVisible(true);
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn1_L));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn2_L));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn3_L));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn4_L));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn5_L));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn1_R));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn2_R));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn3_R));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn4_R));
    this->unschedule(schedule_selector(SpaceSceneLayer::spawn5_R));
    this->unschedule(schedule_selector(SpaceSceneLayer::endTutorial));
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

void SpaceSceneLayer::moveEnemy() {
    
    
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

void SpaceSceneLayer::reload() {
    _health = 100;
    reloading = false;
}

void SpaceSceneLayer::spawnEnemy()
{
    spawnEnemyAtLoc(enemyIndex,earIndex);
}

void SpaceSceneLayer::tutorialSpawn(int freqIndex, int earIndex)
{
    
    double randDuration = winWidth/enemyVelocity;   // Total travel duration
    float randFrequency = freqIndex * 1550.0 + seed_freq;    // Calculate frequency
    float visibleDelay = (winWidth - radarRadius)/enemyVelocity;
    
    float tmp = TWOPI / 40.0;
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);  // Grab enemy object
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

void SpaceSceneLayer::spawnRandomEnemy()
{
    enemyIndex = (int) floorf(randomValueBetweenD(0.0,6.0)); // Only generate harmonic pure-tones
    earIndex = (int)(randomValueBetweenD(0.0,2.0));
    spawnEnemyAtLoc(enemyIndex,earIndex);
    touchTimerFlag = true;
    initTime = getTimeTick();
    touchAttempts = 0;
    
}

void SpaceSceneLayer::spawn1_L()
{
    tutorialSpawn(1,0);
}
void SpaceSceneLayer::spawn2_L()
{
    tutorialSpawn(2,0);
}
void SpaceSceneLayer::spawn3_L()
{
    tutorialSpawn(3,0);
}
void SpaceSceneLayer::spawn4_L()
{
    tutorialSpawn(4,0);
}
void SpaceSceneLayer::spawn5_L()
{
    tutorialSpawn(5,0);
}
void SpaceSceneLayer::spawn1_R()
{
    tutorialSpawn(1,1);
}
void SpaceSceneLayer::spawn2_R()
{
    tutorialSpawn(2,1);
}
void SpaceSceneLayer::spawn3_R()
{
    tutorialSpawn(3,1);
}
void SpaceSceneLayer::spawn4_R()
{
    tutorialSpawn(4,1);
}
void SpaceSceneLayer::spawn5_R()
{
    tutorialSpawn(5,1);
}

void SpaceSceneLayer::spawnLowFreqEnemy()
{
    spawnEnemyAtLoc(1,0);
}

void SpaceSceneLayer::spawnHighFreqEnemy()
{
    spawnEnemyAtLoc(5,1);
}

void SpaceSceneLayer::spawnEnemyAtLoc(int freqIndex, int earIndex)
{
    double randDuration = winWidth/enemyVelocity;   // Total travel duration
    float randFrequency = freqIndex * 1550.0 + seed_freq;    // Calculate frequency
    float visibleDelay = (winWidth - radarRadius)/enemyVelocity;
    
    float tmp = TWOPI / 40.0;
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);  // Grab enemy object
    enemy->stopAllActions();
    enemy->setOpacity(0);
    enemy->setVisible(true);
    if (earIndex) {
        enemy->setPosition(ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(winWidth * cos(tmp*(freqIndex + 4)), winWidth * sin(tmp*(freqIndex + 4)))));
        enemy->setRotation(45.0f - (freqIndex/10.0)*90.0f);
    } else {
        enemy->setPosition(ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(-winWidth * cos(tmp*(freqIndex + 4)), winWidth * sin(tmp*(freqIndex + 4)))));
        enemy->setRotation(-45.0 + (freqIndex/10.0)*90.0f);
    }
    sineTones[_nextAsteroid] = randFrequency;
    timeTargets[_nextAsteroid] = randDuration;
    toneGenHelp->addTone(randFrequency, visibleDelay, earIndex);   // Play pure-tone
    
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

void SpaceSceneLayer::spawnAsteroid()
{
    double randDuration = randomValueBetweenD(2.0,3.0);
    int multiple = (int) floorf(randomValueBetweenD(3.0,11.0)); // Only generate harmonic pure-tones
    float randY = winWidth/10.0 * multiple;
    
    CCInteger *randomIndex = (CCInteger *) _spawnQueue->randomObject();
    
    CCSprite *asteroid = (CCSprite *) _asteroids->objectAtIndex(randomIndex->getValue());
    asteroid->stopAllActions();
    asteroid->setTag(randomIndex->getValue());
    CCAnimation *normal = animCache->animationByName("asteroid");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                      ccp(0,randY)),CCCallFuncN::create(this,callfuncN_selector(SpaceSceneLayer::setAsteroidInvisible)),NULL);
    asteroid->setPosition( ccp(winHeight,randY));
    asteroid->setRotation(-(randY/winHeight)*90.0f - 225.0f);
    asteroid->runAction (moveSeq);
    asteroid->runAction(seq);
    
    asteroid->setVisible(true);
    _spawnQueue->removeObject(randomIndex);
}


float SpaceSceneLayer::randomValueBetween( float low , float high ) {
    return (((float) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

double SpaceSceneLayer::randomValueBetweenD( double low , double high ) {
    return (((double) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

float SpaceSceneLayer::getTimeTick() {
    timeval time;
    gettimeofday(&time, NULL);
    unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (float) millisecs;
}

void SpaceSceneLayer::setInvisible(CCNode * node) {
    node->setVisible(false) ;
}

void SpaceSceneLayer::setAsteroidInvisible(CCNode * node) {
    node->setVisible(false);
    _spawnQueue->addObject(CCInteger::create(node->getTag()));
    _curAsteroidCount--;
}

void SpaceSceneLayer::setEnemyInvisible(CCNode * node) {
    node->setVisible(false);
    //enemySpawned = false;
}

void SpaceSceneLayer::setFireInvisible(CCNode *emitter){
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
    asteroidSpawned = false;
    emitter->setVisible(false);
    _backgroundNode->removeChild(emitter, 1);
    
}

void SpaceSceneLayer::stopTones(){
    toneGenHelp->stop();
}

void SpaceSceneLayer::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{

}
void SpaceSceneLayer::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
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

void SpaceSceneLayer::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
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
    if (_health > 0) {
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
                                                  CCMoveTo::create(0.5,ccpAdd(ccp(winHeight * 0.5, winWidth * 0.1),ccp(winWidth * cos(shot_angle), winWidth * sin(shot_angle)))),
                                                  CCCallFuncN::create(this,callfuncN_selector(SpaceSceneLayer::setInvisible)) ,
                                                  NULL  // DO NOT FORGET TO TERMINATE WITH NULL
                                                  ) ) ;
        _health -= 20;
        hpBar->setPercentage(_health);
    }
    
    if (touchTimerFlag) {
        touchTime = getTimeTick() - initTime; // Initial Reaction Time
        touchTimerFlag = false;
    }
    touchAttempts++;
}

void SpaceSceneLayer::menuCloseCallback(CCObject* pSender)
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
    toneGenHelp->playBackgroundMusic("OutThere_0.aiff");
    ((MainMenu *) pScene)->setToneGenerator(toneGenHelp);
    ((MainMenu *) pScene)->setDataStore(dataStoreHandler);
    CCDirector::sharedDirector()->replaceScene(pScene);
}

void SpaceSceneLayer::nextLevel()
{
    if (radarRadius > winWidth/10) {
        radarRadius -= winWidth/10;
        reloadTime += 1.0;
    } else {
        enemyVelocity += 10;
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
    
    seed_freq = floorf(randomValueBetween(20.0,4000.0));
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn1_L), 1.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn2_L), 2.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn3_L), 3.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn4_L), 4.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn5_L), 5.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn1_R), 6.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn2_R), 7.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn3_R), 8.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn4_R), 9.0f);
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawn5_R), 10.0f);
    
    
    level++;
}

PauseSceneLayer::PauseSceneLayer()
{
    // Label Item (CCLabelBMFont)
    CCMenuItemFont* item1 = CCMenuItemFont::create("Resume", this, menu_selector(PauseSceneLayer::resumeCallback));
    
    
    CCMenuItemFont::setFontName("PressStart2P-Regular");
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont*title2 = CCMenuItemFont::create("Effects");
    title2->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle* item2 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(PauseSceneLayer::fxCallback),
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
                                                                 menu_selector(PauseSceneLayer::musicCallback),
                                                                 CCMenuItemFont::create( "On" ),
                                                                 CCMenuItemFont::create( "Off"),
                                                                 NULL );
    
    
    CCMenuItemFont* item4 = CCMenuItemFont::create("Quit", this, menu_selector(PauseSceneLayer::MainMenuCallback));
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

PauseSceneLayer::~PauseSceneLayer()
{
    
}

void PauseSceneLayer::resumeCallback(CCObject* pSender)
{
    ((SpaceScene *)(this->getParent()))->RecursivelyResumeAllChildren(this->getParent());
    ((SpaceScene *)(this->getParent()))->getToneGenerator()->enableTones();
    ((SpaceScene *)(this->getParent()))->pauseButton->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}

void PauseSceneLayer::MainMenuCallback(CCObject* pSender)
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

void PauseSceneLayer::musicCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->disableBackground();
    } else {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->enableBackground();
    }
}

void PauseSceneLayer::fxCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->disableTones();
    } else {
        ((SpaceScene *)(this->getParent()))->getToneGenerator()->enableTones();
    }
}

iOSBridge::ToneGeneratorHelper* SpaceScene::getToneGenerator()
{
    return this->toneGenHelp;
}

void SpaceScene::setToneGenerator(iOSBridge::ToneGeneratorHelper * gen)
{
    this->toneGenHelp = gen;
}

iOSBridge::DataStore* SpaceScene::getDataStore()
{
    return this->dataStoreHandler;
}

void SpaceScene::setDataStore(iOSBridge::DataStore * gen)
{
    this->dataStoreHandler = gen;
}

void SpaceScene::runGame()
{
    CCLayer* pLayer = new SpaceSceneLayer();
    
    switch (gameMode) {
        case 1:
            ((SpaceSceneLayer *) pLayer)->setAccelerometerEnabled(false);
            break;
            
        default:
            break;
    }
    
    addChild(pLayer);
    pLayer->autorelease();
    
}

SpaceScene::SpaceScene()
{
    CCScene::init();
        
    pauseButton = CCMenuItemImage::create("glyphicons_174_pause.png", "glyphicons_174_pause.png", this, menu_selector(SpaceScene::pauseMenuCallback) );
    pauseButton->setScale(2.0);
    CCMenu *pMenu =CCMenu::create(pauseButton, NULL);
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    pMenu->setPosition(CCPointZero);
    pauseButton->setPosition( ccp(s.width*0.95,s.height*0.95) );
    
    addChild(pMenu, 1);
}

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    struct WriteThis *pooh = (struct WriteThis *)userp;
    
    if(size*nmemb < 1)
        return 0;
    
    if(pooh->sizeleft) {
        *(char *)ptr = pooh->readptr[0]; /* copy one single byte */
        cout << pooh->readptr[0] << "\n";
        pooh->readptr++;                 /* advance pointer */
        pooh->sizeleft--;                /* less data left */
        return 1;                        /* we return 1 byte at a time! */
    }
    
    return 0;                          /* no more data left to deliver */
}

int SpaceScene::sendData()
{
    CURL *curl;
    CURLcode res;
    
    struct WriteThis pooh;
    
    char json[100];
    sprintf(json, "{\"data_point\":{\"uuid\":%d,\"freq\":%f,\"game_type\":\"normal\",\"vol\":%f}}",12345,1000.0,1.0);
    pooh.readptr =  json;
    pooh.sizeleft = strlen(json);
    
    cout << "\n\n" << curl_easy_escape(curl,json,strlen(json)) << "\n\n";
    
    /* In windows, this will init the winsock stuff */
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    /* Check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_global_init() failed: %s\n",
                curl_easy_strerror(res));
        return 1;
    }
    
    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. */
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5000/data_points.json");
        
        /* Now specify we want to POST data */
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
                
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
                
        /* get verbose debug output please */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        
        /*
         If you use POST to a HTTP 1.1 server, you can send data without knowing
         the size before starting the POST if you use chunked encoding. You
         enable this by adding a header like "Transfer-Encoding: chunked" with
         CURLOPT_HTTPHEADER. With HTTP 1.0 or without chunked transfer, you must
         specify the size in the request.
         */
#ifdef USE_CHUNKED
        {
            struct curl_slist *chunk = NULL;
            
            //chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
            chunk = curl_slist_append(chunk, "content-type: application/json");
            chunk = curl_slist_append(chunk, "accept: application/json");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            /* use curl_slist_free_all() after the *perform() call to free this
             list again */
        }
#else
        /* Set the expected POST size. If you want to POST large amounts of data,
         consider CURLOPT_POSTFIELDSIZE_LARGE */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, pooh.sizeleft);
#endif
        
#ifdef DISABLE_EXPECT
        /*
         Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue"
         header.  You can disable this header with CURLOPT_HTTPHEADER as usual.
         NOTE: if you want chunked transfer too, you need to combine these two
         since you can only set one list of headers with CURLOPT_HTTPHEADER. */
        
        /* A less good option would be to enforce HTTP 1.0, but that might also
         have other implications. */
        {
            struct curl_slist *chunk = NULL;
            
            chunk = curl_slist_append(chunk, "Expect:");
            chunk = curl_slist_append(chunk, "Content-Type: application/json");
            res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            /* use curl_slist_free_all() after the *perform() call to free this
             list again */
        }
#endif
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        
        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}

void SpaceScene::MainMenuCallback(CCObject* pSender)
{
    sendData();
    
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

void SpaceScene::pauseMenuCallback(CCObject* pSender)
{
    CCLayer* pauseLayer = new PauseSceneLayer();
    addChild(pauseLayer);
    ((CCMenuItem *)pSender)->setEnabled(false);
    RecursivelyPauseAllChildren((CCNode *)this->getChildren()->objectAtIndex(0));
    ((SpaceScene *)(this->getChildren()->objectAtIndex(0)))->pauseSchedulerAndActions();
    this->getToneGenerator()->disableTones();
}

void SpaceScene::RecursivelyPauseAllChildren( CCNode * node ) {
    node->pauseSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyPauseAllChildren(n);
    }
}

void SpaceScene::RecursivelyResumeAllChildren( CCNode * node ) {
    node->resumeSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyResumeAllChildren(n);
    }
}
