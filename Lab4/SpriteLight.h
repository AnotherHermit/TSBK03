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

// Globals: The background texture and viewport dimensions (virtual or real pixels)
extern GLuint backgroundTexID;
extern long gWidth, gHeight;

// Functions
TextureData *GetFace(const char *fileName);
void DrawSprite(TextureData *f, FPoint pos, GLfloat rotation);
void DrawBackground();

void InitSpriteLight();

#endif // SPRITELIGHT_H
