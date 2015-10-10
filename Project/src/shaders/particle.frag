///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlen - conwa099
//
///////////////////////////////////////

#version 150

in vec3 exNormal;
in vec2 exTexCoord;
in vec4 exPosition;

out vec4 outColor;

uniform float currT;
uniform mat4 worldView;

void main(void)
{
	// Swap between colors depending on time
	float timeFactor = 20000;
	float currentCounter = mod(currT, timeFactor * 3);
	vec3 newColor;
	if(currentCounter < timeFactor)
	{
		newColor = normalize(vec3(timeFactor - currentCounter, currentCounter, 0));
	}
	else if(currentCounter < timeFactor * 2)
	{
		newColor = normalize(vec3(0, timeFactor * 2 - currentCounter, currentCounter - timeFactor));
	}
	else
	{
		newColor = normalize(vec3(currentCounter - timeFactor * 2, 0, timeFactor * 3 - currentCounter));
	}
	
	// Calculate diffuse light
	vec3 light = mat3(worldView) * vec3(0, 1, 0);
	float shade = max(dot(normalize(exNormal), light), 0.10);
	vec3 shadedColor = newColor * shade;
	
	// Calculate fog
	float dist = length(exPosition);
	float minFogDist = 70.0f;
	float maxFogDist = 150.0f;
	float fogFactor = clamp((dist - minFogDist)/(maxFogDist - minFogDist), 0.0f, 1.0f);
	vec3 foggedColor = (1 - fogFactor) * shadedColor;
			
	// Output complete color
	outColor =  vec4(foggedColor, 1.0f);
}
