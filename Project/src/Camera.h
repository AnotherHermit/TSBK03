///////////////////////////////////////
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

#include "AntTweakBar.h"

#include <vector>

// Uniform struct, needs to be arranged in multiples of 4 * 4 B for tight packing on GPU
struct CameraParam {
	glm::mat4 WTVmatrix;	// 16 * 4 B ->   0 -  15
	glm::mat4 VTPmatrix;	// 16 * 4 B ->  16 -  31
	glm::vec4 normals[8];	// 32 * 4 B ->  32 -  63
	glm::vec4 points[8];	// 32 * 4 B ->  64 -  95
	glm::vec3 position;		//  3 * 4 B ->  96 -  98
	GLuint padding99;		//  1 * 4 B ->  99 -  99
	glm::vec4 lodLevels;	//  4 * 4 B -> 100 - 103
};

class Camera {
private:
	glm::vec3 lookp, yvec;
	glm::vec3 heading, side, up;
	GLfloat mspeed, rspeed, phi, theta;

	glm::vec4 nontransPoints[8];
	glm::vec3 nontransNormals[8];

	bool isPaused;
	bool needUpdate;

	GLuint cameraBuffer;
	GLint *winWidth, *winHeight;

	CameraParam param;

	TwStructMember cameraTwMembers[7];
	TwType cameraTwStruct;

	void UpdateCullingBox();
	void Update();
	void UploadParams();

public:
	Camera(glm::vec3 startpos, GLint *screenWidth, GLint *screenHeight, glm::vec4 inLodLevels);
	void SetFrustum();
	void ResetCamera(glm::vec3 pos);

	void MoveForward(GLfloat deltaT);
	void MoveRight(GLfloat deltaT);
	void MoveUp(GLfloat deltaT);
	void RotateCamera(GLint dx, GLint dy);

	void UpdateCamera();

	void TogglePause() { isPaused = !isPaused; }

	GLfloat* GetSpeedPtr() { return &mspeed; }
	GLfloat* GetRotSpeedPtr() { return &rspeed; }

	TwType GetCameraTwType() { return cameraTwStruct; }

	static void TW_CALL SetLODCB(const void* value, void* clientData);
	static void TW_CALL GetCamParamsCB(void* value, void* clientData);
};

#endif // CAMERA_H
