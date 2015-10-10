#include "BoidHandler.h"
#include "Boid.h"

#include <iostream>

// BoidHandler::BoidHandler() {}

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
void BoidHandler::addSheep(int numBoids, TextureData *f, BoidGene *g)
{
    for(int i = 0; i < numBoids; i++)
    {
        Object *temp = new Sheep(f, g);
        boids.push_back(temp);
    }
}

void BoidHandler::addDog(int numBoids, TextureData *f, BoidGene *g)
{
    for(int i = 0; i < numBoids; i++)
    {
        Object *temp = new Dog(f, g);
        boids.push_back(temp);
    }
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
