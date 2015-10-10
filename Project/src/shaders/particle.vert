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

uniform mat4 modelWorld;
uniform mat4 worldView;
uniform mat4 proj;

void main(void)
{
	exNormal = mat3(worldView * modelWorld) * inNormal;

	exTexCoord = inTexCoord;
	
	vec4 temp = worldView * (modelWorld * vec4(inPosition, 1.0f) + vec4(posValue, 0.0f));
	
	exPosition = temp;
	
	gl_Position = proj * temp;
}
