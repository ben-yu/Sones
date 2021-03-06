//
//  SonenceAppDelegate.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//  Copyright APL 2012. All rights reserved.
//

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "CCApplication.h"
#include "cocos2d.h"
#include "SpaceScene.h"
#include "MainMenu.h"
#include "AudiogramScene.h"

/**
@brief    The cocos2d Application.

The reason to implement with private inheritance is to hide some interface details of CCDirector.
*/
class  AppDelegate : private cocos2d::CCApplication
{
public:
    AppDelegate();
    virtual ~AppDelegate();
    
    /**
     @brief	Implement for initialize OpenGL instance, set source path, etc...
     */
    virtual bool initInstance();

    /**
    @brief    Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function is called when the application enters the background
    @param  the pointer of the application instance
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function is called when the application enters the foreground
    @param  the pointer of the application instance
    */
    virtual void applicationWillEnterForeground();
    
    void* getRootVC();
    void setRootVC(void* vc);
    
private:
    void* rootVC;
    CCScene* pScene;
};

#endif // _APP_DELEGATE_H_

