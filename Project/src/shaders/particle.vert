///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 150

in vec3 posValue;
in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec3 exNormal;
out vec2 exTexCoord;
out vec4 exPosition;

uniform mat4 MTWmatrix;
uniform mat4 WTVmatrix;
uniform mat4 VTPmatrix;

void main(void)
{
	exNormal = mat3(WTVmatrix * MTWmatrix) * inNormal;

	exTexCoord = inTexCoord;
	
	vec4 temp = WTVmatrix * (MTWmatrix * vec4(inPosition, 1.0f) + vec4(posValue, 0.0f));
	
	exPosition = temp;
	
	gl_Position = VTPmatrix * temp;
}
