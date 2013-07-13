//
//  Audiogram.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2013-01-10.
//
//

#include "AudiogramScene.h"
#include "MainMenu.h"
#include "DataStore.h"

AudiogramSceneLayer::AudiogramSceneLayer()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    winWidth = CCDirector::sharedDirector()->getWinSize().width;
    winHeight = CCDirector::sharedDirector()->getWinSize().height;
    
    title = CCLabelTTF::create("Target Practice", "Ubuntu-Regular", 20);
    title->setPosition(ccp(s.width/2, 0.9*s.height));
    addChild(title);
    
    CCMenuItemImage *left = CCMenuItemImage::create("circle_arrow_left.png", "circle_arrow_left.png", this, menu_selector(AudiogramSceneLayer::leftCallback));
    CCMenuItemImage *right = CCMenuItemImage::create("circle_arrow_right.png", "circle_arrow_right.png", this, menu_selector(AudiogramSceneLayer::rightCallback));
    CCMenu* menuToggle = CCMenu::create(left, right, NULL);
    left->setPosition(ccp(-s.width*0.2, 0));
    right->setPosition(ccp(s.width*0.2, 0));
    menuToggle->setPosition(ccp(s.width/2, 0.9*s.height));
    addChild(menuToggle);
    
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
    
    xScale = 8000.0;
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
    
    CCSize s = CCDirector::sharedDirector()->getWinSize();

    dsPtr = ((MainMenu *)((CCLayerMultiplex *)m_pParent)->getParent())->getDataStore();
    displayData = dsPtr->readData();
    
    sortedPoints = (iOSBridge::DataPoint *)malloc(sizeof(iOSBridge::DataPoint)*dsPtr->numOfElements);
    memcpy(sortedPoints, displayData, dsPtr->numOfElements*sizeof(iOSBridge::DataPoint));
    
    std::sort(sortedPoints, sortedPoints+(dsPtr->numOfElements),AudiogramSceneLayer::freq_sorter);
    
}

bool AudiogramSceneLayer::vol_sorter(iOSBridge::DataPoint const &lhs, iOSBridge::DataPoint const &rhs)
{
    return lhs.vol < rhs.vol;
}

bool AudiogramSceneLayer::freq_sorter(iOSBridge::DataPoint const &lhs, iOSBridge::DataPoint const &rhs)
{
    if  (lhs.freq == rhs.freq)
        return lhs.vol < rhs.vol;
    return lhs.freq < rhs.freq;
}

void AudiogramSceneLayer::draw()
{
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    glEnable(GL_BLEND); // Enable blending for alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ccDrawColor4F(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
    
    ccDrawSolidRect(ccp(0,0), ccp(s.width,s.height), ccc4f(0,0,0,0.5));
    
    // Graph Axis
    int markLength = 10;
    float axisLength = 0.8*winWidth;
    float axisHeight = 0.8*winHeight;
    ccDrawLine(ccp(winWidth/10.0,2*winHeight/10.0), ccp(winWidth/10.0, winHeight));
    ccDrawLine(ccp(winWidth/10.0,2*winHeight/10.0), ccp(winWidth, 2*winHeight/10.0));
    
    // Grid Lines
    for (int i = 0; i < 10; i++) {
        ccDrawLine(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(-markLength/2,i*axisHeight/10)), ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(markLength/2,i*axisHeight/10)));
        //ccDrawLine(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(i*axisLength/10,-markLength/2)), ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(i*axisLength/10,markLength/2)));
    }
    
    // Plot Data
    ccDrawColor4F(0.0, 1.0, 0.0, 1.0);
    int max = dsPtr->numOfElements;
    for (int i = 0; i < max - 1; i++) {
        if ((displayData[i].gameType == gameType) && (displayData[i].channel == displayedChannel || displayedChannel == 2)) {
            ccDrawCircle(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(displayData[i].freq/xScale * axisLength,displayData[i].vol/yScale * axisHeight)), 1, 0.0, 5, false);
            /*
            int tmp = i+1;
            while (tmp < max-1 && sortedPoints[i].freq == sortedPoints[tmp].freq) {
                tmp++;
            }
            if ((sortedPoints[tmp].gameType == gameType) && (sortedPoints[tmp].channel == displayedChannel || displayedChannel == 2)) {
                //ccDrawLine(ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(sortedPoints[i].freq/xScale * axisLength,sortedPoints[i].vol/yScale * axisHeight)), ccpAdd(ccp(winWidth/10.0,2*winHeight/10.0),ccp(sortedPoints[tmp].freq/xScale * axisLength,sortedPoints[tmp].vol/yScale * axisHeight)));
            }
            i = tmp;
            */
        }
    }
}

void AudiogramSceneLayer::leftCallback(CCObject* pSender)
{
    gameType--;
    if (gameType < 0)
    {
        gameType = 3;
    }
    iOSBridge::stringstream tempString;
    switch (gameType) {
        case 0:
            tempString.str("");
            tempString<<"Counting Game";
            yScale = 1.5;
            break;
        case 1:
            tempString.str("");
            tempString<<"Target Practice";
            yScale = 128.0;
            break;
        case 2:
            tempString.str("");
            tempString<<"Moving Invaders";
            yScale = 1.0;
            break;
        case 3:
            tempString.str("");
            tempString<<"Cannon Launch";
            yScale = 1.0;
            break;
            
        default:
            break;
    }
    title->setString(tempString.str().c_str());
}
void AudiogramSceneLayer::rightCallback(CCObject* pSender)
{
    gameType++;
    if (gameType > 3)
    {
        gameType = 0;
    }
    iOSBridge::stringstream tempString;
    switch (gameType) {
        case 0:
            tempString.str("");
            tempString<<"Counting Game";
            break;
        case 1:
            tempString.str("");
            tempString<<"Target Practice";
            break;
        case 2:
            tempString.str("");
            tempString<<"Moving Invaders";
            break;
        case 3:
            tempString.str("");
            tempString<<"Cannon Launch";
            break;
            
        default:
            break;
    }
    title->setString(tempString.str().c_str());
}

void AudiogramSceneLayer::backCallback(CCObject* pSender)
{
    ((CCLayerMultiplex*)m_pParent)->switchTo(0);
}

void AudiogramSceneLayer::earCallback(CCObject* pSender)
{
    displayedChannel = dynamic_cast<CCMenuItemToggle*>(pSender)->getSelectedIndex();
}