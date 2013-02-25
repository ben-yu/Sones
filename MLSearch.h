//
//  MLSearch.h
//  Sonence
//
//  Created by Benjamin Yu on 2013-02-23.
//
//

#ifndef __Sonence__MLSearch__
#define __Sonence__MLSearch__

#include <iostream>

class MLSearch {
public:
    MLSearch();
    ~MLSearch();
    double logistic(double x, double alpha, double beta, double gamma, double lambda);
    double invLogistic(double p_target, double alpha, double beta, double gamma, double lambda);
    double findThreshold(double p_target, double x, double alpha, double beta, double gamma, double lambda);
    
private:
    static const int numOfAlphas = 5;
    static const int numOfGammas = 5;
    
    double probTable[numOfAlphas][numOfGammas];
    double alphaValues[numOfAlphas];
    double gammaValues[numOfGammas];
    
};

#endif /* defined(__Sonence__MLSearch__) */
