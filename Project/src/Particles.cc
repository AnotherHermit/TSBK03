#include "Particles.h"

#include "gtc/type_ptr.hpp"
#include "gtx/transform.hpp"

#include "GL_utilities.h"
#include "TransformFeedback.h"

#include <iostream>

Particles::Particles(GLuint numParticles, GLfloat initRadius)
{
	setParticles = numParticles;
	particles = setParticles*setParticles*setParticles;
	drawParticles = 0;
	radius = initRadius;
	oldT = 0;
	type = 0;
	doUpdate = false;
	
	// Shaders for update pass
	shaderFiles[0] = "src/shaders/update.vert"; 
	// Shaders for culling pass
	shaderFiles[1] = "src/shaders/culling.vert"; 
	shaderFiles[2] = "src/shaders/culling.geom"; 
	// Shaders for drawing
	shaderFiles[3] = "src/shaders/particle.vert"; 
	shaderFiles[4] = "src/shaders/particle.frag"; 
	// Shaders for drawing billboards
	shaderFiles[5] = "src/shaders/billboard.vert";
	shaderFiles[6] = "src/shaders/billboard.geom";
	shaderFiles[7] = "src/shaders/billboard.frag";
	
	SetParticleData();
	InitGlStates();
	
	printError("Particles Constructor");
}

void Particles::Update(GLfloat t)
{	
	GLfloat deltaT = t - oldT;

	glUseProgram(programs[0]);
	glUniform1f(glGetUniformLocation(programs[0], "deltaT"), deltaT);
	glUniform1f(glGetUniformLocation(programs[0], "t"), t);

	printError("Particles Update");
	if(doUpdate)
		DoUpdate(particles);	

	oldT = t;
}

void Particles::Cull(const GLfloat* normals, const GLfloat* points)
{
	glUseProgram(programs[1]);

	glUniform3fv(glGetUniformLocation(programs[1], "boxNormals"), 5, normals);	
	glUniform3fv(glGetUniformLocation(programs[1], "boxPoints"), 5, points);

	glUniform1f(glGetUniformLocation(programs[1], "radius"), radius);
	
	printError("Particles Cull");
	
	drawParticles = DoCulling(particles, cullingBuffer);
	
	// Swap TFB buffer to read and write
	if(doUpdate)
		SwapBuffers();
}

void Particles::DrawParticles(const glm::mat4 proj, const glm::mat4 worldView)
{
	glUseProgram(programs[2]);

	glUniformMatrix4fv(glGetUniformLocation(programs[2], "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(programs[2], "worldView"), 1, GL_FALSE, glm::value_ptr(worldView));
	
	glUniformMatrix4fv(glGetUniformLocation(programs[2], "modelWorld"), 1, GL_FALSE, glm::value_ptr(modelWorld));
	glUniform1f(glGetUniformLocation(programs[2], "currT"), oldT);
	
	// Set the attribute for the draw pass
	glBindVertexArray(model->vao);
	if (drawParticles > 0)
		glDrawElementsInstanced(GL_TRIANGLES, model->numIndices, GL_UNSIGNED_INT, 0L, drawParticles);

	glBindVertexArray(0);	
	
	printError("Particles Draw");
}

void Particles::DrawBillboards(const glm::mat4 proj, const glm::mat4 worldView, const glm::vec3 camerapos)
{
	glUseProgram(programs[3]);

	glUniform3f(glGetUniformLocation(programs[3], "cameraPos"), camerapos.x, camerapos.y, camerapos.z);
	glUniformMatrix4fv(glGetUniformLocation(programs[3], "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(programs[3], "worldView"), 1, GL_FALSE, glm::value_ptr(worldView));

	glUniform1f(glGetUniformLocation(programs[3], "radius"), radius);
	glUniform1f(glGetUniformLocation(programs[3], "currT"), oldT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0); 
	
	glBindVertexArray(billboardVAO);
	
	if (drawParticles > 0)
	{
		glDrawArrays(GL_POINTS, 0, drawParticles);
	}
		
	glBindVertexArray(0);	

	printError("Particles Draw Billboards");
}

void Particles::SetParticles(GLuint newParticles)
{
	SetParticles(newParticles, type);
}

void Particles::SetParticles(GLuint newParticles, GLuint newType)
{
	setParticles = newParticles;
	particles = setParticles*setParticles*setParticles;
	type = newType;
	
	// Free GPU Memory from update buffers and remove the buffers
	glDeleteBuffers(1, &cullingBuffer);
	
	SetParticleData();
	ResetTFB(programs, particleData.data(), particles);
	SetCullingState();
}

void Particles::SetParticleData()
{
	float offset;
	float offsetY;
	
	if (type == 1)
	{
		offset = -((float)setParticles - 1) / 2;
		offsetY = offset;
	}
	else if(type == 2) 
	{
		offset = -((float)setParticles - 1) / 2;
		offsetY = 100.0f;
	}
	else
	{
		offset = 0;
		offsetY = offset;
	}

	particleData.clear();

	// Create the instance data
	for(unsigned int i = 0; i < setParticles; ++i)
	{
		for(unsigned int j = 0; j < setParticles; ++j)
		{
			for(unsigned int k = 0; k < setParticles; ++k)
			{
				// Generate positions
				particleData.push_back(((float)i + offset)*2.0f*radius); // X
				particleData.push_back(((float)j + offsetY)*2.0f*radius); // Y
				particleData.push_back(((float)k + offset)*2.0f*radius); // Z
				
				// Generate velocities
				particleData.push_back(0.0f); // X
				particleData.push_back(0.0f); // Y
				particleData.push_back(0.0f); // Z
				
				// Set acceleration
				particleData.push_back(0.0f); // X
				particleData.push_back(0.0f); // Y
				particleData.push_back(0.0f); // Z
			}
		}
	}
}

void Particles::InitGlStates()
{
	InitTFB(programs, shaderFiles, particleData.data(), particles);

	programs[2] = loadShaders(shaderFiles[3], shaderFiles[4]);
	program = programs[2];	
	
	programs[3] = loadShadersG(shaderFiles[5], shaderFiles[7], shaderFiles[6]);
	
	glm::mat4 initModel = glm::scale(glm::vec3(radius));
	SetModel("resources/groundsphere.obj", initModel);
	
	SetTexture("resources/particle.tga");
	
	SetCullingState();
	
	printError("Particles Init GL States");
}

void Particles::SetCullingState()
{
	// Generate Buffers
	glGenBuffers(1, &cullingBuffer);

	// Bind initial data to buffers
	glBindBuffer(GL_ARRAY_BUFFER, cullingBuffer);
	glBufferData(GL_ARRAY_BUFFER, particles * 3 * sizeof(GLfloat), NULL, GL_STATIC_COPY);
	
	// Set state for drawing
	glBindVertexArray(model->vao);
	
	GLuint drPosAttr = glGetAttribLocation(programs[2], "posValue");
	glEnableVertexAttribArray(drPosAttr);
	glVertexAttribPointer(drPosAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribDivisor(drPosAttr, 1);
	
	glBindVertexArray(0);
	
	glGenVertexArrays(1, &billboardVAO);
	glBindVertexArray(billboardVAO);
	
	drPosAttr = glGetAttribLocation(programs[3], "posValue");
	glEnableVertexAttribArray(drPosAttr);
	glVertexAttribPointer(drPosAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
