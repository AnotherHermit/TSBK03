#ifndef BOIDHANDLER_H
#define BOIDHANDLER_H

#include <vector>

#include "SpriteLight.h"
#include "Boid.h"

float getRandom(int min, int max);

class BoidHandler
{
    std::vector<Boid*> boids;

public:
    BoidHandler(int numBoids, TextureData *f, BoidGene *g);
    ~BoidHandler();

    void addBoids(int numBoids, TextureData *f, BoidGene *g);
    Boid* getBoid(int boidID);
    unsigned int size();
    void boidBehave();
    void boidMoveDraw();
};


#endif // BOIDHANDLER_H
