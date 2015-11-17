///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahl�n - conwa099
//
///////////////////////////////////////

#version 430

in vec3 posValue;

struct Camera {
	mat4 WTVmatrix;
	mat4 VTPmatrix;
	vec4 normals[5];
	vec4 points[5];
	float viewDistance;
	float padding73[3];
};

layout (std140, binding = 10) uniform CameraBuffer {
	Camera cam;
};

void main()
{	
	gl_Position = cam.WTVmatrix * vec4(posValue, 1.0f);
}
