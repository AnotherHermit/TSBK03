#include "BoidHandler.h"
#include <math.h>
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <iostream>

float getRandom(int min, int max)
{
    float retVal = min + (float)rand() / (float)(RAND_MAX/(max-min));
    return retVal;
}

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

// Simple check all distances between all pairs
void BoidHandler::updateDist()
{
    int i = 0;
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        for(auto jt = std::next(it,1); jt != boids.end(); jt++)
        {
            distDiff[i] = ((*it)->position - (*jt)->position).norm();
            i++;
        }
    }
}

// Initialize the size of the distance matrix
void BoidHandler::addBoids(int numBoids, TextureData *f, BoidGene *g)
{
    for(int i = 0; i < numBoids; i++)
    {
        FPoint pos(getRandom(0,gWidth), getRandom(0,gHeight));
        FPoint spd(getRandom(-1,1), getRandom(-1,1));

        SpriteRec *temp = NewSprite(f, pos, spd, g, size());
        boids.push_back(temp);
    }
    // Will always be even so shift will be correct div with 2
    int numDiff = ((size()-1)*size()) >> 1;
    distDiff.resize(numDiff);
    updateDist();
}

// Get the distance between two boids
float BoidHandler::getDist(int boidA, int boidB)
{
    int i = std::min(boidA,boidB);
    int j = std::max(boidA,boidB);
    // Will always be even so shift will be correct div with 2
    int index = j - 1 - i + i * size() - (((i + 1) * i) >> 1);
    return distDiff[index];
}

// Get the pointer to a certain boid
SpriteRec* BoidHandler::getBoid(int boid)
{
    return boids[boid];
}

// Get the number of boids
unsigned int BoidHandler::size()
{
    return boids.size();
}

FPoint BoidHandler::boidCalculate(SpriteRec *b)
{
    FPoint c, s, a, r, p, total;
    FPoint dir;
    float dist, scaling;
    int totalNum = 0;

    float sD = b->gene->sMaxDist;

    // Calculate the cohesion, separation and alignment vectors from the
    // distances to all other boids;
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
		if ((*it) != b)
		{
			dist = getDist(b->ID, (*it)->ID);

            // Cohesion calculates vector towards center of mass
            if(dist < b->gene->cMaxDist)
			{
				c += (*it)->position;
				totalNum++;
			}

            // Separation calculates vector pointing away from close boids
			if(dist < sD)
			{
				scaling = (sD - dist) / sD;
				dir = b->position - (*it)->position;
				s += dir * scaling;
			}

            // Alignment calculates average direction of travel
            if(dist < b->gene->aMaxDist)
            {
                dir = Normalize((*it)->speed);
                a += dir;
            }
		}
	}

    // Only when we had boids close enough do we need to calculate cohesion
    if(totalNum)
	{
		c /= (float)totalNum;
		c -= b->position;
	}

    // Create a random movement
    r = FPoint(getRandom(-1,1), getRandom(-1,1));

    // Normalize to make the weights matter
    c.normalize();
	s.normalize();
	a.normalize();
    r.normalize();

    c *= b->gene->cWeight; // Cohesion
    s *= b->gene->sWeight; // Separation
    a *= b->gene->aWeight; // Alignment
    r *= b->gene->rWeight; // Random direction
    p  = b->speed * b->gene->pWeight; // Previous speed

    total = Normalize(p + c + s + a + r) * b->gene->speed;
    return total;
}

void BoidHandler::boidBehave()
{
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        (*it)->speed = boidCalculate(*it);
    }
}

void BoidHandler::boidHandleDraw()
{
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        HandleSprite(*it);
		DrawSprite(*it);
    }
}
