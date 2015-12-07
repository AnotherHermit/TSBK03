///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Tests.h"
#include <iostream>

TEST_F(ComputeTest, BinTest) {
	// Do the work
	GPUTimer->startTimer();

	parts->ComputeBins();

	GPUTimer->endTimer();
	
	// Get the results
	GLint* resultBin = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[0], GL_READ_WRITE);
	ParticleStruct* resultPart = (ParticleStruct*)glMapNamedBuffer(parts->GetParticleBuffers()[0], GL_READ_WRITE);

	// Compare to CPU based test for bins
	CPUTimer->startTimer();
	for (size_t i = 0; i < parts->GetParticles(); i++) {
		glm::vec3 pos = parts->GetParticleData()[i].position / 20.0f;
		GLuint bin = (GLuint)floor(pos.x) + (GLuint)floor(pos.y) * parts->GetBins() + (GLuint)floor(pos.z) * parts->GetBins() * parts->GetBins();

		resultBin[bin]--;

		ASSERT_EQ(resultPart[i].bin, bin) << "Particle " << i << " has wrong bin.";
	}
	CPUTimer->endTimer();

	// Make sure bin count is correct
	GLint totalSum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		totalSum += resultBin[i];
		EXPECT_EQ(0, resultBin[i]) << "Bin " << i << " has wrong count.";
	}

	ASSERT_EQ(0, totalSum) << "Total sum of particles is wrong";

	// A little cleanup
	glUnmapNamedBuffer(parts->GetBinBuffers()[0]);
	glUnmapNamedBuffer(parts->GetParticleBuffers()[0]);
}

TEST_F(ComputeTest, PrefixTest) {
	// Prepare
	parts->ComputeBins();

	// Do the work
	GPUTimer->startTimer();

	parts->ComputePrefix();

	GPUTimer->endTimer();

	// Get the results
	GLint* BinBuffer = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[0], GL_READ_WRITE);
	GLint* PrefixBuffer = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[1], GL_READ_WRITE);

	// Compare to CPU based test for bins
	CPUTimer->startTimer();

	GLint sum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
	
		EXPECT_EQ(sum, PrefixBuffer[i]) << "Prefix " << i << " is not correct sum.";
		sum += BinBuffer[i];
	}
	CPUTimer->endTimer();
	
	ASSERT_EQ(parts->GetParticles(), sum) << "Total prefix sum is wrong.";

	// A little cleanup
	glUnmapNamedBuffer(parts->GetBinBuffers()[0]);
	glUnmapNamedBuffer(parts->GetBinBuffers()[1]);
}