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
	startMode = 1;
	currVAO = 0;
	currTFB = 1;
	doUpdate = false;
	renderModels = false;

	// Create the query ID
	glGenQueries(1, &qId);
	// Generate Buffers
	glGenBuffers(2, updateBuffer);
	glGenBuffers(1, &cullingBuffer);
	// Generate VAO's
	glGenVertexArrays(2, updateVAO);
	glGenVertexArrays(2, cullingVAO);
}

bool Particles::Init(Camera* setCam) {
	SetParticleData();
	InitGLStates();

	cam = setCam;

	model = new Sphere(1.0f);
	model->Init(setCam, cullingBuffer);

	billboard = new Billboard(1.0f);
	billboard->Init(setCam, cullingBuffer);

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
	InitGLStates();
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

	particleData.resize(particles * 9);

	unsigned int ind = 0;
	// Create the instance data
	for (unsigned int i = 0; i < setParticles; ++i) {
		for (unsigned int j = 0; j < setParticles; ++j) {
			for (unsigned int k = 0; k < setParticles; ++k) {
				// Generate positions
				particleData[ind++] = (((float)i + offset)  * 2.0f * radius); // X
				particleData[ind++] = (((float)j + offsetY) * 2.0f * radius); // Y
				particleData[ind++] = (((float)k + offset)  * 2.0f * radius); // Z

				// Generate velocities
				particleData[ind++] = (0.0f); // X
				particleData[ind++] = (0.0f); // Y
				particleData[ind++] = (0.0f); // Z

				// Set acceleration
				particleData[ind++] = (0.0f); // X
				particleData[ind++] = (0.0f); // Y
				particleData[ind++] = (0.0f); // Z
			}
		}
	}
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
	glUniform1f(glGetUniformLocation(updateShader, "t"), t);

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
		model->Draw(drawParticles);
	} else {
		billboard->Draw(drawParticles);
	}
}
