﻿///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#ifndef CAMERA_H
#define CAMERA_H

#ifdef __APPLE__
#	include <OpenGL/gl3.h>
#	include <SDL2/SDL.h>
#else
#	ifdef  __linux__
#		define GL_GLEXT_PROTOTYPES
#		include <GL/gl.h>
#		include <GL/glu.h>
#		include <GL/glx.h>
#		include <GL/glext.h>
#		include <SDL2/SDL.h>
#	else
#		include "glew.h"
#		include "Windows/sdl2/SDL.h"
#	endif
#endif

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include <vector>

struct CameraParam {
	glm::mat4 WTVmatrix;
	glm::mat4 VTPmatrix;
	glm::vec4 normals[5];
	glm::vec4 points[5];
	GLfloat viewDistance;
	GLfloat padding73[3];
};


class Camera {
private:
	glm::vec3 p, lookp, yvec;
	glm::vec3 heading, side, up;
	GLfloat mspeed, rspeed, phi, theta;

	glm::vec4 nontransPoints[5];
	glm::vec3 nontransNormals[5];

	bool isPaused;

	GLuint cameraBuffer;
	GLuint *winWidth, *winHeight;

	CameraParam param;

	void UpdateCullingBox();
	void Update();
	void UploadParams();

public:
	Camera(glm::vec3 startpos, GLuint *screenWidth, GLuint *screenHeight, GLfloat viewDistance);
	void SetFrustum(GLuint screenWidth, GLuint screenHeight, GLfloat viewDistance);
	void ResetCamera(glm::vec3 pos);

	void MoveForward(GLfloat deltaT);
	void MoveRight(GLfloat deltaT);
	void MoveUp(GLfloat deltaT);
	void RotateCamera(GLint dx, GLint dy);

	void UpdateCamera();

	void TogglePause() { isPaused = !isPaused; }

	const glm::vec3 GetPos() { return p; }
	const glm::vec3 GetHeading() { return heading; }
	const glm::vec3 GetSide() { return side; }
	const glm::vec3 GetUp() { return up; }

	GLfloat* ViewDistancePtr() { return &param.viewDistance; }
	GLfloat* SpeedPtr() { return &mspeed; }
	GLfloat* HeadingPtr() { return glm::value_ptr(heading); }

};

#endif // CAMERA_H
