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

void MainMenuLayer::onEnter()
{
    CCLayer::onEnter();
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    CCLabelTTF* title = CCLabelTTF::create("Sonic Intelligence", "Audiowide-Regular", 30);
    title->setPosition(ccp(s.width * 0.32 ,s.height - LINE_SPACE));
    
    CCLabelTTF* selectedText = CCLabelTTF::create("News / Updates", "ChelaOne-Regular", 18);
    selectedText->setPosition(ccp(s.width * 0.7 ,s.height * 0.75));
    
    CCLabelTTF* label = CCLabelTTF::create("New Game", "Ubuntu-Regular", 14);
    CCMenuItemLabel* pMenuItem = CCMenuItemLabel::create(label, this, menu_selector(MainMenuLayer::startGameCallback));
    CCLabelTTF* optionLabel = CCLabelTTF::create("Options", "Ubuntu-Regular", 14);
    CCMenuItemLabel* optionMenuItem = CCMenuItemLabel::create(optionLabel, this, menu_selector(MainMenuLayer::startGameCallback));
    CCLabelTTF* statsLabel = CCLabelTTF::create("Stats", "Ubuntu-Regular", 14);
    CCMenuItemLabel* statsMenuItem = CCMenuItemLabel::create(statsLabel, this, menu_selector(MainMenuLayer::startGameCallback));
    CCLabelTTF* creditsLabel = CCLabelTTF::create("Credits", "Ubuntu-Regular", 14);
    CCMenuItemLabel* creditsMenuItem = CCMenuItemLabel::create(creditsLabel, this, menu_selector(MainMenuLayer::startGameCallback));
    CCLabelTTF* exitLabel = CCLabelTTF::create("Exit", "Ubuntu-Regular", 14);
    CCMenuItemLabel* exitMenuItem = CCMenuItemLabel::create(exitLabel, this, menu_selector(MainMenuLayer::startGameCallback));
    
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


void MainMenuLayer::startGameCallback(CCObject* pSender)
{
    // get the userdata, it's the index of the menu item clicked
    //CCMenuItem* pMenuItem = (CCMenuItem *)(pSender);
    
    SpaceScene* pScene =  new SpaceScene();
    pScene->setToneGenerator(((MainMenu *)this->getParent())->getToneGenerator());
    if (pScene)
    {
        pScene->runGame();
        pScene->release();
    }
}

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

void MainMenu::initAudio()
{
    toneGenHelp = new iOSBridge::ToneGeneratorHelper(1);
}

void MainMenu::onEnter()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
}
