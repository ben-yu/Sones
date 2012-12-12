#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "ToneGeneratorHelper.h"
#include "Ship.h"
#include <sstream>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

float HelloWorld::sysVer = 0.0;
bool HelloWorld::accelEnable;

CCScene* HelloWorld::scene(float ver, bool accelEnabled, void *rootVC)
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // Save OS version
    sysVer = ver;
    HelloWorld::accelEnable = accelEnabled;
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();
    layer->dataStoreHandler->rootVCPtr = rootVC;
    
    // add layer as a child to scene
    scene->addChild(layer);
        
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    // Get window sizes from director
    if (sysVer < 6.0) {
        winWidth = CCDirector::sharedDirector()->getWinSize().width;
        winHeight = CCDirector::sharedDirector()->getWinSize().height;
    } else {
        winWidth = CCDirector::sharedDirector()->getWinSize().height;
        winHeight = CCDirector::sharedDirector()->getWinSize().width;
    }
    
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
    
    if (sysVer < 6.0) {
        distLabel->setRotation(90.0f);
        distLabel->setPosition(ccp(20,winHeight - 60));
    } else {
        distLabel->setPosition(ccp(50,40));
    }
    this->addChild(distLabel, 1);
    distLabel->setVisible(false);
    
    
    if (sysVer < 6.0) {
        scoreLabel->setRotation(90.0f);
        scoreLabel->setPosition(ccp(40, winHeight - 80));
    } else {
        scoreLabel->setPosition(ccp(70, 40));
    }
    this->addChild(scoreLabel, 1);
    scoreLabel->setVisible(false);
    
    if (sysVer < 6.0) {
        tutorialText->setRotation(90.0f);
        tutorialText->setPosition(ccp(winWidth/2, winHeight/2));
    } else {
        tutorialText->setPosition(ccp(winHeight/2, winWidth/2 ));
    }
    if (sysVer < 6.0) {
        tutorialText2->setRotation(90.0f);
        tutorialText2->setPosition(ccp(winWidth/2, winHeight/2));
    } else {
        tutorialText2->setPosition(ccp(winHeight/2, winWidth/2 ));
    }
    if (sysVer < 6.0) {
        tutorialText3->setRotation(90.0f);
        tutorialText3->setPosition(ccp(winWidth/2, winHeight/2));
    } else {
        tutorialText3->setPosition(ccp(winHeight/2, winWidth/2 ));
    }
    if (sysVer < 6.0) {
        tutorialText4->setRotation(90.0f);
        tutorialText4->setPosition(ccp(winWidth/2, winHeight/2));
    } else {
        tutorialText4->setPosition(ccp(winHeight/2, winWidth/2 ));
    }
    
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
    tempString<<"Lower frequencies will come from your right.";
    tutorialText3->setString(tempString.str().c_str());
    tutorialText3->setColor(ccc3(0, 255, 0));
    tutorialText3->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText3);
    tutorialText3->setVisible(false);
    
  
    tempString.str("");
    tempString<<"Higher frequencies will come from your left.";
    tutorialText4->setString(tempString.str().c_str());
    tutorialText4->setColor(ccc3(0, 255, 0));
    tutorialText4->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText4);
    tutorialText4->setVisible(false);
    
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
    if (sysVer < 6.0) {
        playerPos = ccp(winWidth * 0.1, winHeight * 0.5);
        playerShip->setPosition(playerPos);
        playerShip->setRotation(90.0f);
    } else {
        playerPos = ccp(winHeight * 0.5, winWidth * 0.1);
        playerShip->setPosition(playerPos);
    }
    
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
    if (sysVer < 6.0) {
        _backgroundNode->addChild(_spacedust1, -2 , dustSpeed, ccp(0,0) ); // 2
        _backgroundNode->addChild(_spacedust2, -2 , dustSpeed, ccp(winWidth,0));
        
        _backgroundNode->addChild(_galaxy,-1, bgSpeed , ccp(0,winWidth * 0.7));
        _backgroundNode->addChild(_planetsunrise,-1 , bgSpeed,ccp(800,winWidth * 0.8));
        _backgroundNode->addChild(_spacialanomaly,-1, bgSpeed,ccp(900,winWidth * 0.3));
        _backgroundNode->addChild(_spacialanomaly2,-1, bgSpeed,ccp(1000,winWidth * 0.9));
    } else {
        _spacedust1->setRotation(90.0f);
        _spacedust2->setRotation(90.0f);
        _backgroundNode->addChild(_spacedust1, -2 , dustSpeed, ccp(0,0) ); // 2
        _backgroundNode->addChild(_spacedust2, -2 , dustSpeed, ccp(0,winWidth));
        
        _backgroundNode->addChild(_galaxy,-1, bgSpeed , ccp(winWidth * 0.7,0));
        _backgroundNode->addChild(_planetsunrise,-1 , bgSpeed,ccp(winWidth * 0.8,800));
        _backgroundNode->addChild(_spacialanomaly,-1, bgSpeed,ccp(winWidth * 0.3,1000));
        _backgroundNode->addChild(_spacialanomaly2,-1, bgSpeed,ccp(winWidth * 0.9,1200));
    }
    CCSprite *hp = CCSprite::create("hp.png");
    hpBar = CCProgressTimer::create(hp);
    hpBar->setType(kCCProgressTimerTypeBar);
    if (sysVer < 6.0) {
        //    Setup for a bar starting from the left since the midpoint is 0 for the x
        hpBar->setMidpoint(ccp(0,0));
        //    Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
        hpBar->setBarChangeRate(ccp(1, 0));
    } else {
        hpBar->setMidpoint(ccp(0,0));
        //    Setup for a horizontal bar since the bar change rate is 0 for y meaning no vertical change
        hpBar->setBarChangeRate(ccp(0, 1));
    }
    txtWindow = CCSprite::create("futureui1.png");
    healthBar = CCSprite::create("health.png");
    txtWindow->setScale(0.27);
    if (sysVer < 6.0) {
        txtWindow->setRotation(90.0f);
        healthBar->setRotation(90.0f);
        hpBar->setRotation(90.0f);
        txtWindow->setPosition(ccp(winWidth*0.1,winHeight*0.76));
        healthBar->setPosition(ccp(winWidth*0.25,winHeight*0.85));
        hpBar->setPosition(ccp(winWidth*0.24,winHeight*0.85));
    }else {
        txtWindow->setPosition(ccp(winHeight*0.23,winWidth*0.1));
        healthBar->setPosition(ccp(winHeight*0.13,winWidth*0.25));
        hpBar->setPosition(ccp(winHeight*0.13,winWidth*0.24));
    }
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
    
    this->scheduleOnce(schedule_selector(HelloWorld::playTutorial), 2.0);
    this->scheduleOnce(schedule_selector(HelloWorld::endTutorial), 35.0);
    this->setTouchEnabled(true); // Enable Touch
    this->setAccelerometerEnabled(HelloWorld::accelEnable);
    
    this->scheduleUpdate(); // Start updating
    toneGenHelp = new iOSBridge::ToneGeneratorHelper(KNUMENEMIES); // Start MOMU
    dataStoreHandler = new iOSBridge::DataStore();
    seed_freq = floorf(randomValueBetween(250.0,300.0));
    
    tutorialDuration = 10 * 1000.0;
    playedTutorial = false;
    initTime = getTimeTick();
    
    _nextAsteroid = 0;
    _curAsteroidCount = 0;
    
    return true;
}

void HelloWorld::update(float dt) {
    
    float curTimeMillis = getTimeTick();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    if (_health <= 0) {
        stringstream tempString;
        tempString<<"GAME OVER!";
        tutorialText->setString(tempString.str().c_str());
        
        CCFiniteTimeAction *fadeIn = CCFadeIn::create(1.0);
        //fadeIn->setDuration(1.0);
        CCDelayTime *waitDelay = CCDelayTime::create(0.5);
        CCFiniteTimeAction *fadeOut = CCFadeOut::create(1.0);
        tutorialText->setVisible(true);
        tutorialText->setOpacity(1.0);
        tutorialText->runAction(CCSequence::create(fadeIn,waitDelay,fadeOut,NULL));
        runAction(CCRipple3D::create(CCPointMake(winWidth/2,winHeight/2), 50.0, 4.0, 50.0, ccg(32,24), 5));
        this->scheduleOnce(schedule_selector(HelloWorld::menuCloseCallback), 15.0);
    }
    
    //////////////////////////////
    //
    //  Background Scrolling
    //
    //////////////////////////////
    
    CCPoint backgroundScrollVert;
    if (sysVer < 6.0) {
        backgroundScrollVert = ccp(-500,0) ;
    } else {
        backgroundScrollVert = ccp(0,-500) ;
    }
    _backgroundNode->setPosition(ccpAdd(_backgroundNode->getPosition(),ccpMult(backgroundScrollVert,dt)));
    
    CCArray *spaceDusts = CCArray::arrayWithCapacity(2) ;
    spaceDusts->addObject(_spacedust1) ;
    spaceDusts->addObject(_spacedust2) ;
    if (sysVer < 6.0) {
        for ( int ii = 0  ; ii < spaceDusts->count() ; ii++ ) {
            CCSprite * spaceDust = (CCSprite *)(spaceDusts->objectAtIndex(ii)) ;
            float xPosition = _backgroundNode->convertToWorldSpace(spaceDust->getPosition()).x  ;
            float size = spaceDust->getContentSize().width ;
            if ( xPosition < -size/2) {
                _backgroundNode->incrementOffset(ccp(2*spaceDust->getContentSize().width-20.0,0),spaceDust) ;
            }
        }
    } else {
        for ( int ii = 0  ; ii < spaceDusts->count() ; ii++ ) {
            CCSprite * spaceDust = (CCSprite *)(spaceDusts->objectAtIndex(ii)) ;
            float xPosition = _backgroundNode->convertToWorldSpace(spaceDust->getPosition()).y  ;
            float size = spaceDust->getContentSize().width ;
            if ( xPosition < -size/2) {
                _backgroundNode->incrementOffset(ccp(0,2*spaceDust->getContentSize().width-20.0),spaceDust) ;
            }
        }
    }
    
    CCArray *backGrounds = CCArray::arrayWithCapacity(4) ;
    backGrounds->addObject(_galaxy) ;
    backGrounds->addObject(_planetsunrise) ;
    backGrounds->addObject(_spacialanomaly) ;
    backGrounds->addObject(_spacialanomaly2) ;
    if (sysVer < 6.0) {
        for ( int ii = 0 ; ii <backGrounds->count() ; ii++ ) {
            CCSprite * background = (CCSprite *)(backGrounds->objectAtIndex(ii)) ;
            float xPosition = _backgroundNode->convertToWorldSpace(background->getPosition()).x ;
            float size = background->getContentSize().width ;
            if ( xPosition < -size/2) {
                _backgroundNode->incrementOffset(ccp(2*winWidth + background->getContentSize().width,0),background) ;
            }
        }
    } else {
        for ( int ii = 0 ; ii <backGrounds->count() ; ii++ ) {
            CCSprite * background = (CCSprite *)(backGrounds->objectAtIndex(ii)) ;
            float xPosition = _backgroundNode->convertToWorldSpace(background->getPosition()).y ;
            float size = background->getContentSize().width ;
            if ( xPosition < -size/2) {
                _backgroundNode->incrementOffset(ccp(0,2*winWidth + background->getContentSize().width),background) ;
            }
        }
    }
    
    //////////////////////////////
    //
    //  NPC Spawning
    //
    //////////////////////////////
    
    if (!enemySpawned && playedTutorial) {
        enemySpawned = true;
        
        float spawnRate = randomValueBetween(3.0,5.0) * 1000;
        _nextAsteroidSpawn = spawnRate + curTimeMillis;
        
        this->scheduleOnce(schedule_selector(HelloWorld::spawnEnemy), 2.0);
    }
    
    if ((_curAsteroidCount < KNUMASTEROIDS || _nextAsteroid != 0 )&& playedTutorial) {
        _curAsteroidCount++;
        float spawnRate = randomValueBetween(0.1,0.3);
        
        this->scheduleOnce(schedule_selector(HelloWorld::spawnAsteroid), spawnRate);
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
            if ( CCRect::CCRectIntersectsRect(shipLaser->boundingBox(), asteroid->boundingBox()) ) {
                shipLaser->setVisible(false);
                asteroid->setVisible(false);
                setAsteroidInvisible(asteroid);
                runAction(CCWaves3D::create(5, 40, ccg(15,10), 3));
                _backgroundNode->removeChild(asteroid, 1);
                continue ;
            }
        }
        if ( CCRect::CCRectIntersectsRect(playerShip->boundingBox(), asteroid->boundingBox()) ) {
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
            if ( CCRect::CCRectIntersectsRect(shipLaser->boundingBox(), enemy->boundingBox()) ) {
                shipLaser->setVisible(false);
                enemy->setVisible(false);
                setEnemyInvisible(enemy);
                _backgroundNode->removeChild(enemy, 1);
                float tmpVol = toneGenHelp->removeTone(index);
                score += 10;
                enemySpawned = false;
                dataStoreHandler->saveData((double) sineTones[index], (double) tmpVol);
                continue ;
            }
        }
        if ( CCRect::CCRectIntersectsRect(playerShip->boundingBox(), enemy->boundingBox()) ) {
            enemy->setVisible(false);
            setEnemyInvisible(enemy);
            _backgroundNode->removeChild(enemy, 1);
            float tmpVol = toneGenHelp->removeTone(index);
            //playerShip->runAction( CCBlink::create(1.0, 5));
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
    
    distance++;
    stringstream tempString;
    tempString<<distance<<"m";
    distLabel->setString(tempString.str().c_str());

    tempString.str("");
    tempString<<"Score: "<< score;
    scoreLabel->setString(tempString.str().c_str());
}

void HelloWorld::draw(){
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
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 9*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 8*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 7*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 6*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 5*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 4*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 3*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 2*winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, winHeight/10));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth, 0));
    ccDrawColor4F(0.5, 1.0, 0.5, 0.3);
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.1, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.1, 0));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.2, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.2, 0));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.3, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.3, 0));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.4, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.4, 0));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.5, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.5, 0));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.6, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.6, 0));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.7, winHeight));
    ccDrawLine(playerShip->getPosition(), ccp(winWidth*0.7, 0));

}

void HelloWorld::playTutorial()
{
    CCFiniteTimeAction *fadeIn = CCFadeIn::create(1.0);
    fadeIn->setDuration(1.0);
    CCDelayTime *waitDelay = CCDelayTime::create(0.5);
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
    
    tutorialText->runAction(CCSequence::create (fadeIn,waitDelay,fadeOut,fadeIn,waitDelay,fadeOut,NULL));
    CCFiniteTimeAction *initialDelay = CCDelayTime::create(5.0);
    fadeIn->setDuration(1.0);
    waitDelay = CCDelayTime::create(3.0);
    tutorialText2->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText2->runAction(CCSequence::create(CCDelayTime::create(10.0),fadeOut,NULL));
    initialDelay->setDuration(13.5);
    tutorialText3->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText3->runAction(CCSequence::create(CCDelayTime::create(15.0),fadeOut,NULL));
    this->scheduleOnce(schedule_selector(HelloWorld::spawnLowFreqEnemy), 13.5);
    initialDelay->setDuration(24.0);
    tutorialText4->runAction(CCSequence::create (initialDelay,fadeIn,NULL));
    tutorialText4->runAction(CCSequence::create(CCDelayTime::create(25.0),fadeOut,NULL));
    this->scheduleOnce(schedule_selector(HelloWorld::spawnHighFreqEnemy), 24.0);
}

void HelloWorld::endTutorial()
{
    playedTutorial = true;
    distLabel->setVisible(true);
    scoreLabel->setVisible(true);
}

void HelloWorld::spawnEnemy()
{
    double randDuration = randomValueBetweenD(8.0,9.0);
    int multiple = (int) floorf(randomValueBetweenD(1.0,11.0)); // Only generate harmonic pure-tones
    float randFrequency = multiple * seed_freq;
    float randY = winHeight/10 * (multiple-1);
    float randAsteroidPos = randomValueBetween(0.0,winWidth);
    
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);
    enemy->setOpacity(0);
    
    sineTones[_nextAsteroid] = randFrequency;
    toneGenHelp->addTone(randFrequency, 0.06, 0); // Add pure-tone
    
    enemy->stopAllActions();
    CCAnimation *normal = animCache->animationByName("enemy");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCDelayTime *fadeDelay = CCDelayTime::create(3.0);
    CCFadeIn *fade = CCFadeIn::create(randDuration-3.0);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);

    timeTargets[_nextAsteroid] = randDuration;
    if (sysVer < 6.0) {
        CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                          ccp(winWidth * 0.1,winHeight * 0.5)),CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setEnemyInvisible)),NULL);
        enemy->setPosition( ccp(winWidth+enemy->getContentSize().width/2,randY));
        enemy->setRotation(-(randY/winHeight)*90.0f - 225.0f);
        enemy->runAction (moveSeq);
        enemy->runAction(seq);
        enemy->runAction(fadeAnim);
    } else {
        CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                          ccp(winWidth * 0.5,winHeight * 0.1)),CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setEnemyInvisible)),NULL);
        enemy->setPosition( ccp(randAsteroidPos,winWidth+enemy->getContentSize().width/2));
        enemy->runAction (moveSeq);
        enemy->runAction(seq);
    }
    enemy->setVisible(true);
}

void HelloWorld::spawnEnemyAtLoc(float y)
{
    double randDuration = randomValueBetweenD(8.0,9.0);
    int multiple = y; // Only generate harmonic pure-tones
    float randFrequency = multiple * seed_freq;
    float randY = y * winHeight/10;
    float randAsteroidPos = randomValueBetween(0.0,winWidth);
    
    CCSprite *enemy = (CCSprite *) _enemies->objectAtIndex(0);
    enemy->setOpacity(0);
    
    sineTones[_nextAsteroid] = randFrequency;
    toneGenHelp->addTone(randFrequency, 0.06, 0); // Add pure-tone
    
    enemy->stopAllActions();
    CCAnimation *normal = animCache->animationByName("enemy");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    CCDelayTime *fadeDelay = CCDelayTime::create(3.0);
    CCFadeIn *fade = CCFadeIn::create(randDuration-3.0);
    CCFiniteTimeAction *fadeAnim = CCSequence::create(fadeDelay,fade,NULL);
    
    timeTargets[_nextAsteroid] = randDuration;
    if (sysVer < 6.0) {
        CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                          ccp(winWidth * 0.1,winHeight * 0.5)),CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setEnemyInvisible)),NULL);
        enemy->setPosition( ccp(winWidth+enemy->getContentSize().width/2,randY));
        enemy->setRotation(-(randY/winHeight)*90.0f - 225.0f);
        enemy->runAction (moveSeq);
        enemy->runAction(seq);
        enemy->runAction(fadeAnim);
    } else {
        CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                          ccp(winWidth * 0.5,winHeight * 0.1)),CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setEnemyInvisible)),NULL);
        enemy->setPosition( ccp(randAsteroidPos,winWidth+enemy->getContentSize().width/2));
        enemy->runAction (moveSeq);
        enemy->runAction(seq);
    }
    enemy->setVisible(true);
}

void HelloWorld::spawnLowFreqEnemy()
{
    spawnEnemyAtLoc(1);
}

void HelloWorld::spawnHighFreqEnemy()
{
    spawnEnemyAtLoc(9);
}

void HelloWorld::spawnAsteroid()
{
    double randDuration = randomValueBetweenD(2.0,3.0);
    int multiple = (int) floorf(randomValueBetweenD(1.0,11.0)); // Only generate harmonic pure-tones
    float randFrequency = multiple * seed_freq;
    float randY = winWidth/10.0 * multiple;
    float randAsteroidPos = randomValueBetween(0.0,winWidth);
    
    CCInteger *randomIndex = (CCInteger *) _spawnQueue->randomObject();
    
    CCSprite *asteroid = (CCSprite *) _asteroids->objectAtIndex(randomIndex->getValue());
    asteroid->stopAllActions();
    asteroid->setTag(randomIndex->getValue());
    CCAnimation *normal = animCache->animationByName("asteroid");
    normal->setRestoreOriginalFrame(true);
    CCAnimate *animN = CCAnimate::create(normal);
    CCRepeatForever *seq = CCRepeatForever::create(animN);
    
    if (sysVer < 6.0) {
        CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                          ccp(0,randY)),CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setAsteroidInvisible)),NULL);
        asteroid->setPosition( ccp(winWidth+asteroid->getContentSize().width/2,randY));
        asteroid->setRotation(-(randY/winHeight)*90.0f - 225.0f);
        asteroid->runAction (moveSeq);
        asteroid->runAction(seq);
    } else {
        CCFiniteTimeAction *moveSeq = CCSequence::create(CCMoveTo::create(randDuration,
                                                                          ccp(randY, 0)),CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setAsteroidInvisible)),NULL);
        asteroid->setPosition( ccp(randAsteroidPos,winWidth+asteroid->getContentSize().width/2));
        asteroid->runAction (moveSeq);
        asteroid->runAction(seq);
    }
    asteroid->setVisible(true);
    _spawnQueue->removeObject(randomIndex);
}

float HelloWorld::randomValueBetween( float low , float high ) {
    return (((float) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

double HelloWorld::randomValueBetweenD( double low , double high ) {
    return (((double) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

float HelloWorld::getTimeTick() {
    timeval time;
    gettimeofday(&time, NULL);
    unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (float) millisecs;
}

void HelloWorld::setInvisible(CCNode * node) {
    node->setVisible(false) ;
}

void HelloWorld::setAsteroidInvisible(CCNode * node) {
    node->setVisible(false);
    _spawnQueue->addObject(CCInteger::create(node->getTag()));
    _curAsteroidCount--;
}

void HelloWorld::setEnemyInvisible(CCNode * node) {
    node->setVisible(false);
    //enemySpawned = false;
}

void HelloWorld::setFireInvisible(CCNode *emitter){
    CCSize winSize = CCDirector::sharedDirector()->getWinSize() ;
    asteroidSpawned = false;
    emitter->setVisible(false);
    _backgroundNode->removeChild(emitter, 1);
    
}

void HelloWorld::stopTones(){
    toneGenHelp->stop();
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{

}
void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
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

void HelloWorld::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
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
                                              CCCallFuncN::create(this,callfuncN_selector(HelloWorld::setInvisible)) ,
                                              NULL  // DO NOT FORGET TO TERMINATE WITH NULL
                                              ) ) ;
}

void HelloWorld::didAccelerate(CCAcceleration* pAccelerationValue) {
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


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
