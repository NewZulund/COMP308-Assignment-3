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

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "G308_ImageLoader.h"
#include "G308_Geometry.h"
#include "string.h"
#include "define.h"

G308_Geometry * table;
G308_Geometry * sphere;
G308_Geometry * torus;
G308_Geometry * cube;
G308_Geometry * teapot;
G308_Geometry * bunny;
G308_Geometry * box;

int lockLights = FALSE;

float zoom = 1;
float xRot = 0;
float yRot = 0;
float zRot = 0;

float spotXRot = 0;
float spotYRot = 0;
float spotZRot = 0;
float spotCutOff  = 7.0f;

int modelXRotation = 0;

void loadObjects();
void drawObjects();
void setCamera();
void setLight();
void drawText(char * string, float x, float y);
void keyboardSpecialCall(int key, int x, int y);
void drawSpotlight(float * spotDirection, float cutoff);
void draw2D();

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//If we're using alpha, we need to do this

	setCamera();
	draw2D();

	glPushMatrix();
	setLight();

	//3D
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glShadeModel(GL_SMOOTH);
	drawObjects();
	glPopMatrix();
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
		modelXRotation += 5;
		modelXRotation = modelXRotation % 360;
		break;
	case 'l':
		lockLights == TRUE ? lockLights = FALSE : lockLights = TRUE;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	//Note the addition of GLUT_ALPHA to the display mode flags
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(G308_WIN_WIDTH, G308_WIN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	loadObjects();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecialCall);

	display();
	glutMainLoop();
	return 0;
}
void drawObjects() {
	glPushMatrix();
	glColor3f(0, 0, 0);
	glRotatef(modelXRotation,1,0,0);

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
	glColor3f(1, 1, 0);
	sphere->RenderGeometry();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 0.2f, 0.2f);
	glTranslatef(-4, 0, -4);
	glColor3f(0, 0, 1);
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

void loadObjects() {
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
	bunny->CreateGLPolyGeometry();

	sphere->ReadOBJ("Sphere.obj");
	sphere->CreateGLPolyGeometry();

	teapot->ReadOBJ("Teapot.obj");
	teapot->CreateGLPolyGeometry();

	torus->ReadOBJ("Torus.obj");
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
}

// Set Light
void setLight() {
	glPushMatrix();

	if(lockLights == TRUE){
		glLoadIdentity();
	}

	//Spotlight
	glPushMatrix();
	glRotatef(spotYRot, 0, 1, 0);
	glRotatef(spotZRot, 0, 0, 1);
	glRotatef(spotXRot, 1, 0, 0);

	glTranslatef(0.0,5.0,0.1);

	float spotdirection[] = { 0.0f, -1.0f, 0.0f, 0.0f };
	//float spotposition[] = { 0.0f, 5.0f, 0.1f, 1.0f };
	float spotposition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spotdiffintensity[] = { 0.10f, 0.10f, 0.10f, 1.0f };
	float spotambient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float spotspecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
	const float expo = 2.0f;

	glLightfv(GL_LIGHT0, GL_POSITION, spotposition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotdirection);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, &expo);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutOff);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spotdiffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, spotambient);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, spotambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spotspecular);
	glEnable(GL_LIGHT0);

	drawSpotlight(spotdirection, spotCutOff);

	glPopMatrix();

	//Point light
	glPushMatrix();
	glTranslatef(-1, 3, 2);
	float pointdiffintensity[] = { 0.12f, 0.12f, 0.12f, 1.0f };
	float pointambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float pointspecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, pointdiffintensity);
	glLightfv(GL_LIGHT1, GL_AMBIENT, pointambient);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1.0);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5);
	glEnable(GL_LIGHT1);
	glPopMatrix();

	//Ambient light
	glPushMatrix();
	glTranslatef(-1, 10, 2);
	float ambient[] = { 0.1f, 0.1f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glEnable(GL_LIGHT2);
	glPopMatrix();

	glPopMatrix();

}

void drawSpotlight(float * spotDirection, float cutoff){
	glColor3f(1,0,0);
	glutSolidSphere(0.15f, 10,10);
	glPushMatrix();

	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();

	//Calculate end Width from cutoff
	float length = 1;

	cutoff = cutoff * M_PI / 180; //Convert to degrees
	float diameter = tan(cutoff) * length;

	glRotatef(90, 1,0,0);
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

	gluOrtho2D(0, G308_WIN_WIDTH, 0, G308_WIN_HEIGHT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	//drawPoints();
	glPopMatrix();

	//Text Over UI
	drawText("WASD : Spotlight Controls", -0.8f, 0.9f);
	drawText("ARROW KEYS : Camera Controls", -0.8f, 0.85f);
	drawText("Press , or . to zoom in and out", -0.8f, 0.75f);
	drawText("Mouse menu for animation control. ", -0.8f, 0.7f);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

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
	if(key == GLUT_KEY_UP){
		zRot = zRot + rotation;
	}
	if(key == GLUT_KEY_DOWN){
			zRot = zRot - rotation;
		}
	if(key == GLUT_KEY_LEFT){
		yRot = yRot + rotation;
	}
	if(key == GLUT_KEY_RIGHT){
		yRot = yRot - rotation;
	}

	glutPostRedisplay();
}


