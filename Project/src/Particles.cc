#include "Particles.h"

#include "gtc/type_ptr.hpp"
#include "gtx/transform.hpp"

#include "GL_utilities.h"

#include <iostream>

Particles::Particles(GLuint numParticles, GLfloat initRadius) {
	setParticles = numParticles;
	particles = setParticles*setParticles*setParticles;
	drawParticles = 0;
	radius = initRadius;
	oldT = 0;
	numBins = 16;
	bins = numBins*numBins*numBins;
	binSize = 4.0f;
	displaybin = 0;
	startMode = 1;
	currVAO = 0;
	currTFB = 1;
	doUpdate = false;
	renderModels = false;

	prefixArrayIn = (GLuint*)malloc(sizeof(GLuint) * bins);
	prefixArrayOut = (GLuint*)malloc(sizeof(GLuint) * bins);

	// Create the query ID
	//glGenQueries(1, &qId);
	// Generate Buffers
	//glGenBuffers(2, updateBuffer);
	//glGenBuffers(1, &cullingBuffer);
	// Generate VAO's
	//glGenVertexArrays(2, updateVAO);
	//glGenVertexArrays(2, cullingVAO);


	//create buffers
	glGenBuffers(3, particleBuffers);
	glGenBuffers(2, binBuffers);
	glGenBuffers(1, &counterBuffer);
}

bool Particles::Init(Camera* setCam) {
	//InitGLStates();

	SetParticleData();

	CompileComputeShader(&computeBin, "src/shaders/bin.comp");
	CompileComputeShader(&computeUpdate, "src/shaders/update.comp");
	CompileComputeShader(&computeCull, "src/shaders/cull.comp");

	InitCompute();
	cam = setCam;

	model = new Sphere(1.0f);
	model->Init(setCam, particleBuffers[2]);

	billboard = new Billboard(1.0f);
	billboard->Init(setCam, particleBuffers[2]);

	printError("Particles Constructor");
	return true;
}

void Particles::SetParticles(GLuint newParticles) {
	SetParticles(newParticles, startMode);
}

void Particles::SetParticles(GLuint newParticles, GLuint newType) {
	setParticles = newParticles;
	particles = setParticles*setParticles*setParticles;
	startMode = newType;

	SetParticleData();
	InitCompute();
}

void Particles::SetParticleData() {
	float offset;
	float offsetY;

	if (startMode == 1) {
		// Place particles centered over origin
		offset = -((float)setParticles - 1) / 2;
		offsetY = offset;
	} else if (startMode == 2) {
		// Place particles centered over origin but displaced along y.
		offset = -((float)setParticles - 1) / 2;
		offsetY = 100.0f;
	} else {
		// Place all particles in first octant
		offset = 0;
		offsetY = offset;
	}

	particleData.resize(particles);

	unsigned int ind = 0;
	// Create the instance data
	for (unsigned int i = 0; i < setParticles; ++i) {
		for (unsigned int j = 0; j < setParticles; ++j) {
			for (unsigned int k = 0; k < setParticles; ++k) {
				// Generate positions
				particleData[ind].position.x = (((float)i + offset)  * 2.0f * radius); // X
				particleData[ind].position.y = (((float)j + offsetY) * 2.0f * radius); // Y
				particleData[ind].position.z = (((float)k + offset)  * 2.0f * radius); // Z

				// Initiate cell
				particleData[ind].bin = 0;

				// Generate velocities
				particleData[ind].velocity.x = (0.0f); // X
				particleData[ind].velocity.y = (0.0f); // Y
				particleData[ind].velocity.z = (0.0f); // Z

				// Just fill it with something
				particleData[ind].padding = 7;

				ind++;
			}
		}
	}
}

void Particles::CompileComputeShader(GLuint* program, const char* path) {
	*program = glCreateProgram();
	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

	char* cs = readFile((char *)path);
	if (cs == NULL) {
		printf("Error reading shader!\n");
	}

	glShaderSource(computeShader, 1, &cs, NULL);
	glCompileShader(computeShader);
	printShaderInfoLog(computeShader, path);
	//get errors 

	glAttachShader(*program, computeShader);
	glLinkProgram(*program);
	//get errors from the program linking.
	printProgramInfoLog(*program, path, NULL, NULL, NULL, NULL);

	printError("Compile compute shader error!");
}

void Particles::InitCompute() {

	int setCounter = 0;

	// Initialize buffers
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, particleData.data(), GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	particleData.clear();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ParticleStruct) * particles, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[2]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 3 * particles, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init Compute Error 1");

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * bins, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * bins, NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("init Compute Error 2");

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &setCounter, GL_STREAM_READ);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	printError("init Compute Error 3");

	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 3, particleBuffers);
	glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 3, 2, binBuffers);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counterBuffer);

	// Set unchanging uniforms
	glUseProgram(computeBin);
	glUniform1ui(glGetUniformLocation(computeBin, "numBins"), numBins);
	glUniform1f(glGetUniformLocation(computeBin, "binSize"), binSize);

	glUseProgram(computeCull);
	glUniform1f(glGetUniformLocation(computeCull, "radius"), radius);

	printError("init Compute Error 4");
}

void Particles::DoCompute(GLfloat t) {
	GLfloat deltaT = t - oldT;
	GLuint reset = 0;

	if (renderModels) {
		model->Update(t);
	} else {
		billboard->Update(t);
	}
	
	// ========== Calculate Bin =========
	glUseProgram(computeBin);
	glDispatchCompute(particles / 64, 1, 1);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[0]);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint) * bins, prefixArrayIn);
	glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &reset);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("Do Compute: Bin");

	// ========== Calculate Prefix sum =========
	prefixArrayOut[0] = 0;
	for (size_t i = 1; i < bins; i++) {
		prefixArrayOut[i] = prefixArrayIn[i-1] + prefixArrayOut[i-1];
		//std::cout << prefixArrayOut[i] << std::endl;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint) * bins, prefixArrayOut, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	printError("Do Compute: Prefix");

	// ========== Sort Particles =========


	printError("Do Compute: Sort");

	// ========== Update Particles =========
	if (doUpdate) {
		glUseProgram(computeUpdate);
		glUniform1f(glGetUniformLocation(computeUpdate, "deltaT"), deltaT);
		glDispatchCompute(particles / 64, 1, 1);
	}

	printError("Do Compute: Update");

	// ========== Cull Particles =========
	glUseProgram(computeCull);
	glUniform3fv(glGetUniformLocation(computeCull, "boxNormals"), 5, cam->GetCullingNormals());
	glUniform3fv(glGetUniformLocation(computeCull, "boxPoints"), 5, cam->GetCullingPoints());
	glUniform1ui(glGetUniformLocation(computeCull, "displaybin"), displaybin);
	glDispatchCompute(particles / 64, 1, 1);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &computeDrawParticles);
	glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &reset);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	printError("Do Compute: Culling");
}

void Particles::InitGLStates() {
	// Create program for the update and culling TFB
	updateShader = loadShaders("src/shaders/update.vert", NULL);
	cullShader = loadShadersG("src/shaders/culling.vert", NULL, "src/shaders/culling.geom");

	// Names for transform feedback return values
	const GLchar* updateVaryings[] = {"updatePosValue", "updateVelValue", "updateAccValue"};
	const GLchar* cullingVaryings[] = {"culledPosValue"};

	// Create the transform feedback output variable
	glTransformFeedbackVaryings(updateShader, 3, updateVaryings, GL_INTERLEAVED_ATTRIBS);
	glTransformFeedbackVaryings(cullShader, 1, cullingVaryings, GL_INTERLEAVED_ATTRIBS);

	// Link program, we do this after creating buffers since the linking depends on
	// knowledge about the output
	glLinkProgram(updateShader);
	printProgramInfoLog(updateShader, "Transform Feedback", NULL, NULL, NULL, NULL);
	printError("Link update shader");

	glLinkProgram(cullShader);
	printProgramInfoLog(cullShader, "Transform Feedback", NULL, NULL, NULL, NULL);
	printError("Link cull shader");

	// Create a culling buffer
	glBindBuffer(GL_ARRAY_BUFFER, cullingBuffer);
	glBufferData(GL_ARRAY_BUFFER, particles * 3 * sizeof(GLfloat), NULL, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Print errors
	printError("Create Culling Buffer");

	// Set VAOs
	// Get attributes
	GLint inPosAttr = glGetAttribLocation(updateShader, "posValue");
	GLint inVelAttr = glGetAttribLocation(updateShader, "velValue");
	GLint inAccAttr = glGetAttribLocation(updateShader, "accValue");
	GLint upPosAttr = glGetAttribLocation(cullShader, "posValue");

	for (int i = 0; i < 2; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, updateBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, particles * 9 * sizeof(GLfloat), particleData.data(), GL_STATIC_COPY);

		// Set update VAO states
		glBindVertexArray(updateVAO[i]);

		glEnableVertexAttribArray(inPosAttr);
		glVertexAttribPointer(inPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);
		glEnableVertexAttribArray(inVelAttr);
		glVertexAttribPointer(inVelAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(inAccAttr);
		glVertexAttribPointer(inAccAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(6 * sizeof(GLfloat)));

		glBindVertexArray(0);

		// Set culling VAO state
		glBindVertexArray(cullingVAO[i]);

		glEnableVertexAttribArray(upPosAttr);
		glVertexAttribPointer(upPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	printError("Create Transform Feedback VAOs");
}

void Particles::Update(GLfloat t) {
	GLfloat deltaT = t - oldT;

	model->Update(t);
	billboard->Update(t);

	glUseProgram(updateShader);
	glUniform1f(glGetUniformLocation(updateShader, "deltaT"), deltaT);
	//glUniform1f(glGetUniformLocation(updateShader, "t"), t);

	printError("Particles Update");
	if (doUpdate && particles > 0) {
		// Disable rasterizer since we dont want to draw
		glEnable(GL_RASTERIZER_DISCARD);

		// Set the attributes for the first pass
		glBindVertexArray(updateVAO[currVAO]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, updateBuffer[currTFB]);

			// Enter transform feedback mode
			glBeginTransformFeedback(GL_POINTS);

				// Perform the transform feedback
				glDrawArrays(GL_POINTS, 0, particles);

			// End transform feedback mode
			glEndTransformFeedback();

		// Enable the rasterizer again
		glDisable(GL_RASTERIZER_DISCARD);

		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
		glBindVertexArray(0);

		printError("Do Update");
	}

	oldT = t;
}

void Particles::Cull() {
	glUseProgram(cullShader);

	glUniform3fv(glGetUniformLocation(cullShader, "boxNormals"), 5, cam->GetCullingNormals());
	glUniform3fv(glGetUniformLocation(cullShader, "boxPoints"), 5, cam->GetCullingPoints());
	glUniform1f(glGetUniformLocation(cullShader, "radius"), radius);

	printError("Particles Before Cull");
	
	if (particles > 0) {
		// Disable rasterizer since we dont want to draw
		glEnable(GL_RASTERIZER_DISCARD);

		// Set the attributes for the second pass
		glBindVertexArray(cullingVAO[currTFB]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, cullingBuffer);

		// Enter transform feedback mode
		glBeginTransformFeedback(GL_POINTS);

			// Start recording query
			glBeginQuery(GL_PRIMITIVES_GENERATED, qId);

				// Perform the transform feedback
				glDrawArrays(GL_POINTS, 0, particles);

			// End Query
			glEndQuery(GL_PRIMITIVES_GENERATED);

		// End transform feedback mode
		glEndTransformFeedback();

		// Enable the rasterizer again
		glDisable(GL_RASTERIZER_DISCARD);

		// Get number of culled objects
		glGetQueryObjectuiv(qId, GL_QUERY_RESULT, &drawParticles);

		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
		glBindVertexArray(0);

		printError("Do Culling");
	}

	// Swap TFB buffer to read and write
	if (doUpdate) {
		currVAO = currTFB;
		currTFB = 1 - currVAO;
	}
}

void Particles::Draw() {
	if (renderModels) {
		model->Draw(computeDrawParticles);
	} else {
		billboard->Draw(computeDrawParticles);
	}
}
