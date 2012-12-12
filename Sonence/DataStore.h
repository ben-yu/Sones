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
    
    class  DataStore {
        
    public:
        
        void* rootVCPtr;
        
        DataStore();
        ~DataStore();
        
        void saveData(double freq, double volume);
    };
    
}

#endif /* defined(__Sonence__DataStore__) */
