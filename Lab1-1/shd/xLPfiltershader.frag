#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
	float offset = 1.0 / 512.0;	
	vec4 total_Color;
	
	total_Color = 2.0 * texture(texUnit, outTexCoord);
	total_Color += texture(texUnit, outTexCoord + vec2(-offset, 0));
	total_Color += texture(texUnit, outTexCoord + vec2(+offset, 0));
		
	out_Color = total_Color / 4.0;
}
