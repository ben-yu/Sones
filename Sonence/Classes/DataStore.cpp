//
//  DataStore.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2012-11-13.
//
//

#include "DataStore.h"
#import "RootViewController.h"


namespace iOSBridge {
    
    DataStore::DataStore(){
    }
    
    DataStore::~DataStore(){
    }
    
    void DataStore::saveData(double freq, double volume) {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        [vc storeDataPoint:@"test" freqThresh:freq volThresh:volume];
    }
    
    
}