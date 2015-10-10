///////////////////////////////////////
//
//	Computer Graphics TSBK07
//	Conrad Wahlén - conwa099
//
///////////////////////////////////////

#version 150

in vec3 posValue;
in vec3 velValue;
in vec3 accValue;

out vec3 updatePosValue;
out vec3 updateVelValue;
out vec3 updateAccValue;

uniform float deltaT;
uniform float t;

// Start From Stack Overflow
float rand(vec2 co){
    return 2*(fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453) - 0.5);
}
// End From Stack Overflow

// Version 1 falling cube
/*
void main()
{
	if(posValue.y < 0)
	{	
		float rand1 = rand(posValue.xy * velValue.y);
		float rand2 = rand(posValue.yz * velValue.y);
		vec3 randDir = normalize(vec3(rand1, 0, rand2));
	
		updatePosValue = posValue * vec3(1,0,1);
		updateVelValue = randDir * length(velValue)*2;
		updateAccValue = vec3(0);
	}
	else if( posValue.y == 0)
	{
		updatePosValue = posValue + velValue * deltaT * 0.001;
		updateVelValue = velValue + accValue * 0.1 * deltaT * 0.001;
		updateAccValue = -30 * velValue;
	}	
	else
	{
		updatePosValue = posValue + velValue * deltaT * 0.001;
		updateVelValue = velValue + accValue * 0.1 * deltaT * 0.001;
		updateAccValue = -vec3(0,300.0f, 0);
		//updateAccValue = -normalize(posValue)*10000/(length(posValue)+0.01);
	}	
}
*/
// Version 2 gravity center in origin
void main()
{
	updatePosValue = posValue + velValue * deltaT * 0.001;
	updateVelValue = velValue + accValue * 0.1 * deltaT * 0.001;
	updateAccValue = normalize(vec3(100*sin(0.001*t), 0, 100*cos(0.001*t))-posValue)*10000/(length(posValue)+10.0);
}

