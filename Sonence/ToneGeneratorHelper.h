//
//  ToneGeneratorHelper.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-10-14.
//
//

#ifndef __Sonence__ToneGeneratorHelper__
#define __Sonence__ToneGeneratorHelper__

namespace iOSBridge{
    
    class  ToneGeneratorHelper {
        
    public:
        
        void* toneGenerator_objc;
        ToneGeneratorHelper(void);
        ToneGeneratorHelper(int numOfAsteroids);
        ~ToneGeneratorHelper();
        void ShowAlert();
        void playRandomTone();
        void addTone(float frequency, double duration, int index);
        float removeTone(int index);
        void play();
        void stop();
    };
    
}

#endif /* defined(__Sonence__ToneGeneratorHelper__) */
