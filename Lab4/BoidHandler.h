#ifndef BOIDHANDLER_H
#define BOIDHANDLER_H

#include <vector>

#include "SpriteLight.h"

class BoidHandler
{
    std::vector<float> distDiff;
    std::vector<SpriteRec*> boids;

public:
    BoidHandler(int numBoids, TextureData* f);
    ~BoidHandler();
    void updateDist();
    void addBoids(int numBoids, TextureData* f);
    float getDist(int boidA, int boidB);
    int getNum();
    SpriteRec* getBoid(int boid);
};


#endif // BOIDHANDLER_H
