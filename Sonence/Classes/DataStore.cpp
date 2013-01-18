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
    
    void DataStore::saveData(double freq, double volume) {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        [vc storeDataPoint:@"test" freqThresh:freq volThresh:volume];
    }
    
    DataPoint* DataStore::readData() {
        RootViewController *vc = (RootViewController *) rootVCPtr;
        NSMutableArray* tmpData =  [vc getData];
        //if (!returnData) {
            numOfElements = [tmpData count];
            returnData = (DataPoint*) calloc(sizeof(DataPoint), numOfElements);
        //}
            
        
        for (int i = 1; i < numOfElements; i++) {
            returnData[i].freq = [((NSValue *)([tmpData objectAtIndex:i])) CGPointValue].x;
            returnData[i].vol = [((NSValue *)([tmpData objectAtIndex:i])) CGPointValue].y;
        }
        
        return returnData;
        
    }

    
}