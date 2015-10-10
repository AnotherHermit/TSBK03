///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 150

layout(points) in;
layout(points, max_vertices = 1) out;

in vec3 exPosValue[1];
flat in int wantToDraw[1];

out vec3 culledPosValue;

void main()
{
	if(wantToDraw[0] == 5)
	{
		culledPosValue = exPosValue[0];
		EmitVertex();
	}
	EndPrimitive();
}
