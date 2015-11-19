///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 430 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texValue;
out vec3 exNormal;
out vec4 outPosition;

struct Camera {
	mat4 WTVmatrix;
	mat4 VTPmatrix;
	vec4 normals[5];
	vec4 points[5];
	vec3 position;
	float viewDistance;
};

struct Program {
	float currentT;
	float deltaT;
	float radius;
	float simSpeed;
};

layout (std140, binding = 10) uniform CameraBuffer {
	Camera cam;
};

layout (std140, binding = 12) uniform ProgramBuffer {
	Program prog;
};


void main()
{
	// Create the initial positions
	vec3 toEmit = gl_in[0].gl_Position.xyz;
	vec3 toView = vec3(0,0,1);
	vec3 left = vec3(-1, 0,0);
	vec3 top = vec3(0,1,0);
	vec4 tempPos;
	
	// Create the bottom left corner
	toEmit += prog.radius * (left - top);
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = cam.VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(0.0f, 0.0f);
	exNormal = toView - top + left;
	EmitVertex();
	
	// Create the bottom right corner
	toEmit -= 2 * prog.radius * left;
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = cam.VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(1.0f, 0.0f);
	exNormal = toView - top - left;
	EmitVertex();
	
	// Create the top left corner
	toEmit += 2 * prog.radius * (top + left);
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = cam.VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(0.0f, 1.0f);
	exNormal = toView + top + left;
	EmitVertex();
		
	// Create the top right corner
	toEmit -= 2 * prog.radius * left;
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = cam.VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(1.0f, 1.0f);
	exNormal = toView + top - left;
	EmitVertex();
	
	EndPrimitive();
}
