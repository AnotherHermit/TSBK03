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

TEST_P(ComputeBin, BinPrefixTest) {
	// Run the GPU solution
	glFinish();
	GPUTimer->startTimer();
	parts->ComputeBins();
	glFinish();
	parts->ComputePrefixGather();
	glFinish();
	parts->ComputePrefixReduce();
	glFinish();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();
	glFinish();

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
		//ASSERT_EQ(CPUBin[i], resultBin[i]) << "Bin " << i << " has wrong count.";
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
	
	//ParticleStruct* resultPart = (ParticleStruct*)glMapNamedBuffer(parts->GetParticleBuffers()[0], GL_READ_ONLY);
	for (size_t i = 0; i < parts->GetParticles(); i++) {
		ASSERT_EQ(parts->GetParticleData()[i].bin, resultPart[i].bin) << "Particle " << i << " has wrong bin.";
	}
	//glUnmapNamedBuffer(parts->GetParticleBuffers()[0]);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Check that bins have the correct count
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, parts->GetBinBuffers()[0]);
	GLint* resultBin = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);

	//GLint* resultBin = (GLint*)glMapNamedBuffer(parts->GetBinBuffers()[0], GL_READ_ONLY);
	GLuint totalSum = 0;
	for (size_t i = 0; i < parts->GetTotalBins(); i++) {
		//ASSERT_EQ(CPUBin[i], resultBin[i]) << "Bin " << i << " has wrong count." ;
		totalSum += resultBin[i];
	}
	//glUnmapNamedBuffer(parts->GetBinBuffers()[0]);

	EXPECT_EQ(parts->GetParticles(), totalSum);

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	
}



TEST_P(ComputePrefix, PrefixGatherFunctionTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	GPUTimer->endTimer();
	glFinish();

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
	glFinish();

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
	glFinish();

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

TEST_P(ComputePrefix, PrefixSumTest) {
	// Run the GPU solution
	GPUTimer->startTimer();
	parts->ComputePrefixGather();
	parts->ComputePrefixReduce();
	parts->ComputePrefixSpread();
	GPUTimer->endTimer();
	glFinish();

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

