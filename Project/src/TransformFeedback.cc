///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

// ==== Includes ==== //

#include "TransformFeedback.h"

#include "GL_utilities.h"

#include <iostream>

GLuint updateVAO[2];
GLuint cullingVAO[2];
GLuint updateBuffer[2];
GLint currVAO = 0;
GLint currTFB = 1;
GLuint qId;

// Set buffers to hold particle data
void SetBuffers(const GLvoid* const data, const GLuint particles)
{
	// Generate Buffers
	glGenBuffers(2, updateBuffer);
	
	// Bind initial data to buffers
	glBindBuffer(GL_ARRAY_BUFFER, updateBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, particles * 9 * sizeof(GLfloat), data, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, updateBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, particles * 9 * sizeof(GLfloat), data, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Print errors
	printError("Create Transform Feedback Buffers");
}

// Set the VAO for the transform feedback calls
void SetTFBVAO(const GLuint* const program)
{
	// Get attributes
	GLint inPosAttr = glGetAttribLocation(program[0], "posValue");
	GLint inVelAttr = glGetAttribLocation(program[0], "velValue");
	GLint inAccAttr = glGetAttribLocation(program[0], "accValue");
	GLint upPosAttr = glGetAttribLocation(program[1], "posValue");

	glGenVertexArrays(2, updateVAO);
	glGenVertexArrays(2, cullingVAO);
	
	for (int i = 0; i < 2; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, updateBuffer[i]);		
		
		// Set update VAO states
		glBindVertexArray(updateVAO[i]);
		
		glEnableVertexAttribArray(inPosAttr);
		glVertexAttribPointer(inPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*9, 0);
		glEnableVertexAttribArray(inVelAttr);
		glVertexAttribPointer(inVelAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*9, 
																	(void*) (3*sizeof(GLfloat)));
		glEnableVertexAttribArray(inAccAttr);
		glVertexAttribPointer(inAccAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*9, 
																	(void*) (6*sizeof(GLfloat)));

		glBindVertexArray(0);
		
		// Set culling VAO state
		glBindVertexArray(cullingVAO[i]);
		
		glEnableVertexAttribArray(upPosAttr);
		glVertexAttribPointer(upPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*9, 0);
		
		glBindVertexArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	// Print errors
	printError("Create Transform Feedback VAOs");
}



// Set transform feedback, specify the output names to the program
void SetTFB(const GLuint program, const GLchar** varyings, const int numVaryings)
{
	// Create the transform feedback output variable
	glTransformFeedbackVaryings(program, numVaryings, varyings, GL_INTERLEAVED_ATTRIBS);
	
	// Link program, we do this after creating buffers since the linking depends on 
	// knowledge about the output
	glLinkProgram(program);
	
	// Print info and errors
	printProgramInfoLog(program, "Transform Feedback", NULL, NULL, NULL,NULL);
	printError("Init Transform Feedback");
}

// Load the shaders and compile the programs for tranform feedback
void InitTFB(GLuint* const program, const GLchar** shaderFilenames, const GLvoid* const data, 
																			const GLuint particles)
{
	// Create the query ID
	glGenQueries(1, &qId);
	
	// Names for transform feedback return values
	const GLchar* updateVaryings[] = {"updatePosValue", "updateVelValue", "updateAccValue"};
	const GLchar* cullingVaryings[] = {"culledPosValue"};
	
	// Upload data to GPU buffer
	SetBuffers(data, particles);
	
	// Create program for the update TFB
	program[0] = loadShaders(shaderFilenames[0], NULL);
	SetTFB(program[0], updateVaryings, 3);
	
	// Create program for the culling TFB
	program[1] = loadShadersG(shaderFilenames[1], NULL, shaderFilenames[2]);
	SetTFB(program[1], cullingVaryings, 1);
	
	// Create and set VAOs
	SetTFBVAO(program);
}

void ResetTFB(const GLuint* const program, const GLvoid* const data, const GLuint particles)
{
	// Free GPU Memory from update buffers and remove the buffers
	glDeleteBuffers(2, updateBuffer);
	
	// Remove the VAOs
	glDeleteVertexArrays(2, updateVAO);
	glDeleteVertexArrays(2, cullingVAO);
	
	// Set the new data
	SetBuffers(data, particles);
	
	// Create and set VAOs
	SetTFBVAO(program);
}

// Perform the update transform feedback on the particles
void DoUpdate(const GLuint particles)
{
	if(particles > 0)
	{
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
	
	glBindVertexArray(0);
		
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	
	printError("Do Update");
	}
}

// Perform the culling transform feedback on the particles
GLuint DoCulling(const GLuint particles, const GLuint cullingBuffer)
{
	GLuint remParticles = 0;
	if(particles > 0)
	{
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
	glGetQueryObjectuiv(qId, GL_QUERY_RESULT, &remParticles);
	
	glBindVertexArray(0);
		
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	
	printError("Do Culling");
	}
	// Return the number of particles remaining
	return remParticles;
}

void SwapBuffers()
{
	// Swap curr_VAO and curr_TFB
	currVAO = currTFB;
	currTFB = 1 - currVAO;
}

