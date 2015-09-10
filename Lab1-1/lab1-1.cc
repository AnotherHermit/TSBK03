// Lab 1-1, multi-pass rendering with FBOs and HDR.
// Revision for 2013, simpler light, start with rendering on quad in final stage.
// Switched to low-res Stanford Bunny for more details.
// No HDR is implemented to begin with. That is your task.

// You can compile like this:
// gcc lab1-1.c ../common/*.c -lGL -o lab1-1 -I../common

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#ifdef _WIN32
// MS
		#include <windows.h>
		#include <stdio.h>
		#include "glew.h"
		#include "freeglut.h"
	#else
// Linux
		#include <stdio.h>
		#include <GL/gl.h>
		#include "MicroGlut.h"
//		#include <GL/glut.h>
	#endif
#endif

#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "zpr.h"

#include <iostream>

// initial width and heights
#define W 512
#define H 512

#define NUM_LIGHTS 4

void OnTimer(int value);

mat4 projectionMatrix;
mat4 viewMatrix;


GLfloat square[] = {
							-1,-1,0,
							-1,1, 0,
							1,1, 0,
							1,-1, 0};
GLfloat squareTexCoord[] = {
							 0, 0,
							 0, 1,
							 1, 1,
							 1, 0};
GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

Model* squareModel;

//----------------------Globals-------------------------------------------------
Point3D cam, point;
Model *model1;
FBOstruct *fbo1, *fbo2, *fbo3;
GLuint phongshader = 0, plaintextureshader = 0;
GLuint xLPfiltershader = 0, yLPfiltershader = 0, clampshader = 0, bloomingshader = 0;

//-------------------------------------------------------------------------------------

void init(void)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	// Load and compile shaders
	plaintextureshader = loadShaders("Lab1-1/shd/plaintextureshader.vert", "Lab1-1/shd/plaintextureshader.frag");  // puts texture on teapot
	xLPfiltershader = loadShaders("Lab1-1/shd/plaintextureshader.vert", "Lab1-1/shd/xLPfiltershader.frag");
	yLPfiltershader = loadShaders("Lab1-1/shd/plaintextureshader.vert", "Lab1-1/shd/yLPfiltershader.frag");
	clampshader = loadShaders("Lab1-1/shd/plaintextureshader.vert", "Lab1-1/shd/clampshader.frag");
	bloomingshader = loadShaders("Lab1-1/shd/plaintextureshader.vert", "Lab1-1/shd/bloomingshader.frag");
	phongshader = loadShaders("Lab1-1/shd/phong.vert", "Lab1-1/shd/phong.frag");  // renders with light (used for initial renderin of teapot)

	printError("init shader");

	fbo1 = initFBO(W, H, 0);
	fbo2 = initFBO(W, H, 0);
	fbo3 = initFBO(W, H, 0);

	// load the model
//	model1 = LoadModelPlus("teapot.obj");
	model1 = LoadModelPlus("Lab1-1/obj/stanford-bunny.obj");

	squareModel = LoadDataToModel(
			square, NULL, squareTexCoord, NULL,
			squareIndices, 4, 6);

	cam = SetVector(0, 5, 15);
	point = SetVector(0, 1, 0);

	glutTimerFunc(5, &OnTimer, 0);

	zprInit(&viewMatrix, cam, point);
}

void OnTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(5, &OnTimer, value);
}


void filterLP(int passes)
{
	for(int i = 0; i < passes; i++)
	{
		// Filter X
		useFBO(fbo2, fbo1, 0L);
		glClearColor(0.0, 0.0, 0.0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(xLPfiltershader);
		
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		
		DrawModel(squareModel, xLPfiltershader, "in_Position", NULL, "in_TexCoord");
		
		// Filter Y
		useFBO(fbo1, fbo2, 0L);
		glClearColor(0.0, 0.0, 0.0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(yLPfiltershader);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		
		DrawModel(squareModel, yLPfiltershader, "in_Position", NULL, "in_TexCoord");
	}
}

//-------------------------------callback functions------------------------------------------
void display(void)
{
	mat4 vm2;
	
	// This function is called whenever it is time to render
	//  a new frame; due to the idle()-function below, this
	//  function will get called several times per second

	// render to fbo1!
	useFBO(fbo3, 0L, 0L);

	// Clear framebuffer & zbuffer
	glClearColor(0.1, 0.1, 0.3, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate shader program
	glUseProgram(phongshader);

	vm2 = viewMatrix;
	// Scale and place bunny since it is too small
	vm2 = Mult(vm2, T(0, -8.5, 0));
	vm2 = Mult(vm2, S(80,80,80));

	glUniformMatrix4fv(glGetUniformLocation(phongshader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(phongshader, "modelviewMatrix"), 1, GL_TRUE, vm2.m);
	glUniform3fv(glGetUniformLocation(phongshader, "camPos"), 1, &cam.x);
	glUniform1i(glGetUniformLocation(phongshader, "texUnit"), 0);

	// Enable Z-buffering
	glEnable(GL_DEPTH_TEST);
	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	DrawModel(model1, phongshader, "in_Position", "in_Normal", NULL);

	// Done rendering the FBO! Set up for rendering on screen, using the result as texture!
	
	// Clamp
	useFBO(fbo1, fbo3, 0L);
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(clampshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	DrawModel(squareModel, clampshader, "in_Position", NULL, "in_TexCoord");
	
	// LP the clamped values
	filterLP(20);
	
	// Add together
	
//	glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.
	useFBO(0L, fbo1, fbo3);

	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate second shader program
	glUseProgram(bloomingshader);
	glUniform1i(glGetUniformLocation(bloomingshader, "bloomingTex"), 0);
	glUniform1i(glGetUniformLocation(bloomingshader, "originalTex"), 1);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(squareModel, bloomingshader, "in_Position", NULL, "in_TexCoord");

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	GLfloat ratio = (GLfloat) w / (GLfloat) h;
	projectionMatrix = perspective(90, ratio, 1.0, 1000);
}


// This function is called whenever the computer is idle
// As soon as the machine is idle, ask GLUT to trigger rendering of a new
// frame
void idle()
{
  glutPostRedisplay();
}

//-----------------------------main-----------------------------------------------
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(W, H);

	glutInitContextVersion(3, 2);
	glutCreateWindow ("Render to texture with FBO");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	#ifdef _WIN32
		glewExperimental = GL_TRUE; 
		glewInit();
	#endif
	
	init();
	glutMainLoop();
	exit(0);
}

