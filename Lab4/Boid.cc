#include "Boid.h"
#include "SpriteLight.h"

// Rand and Srand
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>

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
	data[MAX_DIST] = fmax(fmax(data[COH_DIST], data[SEP_DIST]), data[ALI_DIST]);
	data[COH_WEIGHT] = 0.5f;
	data[SEP_WEIGHT] = 1.0f;
	data[ALI_WEIGHT] = 0.5f;
	data[RND_WEIGHT] = 0.0f;
	data[PRV_WEIGHT] = 10.0f;
	data[SPEED] = 1.0f;
}

// ===== End BoidGene functions =====

// ===== Boid ======

Boid::Boid(TextureData *f)
    : rotation(0), face(f)
{
    FPoint pos(getRandom(0,gWidth), getRandom(0,gHeight));
    FPoint spd(getRandom(-1,1), getRandom(-1,1));

    position = pos;
    speed = spd;
}

Boid::Boid(TextureData *f, FPoint pos, FPoint spd)
    : position(pos), speed(spd), rotation(0), face(f)
{}

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

void Boid::draw()
{
    DrawSprite(face, position, rotation);
}

FPoint Boid::getPos()
{
    return position;
}

FPoint Boid::getSpd()
{
    return speed;
}

// ===== End Boid ======

// ===== Sheep =====

Sheep::Sheep(TextureData *f, BoidGene *g)
    : Boid(f), gene(g)
{}

Sheep::Sheep(TextureData *f, FPoint pos, FPoint spd, BoidGene *g)
    : Boid(f, pos, spd), gene(g)
{}

void Sheep::update(std::vector<Boid*> &allBoids)
{
    FPoint c, s, a, r, p;
    FPoint dir;
    float dist, scaling;
    int totalNum = 0;

    float sD = gene->data[SEP_DIST];

    // Calculate the cohesion, separation and alignment vectors from the
    // distances to all other boids;
    for(auto it = allBoids.begin(); it != allBoids.end(); it++)
    {
        if ((*it) != this)
        {
            dist = (position - (*it)->getPos()).norm();

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
                dir = position - (*it)->getPos();
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

    // Only when we had boids close enough do we need to calculate cohesion
    if(totalNum)
    {
        c /= (float)totalNum;
        c -= position;
    }

    // Create a random movement
    r = FPoint(getRandom(-1,1), getRandom(-1,1));

    // Normalize to make the weights matter
    c.normalize();
    s.normalize();
    a.normalize();
    r.normalize();

    c *= gene->data[COH_WEIGHT]; // Cohesion
    s *= gene->data[SEP_WEIGHT]; // Separation
    a *= gene->data[ALI_WEIGHT]; // Alignment
    r *= gene->data[RND_WEIGHT]; // Random direction
    p  = speed * gene->data[PRV_WEIGHT]; // Previous speed

    speed = Normalize(p + c + s + a + r) * gene->data[SPEED];
}

// ===== End Sheep =====
