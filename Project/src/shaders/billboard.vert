///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahl�n - conwa099
//
///////////////////////////////////////

#version 150

in vec3 posValue;

uniform mat4 WTVmatrix;

void main()
{	
	gl_Position = WTVmatrix * vec4(posValue, 1.0f);
}
