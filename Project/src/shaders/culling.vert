///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 150

in vec3 posValue;

out vec3 exPosValue;
flat out int wantToDraw;

uniform float radius;
uniform vec3 boxNormals[5];
uniform vec3 boxPoints[5];

void main(void)
{
	exPosValue = posValue;
	
	int intersections = 0;
	
	for(int i = 0; i < 5; ++i)
	{
		vec3 new_point = posValue + boxNormals[i] * radius;
		float distance = dot(new_point, boxNormals[i]) - dot(boxPoints[i], boxNormals[i]);
		if(distance > 0)
			intersections++;
	}
	
	wantToDraw = intersections;
}
