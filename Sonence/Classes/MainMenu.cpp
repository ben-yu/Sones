//
//  MainMenu.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2012-12-22.
//
//

#include "MainMenu.h"
#include "SpaceScene.h"
#include "SpacePhysics.h"
#include "Catapult.h"
#include "CountingGame.h"

#define TRANSITION_DURATION (1.2f)

//------------------------------------------------------------------
//
// MainMenuLayer
//
//------------------------------------------------------------------

MainMenuLayer::MainMenuLayer()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCLabelTTF* title = CCLabelTTF::create("Sonic Intelligence", "Audiowide-Regular", floor(30 * s.width/640));
    title->setPosition(ccp(s.width * 0.32 ,s.height*0.9));
    
    CCLabelTTF* selectedText = CCLabelTTF::create("News / Updates", "ChelaOne-Regular", floor(14 * s.width/640));
    selectedText->setPosition(ccp(s.width * 0.7 ,s.height * 0.75));
    
    CCLabelTTF* label = CCLabelTTF::create("New Game", "Ubuntu-Regular", floor(14 * s.width/640));
    CCMenuItemLabel* pMenuItem = CCMenuItemLabel::create(label, this, menu_selector(MainMenuLayer::levelsCallback));
    CCLabelTTF* optionLabel = CCLabelTTF::create("Options", "Ubuntu-Regular", floor(14 * s.width/640));
    CCMenuItemLabel* optionMenuItem = CCMenuItemLabel::create(optionLabel, this, menu_selector(MainMenuLayer::optionsCallback));
    CCLabelTTF* statsLabel = CCLabelTTF::create("Stats", "Ubuntu-Regular", floor(14 * s.width/640));
    CCMenuItemLabel* statsMenuItem = CCMenuItemLabel::create(statsLabel, this, menu_selector(MainMenuLayer::statsCallback));
    CCLabelTTF* creditsLabel = CCLabelTTF::create("Credits", "Ubuntu-Regular", floor(14 * s.width/640));
    CCMenuItemLabel* creditsMenuItem = CCMenuItemLabel::create(creditsLabel, this, menu_selector(MainMenuLayer::creditsCallback));
    CCLabelTTF* exitLabel = CCLabelTTF::create("Exit", "Ubuntu-Regular", floor(14 * s.width/640));
    CCMenuItemLabel* exitMenuItem = CCMenuItemLabel::create(exitLabel, this, menu_selector(MainMenuLayer::exitCallback));
    
    CCMenu* pMenu =CCMenu::create(pMenuItem, NULL);
    pMenu->addChild(optionMenuItem);pMenu->addChild(statsMenuItem);pMenu->addChild(creditsMenuItem);pMenu->addChild(exitMenuItem);
    pMenu->setPosition(CCPointZero);
    pMenuItem->setPosition( CCPointMake( s.width/4, 6.3*s.height/10));
    optionMenuItem->setPosition( CCPointMake( s.width / 4, 5.0*s.height/10 ));
    statsMenuItem->setPosition( CCPointMake( s.width / 4, 3.7*s.height/10 ));
    creditsMenuItem->setPosition( CCPointMake( s.width / 4, 2.4*s.height/10 ));
    exitMenuItem->setPosition( CCPointMake( s.width / 4, 1*s.height/10 ));
    this->addChild(pMenu,1);
    this->addChild(title,1);
    this->addChild(selectedText,1);
}

void MainMenuLayer::draw(){
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    ccDrawColor4F(1.0, 1.0, 1.0, 1.0);
    glLineWidth(1);
    ccDrawLine(ccp(s.width/10.0,0), ccp(s.width/10.0, 7.0*s.height/10.0));
    ccDrawLine(ccp(4.0*s.width/10.0,0), ccp(4.0*s.width/10.0, 7.0*s.height/10.0));
    ccDrawLine(ccp(9.5*s.width/10.0,0), ccp(9.5*s.width/10.0, 8.0*s.height/10.0));
    ccDrawLine(ccp(5.0*s.width/10.0,8.0*s.height/10.0), ccp(5.0*s.width/10.0, 7.0*s.height/10.0));
    ccDrawLine(ccp(5.0*s.width/10.0,8.0*s.height/10.0), ccp(9.5*s.width/10.0, 8.0*s.height/10.0));
    ccDrawLine(ccp(s.width/10.0, 7.0*s.height/10.0), ccp(5.0*s.width/10.0, 7.0*s.height/10.0));
    ccDrawLine(ccp(s.width/10.0, 5.6*s.height/10.0), ccp(4.0*s.width/10.0, 5.6*s.height/10.0));
    ccDrawLine(ccp(s.width/10.0, 4.3*s.height/10.0), ccp(4.0*s.width/10.0, 4.3*s.height/10.0));
    ccDrawLine(ccp(s.width/10.0, 3.0*s.height/10.0), ccp(4.0*s.width/10.0, 3.0*s.height/10.0));
    ccDrawLine(ccp(s.width/10.0, 1.7*s.height/10.0), ccp(4.0*s.width/10.0, 1.7*s.height/10.0));
    
}

void MainMenuLayer::levelsCallback(CCObject* pSender){
    ((CCLayerMultiplex*)m_pParent)->switchTo(4);
}

void MainMenuLayer::optionsCallback(CCObject* pSender){
    ((CCLayerMultiplex*)m_pParent)->switchTo(1);
}

void MainMenuLayer::statsCallback(CCObject* pSender){
    ((CCLayerMultiplex*)m_pParent)->switchTo(2);
}

void MainMenuLayer::creditsCallback(CCObject* pSender){
    ((CCLayerMultiplex*)m_pParent)->switchTo(3);
}


void MainMenuLayer::exitCallback(CCObject* pSender){
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}


void MainMenuLayer::startGameCallback(CCObject* pSender)
{
    // get the userdata, it's the index of the menu item clicked
    //CCMenuItem* pMenuItem = (CCMenuItem *)(pSender);
    
    SpaceScene* pScene =  new SpaceScene();
    // fix bug #486, without setDepthTest(false), FlipX,Y will flickers
    CCDirector::sharedDirector()->setDepthTest(false);
    CCScene* tranScene = CCTransitionMoveInL::create(TRANSITION_DURATION, pScene);
    
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->runGame();
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
}

//------------------------------------------------------------------
//
// LevelLayer
//
//------------------------------------------------------------------

LevelLayer::LevelLayer()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCMenuItemImage *item1 = CCMenuItemImage::create("target-dummy.png", "target-dummy-pressed.png", this, menu_selector(LevelLayer::startGameCallback));
    CCMenuItemImage *item2 = CCMenuItemImage::create("alien-skull.png", "alien-skull-pressed.png", this, menu_selector(LevelLayer::startAccelCallback));
    CCMenuItemImage *item3 = CCMenuItemImage::create("cannon.png", "anthem.png", this, menu_selector(LevelLayer::startCannonCallback));
    CCMenuItemImage *item4 = CCMenuItemImage::create("coma.png", "anthem.png", this, menu_selector(LevelLayer::startCountingCallback));

    
    item1->setScale(0.05 * s.width/640);
    item2->setScale(0.05 * s.width/640);
    item3->setScale(0.05 * s.width/640);
    item4->setScale(0.05 * s.width/640);

    
    //item2->setEnabled(false);
    //item3->setEnabled(false);
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(CreditsLayer::backCallback) );
    back->setPosition(ccp(s.width*0.8,s.height*0.2));
    
    CCMenu *menu = CCMenu::create(item1, item2, item3, item4, back, NULL);
    
    menu->setPosition(CCPointZero);
    item1->setPosition(CCPointMake(s.width/4, s.height/2 + 0.05*item2->getContentSize().width*2));
    item2->setPosition(CCPointMake(s.width/4, s.height/2));
    item3->setPosition(CCPointMake(s.width/4, s.height/2 - 0.05*item2->getContentSize().width*2));
    item4->setPosition(CCPointMake(s.width/4, s.height/2 + 0.1*item2->getContentSize().width*2));

    
    CCLabelTTF *item1Label = CCLabelTTF::create("Target Practice", "PressStart2P-Regular", 12.0);
    CCLabelTTF *item2Label = CCLabelTTF::create("Moving Invaders", "PressStart2P-Regular", 12.0);
    CCLabelTTF *item3Label = CCLabelTTF::create("Cannon Launch", "PressStart2P-Regular", 12.0);
    CCLabelTTF *item4Label = CCLabelTTF::create("Counting Game", "PressStart2P-Regular", 12.0);

    
    item1Label->setPosition(CCPointMake(s.width/2, s.height/2 + 0.05*item2->getContentSize().width*2));
    item2Label->setPosition(CCPointMake(s.width/2, s.height/2));
    item3Label->setPosition(CCPointMake(s.width/2, s.height/2 - 0.05*item2->getContentSize().width*2));
    item4Label->setPosition(CCPointMake(s.width/2, s.height/2 + 0.1*item2->getContentSize().width*2));

    
    addChild(menu, 1);
    addChild(item1Label);addChild(item2Label);addChild(item3Label);addChild(item4Label);
}

void LevelLayer::startGameCallback(CCObject* pSender)
{
    
    SpaceScene* pScene =  new SpaceScene();
    // fix bug #486, without setDepthTest(false), FlipX,Y will flickers
    CCDirector::sharedDirector()->setDepthTest(false);
    CCScene* tranScene = CCTransitionZoomFlipX::create(TRANSITION_DURATION, pScene);
    
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

void LevelLayer::startAccelCallback(CCObject* pSender)
{
    SpacePhysicsScene* pScene =  new SpacePhysicsScene();
    // fix bug #486, without setDepthTest(false), FlipX,Y will flickers
    CCDirector::sharedDirector()->setDepthTest(false);
    CCScene* tranScene = CCTransitionZoomFlipX::create(TRANSITION_DURATION, pScene);
    
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->gameMode = 1;
        pScene->runGame();
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
}

void LevelLayer::startCannonCallback(CCObject* pSender)
{
    CannonScene* pScene =  new CannonScene();
    // fix bug #486, without setDepthTest(false), FlipX,Y will flickers
    CCDirector::sharedDirector()->setDepthTest(false);
    CCScene* tranScene = CCTransitionZoomFlipX::create(TRANSITION_DURATION, pScene);
    
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->gameMode = 1;
        pScene->runGame();
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
    pScene->toneGenHelp->playBackgroundMusic("main_background.wav");
    //pScene->toneGenHelp->enableTones();
    pScene->toneGenHelp->playOscillatingTone(1000.0, 5.0, 2);
    pScene->toneGenHelp->oscillateBackground();
}

void LevelLayer::startCountingCallback(CCObject* pSender)
{
    CountingGame* pScene =  new CountingGame();
    // fix bug #486, without setDepthTest(false), FlipX,Y will flickers
    CCDirector::sharedDirector()->setDepthTest(false);
    CCScene* tranScene = CCTransitionZoomFlipX::create(TRANSITION_DURATION, pScene);
    
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->gameMode = 1;
        pScene->runGame();
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
    pScene->toneGenHelp->playBackgroundMusic("main_background.wav");
    pScene->toneGenHelp->enableTones();
    //pScene->toneGenHelp->playOscillatingTone(1000.0, 5.0, 2);
    //pScene->toneGenHelp->oscillateBackground();
}

void LevelLayer::backCallback(CCObject* sender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(0);
}


//------------------------------------------------------------------
//
// OptionsLayer
//
//------------------------------------------------------------------

OptionsLayer::OptionsLayer()
{
    CCMenuItemFont::setFontName("PressStart2P-Regular");
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont*title1 = CCMenuItemFont::create("Effects");
    title1->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle* item1 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(OptionsLayer::fxCallback),
                                                                 CCMenuItemFont::create( "On" ),
                                                                 CCMenuItemFont::create( "Off"),
                                                                 NULL );
    
    CCMenuItemFont::setFontName( "PressStart2P-Regular" );
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont* title2 = CCMenuItemFont::create( "Music" );
    title2->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle *item2 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(OptionsLayer::musicCallback),
                                                                 CCMenuItemFont::create( "On" ),
                                                                 CCMenuItemFont::create( "Off"),
                                                                 NULL );
    
    CCMenuItemFont::setFontName( "PressStart2P-Regular" );
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont* title3 = CCMenuItemFont::create( "Quality" );
    title3->setEnabled( false );
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle *item3 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(OptionsLayer::qualityCallback),
                                                                 CCMenuItemFont::create( "High" ),
                                                                 CCMenuItemFont::create( "Low" ),
                                                                 NULL );
    
    CCMenuItemFont::setFontName( "PressStart2P-Regular" );
    CCMenuItemFont::setFontSize(12);
    CCMenuItemFont* title4 = CCMenuItemFont::create( "Accel. Sensitivity" );
    title4->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle *item4 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(OptionsLayer::sensitivityCallback),
                                                                 CCMenuItemFont::create( "Off" ),
                                                                 NULL );
    
    item4->getSubItems()->addObject( CCMenuItemFont::create( "33%" ) );
    item4->getSubItems()->addObject( CCMenuItemFont::create( "66%" ) );
    item4->getSubItems()->addObject( CCMenuItemFont::create( "100%" ) );
    
    // you can change the one of the items by doing this
    item4->setSelectedIndex( 2 );
    
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize( 34 );
    
    CCLabelTTF* calibrateLabel = CCLabelTTF::create("Calibrate", "Ubuntu-Regular", 24);
    CCMenuItemLabel* cal = CCMenuItemLabel::create(calibrateLabel, this, menu_selector(OptionsLayer::calibrateCallback) );
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(OptionsLayer::backCallback) );
    
    CCMenu *menu = CCMenu::create(
                                  title1, title2,
                                  item1, item2,
                                  title3, title4,
                                  item3, item4,
                                  cal, back, NULL ); // 9 items.
    
    menu->alignItemsInColumns(2, 2, 2, 2, 1, 1, NULL);
    
    addChild(menu);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    menu->setPosition(ccp(s.width/2, s.height/2));
}

void OptionsLayer::musicCallback(CCObject* sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        (((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator())->disableBackground();
    } else {
        (((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator())->enableBackground();
    }
}

void OptionsLayer::qualityCallback(CCObject* sender)
{
    std::cout << "selected item: %x index:%d" << dynamic_cast<CCMenuItemToggle*>(sender)->selectedItem() << dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex();
}

void OptionsLayer::fxCallback(CCObject* sender)
{
    std::cout << "selected item: %x index:%d" << dynamic_cast<CCMenuItemToggle*>(sender)->selectedItem() << dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex();
}

void OptionsLayer::sensitivityCallback(CCObject* sender)
{
    std::cout << "selected item: %x index:%d" << dynamic_cast<CCMenuItemToggle*>(sender)->selectedItem() << dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex();
}


void OptionsLayer::calibrateCallback(CCObject* sender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(5);
}


void OptionsLayer::backCallback(CCObject* sender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(0);
}


//------------------------------------------------------------------
//
// CreditsLayer
//
//------------------------------------------------------------------

CreditsLayer::CreditsLayer()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCLabelTTF *creditText = CCLabelTTF::create("By: Benjamin Yu", "Audiowide-Regular", 12.0);
    CCLabelTTF *creditText2 = CCLabelTTF::create("Libraries\n==========", "Audiowide-Regular", 12.0);
    CCLabelTTF *creditText3 = CCLabelTTF::create("Graphics: cocos2d-x\n Audio Synthesis: momu-stk\n Physics Engine: Box2D\n JSON Parsing: rapidjson\n Networking: libCurl\n", "Audiowide-Regular", 12.0);
    
    creditText->setPosition(ccp(s.width/2,s.height*0.9));
    creditText2->setPosition(ccp(s.width/2,s.height*0.7));
    creditText3->setPosition(ccp(s.width/2,s.height*0.5));
    
    addChild(creditText);
    addChild(creditText2);
    addChild(creditText3);
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(CreditsLayer::backCallback) );
    
    CCMenu* menu = CCMenu::create(back,NULL);
    addChild(menu);
    menu->setPosition(ccp(s.width/2,s.height*0.2));
    
}


void CreditsLayer::onEnter()
{
    CCLayer::onEnter();
}

void CreditsLayer::backCallback(CCObject* sender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(0);
}

//------------------------------------------------------------------
//
// Calibrate Layer
//
//------------------------------------------------------------------

CalibrateLayer::CalibrateLayer()
{
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCLabelTTF *creditText = CCLabelTTF::create("Place your left earbud on the mic", "Audiowide-Regular", 12.0);
    
    creditText->setPosition(ccp(s.width/2,s.height*0.9));
    
    addChild(creditText);
    
    CCLabelTTF* startLabel = CCLabelTTF::create("START", "Ubuntu-Regular", 30);
    CCMenuItemLabel* startButton = CCMenuItemLabel::create(startLabel, this, menu_selector(CalibrateLayer::startCallback));
    startButton->setPosition(ccp(0,s.height/2));
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(CalibrateLayer::backCallback) );
    
    CCMenu* menu = CCMenu::create(startButton,back,NULL);
    addChild(menu);
    menu->setPosition(ccp(s.width/2,s.height*0.2));
    
    
}


void CalibrateLayer::onEnter()
{
    CCLayer::onEnter();
    toneGenHelp = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator();
    toneGenHelp->enableTones();
    toneGenHelp->disableBackground();
    dataStoreHandler = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore();
    freqIndex = 0;
    volIndex = 0;
}

void CalibrateLayer::startCallback(CCObject* sender)
{
    CCLog("Vol: %d" ,this->toneGenHelp->getMicVolume());
    toneGenHelp->enableTones();
    this->schedule(schedule_selector(CalibrateLayer::playSingleTone), 0.5, 10000, 0.0);
    this->schedule(schedule_selector(CalibrateLayer::stopAndMeasureTone), 0.5, 10000, 0.25);
}

void CalibrateLayer::playSingleTone()
{
    toneGenHelp->playConstantTone(1000.0, volIndex/100.0, 0);   // Play pure-tone
}

void CalibrateLayer::stopAndMeasureTone()
{
    if (volIndex >= 100)
    {
        volIndex = 0;
        toneGenHelp->disableTones();
        freqIndex++;
    }
    decibel_levels[freqIndex][volIndex] = this->toneGenHelp->getMicVolume();
    if (volIndex > 0)
        decibel_levels[freqIndex][volIndex] = 20 * log10f(decibel_levels[freqIndex][volIndex]);
    toneGenHelp->removeTone(0);

    volIndex++;
}


void CalibrateLayer::backCallback(CCObject* sender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(1);
}


void CalibrateLayer::calibrateCallback(CCObject* sender)
{
    toneGenHelp = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator();
    dataStoreHandler = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore();
}


//------------------------------------------------------------------
//
// MainMenu
//
//------------------------------------------------------------------

void MainMenu::runMainMenu()
{
    CCLayer* pLayer = new MainMenuLayer();
    addChild(pLayer);
    pLayer->autorelease();
    
    CCDirector::sharedDirector()->replaceScene((CCScene *)this);
}

iOSBridge::ToneGeneratorHelper* MainMenu::getToneGenerator()
{
    return this->toneGenHelp;
}

void MainMenu::setToneGenerator(iOSBridge::ToneGeneratorHelper * gen)
{
    this->toneGenHelp = gen;
}

iOSBridge::DataStore* MainMenu::getDataStore()
{
    return this->dataStoreHandler;
}

void MainMenu::setDataStore(iOSBridge::DataStore * handler)
{
    this->dataStoreHandler = handler;
}

void MainMenu::onEnter()
{
    CCScene::onEnter();
    if (!toneGenHelp) {
        toneGenHelp = new iOSBridge::ToneGeneratorHelper(1);
    }
    toneGenHelp->removeTone(0);
    toneGenHelp->playBackgroundMusic("echelon.wav");
    dataStoreHandler = new iOSBridge::DataStore();
    this->getDataStore()->rootVCPtr = this->rootVC;
}
