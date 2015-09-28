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
	FEA_DIST,
	COH_WEIGHT,
	SEP_WEIGHT,
	ALI_WEIGHT,
	RND_WEIGHT,
	FEA_WEIGHT,
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
    BoidGene* gene;

	void init(TextureData *f, BoidGene *g, FPoint pos, FPoint spd);

public:
	virtual void move();

	virtual FPoint getSpd();
};

class Sheep : public Boid
{
public:
	Sheep(TextureData *f, BoidGene *g);
	Sheep(TextureData *f, BoidGene *g, FPoint pos, FPoint spd);

	virtual void update(std::vector<Object*> &allBoids);
};

class Dog : public Boid
{
public:
	Dog(TextureData *f, BoidGene *g);
	Dog(TextureData *f, BoidGene *g, FPoint pos, FPoint spd);

	virtual void update(std::vector<Object*> &allBoids);
};

#endif // BOID_H
