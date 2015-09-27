#ifndef BOIDHANDLER_H
#define BOIDHANDLER_H

#include <vector>

#include "SpriteLight.h"


float getRandom(int min, int max);

class BoidHandler
{
    std::vector<float> distDiff;
    std::vector<SpriteRec*> boids;

    FPoint boidCalculate(SpriteRec *b);

public:
    BoidHandler(int numBoids, TextureData *f, BoidGene *g);
    ~BoidHandler();

    void updateDist();
    void addBoids(int numBoids, TextureData *f, BoidGene *g);
    float getDist(int boidA, int boidB);
    SpriteRec* getBoid(int boid);
    unsigned int size();
    void boidBehave();
    void boidHandleDraw();
};


#endif // BOIDHANDLER_H
