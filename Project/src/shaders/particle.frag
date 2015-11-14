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
uniform mat4 WTVmatrix;

// Start From Stackoverflow
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
// End From Stackoverflow

void main()
{
	float colorChangeSpeed = 0.00005f;
	float hue = fract(currT * colorChangeSpeed);
	vec3 hsvColor = vec3(hue, 1.0f, 1.0f);
	vec3 rgbColor = hsv2rgb(hsvColor);
	
	// Calculate diffuse light
	vec3 light = mat3(WTVmatrix) * vec3(0, 1, 0);
	float shade = max(dot(normalize(exNormal), light), 0.10);
	vec3 shadedColor = rgbColor * shade;
	
	// Calculate fog
	float dist = length(exPosition);
	float minFogDist = 70.0f;
	float maxFogDist = 150.0f;
	float fogFactor = clamp((dist - minFogDist)/(maxFogDist - minFogDist), 0.0f, 1.0f);
	vec3 foggedColor = (1 - fogFactor) * shadedColor;
			
	// Output complete color
	outColor =  vec4(foggedColor, 1.0f);
}
