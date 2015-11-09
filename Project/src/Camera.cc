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

Camera::Camera(glm::vec3 startpos) {
	isPaused = true;
	p = startpos;

	mspeed = 0.1f;
	rspeed = 0.001f;
	phi = M_PI;
	theta = M_PI / 2.0f;

	// Set starting WTVmatrix
	Update();
}

void Camera::ResetCamera(glm::vec3 pos) {
	p = pos;

	phi = M_PI;
	theta = M_PI / 2.0f;

	// Set starting worldView matrix
	Update();
	UpdateCullingBox();
}

void Camera::SetFrustum(GLfloat in_left, GLfloat in_right, GLfloat in_bottom, GLfloat in_top, GLfloat in_near, GLfloat in_far) {
	VTPmatrix = glm::frustum(in_left, in_right, in_bottom, in_top, in_near, in_far);

	// Add all normals and vectors before transformation
	// Add order is left, right, bottom, top, far. The near plane check is skipped.
	// All normals are pointing inwards towards the center of the frustum.
	for (int i = 0; i < 4; i++)
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

void Camera::UpdateCullingBox() {
	for (int i = 0; i < 5; i++) {
		glm::mat4 WTVInv = glm::inverse(WTVmatrix);
		glm::vec3 transNormals = glm::normalize(glm::mat3(WTVInv) * nontransNormals[i]);
		glm::vec4 transPoints = WTVInv * nontransPoints[i];

		normals[3 * i] = transNormals.x;
		normals[3 * i + 1] = transNormals.y;
		normals[3 * i + 2] = transNormals.z;

		points[3 * i] = transPoints.x;
		points[3 * i + 1] = transPoints.y;
		points[3 * i + 2] = transPoints.z;
	}
}

void Camera::Update() {
	glm::vec3 yvec = glm::vec3(0.0f, 1.0f, 0.0f);

	// Update directions
	heading = glm::normalize(glm::vec3(-sin(theta)*sin(phi), cos(theta), sin(theta)*cos(phi)));
	side = glm::normalize(glm::cross(heading, yvec));
	up = glm::normalize(glm::cross(side, heading));

	// Update camera matrix
	glm::vec3 lookp = p + heading;
	WTVmatrix = lookAt(p, lookp, yvec);
}

void Camera::UpdateCamera() {
	Update();
	UpdateCullingBox();
}

void Camera::MoveForward(GLfloat deltaT) {
	if (!isPaused) {
		p += heading * mspeed * deltaT;
	}
}
void Camera::MoveRight(GLfloat deltaT) {
	if (!isPaused) {
		p += side * mspeed * deltaT;
	}
}
void Camera::MoveUp(GLfloat deltaT) {
	if (!isPaused) {
		p += up * mspeed * deltaT;
	}
}

void Camera::RotateCamera(GLint dx, GLint dy) {
	if (!isPaused) {
		float eps = 0.001f;

		phi += rspeed * dx;
		theta += rspeed * dy;

		phi = fmod(phi, 2.0f * M_PI);
		theta = theta < M_PI - eps ? (theta > eps ? theta : eps) : M_PI - eps;
	}
}
