#ifndef __THRESHOLD_CALCULATOR_H__
#define __THRESHOLD_CALCULATOR_H__

#include "common.h"
#include "minsquare.h"
#include "cftree.h"

class ThresholdCalculator
{
public:
    ThresholdCalculator();

    void setNmax(long max);

    void track(CF_Node *tree, const CF_Cluster &treeCluster);
    data_t getNewThreshold(CF_Node *tree, size_t dim);
private:
    long N, Nmax;
    MinSquare<data_t> tFunc, rFunc;
};

#endif // __THRESHOLD_CALCULATOR_H__
