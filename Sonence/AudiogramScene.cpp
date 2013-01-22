//
//  Audiogram.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-10.
//
//

#include "AudiogramScene.h"
#include "MainMenu.h"

AudiogramSceneLayer::AudiogramSceneLayer()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    winWidth = CCDirector::sharedDirector()->getWinSize().width;
    winHeight = CCDirector::sharedDirector()->getWinSize().height;
        
    CCMenuItemFont *title1 = CCMenuItemFont::create("Ear: ");
    title1->setEnabled(false);
    CCMenuItemFont::setFontName( "Ubuntu-Regular" );
    CCMenuItemFont::setFontSize(34);
    CCMenuItemToggle* item1 = CCMenuItemToggle::createWithTarget(this,
                                                                 menu_selector(AudiogramSceneLayer::earCallback),
                                                                 CCMenuItemFont::create( "Left" ),
                                                                 CCMenuItemFont::create( "Right"),
                                                                 CCMenuItemFont::create( "Both"),
                                                                 NULL );
    

    CCLabelTTF* exitLabel = CCLabelTTF::create("Back", "Ubuntu-Regular", 16);
    CCMenuItemLabel* back = CCMenuItemLabel::create(exitLabel, this, menu_selector(AudiogramSceneLayer::backCallback));
    
    CCMenu* menu = CCMenu::create(title1, item1, back, NULL);
    addChild(menu);
    menu->setPosition(ccp(0.3*s.width, 0.1*s.height));
    menu->alignItemsHorizontally();
    menu->alignItemsHorizontallyWithPadding(20.0);
    back->setPosition(ccp(6*winWidth/10.0,0));
    
    int markLength = 10;
    float axisLength = 0.8*winWidth;
    float axisHeight = 0.8*winHeight;
    
    CCLabelTTF *xTitle = CCLabelTTF::create("Frequency (Hz)", "Ubuntu-Regular", 20);
    CCLabelTTF *yTitle = CCLabelTTF::create("Volume (dB)", "Ubuntu-Regular", 20);
    
    xTitle->setPosition(ccp(winWidth/2,0.15*winHeight));
    yTitle->setPosition(ccp(0.05*winWidth,winHeight/2));
    yTitle->setRotation(-90.0);
    addChild(xTitle);
    addChild(yTitle);
    
    xScale = 3000.0;
    yScale = 98.0;
    
    CCArray* xAxisLabels = CCArray::createWithCapacity(10);
    CCArray* yAxisLabels = CCArray::createWithCapacity(10);
    for (int i = 0; i < 10; i++){
        std::stringstream ss;
        ss << (int) (i*yScale/10);
        CCLabelTTF *yLabel = CCLabelTTF::create(ss.str().c_str(), "Ubuntu-Regular", 12);
        yAxisLabels->addObject(yLabel);
        yLabel->setPosition(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(-markLength,i*axisHeight/10)));
        addChild(yLabel);
    }
    for (int i = 0; i < 10; i++){
        std::stringstream ss;
        ss << (int) (i*xScale/10);
        CCLabelTTF *xLabel = CCLabelTTF::create(ss.str().c_str(), "Ubuntu-Regular", 12);
        xAxisLabels->addObject(xLabel);
        xLabel->setPosition(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(i*axisLength/10,-markLength)));
        addChild(xLabel);
    }

}

void AudiogramSceneLayer::onEnter()
{
    CCLayer::onEnter();
    dsPtr = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore();
    displayData = dsPtr->readData();

}

void AudiogramSceneLayer::draw()
{
            
    glEnable(GL_BLEND); // Enable blending for alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawColor4F(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
    
    // Graph Axis
    int markLength = 10;
    float axisLength = 0.8*winWidth;
    float axisHeight = 0.8*winHeight;
    ccDrawLine(ccp(winWidth/10.0,2*winHeight/10.0), ccp(winWidth/10.0, winHeight));
    ccDrawLine(ccp(winWidth/10.0,2*winHeight/10.0), ccp(winWidth, 2*winHeight/10.0));
    
    // Grid Lines
    for (int i = 0; i < 10; i++) {
        ccDrawLine(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(-markLength/2,i*axisHeight/10)), ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(markLength/2,i*axisHeight/10)));
        ccDrawLine(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(i*axisLength/10,-markLength/2)), ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(i*axisLength/10,markLength/2)));
    }
    
    // Plot Data
    ccDrawColor4F(0.0, 1.0, 0.0, 1.0);
    int max = dsPtr->numOfElements;
    for (int i = 0; i < max - 1; i++) {
        if (displayData[i].channel == displayedChannel || displayedChannel == 2) {
        ccDrawCircle(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(displayData[i].freq/xScale * axisLength,displayData[i].vol/yScale * axisHeight)), 1, 0.0, 5, false);
        //ccDrawLine(ccpAdd(ccp(winWidth/10.0,winHeight/10.0),ccp(displayData[i].freq/xScale * axisLength,displayData[i].vol/yScale * axisHeight)), ccpAdd(ccp(winWidth/10.0,winHeight/10.0),ccp(displayData[i+1].freq/3000.0 * axisLength,displayData[i+1].vol * axisHeight)));
        }
    }
}

void AudiogramSceneLayer::backCallback(CCObject* pSender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(0);
}

void AudiogramSceneLayer::earCallback(CCObject* pSender)
{
    displayedChannel = dynamic_cast<CCMenuItemToggle*>(pSender)->getSelectedIndex();
}