#include "SpaceScene.h"
#include "MainMenu.h"
#include "SimpleAudioEngine.h"
#include "ToneGeneratorHelper.h"
#include "Ship.h"
#include "cocos2d.h"
#include <sstream>

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
    tutorialText = CCLabelTTF::create("", "Audiowide-Regular", 30.0);
    tutorialText2 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    tutorialText3 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    tutorialText4 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    tutorialText5 = CCLabelTTF::create("", "PressStart2P-Regular", 8.0);
    

    distLabel->setPosition(ccp(50,20));
    this->addChild(distLabel, 1);
    distLabel->setVisible(false);
    
    scoreLabel->setPosition(ccp(70, 40));
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
    tempString<<"Lower frequencies will come from your left.";
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
    tempString<<"Avoid shooting the red boxes!";
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
    
    win_height = winWidth;
    
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
    _spacedust1 = CCSprite::create("space.jpeg");
    _spacedust2 = CCSprite::create("space.jpeg");
    _planetsunrise = CCSprite::create("bg_planetsunrise.png");
    _galaxy = CCSprite::create("bg_galaxy.png");
    _spacialanomaly = CCSprite::create("bg_spacialanomaly.png");
    _spacialanomaly2 = CCSprite::create("bg_spacialanomaly2.png");
    
    // 3) Determine relative movement speeds for space dust and background
    CCPoint dustSpeed = ccp(0.1, 0.1);
    CCPoint bgSpeed = ccp(0.05, 0.05);
    
    // 4) Add children to CCParallaxNode
    _spacedust1->setRotation(90.0f);
    _spacedust2->setRotation(90.0f);
    _backgroundNode->addChild(_spacedust1, -2 , dustSpeed, ccp(0,0) ); // 2
    _backgroundNode->addChild(_spacedust2, -2 , dustSpeed, ccp(0,winWidth));
    
    _backgroundNode->addChild(_galaxy,-1, bgSpeed , ccp(winWidth * 0.7,0));
    _backgroundNode->addChild(_planetsunrise,-1 , bgSpeed,ccp(winWidth * 0.8,800));
    _backgroundNode->addChild(_spacialanomaly,-1, bgSpeed,ccp(winWidth * 0.3,1000));
    _backgroundNode->addChild(_spacialanomaly2,-1, bgSpeed,ccp(winWidth * 0.9,1200));
    CCSprite *hp = CCSprite::create("hp.png");
    hpBar = CCProgressTimer::create(hp);
    hpBar->setType(kCCProgressTimerTypeBar);
    hpBar->setMidpoint(ccp(0,0));
    //    Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
    hpBar->setBarChangeRate(ccp(1, 0));
    txtWindow = CCSprite::create("futureui1.png");
    healthBar = CCSprite::create("health.png");
    txtWindow->setScale(0.27);

    txtWindow->setPosition(ccp(winHeight*0.23,winWidth*0.1));
    healthBar->setPosition(ccp(winHeight*0.13,winWidth*0.25));
    hpBar->setPosition(ccp(winHeight*0.13,winWidth*0.24));
    hpBar->setScaleX(0.9);
    hpBar->setScaleY(0.25);
    addChild(txtWindow);
    addChild(healthBar);
    addChild(hpBar);
    
    hpBar->setPercentage(100.0);
    
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
    
    for(int i = 0; i < KNUMENEMIES; i++) {
        CCSprite *enemy = CCSprite::create();
        frame = frameCache->spriteFrameByName("slice_4_2.png");
        enemy->setDisplayFrame(frame);
        enemy->setVisible(false);
        addChild(enemy);
        _enemies->addObject(enemy);
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
    this->toneGenHelp->playBackgroundMusic("main_background.wav");
    this->setTouchEnabled(true); // Enable Touch
    this->setAccelerometerEnabled(SpaceSceneLayer::accelEnable);
    
    this->scheduleUpdate(); // Start updating
    dataStoreHandler = new iOSBridge::DataStore();
    seed_freq = floorf(randomValueBetween(250.0,300.0));
    
    tutorialDuration = 10 * 1000.0;
    playedTutorial = true;
    if (!playedTutorial) {
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::playTutorial), 2.0);
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::endTutorial), 35.0);
    }
    initTime = getTimeTick();
    
    _nextAsteroid = 0;
    _curAsteroidCount = 0;
}

void SpaceSceneLayer::update(float dt) {
    
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
        float size = spaceDust->getContentSize().width ;
        if ( xPosition < -size/2) {
            _backgroundNode->incrementOffset(ccp(0,2*spaceDust->getContentSize().width-20.0),spaceDust) ;
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
        float size = background->getContentSize().width;
        if ( xPosition < -size/2) {
            _backgroundNode->incrementOffset(ccp(0,2*winWidth + background->getContentSize().width),background);
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
        
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawnEnemy), 2.0 - spawnRate);
    }
    
    if ((_curAsteroidCount < KNUMASTEROIDS || _nextAsteroid != 0 )&& playedTutorial) {
        _curAsteroidCount++;
        float spawnRate = randomValueBetween(0.1,0.3);
        
        this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawnAsteroid), spawnRate);
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
                shipLaser->setVisible(false);
                enemy->setVisible(false);
                setEnemyInvisible(enemy);
                _backgroundNode->removeChild(enemy, 1);
                toneGenHelp->removeTone(index);
                score += 10;
                enemySpawned = false;
                //dataStoreHandler->saveData((double) sineTones[index], (double) tmpVol);
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
    
    
    float maxY = winSize.height - playerShip->getContentSize().height/2;
    float minY = playerShip->getContentSize().height/2;
    
    float diff = (_shipPointsPerSecY * dt) ;
    float newY = playerShip->getPosition().y + diff;
    newY = MIN(MAX(newY, minY), maxY);
    playerShip->setPosition(ccp(playerShip->getPosition().x, newY));
    
    if (playedTutorial) {
        distance++;
    }
    stringstream tempString;
    tempString<<distance<<"m";
    distLabel->setString(tempString.str().c_str());

    tempString.str("");
    tempString<<"Score: "<< score;
    scoreLabel->setString(tempString.str().c_str());
}

void SpaceSceneLayer::draw(){
    // Draw Grid Lines
    
    glEnable(GL_BLEND); // Enable blending for alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glLineWidth(2);
    ccDrawColor4F(0.0, 1.0, 0.0, 0.3);
    ccDrawCircle( playerShip->getPosition(), 25, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 50, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 75, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 100, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 125, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 150, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 175, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 200, 0, 50, false);
    ccDrawCircle( playerShip->getPosition(), 250, 0, 50, false);
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(9*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(8*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(7*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(6*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(5*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(4*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(3*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(2*winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight/10,winWidth));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth));
    ccDrawColor4F(0.5, 1.0, 0.5, 0.3);
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.1));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.1));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.2));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.2));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.3));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.3));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.4));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.4));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.5));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.5));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.6));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.6));
    ccDrawLine(playerShip->getPosition(), ccp(winHeight,winWidth*0.7));
    ccDrawLine(playerShip->getPosition(), ccp(0,winWidth*0.7));
    
    int i, n = 120;
    float da, ga, a, b;
    float radius = 40;
    int dt = distance % n;
    const float TWOPI = 3.1415926535 * 2.0;
    
    da = TWOPI / (float)n;
    
    for (i = 0 + dt; i < n + dt; i++)  {
        a = (float)i * da;
        b = a + da;
        ga = 0.90 * (float)(i) / (float)n;
        CCPoint filledVertices[] = { playerShip->getPosition(), ccpAdd(playerShip->getPosition(),ccp(radius * cos(a), radius * sin(a))), ccpAdd(playerShip->getPosition(),ccp(radius * cos(b), radius * sin(b)))};
        ccDrawSolidPoly(filledVertices, 3, ccc4f(0.0,ga, 0.0, ga/3));
    }
}

void SpaceSceneLayer::playTutorial()
{
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
    initialDelay->setDuration(13.5);
    tutorialText3->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText3->runAction(CCSequence::create(CCDelayTime::create(17.0),fadeOut,NULL));
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawnLowFreqEnemy), 13.5);
    initialDelay->setDuration(24.0);
    tutorialText4->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText4->runAction(CCSequence::create(CCDelayTime::create(27.0),fadeOut,NULL));
    this->scheduleOnce(schedule_selector(SpaceSceneLayer::spawnHighFreqEnemy), 24.0);
    initialDelay->setDuration(30.0);
    tutorialText5->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText5->runAction(CCSequence::create(CCDelayTime::create(32.0),fadeOut,NULL));
}

void SpaceSceneLayer::endTutorial()
{
    playedTutorial = true;
    distLabel->setVisible(true);
    scoreLabel->setVisible(true);
}

void SpaceSceneLayer::spawnEnemy()
{
    double randDuration = 17.0;
    int multiple = (int) floorf(randomValueBetweenD(1.0,11.0)); // Only generate harmonic pure-tones
    float randFrequency = multiple * seed_freq;
    float randY = winHeight/10 * (multiple-1);
    
    float visibleDelay = 10.0;
    
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);
    enemy->setOpacity(0);
    
    sineTones[_nextAsteroid] = randFrequency;
    toneGenHelp->addTone(randFrequency, 10.0, 0); // Add pure-tone
    
    enemy->stopAllActions();
    CCAnimation *normal = animCache->animationByName("enemy");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCDelayTime *fadeDelay = CCDelayTime::create(visibleDelay);
    CCFadeIn *fade = CCFadeIn::create(0.01);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);

    timeTargets[_nextAsteroid] = randDuration;

    //CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,playerShip->getPosition()),CCCallFuncN::create(this,callfuncN_selector(SpaceSceneLayer::setEnemyInvisible)),NULL);
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,playerShip->getPosition()),NULL);
    enemy->setPosition( ccp(randY,winWidth+enemy->getContentSize().width/2));
    enemy->setRotation(-45.0 + (randY/winHeight)*90.0f);
    enemy->runAction (moveSeq);
    enemy->runAction(seq);
    enemy->runAction(fadeAnim);
    enemy->setVisible(true);
}

void SpaceSceneLayer::spawnEnemyAtLoc(float y)
{
    double randDuration = randomValueBetweenD(8.0,9.0);
    int multiple = y; // Only generate harmonic pure-tones
    float randFrequency = multiple * seed_freq;
    float randY = y * winHeight/10;
    
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);
    enemy->setOpacity(0);
    
    sineTones[_nextAsteroid] = randFrequency;
    toneGenHelp->addTone(randFrequency, randDuration, 0); // Add pure-tone
    
    enemy->stopAllActions();
    CCAnimation *normal = animCache->animationByName("enemy");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCDelayTime *fadeDelay = CCDelayTime::create(3.0);
    CCFadeIn *fade = CCFadeIn::create(randDuration-3.0);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);
    
    timeTargets[_nextAsteroid] = randDuration;
    CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                      ccp(winHeight * 0.5,winWidth * 0.1)),CCCallFuncN::create(this,callfuncN_selector(SpaceSceneLayer::setEnemyInvisible)),NULL);
    enemy->setPosition(ccp(randY,winWidth+enemy->getContentSize().width/2));
    enemy->setRotation(-45.0 + (randY/winHeight)*90.0f);
    enemy->runAction (moveSeq);
    enemy->runAction(seq);
    enemy->runAction(fadeAnim);
    enemy->setVisible(true);
}

void SpaceSceneLayer::spawnLowFreqEnemy()
{
    spawnEnemyAtLoc(1);
}

void SpaceSceneLayer::spawnHighFreqEnemy()
{
    spawnEnemyAtLoc(9);
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
                                              CCCallFuncN::create(this,callfuncN_selector(SpaceSceneLayer::setInvisible)) ,
                                              NULL  // DO NOT FORGET TO TERMINATE WITH NULL
                                              ) ) ;
}

void SpaceSceneLayer::didAccelerate(CCAcceleration* pAccelerationValue) {
#define KFILTERINGFACTOR 0.1
#define KRESTACCELX -0.0
#define KSHIPMAXPOINTSPERSEC (winSize.height*0.5)
#define KMAXDIFFX 0.2
    
    double rollingX ;
    
    // Cocos2DX inverts X and Y accelerometer depending on device orientation
    // in landscape mode right x=-y and y=x !!! (Strange and confusing choice)
    pAccelerationValue->x = pAccelerationValue->y ;
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

void SpaceSceneLayer::menuCloseCallback(CCObject* pSender)
{
    CCScene* pScene = MainMenu::create();
    CCLayer* pLayer1 = new MainMenuLayer();
    CCLayer* pLayer2 = new OptionsLayer();
    CCLayer* pLayer3 = new StatsLayer();
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
    CCDirector::sharedDirector()->replaceScene(pScene);
}

iOSBridge::ToneGeneratorHelper* SpaceScene::getToneGenerator()
{
    return this->toneGenHelp;
}

void SpaceScene::setToneGenerator(iOSBridge::ToneGeneratorHelper * gen)
{
    this->toneGenHelp = gen;
}

void SpaceScene::runGame()
{
    CCLayer* pLayer = new SpaceSceneLayer();
    addChild(pLayer);
    pLayer->autorelease();
    
    CCDirector::sharedDirector()->replaceScene((CCScene *)this);
}

SpaceScene::SpaceScene()
{
    CCScene::init();
        
    //add the menu item for back to main menu
    //#if (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
    //    CCLabelBMFont* label = CCLabelBMFont::create("MainMenu",  "fonts/arial16.fnt");
    //#else
    CCLabelTTF* label = CCLabelTTF::create("MainMenu", "Arial", 20);
    //#endif
    CCMenuItemLabel* pMenuItem = CCMenuItemLabel::create(label, this, menu_selector(SpaceScene::MainMenuCallback));
    
    CCMenu* pMenu =CCMenu::create(pMenuItem, NULL);
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    pMenu->setPosition( CCPointZero );
    pMenuItem->setPosition( CCPointMake( s.width - 50, 25) );
    
    addChild(pMenu, 1);
}

void SpaceScene::MainMenuCallback(CCObject* pSender)
{
    CCScene* pScene = MainMenu::create();
    CCLayer* pLayer1 = new MainMenuLayer();
    CCLayer* pLayer2 = new OptionsLayer();
    CCLayer* pLayer3 = new StatsLayer();
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
    CCDirector::sharedDirector()->replaceScene(pScene);
}
