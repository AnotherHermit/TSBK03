#include "BoidHandler.h"
#include "Boid.h"
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <iostream>

BoidHandler::BoidHandler(int numBoids, TextureData *f, BoidGene *g)
{
    srand (time(NULL));
    addBoids(numBoids, f, g);
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

// Initialize the size of the distance matrix
void BoidHandler::addBoids(int numBoids, TextureData *f, BoidGene *g)
{
    for(int i = 0; i < numBoids; i++)
    {
        Boid *temp = new Boid(f, g, size());
        boids.push_back(temp);
    }
}

// Get the pointer to a certain boid
Boid* BoidHandler::getBoid(int boidID)
{
    return boids[boidID];
}

// Get the number of boids
unsigned int BoidHandler::size()
{
    return boids.size();
}

void BoidHandler::boidBehave()
{
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        (*it)->update(boids);
    }
}

void BoidHandler::boidMoveDraw()
{
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        (*it)->move();
		(*it)->draw();
    }
}
