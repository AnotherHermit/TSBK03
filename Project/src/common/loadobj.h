#ifndef loadobj_h
#define loadobj_h

#ifdef __APPLE__
#	include <OpenGL/gl3.h>
#	include <SDL2/SDL.h>
#else
#	ifdef  __linux__
#		define GL_GLEXT_PROTOTYPES
#		include <GL/gl.h>
#		include <GL/glu.h>
#		include <GL/glx.h>
#		include <GL/glext.h>
#		include <SDL2/SDL.h>
#	else
#		include "glew.h"
#		include "Windows/sdl2/SDL.h"
#	endif
#endif

typedef struct {
	GLfloat* vertexArray;
	GLfloat* normalArray;
	GLfloat* texCoordArray;
	GLfloat* colorArray; // Rarely used
	GLuint* indexArray;
	int numVertices;
	int numIndices;

	// Space for saving VBO and VAO IDs
	GLuint vao; // VAO
	GLuint vb, ib, nb, tb; // VBOs
} Model;

Model* LoadModel(const char* name);

// NEW:

void DrawModel(Model *m, GLuint program, const char* vertexVariableName, const char* normalVariableName, const char* texCoordVariableName);
void DrawWireframeModel(Model *m, GLuint program, char* vertexVariableName, char* normalVariableName, char* texCoordVariableName);

Model* LoadModelPlus(const char* name,
					 GLuint program,
					 const char* vertexVariableName,
					 const char* normalVariableName,
					 const char* texCoordVariableName);
void EnableModelForShader(Model *m, GLuint program, // NOT TESTED
						  char* vertexVariableName,
						  char* normalVariableName,
						  char* texCoordVariableName);
Model* LoadDataToModel(
	GLfloat *vertices,
	GLfloat *normals,
	GLfloat *texCoords,
	GLfloat *colors,
	GLuint *indices,
	int numVert,
	int numInd);

/*
Model* LoadDataToModel(
			GLfloat *vertices,
			GLfloat *normals,
			GLfloat *texCoords,
			GLfloat *colors,
			GLuint *indices,
			int numVert,
			int numInd,

			GLuint program,
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName);
*/
void BuildModelVAO2(Model *m,
					GLuint program,
					const char* vertexVariableName,
					const char* normalVariableName,
					const char* texCoordVariableName);

void CenterModel(Model *m);
void ScaleModel(Model *m, float sx, float sy, float sz);
void NormalizeModel(Model *m);

#endif
