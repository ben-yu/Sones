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
        void playRandomTone();
        void addTone(float frequency, double duration, int index);
        void playOscillatingTone(float frequency, double duration, int index);
        float removeTone(int index);
        void playBackgroundMusic(std::string fileName);
        void enableBackground();
        void disableBackground();
        void play();
        void stop();
        void enableTones();
        void disableTones();
        void maxToneVolume();
        void playExplosion();
        float getAmplitude();
        float getVolume();

    
    private:
        void* toneGenerator_objc;
    };
    
}

#endif /* defined(__Sonence__ToneGeneratorHelper__) */
