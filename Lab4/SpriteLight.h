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

typedef struct FPoint
{
	GLfloat h, v;

	FPoint(): h(0), v(0) {}
	FPoint(float x, float y): h(x), v(y) {}

	FPoint operator+ (const FPoint& b) const
	{
		FPoint point;
		point.h = this->h + b.h;
		point.v = this->v + b.v;
		return point;
	}

	FPoint operator- (const FPoint& b) const
	{
		FPoint point;
		point.h = this->h - b.h;
		point.v = this->v - b.v;
		return point;
	}

	FPoint operator/ (const float b) const
	{
		FPoint point;
		point.h = this->h / b;
		point.v = this->v / b;
		return point;
	}

	FPoint operator* (const float b) const
	{
		FPoint point;
		point.h = this->h * b;
		point.v = this->v * b;
		return point;
	}

	FPoint(const FPoint& b)
	{
		h = b.h;
		v = b.v;
	}

	FPoint & operator= (const FPoint& b)
	{
		this->h = b.h;
		this->v = b.v;
		return *this;
	}

} FPoint;

inline
float Norm(const FPoint& a)
{
	float norm = sqrt(a.h * a.h + a.v * a.v);
	return norm;
}

inline
FPoint Normalize(const FPoint& a)
{
	FPoint point;
	point = a / Norm(a);
	return point;
}

typedef struct SpriteRec
{
	FPoint position;
	TextureData *face;
	FPoint speed;
	GLfloat rotation;

	// Add custom sprite data here as needed
} SpriteRec, *SpritePtr;

// Globals: The sprite list, background texture and viewport dimensions (virtual or real pixels)
extern SpritePtr gSpriteRoot;
extern GLuint backgroundTexID;
extern long gWidth, gHeight;

// Functions
TextureData *GetFace(const char *fileName);
struct SpriteRec *NewSprite(TextureData *f, GLfloat h, GLfloat v, GLfloat hs, GLfloat vs);
void HandleSprite(SpritePtr sp);
void DrawSprite(SpritePtr sp);
void DrawBackground();

void InitSpriteLight();

#endif // SPRITELIGHT_H
