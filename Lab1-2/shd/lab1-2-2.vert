#version 150

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;
in vec3 Vs;
in vec3 Vt;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 outTexCoord;
out mat3 Mvt;
out vec3 pixPos;  // Needed for specular reflections

void main(void)
{
    outTexCoord = in_TexCoord;
    vec3 normal = mat3(viewMatrix) * in_Normal; // Cheated normal matrix, OK with no non-uniform scaling
    pixPos = vec3(viewMatrix * vec4(in_Position, 1.0));

    vec3 Ps = normalize( mat3(viewMatrix) * Vs );
    vec3 Pt = normalize( mat3(viewMatrix) * Vt );
	
	Mvt = transpose(mat3(Ps, Pt, normal));
	
    gl_Position = projMatrix * vec4(pixPos, 1.0);
}
