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
    for (int i=0; i < numOfAlphas; i++)
    {
        alphaValues[i] = i * 0.1f;
    }
    
    for (int i=0; i < numOfGammas; i++)
    {
        gammaValues[i] = i * 0.1f;
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

double MLSearch::findThreshold(double p_target, double x, double alpha, double beta, double gamma, double lambda)
{
    double max = 0;
    int max_i, max_j;
    // Fill probability table & find max
    for (int i=0; i < numOfAlphas; i++) {
        for (int j=0; j < numOfGammas; j++)
        {
            this->probTable[i][j] += log(logistic(x,alphaValues[i],beta,gammaValues[j],lambda));
            if (probTable[i][j] > max) {
                max = probTable[i][j];
                max_i = i;
                max_j = j;
            }
        }
    }

    // Calculate new level
    return invLogistic(0.0, alphaValues[max_i], beta, gammaValues[max_j], lambda);
    
}