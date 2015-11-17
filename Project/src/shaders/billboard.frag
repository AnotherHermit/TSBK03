///////////////////////////////////////
//
//	Computer Graphics TSBK03
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 430

in vec4 outPosition;
in vec2 texValue;
in vec3 exNormal;

out vec4 outColor;

struct Camera {
	mat4 WTVmatrix;
	mat4 VTPmatrix;
	vec4 normals[5];
	vec4 points[5];
	float viewDistance;
	float padding73[3];
};

struct Program {
	float currentT;
	float deltaT;
	float radius;
	float simSpeed;
};

layout (std140, binding = 10) uniform CameraBuffer {
	Camera cam;
};

layout (std140, binding = 12) uniform ProgramBuffer {
	Program prog;
};

uniform sampler2D texUnit;

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
	float hue = fract(prog.currentT * colorChangeSpeed);
	vec3 hsvColor = vec3(hue, 1.0f, 1.0f);
	vec3 rgbColor = hsv2rgb(hsvColor);

	// Calculate light
	vec3 light = mat3(cam.WTVmatrix) * vec3(0, 1, 0);
	float shade = max(dot(normalize(exNormal), light), 0.1);
	vec3 shadedColor = rgbColor * shade;

	// Calculate fog
	float dist = length(outPosition);
	float minFogDist = cam.viewDistance / 2;
	float maxFogDist = cam.viewDistance;
	float fogFactor = clamp((dist - minFogDist)/(maxFogDist - minFogDist), 0.0f, 1.0f);
	vec3 foggedColor = (1 - fogFactor) * shadedColor;

	// Get texture colors
	vec4 texColor = texture(texUnit, texValue);
	
	// Output complete color
	outColor =  texColor * vec4(foggedColor, 1f);
	
	//if(texColor.r < 0.85 && texColor.g < 0.85 && texColor.b < 0.85)
		//discard;
}
