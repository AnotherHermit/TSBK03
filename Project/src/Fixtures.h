///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef FIXTURES_H
#define FIXTURES_H

#include "Particles.h"

#include "GL_utilities.h"

#include <gtest/gtest.h>

#include <vector>

class ComputeTest : public ::testing::TestWithParam<const int*> {
protected:
	SDL_Window* screen;
	SDL_GLContext glcontext;

	Timer* CPUTimer;
	GLTimer* GPUTimer;

	void InitOpenGL();
	void ExitOpenGL();

	virtual void CPUSolution() = 0;

public:
	ComputeTest() {};
};

class ComputeBin : public ComputeTest {
protected:
	Particles* parts;

	GLint* CPUBin;
	GLint* CPUPrefix;
	
	virtual void CPUSolution();
	void CPUSolutionPrefix();

public:
	ComputeBin() {};

	virtual void SetUp();
	virtual void TearDown();

};

class ComputePrefix : public ComputeTest {
protected:
	Particles* parts;

	GLint* CPUBin;
	GLint* TempBin;
	GLint totalSum;

	virtual void CPUSolution();
	void CPUSolutionGather();
	void CPUSolutionReduce();
	void CPUSolutionSpread();

public:
	ComputePrefix() {};

	virtual void SetUp();
	virtual void TearDown();
};

#endif // FIXTURES_H
