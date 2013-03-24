#include "CountingGame.h"
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

float CountingGameLayer::sysVer = 0.0;
bool CountingGameLayer::accelEnable;

CountingGameLayer::CountingGameLayer(void)
{
    //////////////////////////////
    // 1. super init first
    
    // Get window sizes from director
    winHeight = CCDirector::sharedDirector()->getWinSize().height;
    winWidth = CCDirector::sharedDirector()->getWinSize().width;
    
    //////////////////////////////
    //
    //  Labels & UI
    //
    //////////////////////////////
    
    CCSprite *cat = CCSprite::create("cat.png");
    cat->setPosition(ccp(winWidth/2,winHeight/2));
    addChild(cat);
    
    happyEmoticon = CCSprite::create("happy.png");
    happyEmoticon->setPosition(ccp(winWidth/2, 0.65*winHeight));
    addChild(happyEmoticon);
    happyEmoticon->setVisible(false);
    
    sadEmoticon = CCSprite::create("sad.png");
    sadEmoticon->setPosition(ccp(winWidth/2, 0.65*winHeight));
    addChild(sadEmoticon);
    sadEmoticon->setVisible(false);


    
    oneLabel = CCLabelTTF::create("1", "PressStart2P-Regular", 12.0);
    twoLabel = CCLabelTTF::create("2", "PressStart2P-Regular", 12.0);
    tutorialText = CCLabelTTF::create("", "Audiowide-Regular", 30.0);
    
    oneButton = CCMenuItemImage::create("launchBtn.png", "launchBtnWhenPressed.png", this, menu_selector(CountingGameLayer::pressedOne));
    oneButton->setScale(0.1);
    twoButton = CCMenuItemImage::create("launchBtn.png", "launchBtnWhenPressed.png", this, menu_selector(CountingGameLayer::pressedTwo));
    twoButton->setScale(0.1);
    
    CCMenu *menu = CCMenu::create(oneButton,twoButton, NULL);
    
    addChild(menu);

    oneButton->setPosition(ccp(-winWidth*0.1,0));
    twoButton->setPosition(ccp(winWidth*0.1,0));
    menu->setPosition(ccp(winWidth*0.5, winHeight*0.3));
    
    oneLabel->setPosition(ccp(winWidth*0.4,winHeight*0.4));
    this->addChild(oneLabel, 1);
    oneLabel->setVisible(true);
    
    twoLabel->setPosition(ccp(winWidth*0.6, winHeight*0.4));
    this->addChild(twoLabel, 1);
    twoLabel->setVisible(true);
    
    tutorialText->setPosition(ccp(winWidth*0.5, winHeight*0.6));
    
    stringstream tempString;
    tempString<<"How many tones do you hear?";
    tutorialText->setString(tempString.str().c_str());
    tutorialText->setColor(ccc3(255, 0, 0));
    tutorialText->setHorizontalAlignment(kCCTextAlignmentCenter);
    this->addChild(tutorialText);
    tutorialText->setVisible(false);
}

CountingGameLayer::~CountingGameLayer(void)
{
}

void CountingGameLayer::onEnter()
{
    CCLayer::onEnter();
    
    this->toneGenHelp = ((CountingGame *)this->getParent())->getToneGenerator();
    this->toneGenHelp->playBackgroundMusic("main_background.wav");
    //this->toneGenHelp->enableTones();
    this->dataStoreHandler = ((CountingGame *)this->getParent())->getDataStore();
    this->setTouchEnabled(true); // Enable Touch
    this->searchHelper = new MLSearch();
    searchHelper->setMode(1);

    
    this->scheduleUpdate(); // Start updating
    seed_freq = floorf(randomValueBetween(0.0,1550.0));
    
    tutorialDuration = 10 * 1000.0;
    radarRadius = winWidth/2;
    enemyVelocity = winWidth/20.0;
    playedTutorial = false;
    if (!playedTutorial) {
        this->scheduleOnce(schedule_selector(CountingGameLayer::playTutorial), 2.0);
        this->scheduleOnce(schedule_selector(CountingGameLayer::endTutorial), 35.0);
    }
    
    _nextAsteroid = 0;
    _curAsteroidCount = 0;
    
    for (int i=0; i < 10; i++) {
        baseVolumes[i] = 0.5; //dB vol
        volDiffs[i] = -0.1;
    }
    this->scheduleOnce(schedule_selector(CountingGameLayer::playTones), 3.0);
}

void CountingGameLayer::update(float dt) {
    
    //float curTimeMillis = getTimeTick();
    //CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
}

void CountingGameLayer::draw(){
}

void CountingGameLayer::playTutorial()
{
    
    CCLabelTTF* skipLabel = CCLabelTTF::create("Skip", "Ubuntu-Regular", 12);
    CCMenuItemLabel *tutorialSkipButtonItem = CCMenuItemLabel::create(skipLabel, this, menu_selector(CountingGameLayer::endTutorial));
    tutorialSkipButton = CCMenu::create(tutorialSkipButtonItem,NULL);
    
    tutorialSkipButton->setPosition(ccp(winWidth*0.5,winHeight*0.2));
    tutorialSkipButton->setVisible(true);
    addChild(tutorialSkipButton);
    
    CCFiniteTimeAction *fadeIn = CCFadeIn::create(1.0);
    fadeIn->setDuration(1.0);
    CCDelayTime *waitDelay = CCDelayTime::create(1.5);
    CCFiniteTimeAction *fadeOut = CCFadeOut::create(1.0);
    fadeOut->setDuration(1.0);
    tutorialText->setOpacity(0);
    tutorialText->setVisible(true);;
    
    tutorialText->runAction(CCSequence::create (fadeIn,waitDelay,fadeOut,fadeIn,waitDelay,fadeOut,NULL));
    CCFiniteTimeAction *initialDelay = CCDelayTime::create(5.0);
    fadeIn->setDuration(1.0);
    waitDelay = CCDelayTime::create(3.0);
}

void CountingGameLayer::endTutorial()
{
    playedTutorial = true;
    tutorialSkipButton->setVisible(false);
    removeChild(tutorialSkipButton, true);

    this->unschedule(schedule_selector(CountingGameLayer::endTutorial));
    tutorialText->stopAllActions();
}

void CountingGameLayer::playTones()
{
    numTones = (rand() % 2);
    earIndex = 1;
    freqIndex = 3;
    volDelta = 0.0;
    this->schedule(schedule_selector(CountingGameLayer::scheduleTone), 2.0, numTones, 0.0);
    searchHelper->cur_freq = freqIndex;
}

void CountingGameLayer::scheduleTone()
{
    this->scheduleOnce(schedule_selector(CountingGameLayer::playSingleTone), 0.5);
    this->scheduleOnce(schedule_selector(CountingGameLayer::stopTones), 1.5);
}

void CountingGameLayer::playSingleTone()
{
    float randFrequency = freqIndex * seed_freq + 250.0;    // Calculate frequency
    baseVol = ((float) (pow(10.0,(baseVolumes[earIndex*5 + freqIndex] - 128.0))));
    toneGenHelp->playConstantTone(randFrequency, baseVolumes[earIndex*5 + freqIndex] + volDelta, earIndex);   // Play pure-tone
    
    volDelta = volDiffs[5*earIndex + freqIndex];
}

void CountingGameLayer::stopTones(){
    toneGenHelp->removeTone(0);
}

float CountingGameLayer::randomValueBetween( float low , float high ) {
    return (((float) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

double CountingGameLayer::randomValueBetweenD( double low , double high ) {
    return (((double) arc4random() / 0xFFFFFFFFu) * (high - low)) + low;
}

float CountingGameLayer::getTimeTick() {
    timeval time;
    gettimeofday(&time, NULL);
    unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    return (float) millisecs;
}


void CountingGameLayer::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    
}
void CountingGameLayer::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    CCPoint mean_touch;
    CCSetIterator iter = touches->begin();
    for (; iter != touches->end(); iter++) // Iterate through touch list, only get first touch?
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        CCPoint location = pTouch->getLocation();
        
        mean_touch = location; // Take mean of multi-touch later if needed
        
    }
}

void CountingGameLayer::ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    CCPoint mean_touch;
    CCSetIterator iter = touches->begin();
    for (; iter != touches->end(); iter++) // Iterate through touch list, only get first touch?
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        CCPoint location = pTouch->getLocation();
        
        mean_touch = location; // Take mean of multi-touch later if needed
        
    }
    
    if (touchTimerFlag) {
        touchTime = getTimeTick() - initTime; // Initial Reaction Time
        touchTimerFlag = false;
    }
}

void CountingGameLayer::menuCloseCallback(CCObject* pSender)
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

void CountingGameLayer::pressedOne()
{
    if (numTones == 0) {
        happyEmoticon->setVisible(true);sadEmoticon->setVisible(false);
        //volDiffs[earIndex*5+freqIndex] += searchHelper->getNextTone(baseVolumes[earIndex*5 + freqIndex] + volDiffs[earIndex*5 + freqIndex], earIndex, true);
    } else {
        happyEmoticon->setVisible(false);sadEmoticon->setVisible(true);
        volDiffs[earIndex*5+freqIndex] += searchHelper->getNextTone(baseVolumes[earIndex*5 + freqIndex] + volDiffs[earIndex*5 + freqIndex], earIndex, false);
        dataStoreHandler->saveData("Counting_Game",(double) (freqIndex * seed_freq + 250.0),  (double) (baseVolumes[earIndex*5 + freqIndex] + volDiffs[earIndex*5 + freqIndex]), earIndex, 0,0,0);

    }
    CCLog("Index: %d, Vol: %f", earIndex*5 + freqIndex, volDiffs[earIndex*5 + freqIndex]);
    playTones();
}

void CountingGameLayer::pressedTwo()
{
    if (numTones == 1) {
        happyEmoticon->setVisible(true);sadEmoticon->setVisible(false);
        volDiffs[earIndex*5 + freqIndex] += searchHelper->getNextTone(baseVolumes[earIndex*5 + freqIndex] + volDiffs[earIndex*5 + freqIndex], earIndex, true);
        dataStoreHandler->saveData("Counting_Game",(double) (freqIndex * seed_freq + 250.0),  (double) (baseVolumes[earIndex*5 + freqIndex] + volDiffs[earIndex*5 + freqIndex]), earIndex, 0,0,0);
    } else {
        happyEmoticon->setVisible(false);sadEmoticon->setVisible(true);
        //volDiffs[earIndex*5 + freqIndex] += searchHelper->getNextTone(baseVolumes[earIndex*5 + freqIndex] + volDiffs[earIndex*5 + freqIndex], earIndex, false);
    }
    CCLog("Index: %d, Vol: %f", earIndex*5 + freqIndex, volDiffs[earIndex*5 + freqIndex]);
    playTones();
}

PauseSceneLayer3::PauseSceneLayer3()
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

PauseSceneLayer3::~PauseSceneLayer3()
{
    
}

void PauseSceneLayer3::resumeCallback(CCObject* pSender)
{
    ((CountingGame *)(this->getParent()))->RecursivelyResumeAllChildren(this->getParent());
    ((CountingGame *)(this->getParent()))->getToneGenerator()->enableTones();
    ((CountingGame *)(this->getParent()))->pauseButton->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}

void PauseSceneLayer3::MainMenuCallback(CCObject* pSender)
{
    CountingGame *parent = (CountingGame *)this->getParent();
    
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

void PauseSceneLayer3::musicCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((CountingGame *)(this->getParent()))->getToneGenerator()->disableBackground();
    } else {
        ((CountingGame *)(this->getParent()))->getToneGenerator()->enableBackground();
    }
}

void PauseSceneLayer3::fxCallback(CCObject *sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        ((CountingGame *)(this->getParent()))->getToneGenerator()->disableTones();
    } else {
        ((CountingGame *)(this->getParent()))->getToneGenerator()->enableTones();
    }
}

iOSBridge::ToneGeneratorHelper* CountingGame::getToneGenerator()
{
    return this->toneGenHelp;
}

void CountingGame::setToneGenerator(iOSBridge::ToneGeneratorHelper * gen)
{
    this->toneGenHelp = gen;
}

iOSBridge::DataStore* CountingGame::getDataStore()
{
    return this->dataStoreHandler;
}

void CountingGame::setDataStore(iOSBridge::DataStore * gen)
{
    this->dataStoreHandler = gen;
}

void CountingGame::runGame()
{
    CCLayer* pLayer = new CountingGameLayer();
    
    switch (gameMode) {
        case 1:
            ((CountingGameLayer *) pLayer)->setAccelerometerEnabled(false);
            break;
            
        default:
            break;
    }
    
    addChild(pLayer);
    pLayer->autorelease();
    
}

CountingGame::CountingGame()
{
    CCScene::init();
    
    pauseButton = CCMenuItemImage::create("glyphicons_174_pause.png", "glyphicons_174_pause.png", this, menu_selector(CountingGame::pauseMenuCallback) );
    pauseButton->setScale(2.0);
    CCMenu *pMenu =CCMenu::create(pauseButton, NULL);
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    pMenu->setPosition(CCPointZero);
    pauseButton->setPosition( ccp(s.width*0.95,s.height*0.95) );
    
    addChild(pMenu, 1);
}

void CountingGame::MainMenuCallback(CCObject* pSender)
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

void CountingGame::pauseMenuCallback(CCObject* pSender)
{
    CCLayer* pauseLayer = new PauseSceneLayer();
    addChild(pauseLayer);
    ((CCMenuItem *)pSender)->setEnabled(false);
    RecursivelyPauseAllChildren((CCNode *)this->getChildren()->objectAtIndex(0));
    ((CountingGame *)(this->getChildren()->objectAtIndex(0)))->pauseSchedulerAndActions();
    this->getToneGenerator()->disableTones();
}

void CountingGame::RecursivelyPauseAllChildren( CCNode * node ) {
    node->pauseSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyPauseAllChildren(n);
    }
}

void CountingGame::RecursivelyResumeAllChildren( CCNode * node ) {
    node->resumeSchedulerAndActions();
    
    CCObject * obj;
    CCARRAY_FOREACH(node -> getChildren(), obj) {
        CCNode * n = (CCNode *)obj;
        RecursivelyResumeAllChildren(n);
    }
}
