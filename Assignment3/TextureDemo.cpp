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

/*
 * This is a modified version of "checker.c" from the OpenGL Redbook.
 *
 * It simply loads a texture (given via the command line) and displays it on two quads.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "G308_ImageLoader.h"
#include "G308_Geometry.h"
#include "string.h"
#include "define.h"

static GLuint texName;
static TextureInfo t;

G308_Geometry * table;
G308_Geometry * sphere;
G308_Geometry * torus;
G308_Geometry * cube;
G308_Geometry * teapot;
G308_Geometry * bunny;

float zoom = 1;
float xRot = 0;
float yRot = 0;
float zRot = 0;

void loadObjects();
void drawObjects();
void setCamera();

void init(char* filename) {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	unsigned int i;
	for (i = 0; i < strlen(filename); i++) {
		if (filename[i] == '.') {
			break;
		}
	}
	char extension[5];
	strcpy(extension, &filename[i + 1]);
	//printf(extension);
	if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0)
		loadTextureFromJPEG(filename, &t);
	else if (strcmp(extension, "png") == 0)
		//Do nothing
		1+1;
	else {
		printf("Invalid format. Only supports JPEG and PNG.\n");
		exit(1);
	}

	//Init the texture storage, and set some parameters.
	//(I high recommend reading up on these commands)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Only useful for PNG files, since JPEG doesn't support alpha
	if (t.hasAlpha) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, t.textureData);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t.width, t.height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, t.textureData);
	}
	//Once the texture has been loaded by GL, we don't need this anymore.
	free(t.textureData);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//If we're using alpha, we need to do this

	setCamera();

	glColor3f(0,0,0);
	if (t.hasAlpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA);
	}
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texName);


	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-2.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-2.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(2.41421, 1.0, -1.41421);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(2.41421, -1.0, -1.41421);
	glEnd();
	glFlush();
	if (t.hasAlpha) {
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA);
	}
	glDisable(GL_TEXTURE_2D);



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
	case '.':
		yRot+= 5;
		break;
	case ',':
		yRot-= 5;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	if (argc != 2) {
		printf("Usage: TextureDemo [texturefile]\n");
		exit(1);
	}
	//Note the addition of GLUT_ALPHA to the display mode flags
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(G308_WIN_WIDTH, G308_WIN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init(argv[1]);
	loadObjects();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
void drawObjects(){
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslatef(0,0,-10);
	printf("Drawing Objects \n");


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(1.0f,0.0f,0.0f); /* set object color as red */
		table->RenderGeometry();
		bunny->RenderGeometry();
		sphere->RenderGeometry();
		teapot->RenderGeometry();
		torus->RenderGeometry();


//		glBegin(GL_TRIANGLES);
//			glColor3f(1,1,1);
//			glVertex3f(-2.0, -1.0, -1.0);
//			glVertex3f(-2.0, 1.0, -1.0);
//			glVertex3f(-0.0, 1.0, -1.0);
//		glEnd();

	glutSolidTeapot(1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glPopMatrix();



}

void loadObjects(){
	printf("Reached Creating objects \n");
	table = new G308_Geometry();
	bunny = new G308_Geometry();
	sphere = new G308_Geometry();
	teapot = new G308_Geometry();
	torus = new G308_Geometry();

	printf("Reached loading objects \n");
	table->ReadOBJ("Table.obj");
	//table->ReadTexture("wood.jpg");
	bunny->ReadOBJ("Bunny.obj");
	sphere->ReadOBJ("Sphere.obj");
	teapot->ReadOBJ("Teapot.obj");
	torus->ReadOBJ("Torus.obj");
}
void setCamera(){
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(G308_FOVY, (double) G308_WIN_WIDTH / (double) G308_WIN_HEIGHT, G308_ZNEAR_3D, G308_ZFAR_3D);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(0.0, 0.0, 7.0 + zoom, 0, 0, 0, 0.0, 1.0, 0.0);

		glRotatef(xRot,1,0,0);
		glRotatef(yRot,0,1,0);
		glRotatef(zRot,0,0,1);
}
