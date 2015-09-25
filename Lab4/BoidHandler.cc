#include "BoidHandler.h"
#include <math.h>
#include <iterator>
#include <stdlib.h>
#include <time.h>
#include <iostream>

BoidHandler::BoidHandler(int numBoids, TextureData *f, BoidGene *g)
{
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
            FPoint diff = (*it)->position - (*jt)->position;

            float dist = sqrt(diff.h * diff.h + diff.v * diff.v);
            distDiff[i] = dist;
            i++;
        }
    }
}

// Initialize the size of the distance matrix
void BoidHandler::addBoids(int numBoids, TextureData *f, BoidGene *g)
{
    srand (time(NULL));
    for(int i = 0; i < numBoids; i++)
    {
        float posh = rand() % 500 + 50;
        float posv = rand() % 700 + 50;
        FPoint pos(posh, posv);

        float spdh = (rand() % 100) / 50.0;
        float spdv = (rand() % 100) / 50.0;
        FPoint spd(spdh, spdv);

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
    int index = j - 1 - i + i * size() - ()((i + 1) * i) >> 1);
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


// Returns a normalized vector pointing to the center of gravity for the boids
// within a certain radius

// Returns a normalized vector pointing away from boids that are close

// Return a normalized vector that aligns the movement of the boids
FPoint boidCalculate(SpriteRec *b, FPpoint *c, FPoint *s, FPoint *a)
{
    int totalNum = 0;
    FPoint dir;
    float dist;
	// Set to -1 to detect if any boid is within distance
	float scaling = -1;

    float cD = b->gene->cMaxDist;
    float sD = b->gene->sMaxDist;
    float aD = b->gene->aMaxDist;

    for(auto it = boids.begin(); it != boids.end(); it++)
    {
		if ((*it) != b)
		{
			dist = getDist(b->ID, (*it)->ID);

            if(dist < cD)
			{
				*coh = *coh + (*it)->position;
				totalNum++;
			}

			if(dist < sD)
			{
				scaling = (sD - dist) / sD;
				dir = b->position - (*it)->position;
				*sep = *sep + dir * scaling;
			}

            if(dist < aD)
            {
                dir = Normalize((*it)->speed);
                *ali = *ali + dir;
                boidFound = true;
            }
		}
	}

    if(totalNum)
	{
		coh = coh / (float)totalNum;
		coh = coh - boid->position;
		coh = Normalize(coh);
	}


	if(scaling > 0.0)
		sep = Normalize(sep);

    if(boidFound)
		ali = Normalize(ali);

	return sep;
}

void BoidHandler::boidBehave()
{
    for(auto it = boids.begin(); it != boids.end(); it++)
    {
        FPoint coh, sep, ali, total;
        coh = Cohesion(boid) * cWeight;
        sep = Separation(boid, i) * sWeight;
        ali = Alignment(boid, i) * aWeight;
        total = coh + sep + ali + boid->speed * pWeight;
        boid->speed = Normalize(total) * 3;
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
