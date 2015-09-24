#include "BoidHandler.h"
#include <math.h>
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <iostream>

BoidHandler::BoidHandler(int numBoids, TextureData* f)
{
    addBoids(numBoids, f);
}

// Don't forget to remove the pointers
BoidHandler::~BoidHandler()
{
    while (!boids.empty())
    {
        free(boids.back());
        boids.pop_back();
    }
}

// Simple check all distances between all pairs
void BoidHandler::updateDist()
{
    int i = 0;
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        for(auto jt = std::next(it,1); jt != boids.end(); jt++)
        {
            FPoint diff = (*it)->position - (*jt)->position;

            float dist = sqrt(diff.h * diff.h + diff.v * diff.v);
            distDiff[i] = dist;
            i++;
        }
    }
}

// Initialize the size of the distance matrix
void BoidHandler::addBoids(int numBoids, TextureData* f)
{
    int newNumBoids = boids.size() + numBoids;
    srand (time(NULL));
    for(int i = 0; i < numBoids; i++)
    {
        int posv = rand() % 700 + 50;
        int posh = rand() % 500 + 50;
        float spdv = (rand() % 100) / 50.0;
        float spdh = (rand() % 100) / 50.0;

        SpriteRec* temp = NewSprite(f, posv, posh, spdv, spdh);
        boids.push_back(temp);
    }

    int numDiff = ((newNumBoids-1)*newNumBoids) / 2;
    distDiff.resize(numDiff);
    updateDist();
}

// Get the distance between two boids
float BoidHandler::getDist(int boidA, int boidB)
{
    int i = std::min(boidA,boidB);
    int j = std::max(boidA,boidB);
    int index = j - 1 - i + i * boids.size() - ((i + 1) * i) / 2;
    return distDiff[index];
}

// Get the number of boids currently in use
int BoidHandler::getNum()
{
    return boids.size();
}

// Get the pointer to a certain boid
SpriteRec* BoidHandler::getBoid(int boid)
{
    return boids[boid];
}
