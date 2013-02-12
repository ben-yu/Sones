//
//  DataStore.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-11-13.
//
//

#ifndef __Sonence__DataStore__
#define __Sonence__DataStore__

#include <iostream.h>

namespace iOSBridge{
    
    using namespace std;
    
    struct DataPoint {
        float freq;
        float vol;
        int channel;
    };
    
    class  DataStore {
        
    public:
        
        void* rootVCPtr;
        
        DataStore();
        ~DataStore();
        
        void saveData(std::string name, double freq, double volume, int channel, int attempts, double init_time, double final_time);
        DataPoint* readData();
        
        DataPoint* returnData;
        int numOfElements;
    };
    
}

#endif /* defined(__Sonence__DataStore__) */
