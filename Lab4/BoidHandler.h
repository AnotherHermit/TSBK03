#ifndef BOIDHANDLER_H
#define BOIDHANDLER_H

#include <vector>

#include "SpriteLight.h"
#include "Boid.h"

float getRandom(int min, int max);

class BoidHandler
{
    std::vector<Object*> boids;

public:
    BoidHandler() {}
    ~BoidHandler();

    void addSheep(int numBoids, TextureData *f, BoidGene *g);
    void addDog(int numBoids, TextureData *f, BoidGene *g);
    unsigned int size();
    void boidBehave();
    void boidMoveDraw();
};


#endif // BOIDHANDLER_H
