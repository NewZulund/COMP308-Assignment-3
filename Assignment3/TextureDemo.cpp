/*
 * Copyright (c) 1993-2003, Silicon Graphics, Inc.
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright
 * notice and this permission notice appear in supporting documentation,
 * and that the name of Silicon Graphics, Inc. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS" AND
 * WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION, LOSS OF
 * PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD
 * PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor clauses
 * in the FAR or the DOD or NASA FAR Supplement.  Unpublished - rights
 * reserved under the copyright laws of the United States.
 *
 * Contractor/manufacturer is:
 *	Silicon Graphics, Inc.
 *	1500 Crittenden Lane
 *	Mountain View, CA  94043
 *	United State of America
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include "G308_ImageLoader.h"
#include "G308_Geometry.h"
#include "string.h"
#include "define.h"
#include "textfile.cpp"

void initShaders();
void setCamera();
void setLight();
void draw2D();
void loadObjects();
void drawObjects();
void drawText(char * string, float x, float y);
void drawSpotlight(float * spotDirection, float cutoff);

void keyboard(unsigned char key, int x, int y);
void keyboardSpecialCall(int key, int x, int y);

void display(void);
void reshape(int w, int h);

void createCubemap();
void createCubeMapModel();
void drawEnvironmentMap();

G308_Geometry * table;
G308_Geometry * sphere;
G308_Geometry * torus;
G308_Geometry * cube;
G308_Geometry * teapot;
G308_Geometry * bunny;
G308_Geometry * box;

int lockLights = FALSE;

float zoom = 100;
float xRot = 0;
float yRot = 0;
float zRot = 0;
int modelXRotation = 0;

float spotXRot = 0;
float spotYRot = 0;
float spotZRot = 0;
float spotCutOff = 7.0f;

GLuint v, f,sv,sf, f2, phongProg, skyProg, cubeMapTex;
GLuint * tex_cube = NULL;

GLuint vbo;
GLuint vao;

void initShaders() {
	char * vs = NULL;
	char * fs = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shaders/phongVert.vert");
	fs = textFileRead("shaders/phongFrag.frag");

	const char * ff = fs;
	const char * vv = vs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShader(v);
	glCompileShader(f);

	phongProg = glCreateProgram();

	glAttachShader(phongProg, f);
	glAttachShader(phongProg, v);

	glLinkProgram(phongProg);
	glUseProgram(phongProg);


	//Skybox prog
	vs = NULL;
	fs = NULL;

	sv = glCreateShader(GL_VERTEX_SHADER);
	sf = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("shaders/skymap.vert");
	fs = textFileRead("shaders/skymap.frag");

	ff = fs;
	vv = vs;

	glShaderSource(sv, 1, &vv, NULL);
	glShaderSource(sf, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShader(sv);
	glCompileShader(sf);

	skyProg = glCreateProgram();

	glAttachShader(skyProg, sf);
	glAttachShader(skyProg, sv);

	glLinkProgram(skyProg);

}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//If we're using alpha, we need to do this
	glLoadIdentity();

	setCamera();
	drawEnvironmentMap();

	glPushMatrix();
	setLight();


	//3D
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glShadeModel(GL_SMOOTH);
	drawObjects();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	draw2D();
	glEnable(GL_LIGHTING);

	glFlush();

}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -3.6);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'd':
		spotYRot += 5;
		break;
	case 'a':
		spotYRot -= 5;
		break;
	case 'w':
		spotXRot -= 5;
		break;
	case 's':
		spotXRot += 5;
		break;
	case 'q':
		spotCutOff -= 1;
		break;
	case 'e':
		spotCutOff += 1;
		break;
	case 't':
		modelXRotation -= 5;
		modelXRotation = modelXRotation % 360;
		break;
	case 'l':
		lockLights == TRUE ? lockLights = FALSE : lockLights = TRUE;
		break;
	case ',':
		zoom += 5;
		break;
	case '.':
		zoom -= 5;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void drawObjects() {

	glPushMatrix();
	glColor3f(0, 0, 0);
	glRotatef(modelXRotation, 1, 0, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(0, 0, 0);
	glColor3f(1, 1, 1);
	table->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(0, 0, 0);
	glColor3f(1, 1, 0.8f);
	bunny->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(-4, 2, 4);
	glColor3ub(205, 127, 50);
	sphere->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(-4, 0, -4);
	glColor3f(0.8, 0.8, 1);
	teapot->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(4.0f, 1.0f, 4.0f);
	glColor3f(1, 0, 0);
	torus->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(4.0f, 2.0f, -4.0f);
	glColor3f(1, 1, 1);
	box->RenderGeometry();
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glPopMatrix();
}

void createCubemap() {

	tex_cube = (GLuint*) malloc(sizeof(GLuint) * 8);

	TextureInfo front;
	loadTextureFromJPEG("cubemap/front.jpg", &front);
	TextureInfo back;
	loadTextureFromJPEG("cubemap/back.jpg", &back);
	TextureInfo left;
	loadTextureFromJPEG("cubemap/left.jpg", &left);
	TextureInfo right;
	loadTextureFromJPEG("cubemap/right.jpg", &right);
	TextureInfo top;
	loadTextureFromJPEG("cubemap/top.jpg", &top);
	TextureInfo bottom;
	loadTextureFromJPEG("cubemap/bottom.jpg", &bottom);

	/*TextureInfo front;
	loadTextureFromJPEG("cubemap/humus/heroessquare/poz.jpg", &front);
	TextureInfo back;
	loadTextureFromJPEG("cubemap/humus/heroessquare/negz.jpg", &back);
	TextureInfo left;
	loadTextureFromJPEG("cubemap/humus/heroessquare/negx.jpg", &left);
	TextureInfo right;
	loadTextureFromJPEG("cubemap/humus/heroessquare/posx.jpg", &right);
	TextureInfo top;
	loadTextureFromJPEG("cubemap/humus/heroessquare/negy.jpg", &top);
	TextureInfo bottom;
	loadTextureFromJPEG("cubemap/humus/heroessquare/posy.jpg", &bottom);	 */

	glEnable(GL_TEXTURE_CUBE_MAP);
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, tex_cube);
	printf("Swag \n");
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_cube);
	printf("Swag 2 \n");

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, 512, 512, 0,
			GL_RGB,
			GL_UNSIGNED_BYTE, right.textureData);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_cube);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, 512, 512, 0,
			GL_RGB,
			GL_UNSIGNED_BYTE, left.textureData);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_cube);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, 512, 512, 0,
			GL_RGB,
			GL_UNSIGNED_BYTE, bottom.textureData);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_cube);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, 512, 512, 0,
			GL_RGB,
			GL_UNSIGNED_BYTE, top.textureData);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_cube);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, 512, 512, 0,
			GL_RGB,
			GL_UNSIGNED_BYTE, front.textureData);
	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex_cube);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, 512, 512, 0,
			GL_RGB,
			GL_UNSIGNED_BYTE, back.textureData);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//cubeMapTex = glGetUniformLocation(phongProg, "cubeMap");
	//glUniform1i(cubeMapTex, *tex_cube);

	glDisable(GL_TEXTURE_CUBE_MAP);

	createCubeMapModel();

}

void loadObjects() {

	//Load Cubemap
	createCubemap();

	table = new G308_Geometry("table");
	bunny = new G308_Geometry("bunny");
	sphere = new G308_Geometry("sphere");
	teapot = new G308_Geometry("teapot");
	torus = new G308_Geometry("torus");
	box = new G308_Geometry("box");

	table->ReadOBJ("Table.obj");
	table->ReadTexture("wood.jpg");
	table->CreateGLPolyGeometry();

	bunny->ReadOBJ("Bunny.obj");
	bunny->ReadTexture("white.jpg");
	bunny->CreateGLPolyGeometry();

	sphere->ReadOBJ("Sphere.obj");
	sphere->ReadTexture("white.jpg");
	sphere->CreateGLPolyGeometry();

	teapot->ReadOBJ("Teapot.obj");
	teapot->ReadTexture("white.jpg");
	teapot->CreateGLPolyGeometry();

	torus->ReadOBJ("Torus.obj");
	torus->ReadTexture("white.jpg");
	torus->CreateGLPolyGeometry();

	box->ReadOBJ("Box.obj");
	box->ReadTexture("brick.jpg");
	box->CreateGLPolyGeometry();

}
void setCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY,
			(double) G308_WIN_WIDTH / (double) G308_WIN_HEIGHT, G308_ZNEAR_3D,
			G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 3.0, 10.0 + zoom, 0, 0, 0, 0.0, 1.0, 0.0);

	glRotatef(zRot, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glRotatef(xRot, 0, 0, 1);


	//TODO add proper perspective to skybox.
	glUseProgram(skyProg);

	GLfloat projMatrix[16];
	GLfloat modelMatrix[16];
	glGetFloatv(GL_PROJECTION, projMatrix);
	glGetFloatv(GL_MODELVIEW, modelMatrix);

	GLdouble cam = glGetUniformLocation(phongProg, "CameraPosition");
	glUniform3f(cam, modelMatrix[0], modelMatrix[1],modelMatrix[2]);

	glUseProgram(phongProg);
}

// Set Light
void setLight() {
	glPushMatrix();

	if (lockLights == TRUE) {
		glLoadIdentity();
	}

	//Spotlight
	glPushMatrix();
	glRotatef(spotYRot, 0, 1, 0);
	glRotatef(spotZRot, 0, 0, 1);
	glRotatef(spotXRot, 1, 0, 0);

	glTranslatef(0.0, 5.0, 0.1);

	float spotdirection[] = { 0.0f, -1.0f, 0.0f, 0.0f };
	float spotposition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spotdiffuse[] = { 0.10f, 0.10f, 0.10f, 1.0f };
	float spotambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float spotspecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
	const float expo = 2.0f;

	glLightfv(GL_LIGHT0, GL_POSITION, spotposition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotdirection);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, &expo);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutOff);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spotdiffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, spotambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spotspecular);
	glEnable(GL_LIGHT0);

	drawSpotlight(spotdirection, spotCutOff);

	glPopMatrix();

	//Point light
	glPushMatrix();
	glTranslatef(-1, 1.5, 3);
	glutSolidSphere(0.1f, 10, 10);
	float pointdiffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float pointambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float pointspecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT1, GL_POSITION, spotposition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, pointspecular);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, pointdiffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, pointambient);

	glEnable(GL_LIGHT1);
	glPopMatrix();

	//TODO make directional
	//Directional light
	glPushMatrix();
	float directdirection[] = { 0.0f, -1.0f, 0.0f, 0.0f };
	float directdiffuse[] = { 0.0, 0.0, 0.0, 1.0f };
	float directambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float directspecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT2, GL_POSITION, directdirection);
	glLightfv(GL_LIGHT2, GL_SPECULAR, directspecular);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, directdiffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, directambient);

	glEnable(GL_LIGHT2);
	glPopMatrix();

	//Ambient light
	glPushMatrix();
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
	glEnable(GL_LIGHT3);
	glPopMatrix();

	glPopMatrix();

}

void drawSpotlight(float * spotDirection, float cutoff) {
	glColor3f(1, 0, 0);
	glutSolidSphere(0.15f, 10, 10);
	glPushMatrix();

	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();

	//Calculate end Width from cutoff
	float length = 1;

	cutoff = cutoff * M_PI / 180; //Convert to degrees
	float diameter = tan(cutoff) * length;

	glRotatef(90, 1, 0, 0);
	gluCylinder(quadratic, 0.01f, diameter, length, 32, 32);
	glPopMatrix();

}

void draw2D() {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	gluOrtho2D(0, G308_WIN_WIDTH, 0, G308_WIN_HEIGHT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);

	//Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Text Over UI
	drawText("WASD : Spotlight Controls | A/E Cutoff", -0.8f, 0.9f);
	drawText("ARROW KEYS : Camera Controls | t Rotate models", -0.8f, 0.85f);
	drawText("Press , or . to zoom in and out", -0.8f, 0.75f);
	drawText("Mouse menu for animation control. ", -0.8f, 0.7f);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void drawText(char * words, float x, float y) {

	glPushMatrix();
	glLoadIdentity();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	glDisable( GL_DEPTH_TEST);
	glRasterPos2f(x, y);

	char buf[300];
	sprintf(buf, words);

	char c = words[0];
	int i = 0;
	while (c != '\0') {

		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
		i++;
		c = words[i];
	}

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable( GL_DEPTH_TEST);
	glPopMatrix();
}

void keyboardSpecialCall(int key, int x, int y) {

	int rotation = 5;
	if (key == GLUT_KEY_UP) {
		zRot = zRot + rotation;
	}
	if (key == GLUT_KEY_DOWN) {
		zRot = zRot - rotation;
	}
	if (key == GLUT_KEY_LEFT) {
		yRot = yRot + rotation;
	}
	if (key == GLUT_KEY_RIGHT) {
		yRot = yRot - rotation;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	//Note the addition of GLUT_ALPHA to the display mode flags
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(G308_WIN_WIDTH, G308_WIN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Shader Project");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecialCall);

	GLenum err = glewInit();

	if (err != GLEW_OK) {
		printf("Glew ERROR \n");
		return 0;
	}

	initShaders();
	loadObjects();

	display();
	glutMainLoop();
	return 0;
}

/*
 * Code from antongerdelan.net
 */
void createCubeMapModel() {
	float points[] = { -10.0f, 10.0f, -10.0f, -10.0f, -10.0f, -10.0f, 10.0f,
			-10.0f, -10.0f, 10.0f, -10.0f, -10.0f, 10.0f, 10.0f, -10.0f, -10.0f,
			10.0f, -10.0f,

			-10.0f, -10.0f, 10.0f, -10.0f, -10.0f, -10.0f, -10.0f, 10.0f,
			-10.0f, -10.0f, 10.0f, -10.0f, -10.0f, 10.0f, 10.0f, -10.0f, -10.0f,
			10.0f,

			10.0f, -10.0f, -10.0f, 10.0f, -10.0f, 10.0f, 10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f, 10.0f, 10.0f, -10.0f, 10.0f, -10.0f, -10.0f,

			-10.0f, -10.0f, 10.0f, -10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f, 10.0f, -10.0f, 10.0f, -10.0f, -10.0f, 10.0f,

			-10.0f, 10.0f, -10.0f, 10.0f, 10.0f, -10.0f, 10.0f, 10.0f, 10.0f,
			10.0f, 10.0f, 10.0f, -10.0f, 10.0f, 10.0f, -10.0f, 10.0f, -10.0f,

			-10.0f, -10.0f, -10.0f, -10.0f, -10.0f, 10.0f, 10.0f, -10.0f,
			-10.0f, 10.0f, -10.0f, -10.0f, -10.0f, -10.0f, 10.0f, 10.0f, -10.0f,
			10.0f };

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points,
	GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

}

void drawEnvironmentMap() {
	glDepthMask (GL_FALSE);
	glUseProgram (skyProg);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_CUBE_MAP, *tex_cube);
	glBindVertexArray (vao);
	glDrawArrays (GL_TRIANGLES, 0, 36);
	glDepthMask (GL_TRUE);
	glUseProgram (phongProg);
}

