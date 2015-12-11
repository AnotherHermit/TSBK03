///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Fixtures.h"

#include "Tests.h"

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
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetParticleBuffers()[0]);
	ParticleStruct* resultPart = (ParticleStruct*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetParticles(); i++) {
		ASSERT_EQ(parts->GetParticleData()[i].bin, resultPart[i].bin) << "Particle " << i << " has wrong bin.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Check that bins have the correct count
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[0]);
	GLint* resultBin = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultBin[i]) << "Bin " << i << " has wrong count.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputePrefix, DISABLED_PrefixGatherFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(parts->GetBinBuffers()[1], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		EXPECT_EQ(CPUBin[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(parts->GetBinBuffers()[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputePrefix, DISABLED_PrefixReduceFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixReduce();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(parts->GetBinBuffers()[1], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		EXPECT_EQ(CPUBin[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(parts->GetBinBuffers()[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputePrefix, DISABLED_PrefixSpreadFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(parts->GetBinBuffers()[1], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		EXPECT_EQ(CPUBin[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(parts->GetBinBuffers()[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputePrefix, DISABLED_PrefixSumTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	parts->ComputePrefixReduce();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();

	CPUTimer->startTimer();
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		totalSum += CPUBin[i];
	}
	CPUTimer->endTimer();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[3]);
	GLint* resultSum = (GLint*)glMapBuffer(parts->GetBinBuffers()[3], GL_READ_ONLY);
	ASSERT_EQ(totalSum, resultSum[1023]);
	glUnmapBuffer(parts->GetBinBuffers()[1]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
