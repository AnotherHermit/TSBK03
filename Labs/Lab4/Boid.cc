// Rand and Srand
#include <stdlib.h>
#include <math.h>
#include <vector>

#include "Boid.h"
#include "SpriteLight.h"

#include <iostream>

float getRandom(int min, int max)
{
    float retVal = min + (float)rand() / (float)(RAND_MAX/(max-min));
    return retVal;
}

// ===== BoidGene functions =====

BoidGene::BoidGene()
{
	data.resize(N_GENE_DATA, 0.0f);
	data[COH_DIST] = 100.0f;
	data[SEP_DIST] = 20.0f;
	data[ALI_DIST] = 70.0f;
	data[FEA_DIST] = 120.0f;
	data[COH_WEIGHT] = 0.5f;
	data[SEP_WEIGHT] = 1.0f;
	data[ALI_WEIGHT] = 0.5f;
	data[RND_WEIGHT] = 0.0f;
    data[FEA_WEIGHT] = 3.0f;
	data[PRV_WEIGHT] = 10.0f;
	data[SPEED] = 1.0f;
}

// ===== End BoidGene functions =====

// ===== Boid ======

void Boid::init(TextureData *f, BoidGene *g, FPoint pos, FPoint spd)
{
    position = pos;
    speed = spd;
    rotation = 0;
    face = f;
    gene = g;
}

// A simple movement
void Boid::move()
{
// Move by speed, bounce off screen edges.
	position += speed;
	if (position.h < 0)
	{
		speed.h = fabs(speed.h);
		position.h = 0;
	}
	if (position.v < 0)
	{
		speed.v = fabs(speed.v);
		position.v = 0;
	}
	if (position.h > gWidth)
	{
		speed.h = -fabs(speed.h);
		position.h = gWidth;
	}
	if (position.v > gHeight)
	{
		speed.v = -fabs(speed.v);
		position.v = gHeight;
	}

	rotation = atan2(speed.v, speed.h) * 180.0/3.1416;
}

FPoint Boid::getSpd()
{
    return speed;
}

// ===== End Boid ======

// ===== Sheep =====

Sheep::Sheep(TextureData *f, BoidGene *g)
    : Boid()
{
    FPoint pos(getRandom(0,gWidth), getRandom(0,gHeight));
    FPoint spd(getRandom(-1,1), getRandom(-1,1));

    init(f, g, pos, spd);
}

Sheep::Sheep(TextureData *f, BoidGene *g, FPoint pos, FPoint spd)
    : Boid()
{
    init(f, g, pos, spd);
}

void Sheep::update(std::vector<Object*> &allBoids, GLfloat deltaT)
{
    c = FPoint();
    s = FPoint();
    a = FPoint();
    r = FPoint();
    f = FPoint();        
    FPoint dir, p;
    float dist, scaling;
    int totalNum = 0;

    float sD = gene->data[SEP_DIST];
    float fD = gene->data[FEA_DIST];

    // Calculate the cohesion, separation and alignment vectors from the
    // distances to all other boids;
    for(auto it = allBoids.begin(); it != allBoids.end(); it++)
    {
        if((*it) != this)
        {
            // Check type of incoming boid
            Dog* isDog = dynamic_cast<Dog*>(*it);
            Sheep* isSheep = dynamic_cast<Sheep*>(*it);

            dist = (position - (*it)->getPos()).norm();

            // Sheep-Dog interaction
            if(isDog)
            {
                if(dist < fD)
                {
                    scaling = (fD - dist) / fD;
                    dir = Normalize(position - (*it)->getPos());
                    f += dir * scaling;
                }
            }
            // Sheep-Sheep interactions
            else if (isSheep)
            {
                // Cohesion calculates vector towards center of mass
                if(dist < gene->data[COH_DIST])
                {
                    c += (*it)->getPos();
                    totalNum++;
                }
                // Separation calculates vector pointing away from close boids
                if(dist < sD)
                {
                    scaling = (sD - dist) / sD;
                    dir = Normalize(position - (*it)->getPos());
                    s += dir * scaling;
                }
                // Alignment calculates average direction of travel
                if(dist < gene->data[ALI_DIST])
                {
                    dir = Normalize((*it)->getSpd());
                    a += dir;
                }
            }
        }
    }

    // Only when we had boids close enough do we need to calculate cohesion
    if(totalNum)
    {
        c /= (float)totalNum;
        c -= position;
    }

    // Create a random movement
    totalTime += deltaT;
    if (totalTime > 1.0f)
    {
    	totalTime = 0.0f;
    	r = FPoint(getRandom(-1,1), getRandom(-1,1));
   	    r.normalize();
	}

    // Normalize to make the weights matter
    c.normalize();
    s.normalize();
    a.normalize();
    r.normalize();
    f.normalize();

    c *= gene->data[COH_WEIGHT]; // Cohesion
    s *= gene->data[SEP_WEIGHT]; // Separation
    a *= gene->data[ALI_WEIGHT]; // Alignment
    r *= gene->data[RND_WEIGHT]; // Random direction
    f *= gene->data[FEA_WEIGHT]; // Fear
    p  = speed * gene->data[PRV_WEIGHT]; // Previous speed

    speed = Normalize(p + c + s + a + r + f) * gene->data[SPEED];
}

// ===== End Sheep =====

// ===== Dog =====

Dog::Dog(TextureData *f, BoidGene *g)
    : Boid()
{
    FPoint pos(getRandom(0,gWidth), getRandom(0,gHeight));
    FPoint spd(getRandom(-1,1), getRandom(-1,1));

    init(f, g, pos, spd);
}

Dog::Dog(TextureData *f, BoidGene *g, FPoint pos, FPoint spd)
    : Boid()
{
    init(f, g, pos, spd);
}

void Dog::update(std::vector<Object*> &allBoids, GLfloat deltaT)
{
	FPoint p;
    // Create a random movement
    totalTime += deltaT;
    if (totalTime > 1.0f)
    {
    	totalTime = 0.0f;
    	r = FPoint(getRandom(-1,1), getRandom(-1,1));
   	    r.normalize();
	}
	
    // Normalize to make the weights matter
    r *= gene->data[RND_WEIGHT]; // Random direction
    p  = speed * gene->data[PRV_WEIGHT]; // Previous speed

    speed = Normalize(p + r) * gene->data[SPEED];
}

// ===== End Dog =====
