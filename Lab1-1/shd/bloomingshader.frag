#version 150

in vec2 outTexCoord;
uniform sampler2D bloomingTex;
uniform sampler2D originalTex;
out vec4 out_Color;

void main(void)
{
	
    out_Color = 10 * texture(bloomingTex, outTexCoord) + 1.0 * texture(originalTex, outTexCoord);
}