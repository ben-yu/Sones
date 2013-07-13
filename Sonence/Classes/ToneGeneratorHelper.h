//
//  ToneGeneratorHelper.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//
#include <string>

#ifndef __Sonence__ToneGeneratorHelper__
#define __Sonence__ToneGeneratorHelper__

namespace iOSBridge{
    
class  ToneGeneratorHelper {
        
    public:
        ToneGeneratorHelper(void);
        ToneGeneratorHelper(int numOfAsteroids);
        ~ToneGeneratorHelper();
        void ShowAlert();
    
        void maxToneVolume();
        void enableTones();
        void disableTones();
        void playRandomTone();
        void addTone(float frequency, double duration, int index);
        void playConstantTone(float frequency, double duration, int index);
        void playOscillatingTone(float frequency, double duration, int index);
        void playDecreasingTone(float frequency, double duration, int index);
        float removeTone(int index);
        int getMicVolume();
        bool getToneStatus();
        bool getBackgroundStatus();

    
        void playBackgroundMusic(std::string fileName);
        void enableBackground();
        void disableBackground();
        void oscillateBackground();
    
        void calibrate();

    
        void play();
        void stop();

        
        void playExplosion();
    
        float getAmplitude();
        float getVolume();
        void setVolume(float level);
    

    
    private:
        void* toneGenerator_objc;
    };
    
}

#endif /* defined(__Sonence__ToneGeneratorHelper__) */
