#include "threshold_calculator.h"
#include <limits>

ThresholdCalculator::ThresholdCalculator() :
    N(0), Nmax(std::numeric_limits<decltype(Nmax)>::max())
{

}

void ThresholdCalculator::setNmax(long max)
{
    Nmax = max;
}

void ThresholdCalculator::track(CF_Node *tree, const CF_Cluster &treeCluster)
{
    N = treeCluster.N;
    rFunc.addPoint(std::make_pair(treeCluster.N, treeCluster.R));
    tFunc.addPoint(std::make_pair(treeCluster.N, getMaxLeafEntryDiameter(tree)));
}

data_t ThresholdCalculator::getNewThreshold(CF_Node *tree, size_t dim)
{
    auto newN = std::min(2 * N, Nmax);

    auto expF = std::max(1.0, rFunc.getY(newN) / rFunc.getY(N));
    auto newT = tFunc.getY(newN);
    newT = std::max(getMinNewThreshold(tree), expF * newT);
    if (newT < tFunc.getY(N))
        newT *= std::pow(newN / N, 1 / dim);
    return newT;
}

