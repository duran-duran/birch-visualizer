#include "oxygine-framework.h"
#include <functional>
#include "res/SingleResAnim.h"
#include "common.h"
#include "cftree.h"
#include "metrics.h"
#include "threshold_calculator.h"
using namespace oxygine;

Resources res;

void example_preinit() {}

void example_init(char *filename)
{
    res.loadXML("res.xml");

    spColorRectSprite canvas = new ColorRectSprite();
    canvas->setColor(Color::White);
    canvas->setSize(640, 640);
    canvas->attachTo(getStage());

    FILE* pfile = fopen(filename, "rb");
    long count;
    int dim;

    fread(&count, sizeof(count), 1, pfile);
    fread(&dim, sizeof(dim), 1, pfile);

    std::vector<DataPoint> points;
    for (long i = 0; i < count; ++i)
    {
        DataPoint point(dim);
        fread(&point[0], sizeof(data_t), dim, pfile);
        points.push_back(point);
    }

    CF_Node *tree;
    CF_Vector subclusters;
    int maxEntries = sqrt(count);
    std::cout << "Max entries in tree: " << maxEntries << std::endl;

    size_t entries = 0;
    double t = /*1*/0;
    size_t branching = 13;
    std::cout << "Start building with threshold " << t << std::endl;
    tree = new CF_Node(t, branching, &entries);
    auto treeCluster = CF_Cluster(tree);

    ThresholdCalculator tCalc;
    tCalc.setNmax(count);
    size_t trackEach = std::log(count);

    for (size_t i = 0; i < points.size(); ++i)
    {
        tree->insert(CF_Cluster(points[i]));
        treeCluster.add(CF_Cluster(points[i]));
        if (i % trackEach == 0 && i != 0)
            tCalc.track(tree, treeCluster);
        if (entries > maxEntries)
        {
            t = tCalc.getNewThreshold(tree, dim);
            subclusters = getAllLeafEntries(tree);
            tree->clear();
            delete tree;
//            t *= 1.1;
            std::cout << "Need rebuilding, new threshold is " << t << std::endl;
            entries = 0;
            tree = new CF_Node(t, branching, &entries);
            for (size_t j = 0; j < subclusters.size(); ++j)
                tree->insert(subclusters[j]);
            treeCluster = CF_Cluster(tree);
        }
    }


    subclusters = getAllLeafEntries(tree);
    std::cout << "Total number of leaf entries: " << subclusters.size() << std::endl;
//    getAllSubclusters(tree, subclusters);
    int n = 0;
    for (size_t i = 0; i < subclusters.size(); ++i)
        n += subclusters[i].N;
    std::cout << "Total number of points in leaf entries: " << n << std::endl;

    auto leafNodes = getAllLeafNodes(tree);
    CF_Vector centroids;
    size_t k = sqrt(subclusters.size());
    for (size_t i = 0; i < k; ++i)
        centroids.emplace_back(DataPoint{(double)rand() / RAND_MAX * treeCluster.R,  (double)rand() / RAND_MAX * treeCluster.R} + treeCluster.X0);
//    size_t k = leafNodes.size();
//    for (size_t i = 0; i < k; ++i)
//    {
//        centroids.push_back(CF_Cluster(leafNodes[i]).X0);
//    }
    std::cout << "Creating " << k << " new clusters via k-means" << std::endl;
    std::vector<CF_Vector> newClusters(k);
    data_t MSE = treeCluster.SS,
           newMSE = MSE;
    do
    {
        MSE = newMSE;
        newMSE = 0;
        for (size_t j = 0; j < k; ++j)
            newClusters[j].clear();
        for (size_t i = 0; i < subclusters.size(); ++i)
        {
            auto closest = subclusters[i].findClosest(centroids);
            size_t ind = closest - centroids.begin();

            newClusters[ind].push_back(subclusters[i]);

            data_t dist = getDistance(subclusters[i], *closest);
            newMSE += dist*dist;

//            std::cout << "Adding " << i << " entry to " << ind << " cluster" << std::endl;
        }
        for (size_t j = 0; j < centroids.size(); ++j)
        {
            if (!newClusters[j].empty())
                centroids[j] = CF_Cluster(newClusters[j]).X0;
        }
        std::cout << "New MSE: " << newMSE << std::endl;
    }
    while (newMSE < MSE);


//    tree->clear();
//    delete tree;

    std::vector<Color> colors;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            for (int k = 0; k < 8; ++k)
                colors.push_back(Color(32 * i + rand() % 32, 32 * j + rand() % 32, 32 * k + rand() % 32));
    std::random_shuffle(colors.begin(), colors.end());

    for (size_t i = 0; i < k; ++i)
    {
        for (size_t j = 0; j < newClusters[i].size(); ++j)
        {
            spColorRectSprite circle = new ColorRectSprite();
            circle->setColor(colors.at(i));
            circle->setAnchor(0.5, 0.5);
            circle->setSize(newClusters[i][j].D, newClusters[i][j].D);
            circle->setPosition(newClusters[i][j].X0[0], newClusters[i][j].X0[1]);
            circle->attachTo(canvas);
        }
    }

//    for (size_t j = 0; j < leafNodes.size(); ++j)
//    {
//        for (auto& cluster : leafNodes[j]->getSubclusters())
//        {
//            spColorRectSprite circle = new ColorRectSprite();
//            circle->setColor(colors[j]);
//            circle->setAnchor(0.5, 0.5);
//            circle->setSize(cluster.D, cluster.D);
//            circle->setPosition(cluster.X0[0], cluster.X0[1]);
//            circle->attachTo(canvas);
//        }
//    }

//    for (size_t i = 0; i < points.size(); ++i)
//    {
//        spColorRectSprite dot = new ColorRectSprite();
//        dot->setColor(Color::Red);
//        dot->setSize(1, 1);
//        dot->setPosition(points[i][0], points[i][1]);
//        dot->attachTo(canvas);
//    }
}

void example_update()
{
}

void example_destroy()
{
    res.free();
}
