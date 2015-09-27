// SpriteLight - Heavily simplified sprite engine
// by Ingemar Ragnemalm 2009

// What does a mogwai say when it sees a can of soda?
// Eeek! Sprite light! Sprite light!
#ifndef SPRITELIGHT_H
#define SPRITELIGHT_H

#ifdef __APPLE__
// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#ifdef _WIN32
// MS
		#include "glew.h"
		#include "freeglut.h"

	#else
// Linux
		#include "MicroGlut.h"
		#include <GL/gl.h>
//		#include <GL/glut.h>
	#endif
#endif

#include "LoadTGA.h"
#include <iostream>
#include <math.h>

typedef struct BoidGene
{
    float cMaxDist = 100.0f;
    float sMaxDist = 20.0f;
    float aMaxDist = 70.0f;
    float cWeight = 0.5f;
    float sWeight = 1.0f;
    float aWeight = 0.5f;
	float rWeight = 0.0f;
    float pWeight = 10.0f;
	float speed   = 1.0f;

} BoidGene;

typedef struct FPoint
{
	GLfloat h, v;

	FPoint(): h(0.0f), v(0.0f) {}
	FPoint(float x, float y): h(x), v(y) {}
	FPoint(const FPoint& b): h(b.h), v(b.v) {}

	FPoint &operator=(const FPoint &b);
	FPoint operator+(const FPoint &b) const;
	FPoint operator-(const FPoint &b) const;
	FPoint operator/(const float b) const;
	FPoint operator*(const float b) const;
	FPoint &operator+=(const FPoint &b);
	FPoint &operator-=(const FPoint &b);
	FPoint &operator*=(const float b);
	FPoint &operator/=(const float b);

	FPoint &normalize();
	float norm();
} FPoint;

FPoint Normalize(FPoint a);
FPoint Clamp(FPoint a, float c);

typedef struct SpriteRec
{
	FPoint position;
	TextureData *face;
	FPoint speed;
	GLfloat rotation;

	// Add custom sprite data here as needed
	int ID;
	BoidGene* gene;

} SpriteRec, *SpritePtr;

// Globals: The background texture and viewport dimensions (virtual or real pixels)
extern GLuint backgroundTexID;
extern long gWidth, gHeight;

// Functions
TextureData *GetFace(const char *fileName);
struct SpriteRec *NewSprite(TextureData *f, FPoint pos, FPoint spd, BoidGene *g, int id);
void HandleSprite(SpritePtr sp);
void DrawSprite(SpritePtr sp);
void DrawBackground();

void InitSpriteLight();

#endif // SPRITELIGHT_H
