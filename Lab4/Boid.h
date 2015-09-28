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
public:
    FPoint position;
	FPoint speed;
	GLfloat rotation;

	int ID;

    TextureData *face;
    BoidGene* gene;

    Boid(TextureData *f, BoidGene *g, int id);
    Boid(TextureData *f, BoidGene *g, int id, FPoint pos, FPoint spd);

    void update(std::vector<Boid*> &allBoids);
    void move();
    void draw();
};

#endif // BOID_H
