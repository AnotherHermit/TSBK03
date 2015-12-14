///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef TESTS_H
#define TESTS_H

#include <gtest/gtest.h>

#include "Particles.h"
#include "GL_utilities.h"

class myTestCase {
public:
	myTestCase(PartCount v1, GLuint v2, GLfloat v3);
	PartCount numParticles;
	GLuint numBins; // Should be a multiple of 16
	GLfloat binSize; 
};

#endif // TESTS_H