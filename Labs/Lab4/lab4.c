// Demo of heavily simplified sprite engine
// by Ingemar Ragnemalm 2009
// used as base for lab 4 in TSBK03.
// OpenGL 3 conversion 2013.
#ifdef __APPLE__
// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#ifdef _WIN32
// MS
		#include "glew.h"
		#include "freeglut.h"
	#else
// Linux
		#include "MicroGlut.h"
		#include <GL/gl.h>
//		#include <GL/glut.h>
	#endif
#endif

#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

#include "LoadTGA.h"
#include "GL_utilities.h"

#include "SpriteLight.h"
#include "BoidHandler.h"

// L�gg till egna globaler h�r efter behov.
TextureData *sheepFace, *blackFace, *dogFace, *foodFace;
BoidGene *sheepGene, *blackGene, *dogGene;
BoidHandler *boidz;

static double startTime = 0.0;
GLfloat deltaT = 0.0f;

void resetElapsedTime()
{
  struct timeval timeVal;
  gettimeofday(&timeVal, 0);
  startTime = (double) timeVal.tv_sec + (double) timeVal.tv_usec * 0.000001;
}

float getElapsedTime()
{
  struct timeval timeVal;
  gettimeofday(&timeVal, 0);
  double currentTime = (double) timeVal.tv_sec
    + (double) timeVal.tv_usec * 0.000001;

  return currentTime - startTime;
}

// Drawing routine
void Display()
{
	glClearColor(0, 0, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT+GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawBackground();

	boidz->boidBehave(deltaT);
	boidz->boidMoveDraw();

	glutSwapBuffers();
}

void Reshape(int h, int v)
{
	glViewport(0, 0, h, v);
	gWidth = h;
	gHeight = v;
}

void Timer(int value)
{
	glutTimerFunc(20, Timer, 0);
	deltaT = getElapsedTime();
	resetElapsedTime();
	glutPostRedisplay();
}

void Key(unsigned char key,
         __attribute__((unused)) int x,
         __attribute__((unused)) int y)
{
  switch (key)
  {
    case '+':
    	sheepGene->data[PRV_WEIGHT] += 0.1;
    	printf("pWeight = %f\n", sheepGene->data[PRV_WEIGHT]);
    	break;
    case '-':
    	sheepGene->data[PRV_WEIGHT] -= 0.1;
    	printf("pWeight = %f\n", sheepGene->data[PRV_WEIGHT]);
    	break;
	case 'd':
    	sheepGene->data[COH_WEIGHT] += 0.1;
    	printf("cWeight = %f\n", sheepGene->data[COH_WEIGHT]);
    	break;
	case 'c':
    	sheepGene->data[COH_WEIGHT] -= 0.1;
    	printf("cWeight = %f\n", sheepGene->data[COH_WEIGHT]);
    	break;
	case 's':
    	sheepGene->data[SEP_WEIGHT] += 0.1;
    	printf("sWeight = %f\n", sheepGene->data[SEP_WEIGHT]);
    	break;
	case 'x':
    	sheepGene->data[SEP_WEIGHT] -= 0.1;
    	printf("sWeight = %f\n", sheepGene->data[SEP_WEIGHT]);
    	break;
	case 'a':
		sheepGene->data[ALI_WEIGHT] += 0.1;
		printf("aWeight = %f\n", sheepGene->data[ALI_WEIGHT]);
		break;
	case 'z':
    	sheepGene->data[ALI_WEIGHT] -= 0.1;
    	printf("aWeight = %f\n", sheepGene->data[ALI_WEIGHT]);
    	break;
	case 'b':
    	boidz->addSheep(1, sheepFace, sheepGene);
    	printf("Added a sheep, number of sheep = %d\n", boidz->size());
    	break;
    case 0x1b:
      exit(0);
  }
}

void Init()
{
	srand(time(NULL)); // Seed the random function

	LoadTGATextureSimple("Lab4/tex/leaves.tga", &backgroundTexID); // Bakgrund

	sheepFace = GetFace("Lab4/tex/sheep.tga"); // Ett f�r
	blackFace = GetFace("Lab4/tex/blackie.tga"); // Ett svart f�r
	dogFace = GetFace("Lab4/tex/dog.tga"); // En hund
	foodFace = GetFace("Lab4/tex/mat.tga"); // Mat

	sheepGene = new BoidGene();

	blackGene = new BoidGene();
	blackGene->data[RND_WEIGHT] = 0.5f;
	blackGene->data[COH_WEIGHT] = 0.0f;
	blackGene->data[ALI_WEIGHT] = 0.0f;

	dogGene = new BoidGene();
	dogGene->data[RND_WEIGHT] = 0.5f;
	dogGene->data[PRV_WEIGHT] = 1.0f;
	dogGene->data[SPEED] = 3.0f;

	boidz = new BoidHandler();
	boidz->addSheep(10, sheepFace, sheepGene);
	boidz->addSheep(1, blackFace, blackGene);
	boidz->addDog(2, dogFace, dogGene);
	
	resetElapsedTime();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(gWidth, gHeight);
	glutInitContextVersion(3, 2);
	glutCreateWindow("SpriteLight demo / Flocking");

	glutDisplayFunc(Display);
	glutTimerFunc(20, Timer, 0); // Should match the screen synch
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Key);
	#ifdef WIN32
		glewInit();
	#endif

	InitSpriteLight();

	Init();

	glutMainLoop();
	return 0;
}
