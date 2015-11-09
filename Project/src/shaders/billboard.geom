///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 150 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texValue;
out vec3 exNormal;
out vec4 outPosition;

uniform mat4 VTPmatrix;
uniform float radius;

void main()
{
	// Create the initial positions
	vec3 toEmit = gl_in[0].gl_Position.xyz;
	vec3 toView = vec3(0,0,1);
	vec3 left = vec3(-1, 0,0);
	vec3 top = vec3(0,1,0);
	vec4 tempPos;
	
	// Create the bottom left corner
	toEmit += radius * (left - top);
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(0.0f, 0.0f);
	exNormal = toView - top + left;
	EmitVertex();
	
	// Create the bottom right corner
	toEmit -= 2 * radius * left;
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(1.0f, 0.0f);
	exNormal = toView - top - left;
	EmitVertex();
	
	// Create the top left corner
	toEmit += 2 * radius * (top + left);
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(0.0f, 1.0f);
	exNormal = toView + top + left;
	EmitVertex();
		
	// Create the top right corner
	toEmit -= 2 * radius * left;
	tempPos = vec4(toEmit, 1.0f);
	gl_Position = VTPmatrix * tempPos;
	outPosition = tempPos;
	texValue = vec2(1.0f, 1.0f);
	exNormal = toView + top - left;
	EmitVertex();
	
	EndPrimitive();
}
