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
    
    CCLabelTTF* title = CCLabelTTF::create("Sones", "Audiowide-Regular", floor(65 * s.width/640));
    title->setPosition(ccp(s.width * 0.5 ,s.height*0.85));
    CCLabelTTF* title2 = CCLabelTTF::create("Sones", "Audiowide-Regular", floor(65 * s.width/640));
    title2->setPosition(ccp(s.width * 0.5 -1,s.height*0.85-1));
    title2->setColor(ccc3(0,0,0));
    
    CCMenuItemImage *item1 = CCMenuItemImage::create("new_game.png", "new_game.png", this, menu_selector(MainMenuLayer::levelsCallback));
    item1->setScale(0.75); item1->setOpacity(200);
    CCMenuItemImage *item2 = CCMenuItemImage::create("options.png", "options.png", this, menu_selector(MainMenuLayer::optionsCallback));
    item2->setScale(0.75); item2->setOpacity(200);
    CCMenuItemImage *item3 = CCMenuItemImage::create("stats.png", "stats.png", this, menu_selector(MainMenuLayer::statsCallback));
    item3->setScale(0.75); item3->setOpacity(200);
    CCMenuItemImage *item4 = CCMenuItemImage::create("credits.png", "credits.png", this, menu_selector(MainMenuLayer::creditsCallback));
    item4->setScale(0.75); item4->setOpacity(200);

    
    CCMenu* pMenu = CCMenu::create(item1,item2,item3,item4,NULL);
    pMenu->setPosition(CCPointZero);
    item1->setPosition( CCPointMake( s.width/2, 6.3*s.height/10));
    item2->setPosition( CCPointMake( s.width / 2, 5.0*s.height/10 ));
    item3->setPosition( CCPointMake( s.width / 2, 3.7*s.height/10 ));
    item4->setPosition( CCPointMake( s.width / 2, 2.4*s.height/10 ));
    this->addChild(pMenu,1);
    this->addChild(title,2);this->addChild(title2,1);
        
}

void MainMenuLayer::onEnter() {
    CCLayer::onEnter();
    CCSize s = CCDirector::sharedDirector()->getWinSize();
        
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
    
    CCMenuItemImage *item1 = CCMenuItemImage::create("counting.png", "counting.png", this, menu_selector(LevelLayer::startCountingCallback));
    CCMenuItemImage *item2 = CCMenuItemImage::create("target_practice.png", "target_practice.png", this, menu_selector(LevelLayer::startGameCallback));
    CCMenuItemImage *item3 = CCMenuItemImage::create("moving_invaders.png", "moving_invaders.png", this, menu_selector(LevelLayer::startAccelCallback));
    CCMenuItemImage *item4 = CCMenuItemImage::create("cannon_launch.png", "cannon_launch.png", this, menu_selector(LevelLayer::startCannonCallback));

    
    item1->setScale(0.75); item1->setOpacity(200);
    item2->setScale(0.75); item2->setOpacity(200);
    item3->setScale(0.75); item3->setOpacity(200);
    item4->setScale(0.75); item4->setOpacity(200);
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(CreditsLayer::backCallback) );
    back->setPosition(ccp(s.width*0.8,s.height*0.2));
    
    CCMenu *menu = CCMenu::create(item1, item2, item3, item4, back, NULL);
    
    menu->setPosition(CCPointZero);
    item1->setPosition(CCPointMake(s.width/2, s.height/2 + item2->getContentSize().height*2));
    item2->setPosition(CCPointMake(s.width/2, s.height/2 + 0.5*item2->getContentSize().height*2));
    item3->setPosition(CCPointMake(s.width/2, s.height/2 ));
    item4->setPosition(CCPointMake(s.width/2, s.height/2 - 0.5*item2->getContentSize().height*2));

    addChild(menu, 1);
}

void LevelLayer::draw()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();

    ccDrawSolidRect(ccp(0,0), ccp(s.width,s.height), ccc4f(0,0,0,0.5));
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
        pScene->rootVC = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->rootVC;
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
        pScene->rootVC = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->rootVC;
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
    pScene->toneGenHelp->playBackgroundMusic("white_noise.wav");
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
        pScene->rootVC = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->rootVC;
        CCDirector::sharedDirector()->replaceScene(tranScene);
        pScene->release();
    }
    pScene->toneGenHelp->playBackgroundMusic("OutThere_0.aiff");
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
    item1 = CCMenuItemToggle::createWithTarget(this,
                                             menu_selector(OptionsLayer::fxCallback),
                                             CCMenuItemFont::create( "Off" ),
                                             CCMenuItemFont::create( "On"),
                                             NULL );
    
    
    CCMenuItemFont::setFontName( "PressStart2P-Regular" );
    CCMenuItemFont::setFontSize(16);
    CCMenuItemFont* title2 = CCMenuItemFont::create( "Music" );
    title2->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    item2 = CCMenuItemToggle::createWithTarget(this,
                                             menu_selector(OptionsLayer::musicCallback),
                                             CCMenuItemFont::create( "Off" ),
                                             CCMenuItemFont::create( "On"),
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
    
    CCLabelTTF* calibrateLabel = CCLabelTTF::create("Calibrate Headphones", "Ubuntu-Regular", 24);
    CCMenuItemLabel* cal = CCMenuItemLabel::create(calibrateLabel, this, menu_selector(OptionsLayer::calibrateCallback) );
    
    CCLabelTTF* label = CCLabelTTF::create("Back", "Ubuntu-Regular", 20);
    CCMenuItemLabel* back = CCMenuItemLabel::create(label, this, menu_selector(OptionsLayer::backCallback) );
    
    CCMenu *menu = CCMenu::create(
                                  title1, title2,
                                  item1, item2,
                                  title4, item4,
                                  cal, back, NULL ); // 9 items.
    
    menu->alignItemsInColumns(2, 2, 1, 1, 1, 1, NULL);
    
    addChild(menu);
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    menu->setPosition(ccp(s.width/2, s.height/2));
    
}

void OptionsLayer::onEnter()
{
    CCLayer::onEnter();
    
    toneGenHelp = (((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getToneGenerator());
    item1->setSelectedIndex(toneGenHelp->getToneStatus());
    item2->setSelectedIndex(toneGenHelp->getBackgroundStatus());


}

void OptionsLayer::draw()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    ccDrawSolidRect(ccp(0,0), ccp(s.width,s.height), ccc4f(0,0,0,0.5));
}

void OptionsLayer::musicCallback(CCObject* sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        toneGenHelp->enableBackground();
    } else {
        toneGenHelp->disableBackground();
    }
}

void OptionsLayer::qualityCallback(CCObject* sender)
{
    std::cout << "selected item: %x index:%d" << dynamic_cast<CCMenuItemToggle*>(sender)->selectedItem() << dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex();
}

void OptionsLayer::fxCallback(CCObject* sender)
{
    if (dynamic_cast<CCMenuItemToggle*>(sender)->getSelectedIndex()) {
        toneGenHelp->enableTones();
    } else {
        toneGenHelp->disableTones();
    }
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

void CreditsLayer::draw()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    ccDrawSolidRect(ccp(0,0), ccp(s.width,s.height), ccc4f(0,0,0,0.5));
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

void CalibrateLayer::draw()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    ccDrawSolidRect(ccp(0,0), ccp(s.width,s.height), ccc4f(0,0,0,0.5));
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
    this->schedule(schedule_selector(CalibrateLayer::playSingleTone), 1.0, 10000, 0.0);
    this->schedule(schedule_selector(CalibrateLayer::stopAndMeasureTone), 1.0, 10000, 0.5);
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
    toneGenHelp->playBackgroundMusic("MyVeryOwnDeadShip.aiff");
    dataStoreHandler = new iOSBridge::DataStore();
    this->getDataStore()->rootVCPtr = this->rootVC;
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    ShaderNode *sn = ShaderNode::shaderNodeWithVertex("shader.vsh", "shader.fsh");
    sn->setPosition(ccp(s.width/2, s.height/2));
    
    addChild(sn,-100);
}


///---------------------------------------
//
// ShaderNode
//
///---------------------------------------
enum
{
    SIZE_X = 256,
    SIZE_Y = 256,
};

ShaderNode::ShaderNode()
:m_center(vertex2(0.0f, 0.0f))
,m_resolution(vertex2(0.0f, 0.0f))
,m_time(0.0f)
,m_uniformCenter(0)
,m_uniformResolution(0)
,m_uniformTime(0)
{
}

ShaderNode* ShaderNode::shaderNodeWithVertex(const char *vert, const char *frag)
{
    ShaderNode *node = new ShaderNode();
    node->initWithVertex(vert, frag);
    node->autorelease();
    
    return node;
}

bool ShaderNode::initWithVertex(const char *vert, const char *frag)
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    loadShaderVertex(vert, frag);
    
    m_time = 0;
    m_resolution = vertex2(s.width, s.height);
    
    scheduleUpdate();
    
    setContentSize(CCSizeMake(s.width, s.height));
    setAnchorPoint(ccp(0.5f, 0.5f));
    
    return true;
}

void ShaderNode::loadShaderVertex(const char *vert, const char *frag)
{
    CCGLProgram *shader = new CCGLProgram();
    shader->initWithVertexShaderFilename(vert, frag);
    
    shader->addAttribute("aVertex", kCCVertexAttrib_Position);
    shader->link();
    
    shader->updateUniforms();
    
    m_uniformCenter = glGetUniformLocation(shader->getProgram(), "center");
    m_uniformResolution = glGetUniformLocation(shader->getProgram(), "resolution");
    m_uniformTime = glGetUniformLocation(shader->getProgram(), "time");
    
    this->setShaderProgram(shader);
    
    shader->release();
}

void ShaderNode::update(float dt)
{
    m_time += dt;
}

void ShaderNode::setPosition(const CCPoint &newPosition)
{
    CCNode::setPosition(newPosition);
    CCPoint position = getPosition();
    m_center = vertex2(position.x * CC_CONTENT_SCALE_FACTOR(), position.y * CC_CONTENT_SCALE_FACTOR());
}

void ShaderNode::draw()
{
    CC_NODE_DRAW_SETUP();
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    float w = s.width, h = s.height;
    GLfloat vertices[12] = {0,0, w,0, w,h, 0,0, 0,h, w,h};
    
    //
    // Uniforms
    //
    getShaderProgram()->setUniformLocationWith2f(m_uniformCenter, m_center.x, m_center.y);
    getShaderProgram()->setUniformLocationWith2f(m_uniformResolution, m_resolution.x, m_resolution.y);
    
    // time changes all the time, so it is Ok to call OpenGL directly, and not the "cached" version
    glUniform1f(m_uniformTime, m_time);
    
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    CC_INCREMENT_GL_DRAWS(1);
}
