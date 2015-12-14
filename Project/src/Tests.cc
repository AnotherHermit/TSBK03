///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#include "Fixtures.h"

#include "Tests.h"

myTestCase::myTestCase(PartCount v1, GLuint v2, GLfloat v3)
	: numParticles(v1), numBins(v2), binSize(v3) {};

INSTANTIATE_TEST_CASE_P(ManyBinTest, ComputeBin, ::testing::Values(
	myTestCase(COUNT2, 16, 30.0f), myTestCase(COUNT2, 32, 30.0f), myTestCase(COUNT2, 64, 30.0f), myTestCase(COUNT2, 96, 30.0f),
	myTestCase(COUNT3, 16, 30.0f), myTestCase(COUNT3, 32, 30.0f), myTestCase(COUNT3, 64, 30.0f), myTestCase(COUNT3, 96, 30.0f),
	myTestCase(COUNT4, 16, 30.0f), myTestCase(COUNT4, 32, 30.0f), myTestCase(COUNT4, 64, 30.0f), myTestCase(COUNT4, 96, 30.0f),
	myTestCase(COUNT5, 16, 30.0f), myTestCase(COUNT5, 32, 30.0f), myTestCase(COUNT5, 64, 30.0f), myTestCase(COUNT5, 96, 30.0f),
	myTestCase(COUNT6, 16, 30.0f), myTestCase(COUNT6, 32, 30.0f), myTestCase(COUNT6, 64, 30.0f), myTestCase(COUNT6, 96, 30.0f)
));

INSTANTIATE_TEST_CASE_P(ManyPrefixTest, ComputePrefix, ::testing::Values(
	myTestCase(COUNT2, 16, 30.0f), myTestCase(COUNT2, 32, 30.0f), myTestCase(COUNT2, 64, 30.0f), myTestCase(COUNT2, 96, 30.0f),
	myTestCase(COUNT3, 16, 30.0f), myTestCase(COUNT3, 32, 30.0f), myTestCase(COUNT3, 64, 30.0f), myTestCase(COUNT3, 96, 30.0f),
	myTestCase(COUNT4, 16, 30.0f), myTestCase(COUNT4, 32, 30.0f), myTestCase(COUNT4, 64, 30.0f), myTestCase(COUNT4, 96, 30.0f),
	myTestCase(COUNT5, 16, 30.0f), myTestCase(COUNT5, 32, 30.0f), myTestCase(COUNT5, 64, 30.0f), myTestCase(COUNT5, 96, 30.0f),
	myTestCase(COUNT6, 16, 30.0f), myTestCase(COUNT6, 32, 30.0f), myTestCase(COUNT6, 64, 30.0f), myTestCase(COUNT6, 96, 30.0f)
));


// ========== Bin tests ==========

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
	GLuint totalSum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultBin[i]) << "Bin " << i << " has wrong count." ;
		totalSum += resultBin[i];
	}
	EXPECT_EQ(parts->GetParticles(), totalSum);

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputeBin, BinAndPrefixTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputeBins();
	parts->ComputePrefixGather();
	parts->ComputePrefixReduce();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();

	CPUTimer->startTimer();
	CPUSolutionPrefix();
	CPUTimer->endTimer();

	// Check that particle is assigned correct bin
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetParticleBuffers()[0]);
	ParticleStruct* resultPart = (ParticleStruct*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultPart");
	for (size_t i = 0; i < parts->GetParticles(); i++) {
		ASSERT_EQ(parts->GetParticleData()[i].bin, resultPart[i].bin) << "Particle " << i << " has wrong bin.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Check that bins have the correct count
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[0]);
	GLint* resultBin = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultBin");
	GLuint totalSum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultBin[i]) << "Bin " << i << " has wrong count.";
		totalSum += resultBin[i];
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	EXPECT_EQ(parts->GetParticles(), totalSum);

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultPrefix");
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUPrefix[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// ========== Prefix tests ==========

TEST_P(ComputePrefix, PrefixGatherFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolutionGather();
	CPUTimer->endTimer();

	// Test the 
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[2]);
	GLint* resultTemp = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultTemp");
	for (size_t i = 0; i < 1024; i++) {
		ASSERT_EQ(TempBin[i], resultTemp[i]) << "Temp Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultPrefix[i]) << "Sum Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputePrefix, PrefixReduceFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	parts->ComputePrefixReduce();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolutionGather();
	CPUSolutionReduce();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[2]);
	GLint* resultTemp = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultTemp");
	for (size_t i = 0; i < 1024; i++) {
		ASSERT_EQ(TempBin[i], resultTemp[i]) << "Temp Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

TEST_P(ComputePrefix, PrefixSpreadFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	parts->ComputePrefixReduce();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();

	// Run the check solution on CPU
	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultPrefix");
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// Currently the same as the one above
TEST_P(ComputePrefix, DISABLED_PrefixSumTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	parts->ComputePrefixReduce();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();

	CPUTimer->startTimer();
	CPUSolution();
	CPUTimer->endTimer();

	// Compare to CPU based test for bins
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[1]);
	GLint* resultPrefix = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	printError("Map Failed resultPrefix");
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		ASSERT_EQ(CPUBin[i], resultPrefix[i]) << "Prefix " << i << " is not correct sum.";
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

