//
//  DataStore.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2012-11-13.
//
//

#include "DataStore.h"
#import "RootViewController.h"
#import "TestData.h"

namespace iOSBridge {
    
    DataStore::DataStore(){
    }
    
    DataStore::~DataStore(){
        free(returnData);
    }
    
    void DataStore::saveData(std::string name, double freq, double volume, int channel , int attempts, double init_time, double final_time) {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        
        [vc storeDataPoint:[NSString stringWithUTF8String:name.c_str()] freqThresh:freq volThresh:volume audioChannel:channel attempts:attempts  reactionTime: init_time
                  lastTime:final_time];
    }
    
    DataPoint* DataStore::readData() {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        DataPoint* tmpData =  [vc getData];
        numOfElements = vc.numOfDataPoints;
        
        return tmpData;
        
    }

    
}