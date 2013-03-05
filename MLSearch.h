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
    double getNextTone(double x, int channel, bool hit);
    
    int cur_freq = 0;
    
private:
    static const int numOfChannels = 2;
    static const int numOfAlphas = 5; // Mean value
    static const int numOfGammas = 5; // False-alarm rate
    static const int numOfFrequencies = 10;
    
    
    float beta = 1.0; // Beta - Psychometric Slope
    float lambda = 0.0;// Lambda - Expectation of attention lapses
    
    double probTable[numOfChannels][numOfFrequencies][numOfAlphas][numOfGammas];
    double freqValues[numOfFrequencies];
    double alphaValues[numOfAlphas]; // Mean value
    double gammaValues[numOfGammas]; // False-alarm rate
    
};

#endif /* defined(__Sonence__MLSearch__) */
