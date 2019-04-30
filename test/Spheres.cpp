#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265

static float alpha = 0;
static float beta = PI / 6.0;
static GLdouble cpos[3];
static float zoomAmount = 60.0;

static GLfloat lpos[] = { 1.0, 4.0, -2.0, 1.0 };
static GLfloat gold[] = { 0.81, 0.71, 0.23, 1.0 };
static GLfloat silver[] = { 0.90, 0.91, 0.98, 1.0 };
static GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat cyan[] = { 0.0, 1.0, 1.0, 0.1 };
static GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };

GLfloat width, height;

static const GLint numSpheres = 3;
static GLfloat spherePos[numSpheres][4]; //numSpheres x 4, x y z r

static const GLint boxLength = 4.0; //x
static const GLint boxHeight = 4.0; //y
static const GLint boxDepth = 4.0;  //z

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(zoomAmount, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void drawBox() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, cyan);

	//Bottom
	glBegin(GL_POLYGON);
	glNormal3f(0.0, -1.0, 0.0);

	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, boxDepth);
	glVertex3f(boxLength, 0.0, boxDepth);
	glVertex3f(boxLength, 0.0, 0.0);
	glEnd();

	//Right
	glBegin(GL_POLYGON);
	glNormal3f(1.0, 0.0, 0.0);

	glVertex3f(boxLength, boxHeight, boxDepth);
	glVertex3f(boxLength, 0.0, boxDepth);
	glVertex3f(boxLength, 0.0, 0.0);
	glVertex3f(boxLength, boxHeight, 0.0);
	glEnd();

	//Back
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, -1.0);

	glVertex3f(boxLength, boxHeight, 0.0);
	glVertex3f(boxLength, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, boxHeight, 0.0);
	glEnd();

	//Left
	glBegin(GL_POLYGON);
	glNormal3f(-1.0, 0.0, 0.0);

	glVertex3f(0.0, boxHeight, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, boxDepth);
	glVertex3f(0.0, boxHeight, boxDepth);
	glEnd();

	//Front	
	/*glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, 1.0);

	glVertex3f(0.0, boxHeight, boxDepth);
	glVertex3f(0.0, 0.0, boxDepth);
	glVertex3f(boxLength, 0.0, boxDepth);
	glVertex3f(boxLength, boxHeight, boxDepth);
	glEnd();*/

	//Top	
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);

	glVertex3f(boxLength, boxHeight, 0.0);
	glVertex3f(boxLength, boxHeight, boxDepth);
	glVertex3f(0.0, boxHeight, boxDepth);
	glVertex3f(0.0, boxHeight, 0.0);
	
	glEnd();
}

void drawSpheres() {

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, red);

	for (int i = 0; i < numSpheres; i++) {
		glPushMatrix();
		glTranslatef(spherePos[i][0], spherePos[i][1], spherePos[i][2]);
		glutSolidSphere(spherePos[i][3], 12, 12);
		glPopMatrix();
	}
}

void display(void)
{
	//Variables for controling the moving rectangle
	static double angleinradian, X, Y, Z, normalX, normalY, normalZ;
	static double X1, Y1, Z1;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cpos[0] = 11.0 * cos(beta) * sin(alpha);
	cpos[1] = 11.0 * sin(beta);
	cpos[2] = 11.0 * cos(beta) * cos(alpha);
	gluLookAt(cpos[0], cpos[1], cpos[2], 0.0, 2.0, 0.0, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glPushMatrix();
	glTranslatef(lpos[0], lpos[1], lpos[2]);

	if (glIsEnabled(GL_LIGHT0)) {
		glMaterialfv(GL_FRONT, GL_EMISSION, white);
	}
	else {
		glMaterialfv(GL_FRONT, GL_EMISSION, black);
	}

	glutSolidSphere(0.1, 10, 8);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	//Set the front and back colors for the rectangles to red and blue, respectively
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gold);
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, silver);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
	

	drawBox();
	drawSpheres();

	glFlush();
	glutSwapBuffers();
}

void fillSpherePos() {
	spherePos[0][0] = 3.0;
	spherePos[0][1] = 1.0;
	spherePos[0][2] = 1.0;
	spherePos[0][3] = 1.0;

	spherePos[1][0] = 1.0;
	spherePos[1][1] = 1.0;
	spherePos[1][2] = 1.0;
	spherePos[1][3] = 1.0;

	spherePos[2][0] = 3.0;
	spherePos[2][1] = 3.0;
	spherePos[2][2] = 1.0;
	spherePos[2][3] = 1.0;
}

void keyboard(unsigned char key, int x, int y)
{
	static int polygonmode[2];

	switch (key) {
	case 27:
		exit(0);
		break;
	case 'x':
		lpos[0] = lpos[0] + 0.2;
		glutPostRedisplay();
		break;
	case 'X':
		lpos[0] = lpos[0] - 0.2;
		glutPostRedisplay();
		break;
	case 'y':
		lpos[1] = lpos[1] + 0.2;
		glutPostRedisplay();
		break;
	case 'Y':
		lpos[1] = lpos[1] - 0.2;
		glutPostRedisplay();
		break;
	case 'z':
		lpos[2] = lpos[2] + 0.2;
		glutPostRedisplay();
		break;
	case 'Z':
		lpos[2] = lpos[2] - 0.2;
		glutPostRedisplay();
		break;
	case 'w':
		glGetIntegerv(GL_POLYGON_MODE, polygonmode);
		if (polygonmode[0] == GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glutPostRedisplay();
		break;
	case '0':
		if (glIsEnabled(GL_LIGHT0)) {
			glDisable(GL_LIGHT0);
		}
		else {
			glEnable(GL_LIGHT0);
		}
		glutPostRedisplay();
		break;
	case '1':
		alpha = 0;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '2':
		alpha = PI / 4;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '3':
		alpha = (2 * PI) / 3;
		beta = PI / 6;
		glutPostRedisplay();
		break;
	case '4':
		alpha = PI;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '+':
		if (zoomAmount > 40) {
			zoomAmount = zoomAmount - 1;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(zoomAmount, (GLfloat)width / (GLfloat)height, 1.0, 20.0);
		glMatrixMode(GL_MODELVIEW);
		glutPostRedisplay();
		break;
	case '-':
		if (zoomAmount < 150) {
			zoomAmount = zoomAmount + 1;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(zoomAmount, (GLfloat)width / (GLfloat)height, 1.0, 20.0);
		glMatrixMode(GL_MODELVIEW);
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void specialkey(GLint key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		alpha = alpha + PI / 180;
		if (alpha > 2 * PI) alpha = alpha - 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		alpha = alpha - PI / 180;
		if (alpha < 0) alpha = alpha + 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (beta < 0.45*PI) beta = beta + PI / 180;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (beta > -0.45*PI) beta = beta - PI / 180;
		glutPostRedisplay();
		break;
	case '1':
		alpha = 0;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '2':
		alpha = PI / 4;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '3':
		alpha = (2 * PI) / 3;
		beta = PI / 6;
		glutPostRedisplay();
		break;
	case '4':
		alpha = PI;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkey);

	fillSpherePos();

	glutMainLoop();
	return 0;
}


/*
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h> // for Sleep() 
#include <math.h> // for sin() and cos()

static GLfloat angle1 = 0.0, angle2 = 0.0, angle3 = 0.0;


static GLfloat none[] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat cyan[] = { 0.0, 1.0, 1.0, 0.1 };
static GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
static const GLfloat PI = 3.14;

static const GLint numSpheres = 3;
static GLfloat spherePos[numSpheres][4]; //numSpheres x 4, x y z r

static const GLint boxLength = 4.0; //x
static const GLint boxHeight = 4.0; //y
static const GLint boxDepth = 4.0;  //z

static float alpha = 0;
static float beta = PI / 6.0;
static float zoomAmount = 60.0;
static GLfloat lpos[] = { 1.0, 4.0, -2.0, 1.0 }; 
static GLdouble cpos[3];
GLfloat width, height;

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(zoomAmount, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}

void drawBox() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, cyan);

	//Bottom
	glBegin(GL_POLYGON);
		glNormal3f(0.0, -1.0, 0.0);

		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, boxDepth);
		glVertex3f(boxLength, 0.0, boxDepth);
		glVertex3f(boxLength, 0.0, 0.0);
	glEnd();

	//Right
	glBegin(GL_POLYGON);
		glNormal3f(1.0, 0.0, 0.0);

		glVertex3f(boxLength, boxHeight, boxDepth);
		glVertex3f(boxLength, 0.0, boxDepth);
		glVertex3f(boxLength, 0.0, 0.0);
		glVertex3f(boxLength, boxHeight, 0.0);
	glEnd();

	//Back
	glBegin(GL_POLYGON);
		glNormal3f(0.0, 0.0, -1.0);

		glVertex3f(boxLength, boxHeight, 0.0);
		glVertex3f(boxLength, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, boxHeight, 0.0);
	glEnd();

	//Left
	glBegin(GL_POLYGON);
		glNormal3f(-1.0, 0.0, 0.0);

		glVertex3f(0.0, boxHeight, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, boxDepth);
		glVertex3f(0.0, boxHeight, boxDepth);
	glEnd();

	//Front	
	glBegin(GL_POLYGON);
		glNormal3f(0.0, 0.0, 1.0);

		glVertex3f(0.0, boxHeight, boxDepth);
		glVertex3f(0.0, 0.0, boxDepth);
		glVertex3f(boxLength, 0.0, boxDepth);
		glVertex3f(boxLength, boxHeight, boxDepth);
	glEnd();

	//Top	
	glBegin(GL_POLYGON);
		glNormal3f(0.0, 1.0, 0.0);

		glVertex3f(0.0, boxHeight, 0.0);
		glVertex3f(0.0, boxHeight, boxDepth);
		glVertex3f(boxLength, boxHeight, boxDepth);
		glVertex3f(boxLength, boxHeight, 0.0);
	glEnd();
}

void drawSpheres() {

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, red);

	for (int i = 0; i < numSpheres; i++) {
		glPushMatrix();
		glTranslatef(spherePos[i][0], spherePos[i][1], spherePos[i][2]);
		glutSolidSphere(spherePos[i][3], 12, 12);
		glPopMatrix();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cpos[0] = 11.0 * cos(beta) * sin(alpha);
	cpos[1] = 11.0 * sin(beta);
	cpos[2] = 11.0 * cos(beta) * cos(alpha);
	gluLookAt(cpos[0], cpos[1], cpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glPushMatrix();
	glTranslatef(lpos[0], lpos[1], lpos[2]);

	if (glIsEnabled(GL_LIGHT0)) {
		glMaterialfv(GL_FRONT, GL_EMISSION, white);
	}
	else {
		glMaterialfv(GL_FRONT, GL_EMISSION, none);
	}

	glutSolidSphere(0.1, 10, 8);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, none);

	drawBox();
	drawSpheres();
	
	glFlush();
	glutSwapBuffers();
}

void animate(void)
{
	angle1 = angle1 + 0.2;
	if (angle1 > 360.0) angle1 = angle1 - 360.0;
	angle2 = angle2 + 1.0;
	if (angle2 > 360.0) angle2 = angle2 - 360.0;
	angle3 = angle3 + 4.0;
	if (angle3 > 360.0) angle3 = angle3 - 360.0;
	Sleep(10);
	glutPostRedisplay();
}


void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		//if (state == GLUT_DOWN)
		//	glutIdleFunc(animate);
		//break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON:
		//if (state == GLUT_DOWN)
		//	glutIdleFunc(NULL);
		//break;
	default:
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	static int polygonmode[2];

	switch (key) {
	case 27:
		exit(0);
		break;
	case 'x':
		lpos[0] = lpos[0] + 0.2;
		glutPostRedisplay();
		break;
	case 'X':
		lpos[0] = lpos[0] - 0.2;
		glutPostRedisplay();
		break;
	case 'y':
		lpos[1] = lpos[1] + 0.2;
		glutPostRedisplay();
		break;
	case 'Y':
		lpos[1] = lpos[1] - 0.2;
		glutPostRedisplay();
		break;
	case 'z':
		lpos[2] = lpos[2] + 0.2;
		glutPostRedisplay();
		break;
	case 'Z':
		lpos[2] = lpos[2] - 0.2;
		glutPostRedisplay();
		break;
	case 'w':
		glGetIntegerv(GL_POLYGON_MODE, polygonmode);
		if (polygonmode[0] == GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glutPostRedisplay();
		break;
	case '0':
		if (glIsEnabled(GL_LIGHT0)) {
			glDisable(GL_LIGHT0);
		}
		else {
			glEnable(GL_LIGHT0);
		}
		glutPostRedisplay();
		break;
	case '1':
		alpha = 0;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '2':
		alpha = PI / 4;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '3':
		alpha = (2 * PI) / 3;
		beta = PI / 6;
		glutPostRedisplay();
		break;
	case '4':
		alpha = PI;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '+':
		if (zoomAmount > 40) {
			zoomAmount = zoomAmount - 1;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(zoomAmount, (GLfloat)width / (GLfloat)height, 1.0, 20.0);
		glMatrixMode(GL_MODELVIEW);
		glutPostRedisplay();
		break;
	case '-':
		if (zoomAmount < 150) {
			zoomAmount = zoomAmount + 1;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(zoomAmount, (GLfloat)width / (GLfloat)height, 1.0, 20.0);
		glMatrixMode(GL_MODELVIEW);
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void specialkey(GLint key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		alpha = alpha + PI / 180;
		if (alpha > 2 * PI) alpha = alpha - 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		alpha = alpha - PI / 180;
		if (alpha < 0) alpha = alpha + 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (beta < 0.45*PI) beta = beta + PI / 180;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (beta > -0.45*PI) beta = beta - PI / 180;
		glutPostRedisplay();
		break;
	case '1':
		alpha = 0;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '2':
		alpha = PI / 4;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	case '3':
		alpha = (2 * PI) / 3;
		beta = PI / 6;
		glutPostRedisplay();
		break;
	case '4':
		alpha = PI;
		beta = PI / 4;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void menu(int value) {
	if (value == 0) {
		exit(0);
	}
	else {
		menuValue = value;
	}
	glutPostRedisplay();
}

void fillSpherePos() {
	spherePos[0][0] = 3.0;
	spherePos[0][1] = 1.0;
	spherePos[0][2] = 1.0;
	spherePos[0][3] = 1.0;

	spherePos[1][0] = 1.0;
	spherePos[1][1] = 1.0;
	spherePos[1][2] = 1.0;
	spherePos[1][3] = 1.0;

	spherePos[2][0] = 3.0;
	spherePos[2][1] = 3.0;
	spherePos[2][2] = 1.0;
	spherePos[2][3] = 1.0;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	fillSpherePos();

	glutMainLoop();
	return 0;
}
*/