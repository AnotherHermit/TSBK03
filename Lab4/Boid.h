#ifndef BOID_H
#define BOID_H

#include <vector>

#include "LoadTGA.h"

#include "SpriteLight.h"

enum GeneData
{
	COH_DIST,
	SEP_DIST,
	ALI_DIST,
	MAX_DIST,
	COH_WEIGHT,
	SEP_WEIGHT,
	ALI_WEIGHT,
	RND_WEIGHT,
	PRV_WEIGHT,
	SPEED,
	N_GENE_DATA
};

typedef struct BoidGene
{
	BoidGene();
	std::vector<float> data;

} BoidGene;

class Boid : public Drawable
{
protected:
	FPoint speed;

	void init(TextureData *f, FPoint pos, FPoint spd);
public:
	virtual void move();

	virtual FPoint getSpd();
};

class Sheep : public Boid
{
    BoidGene* gene;

public:
	Sheep(TextureData *f, BoidGene *g);
	Sheep(TextureData *f, BoidGene *g, FPoint pos, FPoint spd);

	virtual void update(std::vector<Object*> &allBoids);
};

class Dog : public Boid
{
public:
	Dog(TextureData *f);
	Dog(TextureData *f, FPoint pos, FPoint spd);

	virtual void update(std::vector<Boid*> &allBoids);
};

#endif // BOID_H
