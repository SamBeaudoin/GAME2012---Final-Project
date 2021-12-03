//***************************************************************************
// GAME2012_A4_BeaudoinSamuel.cpp by Samuel Beaudoin (C) 2021 All Rights Reserved.
//
// Student ID: 100883341
// 
// Assignment 4 submission.
//
// Description:
// Click run to see the results. The controls for the light are a little finnicky but most times she co-operates
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
GLint width, height, bitDepth;

// Light Variables
glm::vec3 light_position;

// Light objects. Now OOP.
AmbientLight aLight(
	glm::vec3(1.0f, 1.0f, 1.0f),
	1.0f);

PointLight pLight(
	glm::vec3(5.0f, 1.0f, -4.0f),	// Position.
	50.0f,							// Range.
	1.0f, 4.5f, 75.0f,				// Constant, Linear, Exponent.   
	glm::vec3(1.0f, 0.56f, 0.64f),	// Diffuse colour.
	1.0f);							// Diffuse strength.

Material mat = { 1.0f, 32 }; // Alternate way to construct an object.

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Geometry data.
Grid g_grid(50);
Cube c_Cubes(3.0f, 0.5f, 4.0f);
Cube c_Headges[39] =
{
	//x     y     z
	(1.0,   1.0,  21.0),  //Headge 0
	(15.0,   1.0,  1.0),  //Headge 1
	(14.0,   1.0,  1.0),  //Headge 2
	(1.0,   1.0,  21.0),  //Headge 3
	(33.0,   1.0,  1.0),  //Headge 4
	(2.0,   1.0,  1.0),  //Headge 5
	(1.0,   1.0,  8.0),  //Headge 6
	(8.0,   1.0,  1.0),  //Headge 7
	(1.0,   1.0,  8.0),  //Headge 8
	(5.0,   1.0,  1.0),  //Headge 9
	(3.0,   1.0,  1.0),  //Headge 10
	(1.0,   1.0,  3.0),  //Headge 11
	(7.0,   1.0,  1.0),  //Headge 12
	(1.0,   1.0,  4.0),  //Headge 13
	(1.0,   1.0,  4.0),  //Headge 14
	(1.0,   1.0,  4.0),  //Headge 15
	(1.0,   1.0,  4.0),  //Headge 16
	(1.0,   1.0,  3.0),  //Headge 17
	(1.0,   1.0,  1.0),  //Headge 18
	(1.0,   1.0,  1.0),  //Headge 19
	(1.0,   1.0,  1.0),  //Headge 20
	(1.0,   1.0,  4.0),  //Headge 21
	(1.0,   1.0,  2.0),  //Headge 22
	(8.0,   1.0,  1.0),  //Headge 23
	(9.0,   1.0,  1.0),  //Headge 24
	(1.0,   1.0,  2.0),  //Headge 25
	(5.0,   1.0,  1.0),  //Headge 26
	(11.0,   1.0,  1.0),  //Headge 27
	(6.0,   1.0,  1.0),  //Headge 28
	(1.0,   1.0,  7.0),  //Headge 29
	(4.0,   1.0,  1.0),  //Headge 30
	(1.0,   1.0,  7.0),  //Headge 31
	(1.0,   1.0,  4.0),  //Headge 32
	(1.0,   1.0,  4.0),  //Headge 33
	(5.0,   1.0,  1.0),  //Headge 34
	(1.0,   1.0,  6.0),  //Headge 35
	(6.0,   1.0,  1.0),  //Headge 36
	(10.0,   1.0,  1.0),  //Headge 37
	(1.0,   1.0,  10.0)   //Headge 38

};

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(10.0f, 5.0f, 25.0f); // Super pulled back because of grid size.
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
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
	image = stbi_load("Platform_02.png", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &platformID);
	glBindTexture(GL_TEXTURE_2D, platformID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
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

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	// Setting material values.
	glUniform1f(glGetUniformLocation(program, "mat.specularStrength"), mat.specularStrength);
	glUniform1f(glGetUniformLocation(program, "mat.shininess"), mat.shininess);

	// Setting ambient light.
	glUniform3f(glGetUniformLocation(program, "aLight.base.diffuseColour"), aLight.diffuseColour.x, aLight.diffuseColour.y, aLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.base.diffuseStrength"), aLight.diffuseStrength);

	// Setting point light.
	glUniform3f(glGetUniformLocation(program, "pLight.base.diffuseColour"), pLight.diffuseColour.x, pLight.diffuseColour.y, pLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLight.base.diffuseStrength"), pLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "pLight.position"), pLight.position.x, pLight.position.y, pLight.position.z);
	glUniform1f(glGetUniformLocation(program, "pLight.constant"), pLight.constant);
	glUniform1f(glGetUniformLocation(program, "pLight.linear"), pLight.linear);
	glUniform1f(glGetUniformLocation(program, "pLight.exponent"), pLight.exponent);

	// All VAO/VBO data now in Shape.h! But we still need to do this AFTER OpenGL is initialized.
	g_grid.BufferShape();
	c_Cubes.BufferShape();
	for (int i = 0; i < 39; i++)
	{
		c_Headges[i].BufferShape();
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

	pLight.position += light_position;
	glUniform3f(glGetUniformLocation(program, "pLight.position"), pLight.position.x, pLight.position.y, pLight.position.z);

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

	transformObject(glm::vec3(3.0f, 0.5f, 4.0), X_AXIS, 0.0, glm::vec3(15.0f, 0.0f, -26.0f));
	c_Cubes.DrawShape(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Headges
	glBindTexture(GL_TEXTURE_2D, hedgeID);
	
	transformObject(c_Headges[0].getCubePoints(), Y_AXIS, 90.0, glm::vec3(8.0, 0.0f, -13.0f)); //Headge 0
	c_Headges[0].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[1].getCubePoints(), X_AXIS, 0.0, glm::vec3(8.0, 0.0f, -34.0f)); //Headge 1
	c_Headges[1].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[2].getCubePoints(), X_AXIS, 0.0, glm::vec3(27.0, 0.0f, -34.0f));//Headge 2
	c_Headges[2].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[3].getCubePoints(), Y_AXIS, 90.0, glm::vec3(41.0, 0.0f, -14.0f)); //Headge 3
	c_Headges[3].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[4].getCubePoints(), X_AXIS, 0.0, glm::vec3(9.0, 0.0f, -13.0f)); //Headge 4
	c_Headges[4].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[5].getCubePoints(), X_AXIS, 0.0, glm::vec3(9.0, 0.0f, -28.0f)); //Headge 5
	c_Headges[5].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[6].getCubePoints(), Y_AXIS, 90.0, glm::vec3(11.0, 0.0f, -24.0f)); //Headge 6
	c_Headges[6].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[7].getCubePoints(), X_AXIS, 0.0, glm::vec3(12.0, 0.0f, -31.0f)); //Headge 7
	c_Headges[7].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[8].getCubePoints(), Y_AXIS, 90.0, glm::vec3(20.0, 0.0f, -24.0f)); //Headge 8
	c_Headges[8].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[9].getCubePoints(), X_AXIS, 0.0, glm::vec3(11.0, 0.0f, -23.0f)); //Headge 9
	c_Headges[9].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[10].getCubePoints(), X_AXIS, 0.0, glm::vec3(13.0, 0.0f, -23.0f)); //Headge 10
	c_Headges[10].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[11].getCubePoints(), Y_AXIS, 90.0, glm::vec3(13.0, 0.0f, -20.0f)); //Headge 11
	c_Headges[11].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[12].getCubePoints(), X_AXIS, 0.0, glm::vec3(11.0, 0.0f, -20.0f)); //Headge 12
	c_Headges[12].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[13].getCubePoints(), Y_AXIS, 90.0, glm::vec3(11.0, 0.0f, -14.0f)); //Headge 13
	c_Headges[13].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[14].getCubePoints(), Y_AXIS, 90.0, glm::vec3(8.0, 0.0f, -14.0f)); //Headge 14
	c_Headges[14].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[15].getCubePoints(), Y_AXIS, 90.0, glm::vec3(17.0, 0.0f, -16.0f)); //Headge 15
	c_Headges[15].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[16].getCubePoints(), Y_AXIS, 90.0, glm::vec3(20.0, 0.0f, -14.0f)); //Headge 16
	c_Headges[16].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[17].getCubePoints(), Y_AXIS, 90.0, glm::vec3(23.0, 0.0f, -17.0f)); //Headge 17
	c_Headges[17].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[18].getCubePoints(), X_AXIS, 0.0, glm::vec3(24.0, 0.0f, -14.0f)); //Headge 18
	c_Headges[18].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[19].getCubePoints(), X_AXIS, 0.0, glm::vec3(25.0, 0.0f, -15.0f)); //Headge 19
	c_Headges[19].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[20].getCubePoints(), X_AXIS, 0.0, glm::vec3(26.0, 0.0f, -16.0f)); //Headge 20
	c_Headges[20].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[21].getCubePoints(), Y_AXIS, 90.0, glm::vec3(28.0, 0.0f, -14.0f)); //Headge 21
	c_Headges[21].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[22].getCubePoints(), Y_AXIS, 90.0, glm::vec3(30.0, 0.0f, -14.0f)); //Headge 22
	c_Headges[22].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[23].getCubePoints(), X_AXIS, 0.0, glm::vec3(30.0, 0.0f, -16.0f)); //Headge 23
	c_Headges[23].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[24].getCubePoints(), X_AXIS, 0.0, glm::vec3(28.0, 0.0f, -18.0f)); //Headge 24
	c_Headges[24].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[25].getCubePoints(), Y_AXIS, 90.0, glm::vec3(38.0, 0.0f, -16.0f)); //Headge 25
	c_Headges[25].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[26].getCubePoints(), X_AXIS, 0.0, glm::vec3(34.0, 0.0f, -20.0f)); //Headge 26
	c_Headges[26].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[27].getCubePoints(), X_AXIS, 0.0, glm::vec3(21.0, 0.0f, -20.0f)); //Headge 27
	c_Headges[27].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[28].getCubePoints(), X_AXIS, 0.0, glm::vec3(22.0, 0.0f, -22.0f)); //Headge 28
	c_Headges[28].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[29].getCubePoints(), Y_AXIS, 90.0, glm::vec3(23.0, 0.0f, -24.0f)); //Headge 29
	c_Headges[29].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[30].getCubePoints(), X_AXIS, 0.0, glm::vec3(23.0, 0.0f, -31.0f)); //Headge 30
	c_Headges[30].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[31].getCubePoints(), Y_AXIS, 90.0, glm::vec3(26.0, 0.0f, -24.0f)); //Headge 31
	c_Headges[31].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[32].getCubePoints(), Y_AXIS, 90.0, glm::vec3(29.0, 0.0f, -21.0f)); //Headge 32
	c_Headges[32].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[33].getCubePoints(), Y_AXIS, 90.0, glm::vec3(32.0, 0.0f, -22.0f)); //Headge 33
	c_Headges[33].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[34].getCubePoints(), X_AXIS, 0.0, glm::vec3(29.0, 0.0f, -26.0f)); //Headge 34
	c_Headges[34].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[35].getCubePoints(), Y_AXIS, 90.0, glm::vec3(35.0, 0.0f, -23.0f)); //Headge 35
	c_Headges[35].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[36].getCubePoints(), X_AXIS, 0.0, glm::vec3(29.0, 0.0f, -28.0f)); //Headge 36
	c_Headges[36].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[37].getCubePoints(), X_AXIS, 0.0, glm::vec3(29.0, 0.0f, -31.0f)); //Headge 37
	c_Headges[37].DrawShape(GL_TRIANGLES);
	transformObject(c_Headges[38].getCubePoints(), Y_AXIS, 90.0, glm::vec3(38.0, 0.0f, -21.0f)); //Headge 38
	c_Headges[38].DrawShape(GL_TRIANGLES);


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
		// Camera controls
	case 'i':
		light_position.z -= 1.0f * MOVESPEED;
		break;
	case 'j':
		light_position.x -= 1.0f * MOVESPEED;
		break;
	case 'k':
		light_position.z += 1.0f * MOVESPEED;
		break;
	case 'l':
		light_position.x += 1.0f * MOVESPEED;
		break;
	/*case 'i':
		if (!(keys & CAM_FORWARD))
			keys |= CAM_FORWARD;
		break;
	case 'j':
		if (!(keys & CAM_LEFT))
			keys |= CAM_LEFT;
		break;
	case 'k':
		if (!(keys & CAM_BACKWARD))
			keys |= CAM_BACKWARD;
		break;
	case 'l':
		if (!(keys & CAM_RIGHT))
			keys |= CAM_RIGHT;
		break;*/
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


	case 'i':
		light_position.z += 1.0f * MOVESPEED;
		break;
	case 'j':
		light_position.x += 1.0f * MOVESPEED;
		break;				
	case 'k':				
		light_position.z -= 1.0f * MOVESPEED;
		break;				
	case 'l':				
		light_position.x -= 1.0f * MOVESPEED;
		break;

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

	glutCreateWindow("GAME2012_A4_BeaudoinSamuel");

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