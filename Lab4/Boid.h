#ifndef BOID_H
#define BOID_H

#include "LoadTGA.h"
#include <vector>
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

class Boid
{
protected:
	FPoint position;
	FPoint speed;
	GLfloat rotation;

    TextureData *face;

	Boid(TextureData *f);
	Boid(TextureData *f, FPoint pos, FPoint spd);

public:
    virtual void update(std::vector<Boid*> &allBoids) = 0;
    void move();
    void draw();

	FPoint getPos();
	FPoint getSpd();
};

class Sheep : public Boid
{
    BoidGene* gene;

public:
	Sheep(TextureData *f, BoidGene *g);
	Sheep(TextureData *f, FPoint pos, FPoint spd, BoidGene *g);

	virtual void update(std::vector<Boid*> &allBoids);
};


#endif // BOID_H
