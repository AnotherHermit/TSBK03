#version 150

//in vec3 in_Color;
in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;
uniform mat4 matrix;
uniform mat4 boneRot;
uniform vec3 bonePos;

out vec4 g_color;
const vec3 lightDir = normalize(vec3(0.3, 0.5, 1.0));

// Uppgift 3: Soft-skinning på GPU
//
// Flytta över din implementation av soft skinning från CPU-sidan
// till vertexshadern. Mer info finns på hemsidan.

void main(void)
{
	vec3 v1 = in_Position;
	vec3 v2 = in_Position - bonePos;
	v2 = mat3(boneRot) * v2 + bonePos; 
	vec3 v = v1 * in_TexCoord.x + v2 * (1-in_TexCoord.x);
	
	// transformera resultatet med ModelView- och Projection-matriserna
	gl_Position = matrix * vec4(v, 1.0);

	// sätt röd+grön färgkanal till vertex Weights
	vec4 color = vec4(in_TexCoord.x, in_TexCoord.y, 0.0, 1.0);

	// Lägg på en enkel ljussättning på vertexarna 	
	float intensity = dot(in_Normal, lightDir);
	color.xyz *= intensity;

	g_color = color;
}

