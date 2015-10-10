///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlen - conwa099
//
///////////////////////////////////////

#version 150

in vec4 outPosition;
in vec2 texValue;
in vec3 exNormal;

out vec4 outColor;

uniform float currT;
uniform sampler2D texUnit;
uniform mat4 worldView;

void main()
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
		
	// Calculate fog
	float dist = length(outPosition);
	float minFogDist = 70.0f;
	float maxFogDist = 150.0f;
	float fogFactor = clamp((dist - minFogDist)/(maxFogDist - minFogDist), 0.0f, 1.0f);
	vec3 foggedColor = (1 - fogFactor) * newColor;
	
	// Calculate light
	vec3 light = vec3(0.0f, 1.0f, 0.0f);
	float shade = max(dot(normalize(exNormal), mat3(worldView) * light), 0.1);
	
	// Get texture colors
	vec4 texColor = texture(texUnit, texValue);
	
	// Output complete color
	outColor =  texColor * vec4(foggedColor * shade, 0.2f);
	
	if(texColor.r < 0.85 && texColor.g < 0.85 && texColor.b < 0.85)
		discard;
}
