//
//  MainMenu.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2012-12-22.
//
//

#include "MainMenu.h"
#include "SpaceScene.h"

#define LINE_SPACE          40

//------------------------------------------------------------------
//
// MainMenuLayer
//
//------------------------------------------------------------------

MainMenuLayer::MainMenuLayer()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCLabelTTF* title = CCLabelTTF::create("Sonic Intelligence", "Audiowide-Regular", 30);
    title->setPosition(ccp(s.width * 0.32 ,s.height - LINE_SPACE));
    
    CCLabelTTF* selectedText = CCLabelTTF::create("News / Updates", "ChelaOne-Regular", 18);
    selectedText->setPosition(ccp(s.width * 0.7 ,s.height * 0.75));
    
    CCLabelTTF* label = CCLabelTTF::create("New Game", "Ubuntu-Regular", 14);
    CCMenuItemLabel* pMenuItem = CCMenuItemLabel::create(label, this, menu_selector(MainMenuLayer::levelsCallback));
    CCLabelTTF* optionLabel = CCLabelTTF::create("Options", "Ubuntu-Regular", 14);
    CCMenuItemLabel* optionMenuItem = CCMenuItemLabel::create(optionLabel, this, menu_selector(MainMenuLayer::optionsCallback));
    CCLabelTTF* statsLabel = CCLabelTTF::create("Stats", "Ubuntu-Regular", 14);
    CCMenuItemLabel* statsMenuItem = CCMenuItemLabel::create(statsLabel, this, menu_selector(MainMenuLayer::startGameCallback));
    CCLabelTTF* creditsLabel = CCLabelTTF::create("Credits", "Ubuntu-Regular", 14);
    CCMenuItemLabel* creditsMenuItem = CCMenuItemLabel::create(creditsLabel, this, menu_selector(MainMenuLayer::startGameCallback));
    CCLabelTTF* exitLabel = CCLabelTTF::create("Exit", "Ubuntu-Regular", 14);
    CCMenuItemLabel* exitMenuItem = CCMenuItemLabel::create(exitLabel, this, menu_selector(MainMenuLayer::exitCallback));
    
    CCMenu* pMenu =CCMenu::create(pMenuItem, NULL);
    pMenu->addChild(optionMenuItem);pMenu->addChild(statsMenuItem);pMenu->addChild(creditsMenuItem);pMenu->addChild(exitMenuItem);
    pMenu->setPosition(CCPointZero);
    pMenuItem->setPosition( CCPointMake( s.width / 4, (s.height - 3*LINE_SPACE) ));
    optionMenuItem->setPosition( CCPointMake( s.width / 4, (s.height - 4*LINE_SPACE) ));
    statsMenuItem->setPosition( CCPointMake( s.width / 4, (s.height - 5*LINE_SPACE) ));
    creditsMenuItem->setPosition( CCPointMake( s.width / 4, (s.height - 6*LINE_SPACE) ));
    exitMenuItem->setPosition( CCPointMake( s.width / 4, (s.height - 7*LINE_SPACE) ));
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
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->runGame();
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
    
    CCMenuItemImage *item1 = CCMenuItemImage::create("button.png", "button.png", this, menu_selector(LevelLayer::startGameCallback));
    CCMenuItemImage *item2 = CCMenuItemImage::create("Icon.png", "Icon.png", this, menu_selector(LevelLayer::startAccelCallback));
    CCMenuItemImage *item3 = CCMenuItemImage::create("Icon-72.png", "Icon-72.png", this, menu_selector(LevelLayer::startGameCallback));
    
    item1->setScale(0.5);
    item2->setScale(1.5);


    CCMenu *menu = CCMenu::create(item1, item2, item3, NULL);
    
    menu->setPosition(CCPointZero);
    item1->setPosition(CCPointMake(s.width/2 - item2->getContentSize().width*2, s.height/2));
    item2->setPosition(CCPointMake(s.width/2, s.height/2));
    item3->setPosition(CCPointMake(s.width/2 + item2->getContentSize().width*2, s.height/2));
    
    addChild(menu, 1);
}

void LevelLayer::startGameCallback(CCObject* pSender)
{
    // get the userdata, it's the index of the menu item clicked
    //CCMenuItem* pMenuItem = (CCMenuItem *)(pSender);
    
    SpaceScene* pScene =  new SpaceScene();
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->runGame();
        pScene->release();
    }
}

void LevelLayer::startAccelCallback(CCObject* pSender)
{
    // get the userdata, it's the index of the menu item clicked
    //CCMenuItem* pMenuItem = (CCMenuItem *)(pSender);
    
    SpaceScene* pScene =  new SpaceScene();
    pScene->setToneGenerator(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    pScene->setDataStore(((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore());
    if (pScene)
    {
        pScene->gameMode = 1;
        pScene->runGame();
        pScene->release();
    }
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
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont* title4 = CCMenuItemFont::create( "Sensitivity" );
    title4->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle *item4 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(OptionsLayer::sensitivityCallback),
                                                                 CCMenuItemFont::create( "Off" ),
                                                                 NULL );
    
    //UxArray* more_items = UxArray::arrayWithObjects(
    //                                                 CCMenuItemFont::create( "33%" ),
    //                                                 CCMenuItemFont::create( "66%" ),
    //                                                 CCMenuItemFont::create( "100%" ),
    //                                                 NULL );
    // TIP: you can manipulate the items like any other CCMutableArray
    item4->getSubItems()->addObject( CCMenuItemFont::create( "33%" ) );
    item4->getSubItems()->addObject( CCMenuItemFont::create( "66%" ) );
    item4->getSubItems()->addObject( CCMenuItemFont::create( "100%" ) );
    
    // you can change the one of the items by doing this
    item4->setSelectedIndex( 2 );
    
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize( 34 );
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(OptionsLayer::backCallback) );
    
    CCMenu *menu = CCMenu::create(
                                  title1, title2,
                                  item1, item2,
                                  title3, title4,
                                  item3, item4,
                                  back, NULL ); // 9 items.
    
    menu->alignItemsInColumns(2, 2, 2, 2, 1, NULL);
    
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


void OptionsLayer::backCallback(CCObject* sender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(0);
}

//------------------------------------------------------------------
//
// StatsLayer
//
//------------------------------------------------------------------
void StatsLayer::onEnter()
{
    CCLayer::onEnter();
}
//------------------------------------------------------------------
//
// CreditsLayer
//
//------------------------------------------------------------------
void CreditsLayer::onEnter()
{
    CCLayer::onEnter();
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
