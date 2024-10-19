#include "GL/glut.h"
#include <math.h>
#include <stdlib.h>
#define GL_PI 3.1415f
GLfloat xRot = 0.45f;
GLfloat yRot = 0.35f;

GLfloat LUZ[4] = { 0., 80., 0., 1. };

GLfloat velocidad1;
GLfloat velocidad2;
GLboolean bCull = glIsEnabled(GL_CULL_FACE);
GLboolean bDepth = glIsEnabled(GL_DEPTH_TEST);
//variables para configuracion luz Phong
GLboolean bOutline = (GLboolean)true;
GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f },
diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f },
lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 },
specular[] = { 1.0f, 1.0f, 1.0f, 1.0f },

specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat local_view[] = { 0.0 };
#define brazoselec 1
#define manoselec 2
GLfloat mat_diffuse[] = { 0.8, 0.5, 0.1, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat high_shininess[] = { /*100.0*/ 128 };
bool rotateSegment1 = false;
bool rotateSegment2 = false;
bool rotateSegment3 = false;
void normalize(GLfloat v[3]) {
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0.0) { exit(100); return; } // wrong input exit
	v[0] /= d; v[1] /= d; v[2] /= d;
}
void normcrossprod(GLfloat v1[3], GLfloat v2[3], GLfloat out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1]; out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];	normalize(out);
}

void gltGetNormalVector(GLfloat v0[3], GLfloat //build normal 'norm'
	v1[3], GLfloat v2[3], GLfloat norm[3])	// by the triangle vrtices
{
	GLfloat d1[3], d2[3];
	for (int j = 0; j < 3; j++) {
		d1[j] = v0[j] - v1[j];
		d2[j] = v1[j] - v2[j];
	}
	normcrossprod(d1, d2, norm);
}

GLenum shademode = GL_FLAT;

GLfloat ex = 0.0f;
GLfloat ey = 50.0f;
GLfloat ez = -120.0f;
GLfloat delta = 0.01f;
GLfloat deltaR = 0.01f;

GLfloat colors[][3] = { { 0.0, 0.0, 0.0 },{ 1.0, 0.0, 0.0 },
{ 1.0, 1.0, 0.0 },{ 0.0, 1.0, 0.0 },{ 0.0, 0.0, 1.0 },
{ 1.0, 0.0, 1.0 },{ 1.0, 1.0, 1.0 },{ 0.0, 1.0, 1.0 } };


GLfloat texpts[2][2][2] = { {{0.0, 0.0}, {0.0, 2.0}},
{{3.0, 0.0}, {3.0, 2.0}} };
GLfloat ctrlpoints[4][4][3] = {
   { {-1.5, -1.5, 4.0},     {-0.5, -1.5, 2.0},     {0.5, -1.5, -1.0},     {1.5, -1.5, 2.0}},
   { {-1.5, -0.5, 1.0},     {-0.5, -0.5, 3.0},     {0.5, -0.5, 0.0},     {1.5, -0.5, -1.0}},
   { {-1.5, 0.5, 4.0},     {-0.5, 0.5, 0.0},     {0.5, 0.5, 3.0},     {1.5, 0.5, 4.0}},
   { {-1.5, 1.5, -2.0},     {-0.5, 1.5, -2.0},     {0.5, 1.5, 0.0},     {1.5, 1.5, -1.0}}
};
///////// datos del plano de piso y matriz de sombra //////////
GLfloat datosdePlano[] = { 0.f, 1.f/*-1.*/, 0.f, 150 /*o -150 ?*/ };
GLfloat matrizdeSombra[16];

GLfloat ALFA = 0.75f;

/////////////////////////para texturas:////////////////////////////////
#define    checkImageWidth 256
#define    checkImageHeight 256
#define subImageWidth 64
#define subImageHeight 64

static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
static GLubyte otherImage[checkImageHeight][checkImageWidth][4];
static GLubyte subImage[subImageHeight][subImageWidth][4];
static GLuint texName[2];

//////////// para seleccionar una cara destinguida del cubo ////////

#define BUFSIZE 512
GLuint selectBuf[BUFSIZE];

#define CARA_DESTINGUIDA       111

GLboolean CARA_DESTINGUIDA_TOCADA; //revise uso de esta variable a continuacion!

GLfloat STARTcolor_de_cara[] = { 0.0f, 1.0f, 1.0f },
color_de_cara_DESTINGUIDA[3];

void initColors()
{
	for (int i = 0; i < 3; i++) {
		color_de_cara_DESTINGUIDA[i] = STARTcolor_de_cara[i];
	}
}

///////// fin "para seleccionar" ///////////////////

void makeCheckImages(void)
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;   checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;   checkImage[i][j][3] = (GLubyte)255;

			c = ((((i & 0x10) == 0) ^ ((j & 0x10)) == 0)) * 255;
			otherImage[i][j][0] = (GLubyte)0;   otherImage[i][j][1] = (GLubyte)0;
			otherImage[i][j][2] = (GLubyte)c;   otherImage[i][j][3] = (GLubyte)255;
		}
	}

	for (i = 0; i < subImageHeight; i++) {
		for (j = 0; j < subImageWidth; j++) {
			c = ((((i & 0x4) == 0) ^ ((j & 0x4)) == 0)) * 255;
			subImage[i][j][0] = (GLubyte)c;    subImage[i][j][1] = (GLubyte)0;
			subImage[i][j][2] = (GLubyte)0;    subImage[i][j][3] = (GLubyte)255;
		}
	}
}

void gltMakeShadowMatrix(GLfloat vPlaneEquation[], GLfloat vLightPos[], GLfloat destMat[]) {
	GLfloat dot;

	dot = vPlaneEquation[0] * vLightPos[0] + vPlaneEquation[1] * vLightPos[1] +
		vPlaneEquation[2] * vLightPos[2] + vPlaneEquation[3] * vLightPos[3];

	destMat[0] = dot - vLightPos[0] * vPlaneEquation[0];
	destMat[4] = 0.0f - vLightPos[0] * vPlaneEquation[1];
	destMat[8] = 0.0f - vLightPos[0] * vPlaneEquation[2];
	destMat[12] = 0.0f - vLightPos[0] * vPlaneEquation[3];

	destMat[1] = 0.0f - vLightPos[1] * vPlaneEquation[0];
	destMat[5] = dot - vLightPos[1] * vPlaneEquation[1];
	destMat[9] = 0.0f - vLightPos[1] * vPlaneEquation[2];
	destMat[13] = 0.0f - vLightPos[1] * vPlaneEquation[3];

	destMat[2] = 0.0f - vLightPos[2] * vPlaneEquation[0];
	destMat[6] = 0.0f - vLightPos[2] * vPlaneEquation[1];
	destMat[10] = dot - vLightPos[2] * vPlaneEquation[2];
	destMat[14] = 0.0f - vLightPos[2] * vPlaneEquation[3];

	destMat[3] = 0.0f - vLightPos[3] * vPlaneEquation[0];
	destMat[7] = 0.0f - vLightPos[3] * vPlaneEquation[1];
	destMat[11] = 0.0f - vLightPos[3] * vPlaneEquation[2];
	destMat[15] = dot - vLightPos[3] * vPlaneEquation[3];
}


void SetupRC()
{
	velocidad1 = 3;
	velocidad2 = 3;
	// Black background
	glClearColor(0.0f, 0.0f, .0f, 1.0f);
	//para seleccion
	initColors();
	// Set color shading model to flat
	glShadeModel(shademode);
	// Clockwise-wound polygons are front facing; this is reversed
	// because we are using triangle fans
	glFrontFace(GL_CW);

	makeCheckImages();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(2, texName);
	glBindTexture(GL_TEXTURE_2D, texName[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 48, 170, subImageWidth,
		subImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, subImage);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 48, 48, subImageWidth,
		subImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, subImage);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 170, 48, subImageWidth,
		subImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, subImage);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 170, 170, subImageWidth,
		subImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, subImage);

	// Set up and enable light 0
	glFrontFace(GL_CCW);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	GLfloat att[] = { 1. };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	//glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att);
	/*comentado por prof
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);*/

	glEnable(GL_LIGHT0);

	//glLightfv(GL_LIGHT0, GL_POSITION, LUZ); //<- se llama en luz()

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	// Front material ambient and diffuse colors track glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2,
		0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);

	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	/*glEnable(GL_AUTO_NORMAL);*/
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
}

void cuadrado() {
	glBegin(GL_POLYGON);
	glNormal3f(0., 0., -1. /*-1.?*/);//para phong
	glVertex2f(-1., -1.);
	glVertex2f(-1., 1.);
	glVertex2f(1., 1.);
	glVertex2f(1., -1.);
	glEnd();
}

void rayas_en_cuadrado() {
	glColor4f(0., 0., 0., ALFA);
	glBegin(GL_LINES);
	int i, n = 10;
	for (i = 0; i <= n; i++)
	{
		glVertex2f(-1. + i * 2. / n, -1.);
		glVertex2f(-1. + i * 2. / n, 1.);

		glVertex2f(-1., -1. + i * 2. / n);
		glVertex2f(1., -1. + i * 2. / n);
	}
	glEnd();
}

void cilindro() {
	GLfloat radius = 5.0f;  // Aumento del tamaño del radio
	GLfloat height = 20.0f;  // Aumento del tamaño de la altura
	int numSegments = 16;

	GLfloat segmentAngle = 2.0f * 3.14 / numSegments;
	GLfloat halfHeight = height / 2.0f;

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= numSegments; i++) {
		GLfloat angle = i * segmentAngle;
		GLfloat x = radius * cos(angle);
		GLfloat z = radius * sin(angle);

		glVertex3f(x, halfHeight, z);  // Top vertex
		glVertex3f(x, -halfHeight, z); // Bottom vertex
	}
	glEnd();

	// Draw top and bottom circles
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, halfHeight, 0.0f);  // Top center vertex
	for (int i = 0; i <= numSegments; i++) {
		GLfloat angle = i * segmentAngle;
		GLfloat x = radius * cos(angle);
		GLfloat z = radius * sin(angle);

		glVertex3f(x, halfHeight, z);  // Top vertices
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, -halfHeight, 0.0f);  // Bottom center vertex
	for (int i = 0; i <= numSegments; i++) {
		GLfloat angle = i * segmentAngle;
		GLfloat x = radius * cos(angle);
		GLfloat z = radius * sin(angle);

		glVertex3f(x, -halfHeight, z);  // Bottom vertices
	}
	glEnd();
}

void mediaesfera() {
	float radius = 5.0f;
	int slices = 20;
	int stacks = 20;

	float phi, theta;
	float x, y, z;

	for (int i = 0; i < stacks / 2; ++i) {
		phi = i * 3.14 / stacks;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j <= slices; ++j) {
			theta = j * 2.0f * 3.14 / slices;

			x = radius * sin(phi) * cos(theta);
			y = radius * cos(phi);
			z = radius * sin(phi) * sin(theta);
			glVertex3f(x, y, z);

			x = radius * sin(phi + 3.14 / stacks) * cos(theta);
			y = radius * cos(phi + 3.14 / stacks);
			z = radius * sin(phi + 3.14 / stacks) * sin(theta);
			glVertex3f(x, y, z);
		}
		glEnd();
	}
}

void unit_square() {
	glBegin(GL_QUADS);
	glVertex2f(-1., -1.);
	glVertex2f(1., -1.);
	glVertex2f(1., 1.);
	glVertex2f(-1., 1.);
	glEnd();
}

void unit_square_con_textura() {

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName[0]);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);  glVertex2f(-1., -1.); //glVertex3f(400.0f, -150.0f, -700.0f); //glVertex3f(-2.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0);  glVertex2f(1., -1.); //glVertex3f(-400.0f, -150.0f, -700.0f); //glVertex3f(-2.0, 1.0, 0.0);

	glTexCoord2f(1.0, 1.0);   glVertex2f(1., 1.); //glVertex3f(-400.0f, -150.0f, 200.0f); //glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0);   glVertex2f(-1., 1.);// glVertex3f(400.0f, -150.0f, 200.0f);//glVertex3f(0.0, -1.0, 0.0);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	/*glBegin(GL_QUADS);
	glVertex2f(-1., -1.);
	glVertex2f(1., -1.);
	glVertex2f(1., 1.);
	glVertex2f(-1., 1.);
	glEnd();*/
}


void piso() {

	glPushMatrix();
	glTranslatef(0.f, -datosdePlano[3], 50.f);
	glRotatef(270.f, 1.f, 0.f, 0.f);
	// voltea el piso al la cara invisible
	glScalef(250.f, 250.f, 250.f); //piso 3 veces mas ancho que cubo
	glColor4f(0.4f, 0.4f, 0.4f, ALFA);
	cuadrado(); //cuadro_sin_color();
	glDisable(GL_DEPTH_TEST);
	rayas_en_cuadrado();
	if (bDepth)glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}


void drawParallelepiped()
{
	// Vertices of the paralelepiped
	static GLfloat vertices[][3] =
	{
		{ -1.0, -1.0, -1.0 },  // Vertex 0
		{ 1.0, -1.0, -1.0 },   // Vertex 1
		{ 1.0, 1.0, -1.0 },    // Vertex 2
		{ -1.0, 1.0, -1.0 },   // Vertex 3
		{ -1.0, -1.0, 1.0 },   // Vertex 4
		{ 1.0, -1.0, 1.0 },    // Vertex 5
		{ 1.0, 1.0, 1.0 },     // Vertex 6
		{ -1.0, 1.0, 1.0 }     // Vertex 7
	};

	// Faces of the paralelepiped (each face is made up of 4 vertices)
	static GLuint faces[][4] =
	{
		{ 0, 1, 2, 3 },  // Face 0: Bottom
		{ 4, 5, 6, 7 },  // Face 1: Top
		{ 0, 1, 5, 4 },  // Face 2: Left
		{ 2, 3, 7, 6 },  // Face 3: Right
		{ 0, 3, 7, 4 },  // Face 4: Front
		{ 1, 2, 6, 5 }   // Face 5: Back
	};

	// Enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);

	// Set vertex array data
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	// Draw the paralelepiped
	for (int i = 0; i < 6; i++) {
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, faces[i]);
	}

	// Disable vertex arrays
	glDisableClientState(GL_VERTEX_ARRAY);
}

float angleX1 = 0.0f;
float angleX2 = 0.0f;
float angleX3 = 0.0f;
float angleY3 = 0.0f;

void brazo() {


	// Primer paralelepípedo
	glScalef(5.0f, -5.0f, 5.0f);
	glTranslatef(1.f, 2.f, 0.f);
	glPushMatrix();
	glRotatef(angleX1, 1.0f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Segundo paralelepípedo
	glPushMatrix();
	glRotatef(angleX2, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -0.5f, -0.5f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Tercer paralelepípedo
	glPushMatrix();
	glRotatef(angleX3, 1.0f, 0.0f, 0.0f);
	glRotatef(angleY3, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, -1.0f, -1.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();
}
float angleU1 = 0.0f;
float angleU2 = 0.0f;
float angleU3 = 0.0f;
float angleP3 = 0.0f;

void brazo2() {

	// Primer paralelepípedo

	glScalef(5.0f, -5.0f, 5.0f);
	glTranslatef(-1.f, 2.f, 0.f);
	glPushMatrix();
	glRotatef(angleU1, 1.0f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Segundo paralelepípedo
	glPushMatrix();
	glRotatef(angleU2, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -0.5f, -0.5f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Tercer paralelepípedo
	glPushMatrix();
	glRotatef(angleU3, 1.0f, 0.0f, 0.0f);
	glRotatef(angleP3, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, -1.0f, -1.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();
}
void Objetos(void)
{
	glColor3f(0.2f, 0.8f, 0.2f);


	glPushMatrix();
	glTranslatef(0.f, 10.f, 0.f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	mediaesfera();

	glPopMatrix();
	mediaesfera();
	glPushMatrix();

	cilindro();

	glPopMatrix();

	glPushMatrix();
	glPushName(manoselec);
	brazo2();

	glPopMatrix();
	glPushMatrix();
	glPushName(brazoselec);
	brazo();

	glPopMatrix();

}
void curvo(void) {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(25., -25., 0.);
	glScalef(3, 3, 3);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void curvo2(void) {

	glPushMatrix();
	glColor4f(0.5f, 0.5f, 0.5f, ALFA);
	glTranslatef(25., -25., 0.);
	glScalef(3, 3, 3);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();
}
void Objetos_simplificados(void)
{


	glPushMatrix();

	cilindro();

	glPopMatrix();

	glPushMatrix();
	glPushName(manoselec);
	brazo2();

	glPopMatrix();
	glPushMatrix();
	glPushName(brazoselec);
	brazo();

	glPopMatrix();

	glPushMatrix();

	curvo2();

	glPopMatrix();

}
void Sombra_objetos()
{

	glPushMatrix();
	gltMakeShadowMatrix(datosdePlano, LUZ, matrizdeSombra);
	glMultMatrixf(matrizdeSombra);
	glColor4f(0.5f, 0.5f, 0.5f, ALFA);
	Objetos_simplificados();

	glPopMatrix();

}
void StencilConfig()
{// step 1
 // turning off writing to the color buffer and depth buffer so we only
 // write to stencil buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	// enable stencil buffer
	glEnable(GL_STENCIL_TEST);
	// write a one to the stencil buffer everywhere we are about to draw
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	// this is to always pass a one to the stencil buffer where we draw
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// render the plane which the shadow will be on
	// color and depth buffer are disabled, only the stencil buffer
	// will be modified
	piso();//DrawGround();

	// turn the color and depth buffers back on
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	// until stencil test is diabled, only write to areas where the
	// stencil buffer has a one. This is to draw the shadow only on
	// the floor.
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);

	// don't modify the contents of the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	// end step 1
}

void luz() {
	/*Esta funcion renderiza fuente luz y ademas
	mediante la 2a instruccion configura su posicion
	para el modelo de Phong*/
	glColor4f(1.f, 1.f, 0.f, 1.f - ALFA);
	glLightfv(GL_LIGHT0, GL_POSITION, LUZ/*lightPos*/);
	glPushMatrix();
	glTranslatef(LUZ[0], LUZ[1], LUZ[2]);
	glScalef(10., 10., 10.);
	glutSolidSphere(1., 20., 20.);
	glPopMatrix();
}

void RenderScene(GLenum mode)
{
	/* ver el comentario en ChangeSize() respecto 4 siguientes instrucciones: */
	glMatrixMode(GL_PROJECTION);
	if (mode == GL_RENDER) //para NO seleccion	
		glLoadIdentity();
	gluPerspective(60.0, 1.0, 1.5, 1500.0);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(ex, ey, ez, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Turn culling on if flag is set
	if (bCull /*alterar con tecla F1*/)   glEnable(GL_CULL_FACE);
	else        glDisable(GL_CULL_FACE);

	// Enable depth testing if flag is set
	if (bDepth /*alterar con tecla F2*/)  glEnable(GL_DEPTH_TEST);
	else        glDisable(GL_DEPTH_TEST);

	// Draw the back side as a wireframe only, if flag is set
	if (bOutline /*alterar con tecla F3*/) glPolygonMode(GL_BACK, GL_LINE);
	else        glPolygonMode(GL_BACK, GL_FILL);

	// Save matrix state and do the rotation
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	//para reflejo
	StencilConfig();
	glFrontFace(GL_CCW);
	glPushMatrix();

	glTranslatef(0.0f, /*o '+'?*/-datosdePlano[3], 0.0f);
	glScalef(1.0f, -1.0f, 1.0f);  // geometry is mirrored by ground    
	glTranslatef(0.0f, /*o '-'*/datosdePlano[3], 0.0f);
	glEnable(GL_LIGHTING);
	glColor3f(0.3f, 0.8f, 0.2f);
	Objetos_simplificados();  //colorcube2();  //Draw_Objects();
	curvo();
	glDisable(GL_LIGHTING);
	luz();//Draw_Light();


	glPopMatrix();
	glFrontFace(GL_CW);
	glDisable(GL_STENCIL_TEST);
	//fin "reflejo"

	luz();
	//mezclar mundo reflejado con piso
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST); // para luchar con errores de redonde

	piso();

	StencilConfig();
	glDisable(GL_DEPTH_TEST);

	Sombra_objetos();

	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND); //fin 'mezclar' nuevo con viejo

	if (bDepth)  glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	Objetos();
	curvo();

	glDisable(GL_LIGHTING);
	glPopMatrix();

	// Flush drawing commands
	glFlush();//glutSwapBuffers();
}






void display(void) //<- envoltura a RenderScene() para seleccion
{
	glClear(GL_COLOR_BUFFER_BIT);
	RenderScene(GL_RENDER);
}

void seleccion(int name) {
	int i;

	if (name == brazoselec) {
		for (i = 0; i < 3; i++) {
			velocidad1 += 1;
		}

	}
	else if (name == manoselec) {
		for (i = 0; i < 3; i++) {
			velocidad2 += 1;
		}
	}

	glutPostRedisplay();
}
void ProcessesHits(GLint hits, GLuint buffer[]) {
	if (hits == 0) {
		SetupRC();
		return;
	}

	unsigned int i, j;
	GLuint names;
	GLuint* pointer;

	pointer = (GLuint*)buffer;
	for (i = 0; i < hits; i++) {
		names = *pointer;
		pointer += 3;
		for (j = 0; j < names; j++)
			seleccion(*pointer++);
	}
}


void SelectAction(int button, int state, int x, int y) {
	GLuint select_buffer[BUFSIZE];
	GLint viewport[4];
	GLint hits;

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) {
		return;
	}

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(BUFSIZE, select_buffer);

	(void)glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);

	RenderScene(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	hits = glRenderMode(GL_RENDER);
	ProcessesHits(hits, select_buffer);
	glutPostRedisplay();

}
// Called by GLUT library when the window has changed size
void ChangeSize(GLsizei w, GLsizei h)
{    // Set Viewport to window dimensions
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	/*para tomar bajo control la alteraciones de glMatrixMode,
	los llamados a continuacion se trasladaron a RenderScene */

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();	
	//gluPerspective(60.0, 1.0, 1.5, 1500.0);
	//glMatrixMode(GL_MODELVIEW);
}

void SpecialKeys(int key, int x, int y)
{
	GLfloat dx, dz, dy;


	if (key == GLUT_KEY_UP)
	{//increase distance from camera to origin
		ex *= (1.0f + deltaR);  ey *= (1.0f + deltaR);    ez *= (1.0f + deltaR);
	}

	if (key == GLUT_KEY_DOWN)
	{//reduce distance from camera to origin (close up)
		ex *= (1.0f - deltaR); ey *= (1.0f - deltaR);    ez *= (1.0f - deltaR);
	}

	if (key == GLUT_KEY_LEFT)
		//Rotate camera around origin in Oxz plane
	{
		dx = -ez;     dz = ex;
		GLfloat s = sqrtf(ex * ex + ey * ey + ez * ez);
		ex += delta * dx;       ez += delta * dz;
		GLfloat s1 = sqrtf(ex * ex + ey * ey + ez * ez) / s;
		ex /= s1;    ey /= s1;    ey /= s1;
	}

	if (key == GLUT_KEY_RIGHT)
		//Rotate camera around origin in Oxz plane
	{
		dx = -ez;     dz = ex;
		GLfloat s = sqrtf(ex * ex + ey * ey + ez * ez);
		ex -= delta * dx;          ez -= delta * dz;
		GLfloat s1 = sqrtf(ex * ex + ey * ey + ez * ez) / s;
		ex /= s1;    ey /= s1;    ey /= s1;
	}


	if (key == GLUT_KEY_F1) bCull = !bCull;
	if (key == GLUT_KEY_F2)bDepth = !bDepth;
	if (key == GLUT_KEY_F3)bOutline = !bOutline;
	if (key == GLUT_KEY_F4)
	{
		if (shademode == GL_FLAT) { shademode = GL_SMOOTH; }
		else { if (shademode == GL_SMOOTH) { shademode = GL_FLAT; } };
		glShadeModel(shademode);
	}

	if (key == GLUT_KEY_F5)//Rotate camera around origin in Oyz plane
	{
		dy = -ez;     dz = ey;
		GLfloat s = sqrtf(ex * ex + ey * ey + ez * ez);
		ey -= delta * dy;          ez -= delta * dz;
		GLfloat s1 = sqrtf(ex * ex + ey * ey + ez * ez) / s;
		ex /= s1;    ey /= s1;    ey /= s1;
	}

	if (key == GLUT_KEY_F6)//Rotate camera around origin in Oyz plane
	{
		dy = -ez;     dz = ey;
		GLfloat s = sqrtf(ex * ex + ey * ey + ez * ez);
		ey += delta * dy;          ez += delta * dz;
		GLfloat s1 = sqrtf(ex * ex + ey * ey + ez * ez) / s;
		ex /= s1;    ey /= s1;    ey /= s1;
	}
	if (key == GLUT_KEY_F7)
		LUZ[0] += 1;
	if (key == GLUT_KEY_F8)
		LUZ[0] -= 1;
	if (key == GLUT_KEY_F9)
		LUZ[1] -= 1;
	if (key == GLUT_KEY_F10)
		LUZ[1] += 1;

	// Refresh the Window
	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'x':
	case 'X':
		angleX1 = velocidad1;
		angleX2 += velocidad1;
		angleX3 += velocidad1;
		break;
	case 'y':
	case 'Y':
		angleX1 += 5.0f;
		break;
	case 'z':
	case 'Z':
		angleX2 += velocidad2;
		angleX3 += velocidad2;
		break;
	case 'v':
	case 'V':
		angleY3 += 5.0f;
		break;



	case 'b':
		angleU1 += velocidad1;
		angleU2 += velocidad1;
		angleU3 += velocidad1;
		break;
	case 'B':
		angleU1 += velocidad1;
		angleU2 += velocidad1;
		angleU3 += velocidad1;
		break;
	case 'n':
		angleU2 += velocidad2;
		angleU3 += velocidad2;
		break;
	case 'N':
		angleU2 += velocidad2;
		angleU3 -= velocidad2;
		break;
	case 'm':
		angleP3 += 5.0f;
		break;
	case 'M':
		angleP3 -= 5.0f;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}





int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE/*GLUT_DOUBLE*/ | GLUT_RGB | GLUT_STENCIL);
	glutInitWindowSize(200, 200);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(keyboard);

	SetupRC();
	glutMouseFunc(SelectAction);
	glutMainLoop();
	return 0;
}