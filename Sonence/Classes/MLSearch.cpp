//
//  MLSearch.cpp
//  Sonence
//
//  Created by Benjamin Yu on 2013-02-23.
//
//

#include "MLSearch.h"
#include <math.h>

MLSearch::MLSearch()
{
    for (int i=-numOfAlphas/2; i < numOfAlphas/2; i++)
    {
        alphaValues[i+numOfAlphas/2] = i * 0.2f;
    }
    
    for (int i=0; i < numOfGammas; i++)
    {
        gammaValues[i] = i * 0.1f;
    }
    
    for (int channel = 0; channel < numOfChannels; channel++) {
        for (int freq = 0; freq < numOfFrequencies; freq++) {
            for (int i=0; i < numOfAlphas; i++) {
                for (int j=0; j < numOfGammas; j++)
                {
                    this->probTable[channel][freq][i][j] = 1.0;
                }
            }
        }
    }

}

double MLSearch::logistic(double x, double alpha, double beta, double gamma, double lambda)
{
    return gamma+((1-lambda-gamma)*(1/(1+exp(beta*(alpha-x)))));
}

double MLSearch::invLogistic(double p_target, double alpha, double beta, double gamma, double lambda)
{
    return alpha-(1/beta)*log(((1-lambda-gamma)/(p_target-gamma))-1);
}

double MLSearch::getNextTone(double x, int channel, bool hit)
{
    if (mode == 0) {
        double max = 0;
        int max_i, max_j = 0;
        // Fill probability table & find max
        for (int i=0; i < numOfAlphas; i++) {
            for (int j=0; j < numOfGammas; j++)
            {
                if (hit)
                    this->probTable[channel][cur_freq][i][j] *= logistic(x,alphaValues[i],beta,gammaValues[j],lambda);
                else
                    this->probTable[channel][cur_freq][i][j] *= 1.0 - logistic(x,alphaValues[i],beta,gammaValues[j],lambda);
                
                if (probTable[channel][cur_freq][i][j] > max) {
                    max = probTable[channel][cur_freq][i][j];
                    max_i = i;
                    max_j = j;
                }
            }
        }
        // Calculate new level
        return invLogistic(0.6, alphaValues[max_i], beta, gammaValues[max_j], lambda); // Sweetspot of 0.6 TP rates
    } else {
        // Staircase Search
        if (x <= 0 || (hit && !prevMode) || (!hit && prevMode)) {
            stepSize = -stepSize/2;
        }
        prevMode = hit;
        return stepSize;
        
    }
}

void MLSearch::setMode(int mode)
{
    this->mode = mode;
}