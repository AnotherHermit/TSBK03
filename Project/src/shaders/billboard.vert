///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 150

in vec3 posValue;

uniform mat4 worldView;

void main()
{	
	gl_Position = worldView * vec4(posValue, 1.0f);
}
