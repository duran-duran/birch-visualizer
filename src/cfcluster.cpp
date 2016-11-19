#include "cftree.h"
#include "cfcluster.h"
#include "metrics.h"

CF_Cluster::CF_Cluster(const DataPoint &point) :
    N(1),
    LS(point),
    SS(dot(point, point)),
    child(nullptr)
{
    updateMetrics();
}

CF_Cluster::CF_Cluster(const CF_Vector &entries) :
    N(0),
    SS(0),
    child(nullptr)
{
    if (entries.empty())
        return;

    LS.resize(entries.front().LS.size());
    for (size_t i = 0; i < entries.size(); ++i)
    {
        N += entries[i].N;
        LS += entries[i].LS;
        SS += entries[i].SS;
    }

    updateMetrics();
}

CF_Cluster::CF_Cluster(CF_Node *node) :
    N(0),
    SS(0),
    child(node)
{
    auto subclusters = node->getSubclusters();
    if (subclusters.empty())
        return;

    LS.resize(subclusters.front().LS.size());
    for (size_t i = 0; i < subclusters.size(); ++i)
    {
        N += subclusters[i].N;
        LS += subclusters[i].LS;
        SS += subclusters[i].SS;
    }

    updateMetrics();
}

void CF_Cluster::add(const CF_Cluster &entry)
{
    N += entry.N;
    LS += entry.LS;
    SS += entry.SS;

    updateMetrics();
}

void CF_Cluster::remove(const CF_Cluster &entry)
{
    N -= entry.N;
    LS -= entry.LS;
    SS -= entry.SS;

    updateMetrics();
}

void CF_Cluster::updateMetrics()
{
    X0 = LS / (data_t)N;
    R = sqrt((SS - 2 * dot(X0, LS) + N * dot(X0, X0)) / N);
    D = (N > 1) ? sqrt(2 * (N * SS - dot(LS, LS)) / (N * (N - 1))) : 0;
    if (isnan(R) || isinf(R) || isnan(D) || isinf(D))
        int a = 1;
}

CF_Vector_it CF_Cluster::findClosest(CF_Vector &clusters) const
{
    if (clusters.empty())
        return clusters.end();

    auto closest = clusters.begin();
    auto shortestDist = getDistance(*this, *closest);

    for (auto it = closest + 1; it != clusters.end(); ++it)
    {
        auto distance = getDistance(*this, *it);
        if (distance < shortestDist)
        {
            closest = it;
            shortestDist = distance;
        }
    }

    return closest;
}
