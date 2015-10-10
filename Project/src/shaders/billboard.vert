///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahl�n - conwa099
//
///////////////////////////////////////

#version 150

in vec3 posValue;

uniform mat4 worldView;

void main()
{	
	gl_Position = worldView * vec4(posValue, 1.0f);
}
