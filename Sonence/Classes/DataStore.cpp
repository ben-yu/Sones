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
    
    void DataStore::saveData(double freq, double volume, int channel ) {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        [vc storeDataPoint:@"test" freqThresh:freq volThresh:volume audioChannel:channel];
    }
    
    DataPoint* DataStore::readData() {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        numOfElements = vc.numOfDataPoints;
        DataPoint* tmpData =  [vc getData];
        //if (!returnData) {
            //numOfElements = [tmpData count];
            //returnData = (DataPoint*) calloc(sizeof(DataPoint), numOfElements);
        //}
            
        /*
        for (int i = 1; i < numOfElements; i++) {
            returnData[i].freq = [((DataPoint *)([tmpData objectAtIndex:i])) CGPointValue].x;
            returnData[i].vol = [((DataPoint *)([tmpData objectAtIndex:i])) CGPointValue].y;
        }*/
        
        return tmpData;
        
    }

    
}