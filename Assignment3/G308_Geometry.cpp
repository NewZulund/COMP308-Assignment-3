//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include "G308_Geometry.h"
#include "G308_ImageLoader.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

int material;

G308_Geometry::G308_Geometry(char name[]) {
	m_pVertexArray = NULL;
	m_pNormalArray = NULL;
	m_pUVArray = NULL;
	m_pTriangles = NULL;

	mode = G308_SHADE_POLYGON;

	material = 0;
	texName = NULL;
	strcpy(fn, name);

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_glGeomListPoly = m_glGeomListWire = 0;
}

G308_Geometry::~G308_Geometry(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	free(&t);
}

//-------------------------------------------------------
// Read in the OBJ (Note: fails quietly, so take care)
//--------------------------------------------------------
void G308_Geometry::ReadOBJ(const char *filename) {
	char file[50];
	strncpy(file, filename, 51);

	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int numVert, numNorm, numUV, numFace;
	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;
	m_nNumNormal = numNorm;

	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumNormal];

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle[m_nNumPolygon];

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
					{
				sscanf(str, "vt %f %f", &u, &v);
				m_pUVArray[numUV].u = u;
				m_pUVArray[numUV].v = v;
				numUV++;
			} else if (vmode == 'n') // normal
					{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;
				numNorm++;
			} else if (vmode == ' ') // vertex
					{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			if (numNorm > 0 && numUV > 0) {
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2,
						&t2, &n2, &v3, &t3, &n3);
			} else if (numNorm > 0 && numUV == 0) {
				sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3,
						&n3);
			} else if (numUV > 0 && numNorm == 0) {
				sscanf(str, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3,
						&t3);
			} else if (numUV == 0 && numNorm == 0) {
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}
			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}

			// Normal indicies for triangle
			if (numNorm != 0) {
				m_pTriangles[numFace].n1 = n1 - 1;
				m_pTriangles[numFace].n2 = n2 - 1;
				m_pTriangles[numFace].n3 = n3 - 1;
			}

			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t2 - 1;
				m_pTriangles[numFace].t3 = t3 - 1;
			}

			numFace++;
			break;
		default:
			break;
		}
	}

	fclose(fp);

}

//--------------------------------------------------------------
// [Assignment4]
// Create a 2D GL texture from the file given
//--------------------------------------------------------------
void G308_Geometry::ReadTexture(const char* filename) {
	char file[50];
	strncpy(file, filename, 51);

	unsigned int i;
	for (i = 0; i < strlen(filename); i++) {
		if (filename[i] == '.') {
			break;
		}
	}

	char extension[5];
	strcpy(extension, &filename[i + 1]);
	if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0)
		loadTextureFromJPEG(file, &t);
	else {
		printf("Invalid format. Only supports JPEG and PNG.\n");
		exit(1);
	}

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

//--------------------------------------------------------------
// [Assignment4]
// Fill the following function to create display list
// of the obj file to show it as polygon, using texture and normal information (if any)
//--------------------------------------------------------------
void G308_Geometry::CreateGLPolyGeometry() {

	if (m_glGeomListPoly != 0)
		glDeleteLists(m_glGeomListPoly, 1);

	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);
	if (t.hasAlpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA);
	}
	glEnable(GL_TEXTURE_2D);


	//Send texture through to Frag shader
	//glActiveTexture(GL_TEXTURE0 + textureCount);
	//glGetUniformLocation(programId, texName);

	glBindTexture(GL_TEXTURE_2D, texName);

	int count = 0;
	int uvModU = 5;
	int uvModV = 5;

	//int count = m_nNumPolygon;
	for (; count < m_nNumPolygon; count++) {
		G308_Triangle curTri = m_pTriangles[count];

		G308_Point P1 = m_pVertexArray[curTri.v1];
		G308_Normal N1 = m_pNormalArray[curTri.n1];
		G308_UVcoord UV1 = m_pUVArray[curTri.t1];

		G308_Point P2 = m_pVertexArray[curTri.v2];
		G308_Normal N2 = m_pNormalArray[curTri.n2];
		G308_UVcoord UV2 = m_pUVArray[curTri.t2];

		G308_Point P3 = m_pVertexArray[curTri.v3];
		G308_Normal N3 = m_pNormalArray[curTri.n3];
		G308_UVcoord UV3 = m_pUVArray[curTri.t3];

		glShadeModel(GL_SMOOTH);

		glBegin(GL_TRIANGLES);
		glNormal3f(N1.x, N1.y, N1.z);
		glTexCoord2f(UV1.u * uvModU, UV1.v * uvModV);
		glVertex3f(P1.x, P1.y, P1.z);

		glNormal3f(N2.x, N2.y, N2.z);
		glTexCoord2f(UV2.u * uvModU, UV2.v * uvModV);
		glVertex3f(P2.x, P2.y, P2.z);

		glNormal3f(N3.x, N3.y, N3.z);
		glTexCoord2f(UV3.u * uvModU, UV3.v * uvModV);
		glVertex3f(P3.x, P3.y, P3.z);

		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glEndList();

}

void G308_Geometry::CreateGLWireGeometry() {

}
void G308_Geometry::toggleMode() {

}

void G308_Geometry::RenderGeometry() {
	if (fn[0] == 'b') {	//bunny
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_shininess[] = { 20.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}else if (fn[2] == 'r') {	//torus
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_shininess[] = { 56.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}else if (fn[1] == 'p') {	//sphere
		GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat mat_shininess[] = { 5.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}else if (fn[1] == 'e') {	//Teapot
		GLfloat mat_specular[] = { 1, 1, 1, 1.0 };
		GLfloat mat_shininess[] = { 90.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}else if (fn[1] == 'a') {	//Table
		GLfloat mat_specular[] = { 1, 1, 1, 1.0 };
		GLfloat mat_shininess[] = { 14.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	}
	else {
		GLfloat mat_specular[] = { 0.05, 0.05, 0.05, 1.0 };
		GLfloat mat_shininess[] = { 56.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	}
	glCallList(m_glGeomListPoly);
}
