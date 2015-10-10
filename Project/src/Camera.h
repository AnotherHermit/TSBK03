#ifndef CAMERA
#define CAMERA

#include "GL_utilities.h"

#include "glm.hpp"

#include <vector>

class Camera
{
private:
	glm::vec3 p,lookp;
	glm::vec3 heading, side, up;
	GLfloat mspeed, rspeed, phi, theta;
	
	std::vector<glm::vec4> nontransPoints;
	std::vector<glm::vec3> nontransNormals;
	
	GLfloat normals[15];
	GLfloat points[15];
	
	glm::mat4 worldView;
	glm::mat4 proj;
	
	void UpdateCullingBox();
	void Update();
	
public:
	Camera(glm::vec3 startpos);
	void SetFrustum(GLfloat in_left, GLfloat in_right, GLfloat in_bottom, GLfloat in_top, GLfloat in_near, GLfloat in_far);	
	void ResetCamera(glm::vec3 pos);
	
	void MoveForward(GLfloat deltaT);
	void MoveBackward(GLfloat deltaT);
	void MoveLeft(GLfloat deltaT);
	void MoveRight(GLfloat deltaT);
	void MoveUp(GLfloat deltaT);
	void MoveDown(GLfloat deltaT);
	void RotateCamera(GLint dx, GLint dy);
		
	const GLfloat* GetCullingNormals()
	{return normals;}
	
	const GLfloat* GetCullingPoints()
	{return points;}
	
	const glm::mat4 GetProj()
	{return proj;}
	
	const glm::mat4 GetWorldView()
	{return worldView;}
	
	const glm::vec3 GetPos()
	{return p;}

	const glm::vec3 GetHeading()
	{return heading;}

	const glm::vec3 GetSide()
	{return side;}

	const glm::vec3 GetUp()
	{return up;}
};

#endif
