//***************************************************************************
// GAME2012_FinalProject.cpp by Samuel Beaudoin and Patrick Crowley (C) 2021 All Rights Reserved.
//
// Student ID: 100883341  -  Sam
// and	
// Student ID: 101305076  -  Patrick
// 
// Final Project submission.
//
// Description:
// Click run to see the results. Is Castle
//
//*****************************************************************************


////http://glew.sourceforge.net/
//The OpenGL Extension Wrangler Library (GLEW) is a cross-platform open-source C/C++ extension loading library. 
//GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target
//platform. OpenGL core and extension functionality is exposed in a single header file. GLEW has been tested on a 
//variety of operating systems, including Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris.
//
//http://freeglut.sourceforge.net/
//The OpenGL Utility Toolkit(GLUT) is a library of utilities for OpenGL programs, which primarily perform system - level I / O with the host operating system.
//Functions performed include window definition, window control, and monitoring of keyboardand mouse input.
//Routines for drawing a number of geometric primitives(both in solid and wireframe mode) are also provided, including cubes, spheresand the Utah teapot.
//GLUT also has some limited support for creating pop - up menus..

//OpenGL functions are in a single library named GL (or OpenGL in Windows). Function names begin with the letters glSomeFunction*();
//Shaders are written in the OpenGL Shading Language(GLSL)
//To interface with the window system and to get input from external devices into our programs, we need another library. For the XWindow System, this library is called GLX, for Windows, it is wgl,
//and for the Macintosh, it is agl. Rather than using a different library for each system,
//we use two readily available libraries, the OpenGL Extension Wrangler(GLEW) and the OpenGLUtilityToolkit(GLUT).
//GLEW removes operating system dependencies. GLUT provides the minimum functionality that should be expected in any modern window system.
//OpenGL makes heavy use of defined constants to increase code readability and avoid the use of magic numbers.Thus, strings such as GL_FILL and GL_POINTS are defined in header(#include <GL/glut.h>)

//https://glm.g-truc.net/0.9.9/index.html
////OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include "Shape.h"
#include "Light.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 60
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define XYZ_AXIS glm::vec3(1,1,1)
#define SPEED 0.25f

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum keyMasks {
	KEY_FORWARD = 0b00000001,		// 0x01 or   1	or   01
	KEY_BACKWARD = 0b00000010,		// 0x02 or   2	or   02
	KEY_LEFT = 0b00000100,
	KEY_RIGHT = 0b00001000,
	KEY_UP = 0b00010000,
	KEY_DOWN = 0b00100000,
	KEY_MOUSECLICKED = 0b01000000
	// Any other keys you want to add.
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint modelID, viewID, projID;
glm::mat4 View, Projection;

// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Texture variables.
GLuint blankID;
GLuint platformID;
GLuint hedgeID;
GLuint wallID;
GLuint pickUpID;
GLuint wallTopID;
GLuint towerID;
GLuint roofID;
GLuint gateID;

GLint width, height, bitDepth;

GLUquadricObj* quadObj = gluNewQuadric();

// Light objects.
AmbientLight aLight(
	glm::vec3(1.0f, 1.0f, 1.0f),
	0.5f);

PointLight pLights[2] = {
	{ glm::vec3(16.5f, 2.4f, -23.0f),	// Position.
	25.0f,								// Range.
	1.0f, 4.5f, 75.0f,					// Constant, Linear, Quadratic.   
	glm::vec3(0.1f, 1.0f, 0.1f),		// Diffuse colour.
	1.0f },								// Diffuse strength.

	{ glm::vec3(33.5f, 0.4f, -23.0f),	// Position.
	10.0f,								// Range.
	1.0f, 4.5f, 75.0f,					// Constant, Linear, Quadratic.   
	glm::vec3(1.0f, 0.2f, 0.2f),		// Diffuse colour.
	1.0f } };							// Diffuse strength.

Material mat = { 1.0f, 32 }; // Alternate way to construct an object.

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Geometry data.
Grid g_grid(50);

Cube c_Platform(3.0f, 0.2f, 4.0f);

Cube c_HedgeHouse[6] =
{
	{5.0f, 1.0f, 0.1f},		//Back
	{0.1f, 1.0f, 5.2f},		//Right
	{2.0f, 1.0f, 0.1f},		//Front Right
	{2.0f, 1.0f, 0.1f},		//Front Left
	{0.1f, 1.0f, 5.2f},		//Left
	{5.2f, 0.1f, 5.2f},		//Roof
};

Cube c_Headges[39] =
{

	//x				 y			z
	{1.0f,		  1.0f,		21.0f},  //Headge 0
	{15.0f,		  1.0f,		 1.0f},  //Headge 1
	{14.0f,		  1.0f,		 1.0f},  //Headge 2
	{1.0f,		  1.0f,		21.0f},  //Headge 3
	{33.0f,		  1.0f,		 1.0f},  //Headge 4
	{2.0f,		  1.0f,		1.0f},  //Headge 5
	{1.0f,		  1.0f,		8.0f},  //Headge 6
	{8.0f,		  1.0f,		1.0f},  //Headge 7
	{1.0f,		  1.0f,		8.0f},  //Headge 8
	{5.0f,		  1.0f,		1.0f},  //Headge 9
	{3.0f,		  1.0f,		1.0f},  //Headge 10
	{1.0f,		  1.0f,		3.0f},  //Headge 11
	{7.0f,		  1.0f,		1.0f},  //Headge 12
	{1.0f,		  1.0f,		4.0f},  //Headge 13
	{1.0f,		  1.0f,		4.0f},  //Headge 14
	{1.0f,		  1.0f,		4.0f},  //Headge 15
	{1.0f,		  1.0f,		4.0f},  //Headge 16
	{1.0f,		  1.0f,		3.0f},  //Headge 17
	{1.0f,		  1.0f,		1.0f},  //Headge 18
	{1.0f,		  1.0f,		1.0f},  //Headge 19
	{1.0f,		  1.0f,		1.0f},  //Headge 20
	{1.0f,		  1.0f,		4.0f},  //Headge 21
	{1.0f,		  1.0f,		2.0f},  //Headge 22
	{8.0f,		  1.0f,		1.0f},  //Headge 23
	{9.0f,		  1.0f,		1.0f},  //Headge 24
	{1.0f,		  1.0f,		2.0f},  //Headge 25
	{5.0f,		  1.0f,		1.0f},  //Headge 26
	{11.0f,		  1.0f,		1.0f},  //Headge 27
	{6.0f,		  1.0f,		1.0f},  //Headge 28
	{1.0f,		  1.0f,		7.0f},  //Headge 29
	{4.0f,		  1.0f,		1.0f},  //Headge 30
	{1.0f,		  1.0f,		7.0f},  //Headge 31
	{1.0f,		  1.0f,		4.0f},  //Headge 32
	{1.0f,		  1.0f,		4.0f},  //Headge 33
	{5.0f,		  1.0f,		1.0f},  //Headge 34
	{1.0f,		  1.0f,		6.0f},  //Headge 35
	{6.0f,		  1.0f,		1.0f},  //Headge 36
	{10.0f,		  1.0f,		1.0f},  //Headge 37
	{1.0f,		  1.0f,		10.0f}   //Headge 38

};

Cube c_Walls[5] =
{
	{1.0f, 5.0f, 30.0f },	// Wall #0
	{16.0f, 5.0f, 1.0f},	// Wall #1
	{16.0f, 5.0f, 1.0f},	// Wall #2
	{1.0f, 5.0f, 30.0f},	// Wall #3
	{40.0f, 5.0f, 1.0f}		// Wall #4
};

Cube c_PickUp(0.5f, 0.5f, 0.5f);

Cube c_WallToppings[8] =
{
	{0.1f, 0.5f, 31.0f},	// Wall Railing #0
	{0.1f, 0.5f, 29.0f},	// Wall Railing #1
	{40.0f, 0.5f, 0.1f},	// Wall Railing #2
	{38.2f, 0.5f, 0.1f},	// Wall Railing #3
	{0.1f, 0.5f, 31.0f},	// Wall Railing #4
	{0.1f, 0.5f, 29.0f},	// Wall Railing #5
	{40.0f, 0.5f, 0.1f},	// Wall Railing #6
	{38.2f, 0.5f, 0.1f},	// Wall Railing #7
};

Cube c_WallCrenells[9] =
{
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f},
	{0.1f, 0.8f, 3.0f}
};

Cube c_WallCrenells1[13] =
{
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f},
	{2.6f, 0.8f, 0.1f}
};

Prism p_Towers[4]
{
	8,
	8,
	8,
	8
};

Prism p_GuardTowers[4]
{
	8,
	8,
	8,
	8
};

Cone c_guardTowerRoof[4]
{
	8,
	8,
	8,
	8
};

Cone c_towerRoof[4]
{
	8,
	8,
	8,
	8
};

Cube c_GuardHouse[6] =
{
	{4.0f, 5.0f, 4.0f},		// Guardhouse
	{1.0f, 2.0f, 0.1f},		// Gate #1
	{1.0f, 2.0f, 0.1f},		// Gate #2
	{1.75f, 0.75, 0.3f},	// High Step
	{1.75f, 0.50, 0.3f},	// Med Step
	{1.75f, 0.25, 0.3f}		// Low Step
};

Cube c_GuardHouseTrimmings[10] =
{
	{0.1f, 2.0f, 0.2f},		// Outside Left 
	{0.1f, 2.0f, 0.2f},		// Outside Right
	{1.2f, 0.1f, 0.2f},		// Outside Lip
	{0.1f, 2.0f, 0.2f},		// Inside Left 
	{0.1f, 2.0f, 0.2f},		// Inside Right
	{1.2f, 0.1f, 0.2f},		// Inside Lip
	{0.1f, 0.1f, 1.345f},	// Rail Left
	{0.1f, 0.1f, 1.345f},	// Rail Right
	{0.1f, 1.0f, 0.1f},		// Rail Right Support
	{0.1f, 1.0f, 0.1f},		// Rail Left Support
};

Cone c_RailingTopper[2] =
{
	{4}, {4}
};

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(25.0f, 25.0f, -18.0f); 
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = -105.0f;
	yaw = 90.0f;
	// View will now get set only in transformObject
}

void init(void)
{
	srand((unsigned)time(NULL));
	// Create shader program executable.
	vertexShaderId = setShader((char*)"vertex", (char*)"cube.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cube.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projID = glGetUniformLocation(program, "projection");
	
	// Projection matrix : 45∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	stbi_set_flip_vertically_on_load(true);

	// Load first image.
	unsigned char* image = stbi_load("Ground.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &blankID);
	glBindTexture(GL_TEXTURE_2D, blankID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End first image.

	// Load second image.
	image = stbi_load("Platform.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &platformID);
	glBindTexture(GL_TEXTURE_2D, platformID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End second image.

	// Load third image.
	image = stbi_load("hedge_ivy.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &hedgeID);
	glBindTexture(GL_TEXTURE_2D, hedgeID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End third image.

	// Load Fourth image.
	image = stbi_load("Wall.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &wallID);
	glBindTexture(GL_TEXTURE_2D, wallID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End Fourth image.

	// Load 5th image.
	image = stbi_load("Ruin.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &pickUpID);
	glBindTexture(GL_TEXTURE_2D, pickUpID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 5th image.

	// Load 6th image.
	image = stbi_load("Tile_Sandstone.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &wallTopID);
	glBindTexture(GL_TEXTURE_2D, wallTopID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 6th image.

	// Load 7th image.
	image = stbi_load("Tower.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &towerID);
	glBindTexture(GL_TEXTURE_2D, towerID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 7th image.

	// Load 8th image.
	image = stbi_load("Roof.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &roofID);
	glBindTexture(GL_TEXTURE_2D, roofID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 8th image.

	// Load 8th image.
	image = stbi_load("gate.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &gateID);
	glBindTexture(GL_TEXTURE_2D, gateID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End 8th image.

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	// Setting material values.
	glUniform1f(glGetUniformLocation(program, "mat.specularStrength"), mat.specularStrength);
	glUniform1f(glGetUniformLocation(program, "mat.shininess"), mat.shininess);

	// Setting ambient light.
	glUniform3f(glGetUniformLocation(program, "aLight.base.diffuseColour"), aLight.diffuseColour.x, aLight.diffuseColour.y, aLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.base.diffuseStrength"), aLight.diffuseStrength);

	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLights[0].base.diffuseColour"), pLights[0].diffuseColour.x, pLights[0].diffuseColour.y, pLights[0].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[0].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[0].quadratic"), pLights[0].quadratic);

	glUniform3f(glGetUniformLocation(program, "pLights[1].base.diffuseColour"), pLights[1].diffuseColour.x, pLights[1].diffuseColour.y, pLights[1].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].base.diffuseStrength"), pLights[1].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[1].position.x, pLights[1].position.y, pLights[1].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].constant"), pLights[1].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[1].linear"), pLights[1].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[1].quadratic"), pLights[1].quadratic);

	// All VAO/VBO data now in Shape.h! But we still need to do this AFTER OpenGL is initialized.
	g_grid.BufferShape();
	c_Platform.BufferShape();

	// Buffer Hedge Data
	for (int i = 0; i < 39; i++)
	{
		c_Headges[i].BufferShape();
	}

	for (int i = 0; i < 6; i++)
	{
		c_HedgeHouse[i].BufferShape();
	}

	// Buffer Wall Data
	for (int i = 0; i < 5; i++)
	{
		c_Walls[i].BufferShape();
	}

	for (int i = 0; i < 8; i++)
	{
		c_WallToppings[i].BufferShape();
	}

	for (int i = 0; i < 9; i++)
	{
		c_WallCrenells[i].BufferShape();
	}

	for (int i = 0; i < 13; i++)
	{
		c_WallCrenells1[i].BufferShape();
	}

	for (int i = 0; i < 6; i++)
	{
		c_GuardHouse[i].BufferShape();
	}
	
	for (int i = 0; i < 10; i++)
	{
		c_GuardHouseTrimmings[i].BufferShape();
	}

	for (int i = 0; i < 2; i++)
	{
		c_RailingTopper[i].BufferShape();
	}

	// Pick-up
	c_PickUp.BufferShape();

	for (int i = 0; i < 4; i++)
	{
		p_Towers[i].BufferShape();
	}

	for (int i = 0; i < 4; i++)
	{
		c_towerRoof[i].BufferShape();
	}

	for (int i = 0; i < 4; i++)
	{
		p_GuardTowers[i].BufferShape();
	}

	for (int i = 0; i < 4; i++)
	{
		c_guardTowerRoof[i].BufferShape();
	}


	// Enable depth testing and face culling. 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	
	// We must now update the View.
	calculateView();

	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, blankID); // Use this texture for all shapes.
	
	// Grid.
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	g_grid.DrawShape(GL_TRIANGLES);
	//g_grid.DrawShape(GL_LINE_STRIP);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Rising Platform
	glBindTexture(GL_TEXTURE_2D, platformID);

	transformObject(glm::vec3(3.0f, 0.2f, 4.0), X_AXIS, 0.0, glm::vec3(15.0f, 0.0f, -29.0f));
	c_Platform.DrawShape(GL_TRIANGLES);



	glBindTexture(GL_TEXTURE_2D, 0);

	//Headges
	glBindTexture(GL_TEXTURE_2D, hedgeID);


	transformObject(c_Headges[0].getCubePoints(), Y_AXIS, 0.0, glm::vec3(8.0, 0.0f, -33.0f)); //Headge 0
	c_Headges[0].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[1].getCubePoints(), Y_AXIS, 0.0, glm::vec3(8.0, 0.0f, -34.0f)); //Headge 1
	c_Headges[1].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[2].getCubePoints(), X_AXIS, 0.0, glm::vec3(27.0, 0.0f, -34.0f));//Headge 2
	c_Headges[2].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[3].getCubePoints(), Y_AXIS, 0.0, glm::vec3(41.0, 0.0f, -34.0f)); //Headge 3
	c_Headges[3].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[4].getCubePoints(), X_AXIS, 0.0, glm::vec3(9.0, 0.0f, -13.0f)); //Headge 4
	c_Headges[4].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[5].getCubePoints(), X_AXIS, 0.0, glm::vec3(9.0, 0.0f, -28.0f)); //Headge 5
	c_Headges[5].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[6].getCubePoints(), Y_AXIS, 0.0, glm::vec3(11.0, 0.0f, -31.0f)); //Headge 6
	c_Headges[6].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[7].getCubePoints(), X_AXIS, 0.0, glm::vec3(12.0, 0.0f, -31.0f)); //Headge 7
	c_Headges[7].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[8].getCubePoints(), Y_AXIS, 0.0, glm::vec3(20.0, 0.0f, -31.0f)); //Headge 8
	c_Headges[8].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[9].getCubePoints(), X_AXIS, 0.0, glm::vec3(11.0, 0.0f, -23.0f)); //Headge 9
	c_Headges[9].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[10].getCubePoints(), X_AXIS, 0.0, glm::vec3(18.0, 0.0f, -23.0f)); //Headge 10
	c_Headges[10].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[11].getCubePoints(), Y_AXIS, 0.0, glm::vec3(18.0, 0.0f, -22.0f)); //Headge 11
	c_Headges[11].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[12].getCubePoints(), X_AXIS, 0.0, glm::vec3(11.0, 0.0f, -20.0f)); //Headge 12
	c_Headges[12].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[13].getCubePoints(), Y_AXIS, 0.0, glm::vec3(11.0, 0.0f, -17.0f)); //Headge 13
	c_Headges[13].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[14].getCubePoints(), Y_AXIS, 0.0, glm::vec3(14.0, 0.0f, -17.0f)); //Headge 14
	c_Headges[14].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[15].getCubePoints(), Y_AXIS, 0.0, glm::vec3(17.0, 0.0f, -19.0f)); //Headge 15
	c_Headges[15].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[16].getCubePoints(), Y_AXIS, 0.0, glm::vec3(20.0, 0.0f, -17.0f)); //Headge 16
	c_Headges[16].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[17].getCubePoints(), Y_AXIS, 0.0, glm::vec3(23.0, 0.0f, -19.0f)); //Headge 17
	c_Headges[17].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[18].getCubePoints(), X_AXIS, 0.0, glm::vec3(24.0, 0.0f, -14.0f)); //Headge 18
	c_Headges[18].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[19].getCubePoints(), X_AXIS, 0.0, glm::vec3(25.0, 0.0f, -15.0f)); //Headge 19
	c_Headges[19].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[20].getCubePoints(), X_AXIS, 0.0, glm::vec3(26.0, 0.0f, -16.0f)); //Headge 20
	c_Headges[20].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[21].getCubePoints(), Y_AXIS, 0.0, glm::vec3(28.0, 0.0f, -17.0f)); //Headge 21
	c_Headges[21].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[22].getCubePoints(), Y_AXIS, 0.0, glm::vec3(30.0, 0.0f, -15.0f)); //Headge 22
	c_Headges[22].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[23].getCubePoints(), X_AXIS, 0.0, glm::vec3(30.0, 0.0f, -16.0f)); //Headge 23
	c_Headges[23].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[24].getCubePoints(), X_AXIS, 0.0, glm::vec3(28.0, 0.0f, -18.0f)); //Headge 24
	c_Headges[24].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[25].getCubePoints(), Y_AXIS, 0.0, glm::vec3(38.0, 0.0f, -17.0f)); //Headge 25
	c_Headges[25].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[26].getCubePoints(), X_AXIS, 0.0, glm::vec3(34.0, 0.0f, -20.0f)); //Headge 26
	c_Headges[26].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[27].getCubePoints(), X_AXIS, 0.0, glm::vec3(21.0, 0.0f, -20.0f)); //Headge 27
	c_Headges[27].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[28].getCubePoints(), X_AXIS, 0.0, glm::vec3(22.0, 0.0f, -22.0f)); //Headge 28
	c_Headges[28].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[29].getCubePoints(), Y_AXIS, 0.0, glm::vec3(23.0, 0.0f, -30.0f)); //Headge 29
	c_Headges[29].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[30].getCubePoints(), X_AXIS, 0.0, glm::vec3(23.0, 0.0f, -31.0f)); //Headge 30
	c_Headges[30].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[31].getCubePoints(), Y_AXIS, 0.0, glm::vec3(26.0, 0.0f, -30.0f)); //Headge 31
	c_Headges[31].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[32].getCubePoints(), Y_AXIS, 0.0, glm::vec3(29.0, 0.0f, -24.0f)); //Headge 32
	c_Headges[32].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[33].getCubePoints(), Y_AXIS, 0.0, glm::vec3(32.0, 0.0f, -25.0f)); //Headge 33
	c_Headges[33].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[34].getCubePoints(), X_AXIS, 0.0, glm::vec3(29.0, 0.0f, -26.0f)); //Headge 34
	c_Headges[34].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[35].getCubePoints(), Y_AXIS, 0.0, glm::vec3(35.0, 0.0f, -28.0f)); //Headge 35
	c_Headges[35].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[36].getCubePoints(), X_AXIS, 0.0, glm::vec3(29.0, 0.0f, -28.0f)); //Headge 36
	c_Headges[36].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[37].getCubePoints(), X_AXIS, 0.0, glm::vec3(29.0, 0.0f, -31.0f)); //Headge 37
	c_Headges[37].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[38].getCubePoints(), Y_AXIS, 0.0, glm::vec3(38.0, 0.0f, -30.0f)); //Headge 38
	c_Headges[38].DrawShape(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, wallID);

	// Walls
	transformObject(glm::vec3(1.0f, 5.0f, 30.0f), Y_AXIS, 0.0f, glm::vec3(5.0f, 0.0f, -39.0f));
	c_Walls[0].RecolorShape(0.5f, 0.5f, 0.5f);
	c_Walls[0].DrawShape(GL_TRIANGLES);
	transformObject(glm::vec3(18.0f, 5.0f, 1.0f), X_AXIS, 0.0, glm::vec3(5.0f, 0.0f, -40.0f));
	c_Walls[1].RecolorShape(0.5f, 0.5f, 0.5f);
	c_Walls[1].DrawShape(GL_TRIANGLES);
	transformObject(glm::vec3(18.0f, 5.0f, 1.0f), X_AXIS, 0.0, glm::vec3(27.0f, 0.0f, -40.0f));
	c_Walls[2].RecolorShape(0.5f, 0.5f, 0.5f);
	c_Walls[2].DrawShape(GL_TRIANGLES);
	transformObject(glm::vec3(1.0f, 5.0f, 30.0f), X_AXIS, 0.0, glm::vec3(44.0f, 0.0f, -39.0f));
	c_Walls[3].RecolorShape(0.5f, 0.5f, 0.5f);
	c_Walls[3].DrawShape(GL_TRIANGLES);
	transformObject(glm::vec3(40.0f, 5.0f, 1.0f), X_AXIS, 0.0, glm::vec3(5.0f, 0.0f, -10.0f));
	c_Walls[4].RecolorShape(0.5f, 0.5f, 0.5f);
	c_Walls[4].DrawShape(GL_TRIANGLES);

	// Hedge House

	//{ 5.0f, 1.5f, 0.1f},		//Back
	//{ 0.1f, 1.5f, 5.0f },		//Right
	//{ 2.0f, 1.5f, 0.1f },		//Front Right
	//{ 2.0f, 1.5f, 0.1f },		//Front Left
	//{ 0.1f, 1.5f, 5.0f },		//Left
	//{ 5.0f, 0.1f, 5.0f },		//Roof
	glBindTexture(GL_TEXTURE_2D, wallTopID);

	transformObject(c_HedgeHouse[0].getCubePoints(), X_AXIS, 0.0, glm::vec3(14.0f, 0.0f, -29.1f));
	c_HedgeHouse[0].RecolorShape(0.5f, 1.0f, 0.5f);
	c_HedgeHouse[0].DrawShape(GL_TRIANGLES);

	transformObject(c_HedgeHouse[1].getCubePoints(), X_AXIS, 0.0, glm::vec3(19.0f, 0.0f, -29.1f));
	c_HedgeHouse[1].RecolorShape(0.5f, 1.0f, 0.5f);
	c_HedgeHouse[1].DrawShape(GL_TRIANGLES);

	transformObject(c_HedgeHouse[2].getCubePoints(), X_AXIS, 0.0, glm::vec3(17.0f, 0.0f, -24.0f));
	c_HedgeHouse[2].RecolorShape(0.5f, 1.0f, 0.5f);
	c_HedgeHouse[2].DrawShape(GL_TRIANGLES);

	transformObject(c_HedgeHouse[3].getCubePoints(), X_AXIS, 0.0, glm::vec3(14.0f, 0.0f, -24.0f));
	c_HedgeHouse[3].RecolorShape(0.5f, 1.0f, 0.5f);
	c_HedgeHouse[3].DrawShape(GL_TRIANGLES);

	transformObject(c_HedgeHouse[4].getCubePoints(), X_AXIS, 0.0, glm::vec3(13.9f, 0.0f, -29.1f));
	c_HedgeHouse[4].RecolorShape(0.5f, 1.0f, 0.5f);
	c_HedgeHouse[4].DrawShape(GL_TRIANGLES);

	transformObject(c_HedgeHouse[5].getCubePoints(), X_AXIS, 0.0, glm::vec3(13.9f, 1.0f, -29.1f));
	c_HedgeHouse[5].RecolorShape(0.5f, 1.0f, 0.5f);
	c_HedgeHouse[5].DrawShape(GL_TRIANGLES);

	// Wall Toppings


	transformObject(c_WallToppings[0].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f, 5.0f, -40.0f)); // 0
	c_WallToppings[0].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[0].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[1].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.9f, 5.0f, -39.0f)); // 1
	c_WallToppings[1].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[1].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[2].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f, 5.0f, -40.0f)); // 2
	c_WallToppings[2].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[2].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[3].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.9f, 5.0f, -39.1f)); // 3
	c_WallToppings[3].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[3].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[4].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(44.9f, 5.0f, -40.0f)); // 4
	c_WallToppings[4].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[4].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[5].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(44.0f, 5.0f, -39.0f)); // 5
	c_WallToppings[5].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[5].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[6].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f, 5.0f, -9.1f)); // 6
	c_WallToppings[6].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[6].DrawShape(GL_TRIANGLES);
	transformObject(c_WallToppings[7].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.9f, 5.0f, -10.0f)); // 7
	c_WallToppings[7].RecolorShape(0.5f, 0.5f, 0.5f);
	c_WallToppings[7].DrawShape(GL_TRIANGLES);

	// Wall Crenells
	float j = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		transformObject(c_WallCrenells[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f, 5.0f, -40.0f + j ));	// WallTop 0
		c_WallCrenells[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells[i].DrawShape(GL_TRIANGLES);
		j += 3.5f;
	}

	j = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		transformObject(c_WallCrenells[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.9f, 5.0f, -40.0f + j));	// WallTop 1
		c_WallCrenells[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells[i].DrawShape(GL_TRIANGLES);
		j += 3.5f;
	}
	 
	j = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		transformObject(c_WallCrenells[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(44.9f, 5.0f, -40.0f + j));	// WallTop 4
		c_WallCrenells[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells[i].DrawShape(GL_TRIANGLES);
		j += 3.5f;
	}

	j = 0.0f;
	for (int i = 0; i < 9; i++)
	{
		transformObject(c_WallCrenells[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(44.0f, 5.0f, -40.0f + j));	// WallTop 5
		c_WallCrenells[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells[i].DrawShape(GL_TRIANGLES);
		j += 3.5f;
	}

	j = 0.0f;
	for (int i = 0; i < 13; i++)
	{
		transformObject(c_WallCrenells1[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f + j, 5.0f, -40.0f));	// WallTop 2
		c_WallCrenells1[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells1[i].DrawShape(GL_TRIANGLES);
		j += c_WallCrenells1[i].getCubePoints().x + 0.5f;
	}

	j = 0.0f;
	for (int i = 0; i < 13; i++)
	{
		transformObject(c_WallCrenells1[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f + j, 5.0f, -39.1f));	// WallTop 3
		c_WallCrenells1[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells1[i].DrawShape(GL_TRIANGLES);
		j += c_WallCrenells1[i].getCubePoints().x + 0.5f;
	}
	
	j = 0.0f;
	for (int i = 0; i < 13; i++)
	{
		transformObject(c_WallCrenells1[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f + j, 5.0f, -9.1f));	// WallTop 6
		c_WallCrenells1[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells1[i].DrawShape(GL_TRIANGLES);
		j += c_WallCrenells1[i].getCubePoints().x + 0.5f;
	}

	j = 0.0f;
	for (int i = 0; i < 13; i++)
	{
		transformObject(c_WallCrenells1[i].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(5.0f + j, 5.0f, -10.0f));	// WallTop 7
		c_WallCrenells1[i].RecolorShape(0.5f, 0.5f, 0.5f);
		c_WallCrenells1[i].DrawShape(GL_TRIANGLES);
		j += c_WallCrenells1[i].getCubePoints().x + 0.5f;
	}

	// Guardhouse!
	transformObject(c_GuardHouse[0].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(23.0f, 0.0f, -40.0f));	// Guardhouse Size = {4.0f, 5.0f, 3.0f}
	c_GuardHouse[0].RecolorShape(0.54f, 0.6f, 0.35f);
	c_GuardHouse[0].DrawShape(GL_TRIANGLES);														
			
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, gateID);

	transformObject(c_GuardHouse[1].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.5f, 0.0f, -40.1f));	// Gate #1 {1.0f, 2.0f, 0.1f}
	c_GuardHouse[1].DrawShape(GL_TRIANGLES);														
																							
	transformObject(c_GuardHouse[2].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.5f, 0.75f, -36.0f));	// Gate #2
	c_GuardHouse[2].DrawShape(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, wallTopID);

	transformObject(c_GuardHouse[3].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.125f, 0.0f, -36.0f));	// High Step {1.75f, 0.75, 0.3f}
	c_GuardHouse[3].RecolorShape(0.5f, 0.5f, 0.5f);
	c_GuardHouse[3].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouse[4].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.125f, 0.0f, -35.7f));	// Med Step
	c_GuardHouse[4].RecolorShape(0.5f, 0.5f, 0.5f);
	c_GuardHouse[4].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouse[5].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.125f, 0.0f, -35.4f));	// Low Step
	c_GuardHouse[5].RecolorShape(0.5f, 0.5f, 0.5f);
	c_GuardHouse[5].DrawShape(GL_TRIANGLES);

	// Guardhouse Trimmings!

	glBindTexture(GL_TEXTURE_2D, towerID);

	transformObject(c_GuardHouseTrimmings[0].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.4f, 0.0f, -40.2f)); //{0.1f, 2.0f, 0.2f},	// Outside Left 
	c_GuardHouseTrimmings[0].DrawShape(GL_TRIANGLES);
	
	transformObject(c_GuardHouseTrimmings[1].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(25.5f, 0.0f, -40.2f));//{ 0.1f, 2.0f, 0.2f },		// Outside Right
	c_GuardHouseTrimmings[1].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouseTrimmings[2].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.4f, 2.0f, -40.2f)); //{2.2f, 0.1f, 0.2f},	// Outside Left 
	c_GuardHouseTrimmings[2].DrawShape(GL_TRIANGLES);


	transformObject(c_GuardHouseTrimmings[3].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.4f, 0.75f, -36.0f));//{0.1f, 2.0f, 0.2f},		// Inside Left 
	c_GuardHouseTrimmings[3].DrawShape(GL_TRIANGLES);														 
																											 
	transformObject(c_GuardHouseTrimmings[4].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(25.5f, 0.75f, -36.0f));//{0.1f, 2.0f, 0.2f},		// Inside Right
	c_GuardHouseTrimmings[4].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouseTrimmings[5].getCubePoints(), Y_AXIS, 0.0f, glm::vec3(24.4f, 2.75f, -36.0f));//{2.2f, 0.1f, 0.2f},		// Inside Lip
	c_GuardHouseTrimmings[5].DrawShape(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, roofID);

	transformObject(c_GuardHouseTrimmings[6].getCubePoints(), X_AXIS, 30.0f, glm::vec3(24.3f, 1.35f, -36.4f));//{0.1f, 0.1f, 1.345f},		// Rail Left
	c_GuardHouseTrimmings[6].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouseTrimmings[7].getCubePoints(), X_AXIS, 30.0f, glm::vec3(25.6f, 1.35f, -36.4f));//{0.1f, 0.1f, 1.345f},		// Rail Right
	c_GuardHouseTrimmings[7].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouseTrimmings[8].getCubePoints(), X_AXIS, 0.0f, glm::vec3(25.6f, -0.2f, -35.25f));//{0.1f, 0.1f, 1.345f},		// Rail Support Right
	c_GuardHouseTrimmings[8].DrawShape(GL_TRIANGLES);

	transformObject(c_GuardHouseTrimmings[9].getCubePoints(), X_AXIS, 0.0f, glm::vec3(24.3f, -0.2f, -35.25f));//{0.1f, 0.1f, 1.345f},		// Rail Support Left
	c_GuardHouseTrimmings[9].DrawShape(GL_TRIANGLES);

	// Railing Toppers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, hedgeID);
	transformObject({ 0.2f,0.2f,0.2f }, Y_AXIS, 0.0f, glm::vec3(24.25f, 0.8f, -35.3f)); // Rail Cone Left
	c_RailingTopper[0].DrawShape(GL_TRIANGLES);

	transformObject({ 0.2f,0.2f,0.2f }, Y_AXIS, 0.0f, glm::vec3(25.55f, 0.8f, -35.3f)); // Rail Cone Right
	c_RailingTopper[1].DrawShape(GL_TRIANGLES);

	// Pick-Up
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_2D, pickUpID);

	transformObject(glm::vec3(0.5f, 0.5f, 0.5), XYZ_AXIS, angle += 1.0f, glm::vec3(33.2f, 0.6f, -24.5f));
	c_PickUp.DrawShape(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Prism.
	glBindTexture(GL_TEXTURE_2D, towerID);
	transformObject(glm::vec3(4.0f, 8.5f, 4.0f), X_AXIS, 0.0f, glm::vec3(3.5f, 0.0f, -11.5f));
	p_Towers[0].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(4.0f, 8.5f, 4.0f), X_AXIS, 0.0f, glm::vec3(42.5f, 0.0f, -11.5f));
	p_Towers[1].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(4.0f, 8.5f, 4.0f), X_AXIS, 0.0f, glm::vec3(42.5f, 0.0f, -41.5f));
	p_Towers[2].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(4.0f, 8.5f, 4.0f), X_AXIS, 0.0f, glm::vec3(3.5f, 0.0f, -41.5f));
	p_Towers[3].DrawShape(GL_TRIANGLES);


	glBindTexture(GL_TEXTURE_2D, 0);

	// Cone.
	glBindTexture(GL_TEXTURE_2D, roofID);
	transformObject(glm::vec3(6.0f, 4.0f, 6.0f), X_AXIS, 0.0f, glm::vec3(2.5f, 8.5f, -12.5f));
	c_towerRoof[0].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(6.0f, 4.0f, 6.0f), X_AXIS, 0.0f, glm::vec3(41.5f, 8.5f, -12.5f));
	c_towerRoof[1].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(6.0f, 4.0f, 6.0f), X_AXIS, 0.0f, glm::vec3(41.5f, 8.5f, -42.5f));
	c_towerRoof[2].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(6.0f, 4.0f, 6.0f), X_AXIS, 0.0f, glm::vec3(2.5f, 8.5f, -42.5f));
	c_towerRoof[3].DrawShape(GL_TRIANGLES);

	// Guard house Towers
	// Prism.
	glBindTexture(GL_TEXTURE_2D, towerID);
	transformObject(glm::vec3(2.0f, 8.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(22.0f, 0.0f, -40.5f));
	p_GuardTowers[0].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(2.0f, 8.5f, 2.0f), X_AXIS, 0.0f, glm::vec3(26.0f, 0.0f, -40.5f));
	p_GuardTowers[1].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(1.0f, 5.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(22.5f, 0.0f, -36.5f));
	p_GuardTowers[2].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(1.0f, 5.25f, 1.0f), X_AXIS, 0.0f, glm::vec3(26.5f, 0.0f, -36.5f));
	p_GuardTowers[3].DrawShape(GL_TRIANGLES);


	glBindTexture(GL_TEXTURE_2D, 0);

	// Cone.
	glBindTexture(GL_TEXTURE_2D, roofID);
	transformObject(glm::vec3(3.0f, 4.0f, 3.0f), X_AXIS, 0.0f, glm::vec3(21.5f, 8.5f, -41.0f));
	c_guardTowerRoof[0].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(3.0f, 4.0f, 3.0f), X_AXIS, 0.0f, glm::vec3(25.5f, 8.5f, -41.0f));
	c_guardTowerRoof[1].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(1.5f, 2.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(22.25f, 5.25f, -36.75f));
	c_guardTowerRoof[2].DrawShape(GL_TRIANGLES);

	transformObject(glm::vec3(1.5f, 2.0f, 1.5f), X_AXIS, 0.0f, glm::vec3(26.25f, 5.25, -36.75f));
	c_guardTowerRoof[3].DrawShape(GL_TRIANGLES);


	glClearColor(0.63f, 0.89f, 0.72f, 1.0f); // Set Background Color
	
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position += upVec * MOVESPEED;
	if (keys & KEY_DOWN)
		position -= upVec * MOVESPEED;

	/*if (keys & CAM_FORWARD)
		light_position -= upVec * MOVESPEED;
	if (keys & CAM_BACKWARD)
		light_position += upVec * MOVESPEED;
	if (keys & CAM_LEFT)
		light_position -= upVec * MOVESPEED;
	if (keys & CAM_RIGHT)
		light_position += upVec * MOVESPEED;*/
}

void timer(int) { // Tick of the frame.
	// Get first timestamp
	int start = glutGet(GLUT_ELAPSED_TIME);
	// Update call
	parseKeys();
	// Display call
	glutPostRedisplay();
	// Calling next tick
	int end = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc((1000 / FPS) - (end-start), timer, 0);
}

// Keyboard input processing routine.
// Keyboard input processing routine.
void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT;
		break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT;
		break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP;
		break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN;
		break;
	default:
		break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP: // Up arrow.
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case GLUT_KEY_DOWN: // Down arrow.
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 's':
		keys &= ~KEY_BACKWARD;
		break;
	case 'a':
		keys &= ~KEY_LEFT;
		break;
	case 'd':
		keys &= ~KEY_RIGHT;
		break;
	case 'r':
		keys &= ~KEY_UP;
		break;
	case 'f':
		keys &= ~KEY_DOWN;
		break;


	//case 'i':
	//	light_position.z += 1.0f * MOVESPEED;
	//	break;
	//case 'j':
	//	light_position.x += 1.0f * MOVESPEED;
	//	break;				
	//case 'k':				
	//	light_position.z -= 1.0f * MOVESPEED;
	//	break;				
	//case 'l':				
	//	light_position.x -= 1.0f * MOVESPEED;
	//	break;

	/*case 'i':
		keys &= ~CAM_FORWARD;
		break;
	case 'k':
		keys &= ~CAM_BACKWARD;
		break;
	case 'j':
		keys &= ~CAM_LEFT;
		break;
	case 'l':
		keys &= ~CAM_RIGHT;*/
		break;

	case ' ':
		resetView();
		break;
	default:
		break;
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP:
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD
		break;
	case GLUT_KEY_DOWN:
		keys &= ~KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

//---------------------------------------------------------------------
//
// clean
//
void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &blankID);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	//Before we can open a window, theremust be interaction between the windowing systemand OpenGL.In GLUT, this interaction is initiated by the following function call :
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);

	//if you comment out this line, a window is created with a default size
	glutInitWindowSize(980, 980);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("GAME2012_FinalProject");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.

	atexit(clean); // This useful GLUT function calls specified function before exiting program. 
	glutMainLoop();

	return 0;
}