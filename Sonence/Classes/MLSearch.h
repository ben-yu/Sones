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
    void setMode(int mode);
    
    int cur_freq = 0;
    
private:
    static const int numOfChannels = 2;
    static const int numOfAlphas = 300; // Mean value
    static const int numOfGammas = 5; // False-alarm rate
    static const int numOfFrequencies = 10;
    
    
    double beta = 1.0; // Beta - Psychometric Slope
    double lambda = 0.0;// Lambda - Expectation of attention lapses
    double stepSize = -0.1;
    
    int mode = 0;
    bool prevMode = true;
    int hitCount = 0;
    
    double probTable[numOfChannels][numOfFrequencies][numOfAlphas][numOfGammas];
    double freqValues[numOfFrequencies];
    double alphaValues[numOfAlphas]; // Mean value
    double gammaValues[numOfGammas]; // False-alarm rate
    
    double stepSizes[numOfFrequencies] = {-0.1,-0.1,-0.1,-0.1,-0.1,
                                          -0.1,-0.1,-0.1,-0.1,-0.1};
    
};

#endif /* defined(__Sonence__MLSearch__) */
