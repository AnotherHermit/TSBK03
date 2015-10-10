#include "Drawable.h"

#include <iostream>


#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include "LoadTGA.h"
#include "GL_utilities.h"

Drawable::Drawable()
{
	program = -1;
	textureId = -1;

	inputNames[0] = "inPosition";
	inputNames[1] = "inNormal";
	inputNames[2] =	"inTexCoord";
	
	modelWorld = glm::mat4();
	modelInit = glm::mat4();
	
	baseColor = glm::vec3(0, 0, 1.0f);
}

void Drawable::SetProgram(const char** shaderLoc)
{
	program = loadShaders(shaderLoc[0], shaderLoc[1]);
}

void Drawable::SetModel(const char* modelLoc, glm::mat4 initTransform)
{
	glUseProgram(program);
	modelInit = initTransform;
	modelWorld = modelInit;
	
	model = LoadModel(modelLoc);
	NormalizeModel(model);
	
	BuildModelVAO2(model, program, inputNames[0], inputNames[1], inputNames[2]);
	
	printError("Set Model");
}

void Drawable::SetTexture(const char* textureLoc)
{
	baseColor = glm::vec3(1.0f, 0, 1.0f); // To detect any errors
	
	glActiveTexture(GL_TEXTURE0);
	LoadTGATextureSimple(textureLoc, &textureId);
}

void Drawable::SetColor(glm::vec3 colorVec)
{
	baseColor = colorVec;
}

void Drawable::Update(GLfloat deltaT)
{
	return;
}

void Drawable::Draw() 
{	
	glUseProgram(program);
	GLint textureLoc = glGetUniformLocation(program, "texUnit");
	GLint colorLoc = glGetUniformLocation(program, "inColor");
	
	if(textureLoc != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(textureLoc, 0); 
	}
	
	if(colorLoc != -1)
		glUniform3f(colorLoc, baseColor.x, baseColor.y, baseColor.z);
	
	if(model != NULL)
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "modelWorld"), 1, GL_FALSE, glm::value_ptr(modelWorld));
		DrawModel(model, program, inputNames[0], inputNames[1], inputNames[2]);
	}
	else
		std::cerr << "No model assosiated with Drawable!" << std::endl;
}
