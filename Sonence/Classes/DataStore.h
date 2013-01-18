//
//  DataStore.h
//  Sonence
//
//  Created by Benjamin Yu on 2012-11-13.
//
//

#ifndef __Sonence__DataStore__
#define __Sonence__DataStore__

namespace iOSBridge{
    
    struct DataPoint {
        float freq;
        float vol;
    };
    
    class  DataStore {
        
    public:
        
        void* rootVCPtr;
        
        DataStore();
        ~DataStore();
        
        void saveData(double freq, double volume);
        DataPoint* readData();
        
        DataPoint* returnData;
        int numOfElements;
    };
    
}

#endif /* defined(__Sonence__DataStore__) */
