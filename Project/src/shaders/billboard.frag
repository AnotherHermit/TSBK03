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
	float colorChangeSpeed = 0.05f;
	float hue = fract(currT * colorChangeSpeed);
	vec3 hsvColor = vec3(hue, 1.0f, 1.0f);
	vec3 rgbColor = hsv2rgb(hsvColor);

		
	// Calculate fog
	float dist = length(outPosition);
	float minFogDist = 70.0f;
	float maxFogDist = 150.0f;
	float fogFactor = clamp((dist - minFogDist)/(maxFogDist - minFogDist), 0.0f, 1.0f);
	vec3 foggedColor = (1 - fogFactor) * rgbColor;
	
	// Calculate light
	vec3 light = vec3(0.0f, 1.0f, 0.0f);
	float shade = max(dot(normalize(exNormal), mat3(WTVmatrix) * light), 0.1);
	
	// Get texture colors
	vec4 texColor = texture(texUnit, texValue);
	
	// Output complete color
	outColor =  texColor * vec4(foggedColor * shade, 0.2f);
	
	if(texColor.r < 0.85 && texColor.g < 0.85 && texColor.b < 0.85)
		discard;
}
