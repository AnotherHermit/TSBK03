#include "Camera.h"

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include <SDL2/SDL.h>
#else
	#ifdef  __linux__
		#define GL_GLEXT_PROTOTYPES
		#include <GL/gl.h>
		#include <GL/glu.h>
		#include <GL/glx.h>
		#include <GL/glext.h>
		#include <SDL2/SDL.h>
	#else
		#include "glew.h"
		#include "Windows/sdl2/SDL.h"
	#endif
#endif

#include <iostream>

#include "GL_utilities.h"

#include "gtc/matrix_transform.hpp"
#include "gtx/transform.hpp"
#include "gtx/rotate_vector.hpp"

Camera::Camera(glm::vec3 startpos)
{
	isMoving = false;
	p = startpos;

	mspeed = 0.1f;
	rspeed = 0.001f;
	phi = 0;
	theta = 0;
	
	// Set starting worldView matrix
	Update();
}

void Camera::ResetCamera(glm::vec3 pos)
{
	p = pos;

	phi = 0;
	theta = 0;
	
	// Set starting worldView matrix
	Update();
	UpdateCullingBox();
}

void Camera::SetFrustum(GLfloat in_left, GLfloat in_right, GLfloat in_bottom, GLfloat in_top, GLfloat in_near, GLfloat in_far)
{
	proj = glm::frustum(in_left, in_right, in_bottom, in_top, in_near, in_far);
	
	// Add all normals and vectors before transformation
	// Add order is left, right, bottom, top, far. The near plane check is skipped.
	// All normals are pointing inwards towards the center of the frustum.
	for(int i = 0; i < 4; i++)
		nontransPoints.push_back(glm::vec4(0.0, 0.0, 0.0, 1.0));
		
	nontransPoints.push_back(glm::vec4(0.0, 0.0, -in_far, 1.0));
	
	// The near corners of the frustum
	glm::vec3 a = glm::vec3(in_left, in_top, -in_near);
	glm::vec3 b = glm::vec3(in_right, in_top, -in_near);
	glm::vec3 c = glm::vec3(in_right, in_bottom, -in_near);
	glm::vec3 d = glm::vec3(in_left, in_bottom, -in_near);

	nontransNormals.push_back(glm::cross(a, b));
	nontransNormals.push_back(glm::cross(b, c));
	nontransNormals.push_back(glm::cross(c, d));
	nontransNormals.push_back(glm::cross(d, a));
	nontransNormals.push_back(glm::vec3(0.0, 0.0, 1.0));
	
	UpdateCullingBox();
}

void Camera::UpdateCullingBox()
{
	for(int i = 0; i < 5; i++)
	{
		glm::vec3 transNormals = glm::normalize(glm::mat3(glm::inverse(worldView)) * nontransNormals[i]);
		glm::vec4 transPoints = glm::inverse(worldView) * nontransPoints[i];

		normals[3*i] = transNormals.x;
		normals[3*i+1] = transNormals.y;
		normals[3*i+2] = transNormals.z;

		points[3*i] = transPoints.x;
		points[3*i+1] = transPoints.y;
		points[3*i+2] = transPoints.z;
	}
}

void Camera::Update()
{
	glm::vec3 yvec = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 zvec = glm::vec3(0.0f, 0.0f, -1.0f);
	
	// Update directions
	heading = glm::rotate(zvec, phi, yvec);
	side = glm::cross(heading, yvec);
	up = glm::rotate(yvec, theta, side);
	heading = glm::cross(up, side);

	// Update camera matrix
	lookp = p + heading;
	worldView = lookAt(p,lookp,yvec);
}

void Camera::MoveForward(GLfloat deltaT)
{
	if (isMoving)
	{
		p = p + heading * mspeed * deltaT;
		Update();
		UpdateCullingBox();
	}
}
void Camera::MoveBackward(GLfloat deltaT)
{
	if (isMoving)
	{
		p = p - heading * mspeed * deltaT;
		Update();
		UpdateCullingBox();
	}
}
void Camera::MoveLeft(GLfloat deltaT)
{
	if (isMoving)
	{
		p = p - side * mspeed * deltaT;
		Update();
		UpdateCullingBox();
	}
}
void Camera::MoveRight(GLfloat deltaT)
{
	if (isMoving)
	{
		p = p + side * mspeed * deltaT;
		Update();
		UpdateCullingBox();
	}
}
void Camera::MoveUp(GLfloat deltaT)
{
	if (isMoving)
	{
		p = p + up * mspeed * deltaT;
		Update();
		UpdateCullingBox();
	}
}
void Camera::MoveDown(GLfloat deltaT)
{
	if (isMoving)
	{
		p = p - up * mspeed * deltaT;
		Update();
		UpdateCullingBox();
	}
}

void Camera::RotateCamera(GLint dx, GLint dy)
{
	if (isMoving)
	{
		phi -= (float)dx * rspeed;
		if (phi > 2.0f * M_PI)
			phi -= 2.0f * M_PI;
		else if (phi < 0.0f)
			phi += 2.0f * M_PI;

		theta -= (float)dy * rspeed;
		if (theta > M_PI / 2.1f)
			theta = M_PI / 2.1f;
		else if (theta < -M_PI / 2.1f)
			theta = -M_PI / 2.1f;

		Update();
		UpdateCullingBox();
	}
}
