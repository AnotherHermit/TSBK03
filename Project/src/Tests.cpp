///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Tests.h"

#include "Fixtures.h"

INSTANTIATE_TEST_CASE_P(DifferentNumParticlesTest, ComputeBin, ::testing::Values(&NumParticles[0], &NumParticles[1], &NumParticles[2], &NumParticles[3], &NumParticles[4]));
INSTANTIATE_TEST_CASE_P(DifferentNumParticlesTest, ComputePrefix, ::testing::Values(&NumParticles[0], &NumParticles[1], &NumParticles[2], &NumParticles[3], &NumParticles[4]));

TEST_P(ComputeBin, BinFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputeBins();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Check that particle is assigned correct bin
	ParticleStruct* resultPart = (ParticleStruct*)glMapNamedBuffer(parts->GetParticleBuffers()[0], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetParticles(); i++) {
		ASSERT_EQ(parts->GetParticleData()[i].bin, resultPart[i].bin) << "Particle " << i << " has wrong bin.";
	}
	glUnmapNamedBuffer(parts->GetParticleBuffers()[0]);

	// Check that bins have the correct count
	GLint* resultBin = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[0], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultBin[i]) << "Bin " << i << " has wrong count.";
	}
	glUnmapNamedBuffer(parts->GetBinBuffers()[0]);
}

TEST_P(ComputePrefix, PrefixFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefix();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	GLint* resultPrefix = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[1], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapNamedBuffer(parts->GetBinBuffers()[1]);
}

TEST_P(ComputePrefix, DISABLED_PrefixSumTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefix();
	GPUTimer->endTimer();

	GLint* resultSum = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[1], GL_READ_ONLY);
	ASSERT_EQ(totalSum, resultSum[0]);
	glUnmapNamedBuffer(parts->GetBinBuffers()[1]);
}