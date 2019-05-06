

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <ctime> 
#include <Windows.h>


#define PI 3.14159265

static float alpha = 0.0;
static float beta = PI / 6.0;
static GLdouble cpos[3];
static GLint ww, hh;      /* window width and heght */
static GLfloat cL = 10.0;
static GLdouble angleView = 30.0;

static GLfloat lpos[] = { 0.0, 1.0, 0.0, 0.0 };
static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat gray[] = { 0.5, 0.5, 0.5, 1.0 };
static GLfloat darkGray[] = { 0.2, 0.2, 0.2, 1.0 };
static GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat windowColor[] = { 0.0, 191.0 / 255.0, 1.0, 0.1 };
static GLfloat lightGray[] = { 0.8, 0.8, 0.8, 0.0 };

static GLint numSpheres;

static GLdouble a = 0.0;
static GLdouble b = 0.0;
static GLdouble c = 0.0;
static GLdouble d = 0.0;
static GLdouble e = 0.0;
static GLdouble f = 0.0;

static GLfloat radius;

static GLfloat boxL = 1.0;
static GLfloat boxW = 1.0;
static GLfloat boxH = 2.0;


static GLint wallOrder[4] = { 0, 1, 2, 3 }; //initial wall render order. -z,-x,+x,+z


static GLfloat zNegative[4][3] = { {boxW, 0.0, -boxL}, { -boxW, 0.0, -boxL },		//array for vertices for wall facing -z direction
								   { -boxW, boxH, -boxL }, {boxW, boxH, -boxL } };

static GLfloat zPositive[4][3] = { {-boxW, 0.0, boxL}, {boxW, 0.0, boxL},
									{boxW, boxH, boxL}, {-boxW, boxH, boxL} };			//array for vertices for wall facing +z direction

static GLfloat xNegative[4][3] = { {-boxW, 0.0, -boxL}, { -boxW, 0.0, boxL },	//array for vertices for wall facing -x direction
								  { -boxW, boxH, boxL }, { -boxW, boxH, -boxL } };;

static GLfloat xPositive[4][3] = { {boxW, 0.0, boxL}, { boxW, 0.0, -boxL }, 	//array for vertices for wall facing +x direction
								  { boxW, boxH, -boxL }, { boxW, boxH, boxL } };

static GLfloat zNegBG[4][3] = { {50.0, -50.0, -50.0}, { -50.0, -50.0, -50.0 },
								   { -50.0, 50.0, -50.0 }, { 50.0, 50.0, -50.0 } };

static GLfloat zPosBG[4][3] = { {-50.0, -50.0, 50.0}, {50.0, -50.0, 50.0},
								   {50.0, 50.0, 50.0}, {-50.0, 50.0, 50.0} };

static GLfloat xNegBG[4][3] = { {-50.0, -50.0, -50.0}, { -50.0, -50.0, 50.0 },
								   { -50.0, 50.0, 50.0 }, { -50.0, 50.0, -50.0 } };

static GLfloat xPosBG[4][3] = { {50.0, -50.0, 50.0}, { 50.0, -50.0, -50.0 },
								   { 50.0, 50.0, -50.0 }, { 50.0, 50.0, 50.0 } };
static GLfloat L[500][3];
static GLfloat V[500][3];
static GLint flyingPolarity[500];
static GLfloat F[500][3];

int val;
int menuFlyingMagnets; //Top Level sub menu for flying magnets
int menuGroundMagnets; //Top level sub menu for ground magnets
int fpMenu;		//sub menu for flying magnet polarities
int gpMenu;		//sub menu for ground magnet polarities
int glMenu;		//sub menu for ground magnet locations
int gnMenu;		//sub menu for number of ground magnets
int resetMenu; //menu option to reset the program
int exitMenu;  //menu option to exit the program
int menuTop; //Top level menu

void writemessage()
{
	printf("\n\
		   z/Z ----------------------- increase/decrease box length\n\
		   x/X ----------------------- increase/decrease box width\n\
		   y/Y ----------------------- increase/decrease box height\n\
		   \n");
}

void init() {
	numSpheres = 5;
	radius = 0.1;
	boxL = 1.0;
	boxW = 1.0;
	boxH = 2.0;
	cL = 10.0;
	angleView = 30.0;
	int i;
	for (i = 0; i < numSpheres; i++) {
		L[i][0] = -boxW + radius + ((double)i) * (boxW/ (double)numSpheres);
		L[i][1] = radius;
		L[i][2] = 0;
	}
	
}

void reshape(int w, int h)
{
	ww = w;
	hh = h;
	glViewport(0, 0, (GLsizei)ww, (GLsizei)hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleView, (GLfloat)ww / (GLfloat)hh, 1.0, 100.0);
}
void setWallOrder() {
	GLfloat halfW = 0.5 * boxW;
	GLfloat halfL = 0.5 * boxL;
	if (alpha > 0) {
		if (alpha < atan(halfW / halfL)) {
			GLint temp[] = { 0, 1, 2, 3 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else if (alpha < (atan(-halfW / halfL) + PI)) {
			GLint temp[] = { 1, 3, 0, 2 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else if (alpha < (atan(halfW / halfL) + PI)) {
			GLint temp[] = { 3, 2, 1, 0 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else if (alpha < (atan(-halfW / halfL) + 2 * PI)) {
			GLint temp[] = { 2, 0, 3, 1 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else {
			GLint temp[] = { 0, 1, 2, 3 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
	}
	else if (alpha == 0) {
		GLint temp[] = { 0, 1, 2, 3 };
	}
	else {
		if (alpha > atan(-halfW / halfL)) {
			GLint temp[] = { 0, 1, 2, 3 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else if (alpha > (atan(halfW / halfL) - PI)) {
			GLint temp[] = { 2, 0, 3, 1 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else if (alpha > (atan(-halfW / halfL) - PI)) {
			GLint temp[] = { 3, 2, 1, 0 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else if (alpha > (atan(halfW / halfL) - 2 * PI)) {
			GLint temp[] = { 1, 3, 0, 2 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
		else {
			GLint temp[] = { 0, 1, 2, 3 };
			int i;
			for (i = 0; i < 4; i++)
				wallOrder[i] = temp[i];
		}
	}


}
void drawWalls() {
	int i;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, windowColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, windowColor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
	setWallOrder();
	if (beta < 0.20) {
		glBegin(GL_POLYGON);
		glVertex3f(-boxW, boxH, -boxL);
		glVertex3f(-boxW, boxH, boxL);
		glVertex3f(boxW, boxH, boxL);
		glVertex3f(boxW, boxH, -boxL);
		glEnd();
	}
	zNegative[0][0] = boxW;
	zNegative[0][1] = 0.0;
	zNegative[0][2] = -boxL;
	zNegative[1][0] = -boxW;
	zNegative[1][1] = 0.0;
	zNegative[1][2] = -boxL;
	zNegative[2][0] = -boxW;
	zNegative[2][1] = boxH;
	zNegative[2][2] = -boxL;
	zNegative[3][0] = boxW;
	zNegative[3][1] = boxH;
	zNegative[3][2] = -boxL;
	zPositive[0][0] = -boxW;
	zPositive[0][1] = 0.0;
	zPositive[0][2] = boxL;
	zPositive[1][0] = boxW;
	zPositive[1][1] = 0.0;
	zPositive[1][2] = boxL;
	zPositive[2][0] = boxW;
	zPositive[2][1] = boxH;
	zPositive[2][2] = boxL;
	zPositive[3][0] = -boxW;
	zPositive[3][1] = boxH;
	zPositive[3][2] = boxL;
	xNegative[0][0] = -boxW;
	xNegative[0][1] = 0.0;
	xNegative[0][2] = -boxL;
	xNegative[1][0] = -boxW;
	xNegative[1][1] = 0.0;
	xNegative[1][2] = boxL;
	xNegative[2][0] = -boxW;
	xNegative[2][1] = boxH;
	xNegative[2][2] = boxL;
	xNegative[3][0] = -boxW;
	xNegative[3][1] = boxH;
	xNegative[3][2] = -boxL;
	xPositive[0][0] = boxW;
	xPositive[0][1] = 0.0;
	xPositive[0][2] = boxL;
	xPositive[1][0] = boxW;
	xPositive[1][1] = 0.0;
	xPositive[1][2] = -boxL;
	xPositive[2][0] = boxW;
	xPositive[2][1] = boxH;
	xPositive[2][2] = -boxL;
	xPositive[3][0] = boxW;
	xPositive[3][1] = boxH;
	xPositive[3][2] = boxL;
	for (i = 0; i < 4; i++) {
		if (i == 2) {

		}
		if (wallOrder[i] == 0) { //draw -z wall
			glNormal3f(0.0, 0.0, -1.0);
			glBegin(GL_POLYGON);
			int j;
			for (j = 0; j < 4; j++) {
				glVertex3fv(zNegative[j]);
			}
			glEnd();
		}
		if (wallOrder[i] == 1) { //draw -x wall
			glNormal3f(-1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);
			int j;
			for (j = 0; j < 4; j++) {
				glVertex3fv(xNegative[j]);
			}
			glEnd();
		}
		if (wallOrder[i] == 2) { //draw +x wall
			glNormal3f(1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);
			int j;
			for (j = 0; j < 4; j++) {
				glVertex3fv(xPositive[j]);
			}
			glEnd();
		}
		if (wallOrder[i] == 3) { //draw +z wall
			glNormal3f(0.0, 0.0, 1.0);
			glBegin(GL_POLYGON);
			int j;
			for (j = 0; j < 4; j++) {
				glVertex3fv(zPositive[j]);
			}
			glEnd();
		}
	}
	if (beta > 0) {
		glBegin(GL_POLYGON);
		glVertex3f(-boxW, boxH, -boxL);
		glVertex3f(-boxW, boxH, boxL);
		glVertex3f(boxW, boxH, boxL);
		glVertex3f(boxW, boxH, -boxL);
		glEnd();
	}
	glDisable(GL_BLEND);
}
void drawBG() {
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lightGray);
	int i;
	glBegin(GL_POLYGON);
	for (i = 3; i > -1; i--) {
		glVertex3fv(xNegBG[i]);
	}
	glEnd();
	glBegin(GL_POLYGON);
	for (i = 3; i > -1; i--) {
		glVertex3fv(xPosBG[i]);
	}
	glEnd();
	glBegin(GL_POLYGON);
	for (i = 3; i > -1; i--) {
		glVertex3fv(zNegBG[i]);
	}
	glEnd();
	glBegin(GL_POLYGON);
	for (i = 3; i > -1; i--) {
		glVertex3fv(zPosBG[i]);
	}
	glEnd();
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-50.0, -50.0, -50.0);
	glVertex3f(-50.0, -50.0, 50.0);
	glVertex3f(50.0, -50.0, 50.0);
	glVertex3f(50.0, -50.0, -50.0);
	glEnd();
}


void drawSpheres() {

	int i;
	///glPushMatrix()
	for (i = 0; i < numSpheres; i++) {
		if (i%2 == 0)
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
		else
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
		glPushMatrix();
		glTranslatef(L[i][0], L[i][1], L[i][2]);
		glutSolidSphere(radius, 25, 25);
		glPopMatrix();
	}
}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cpos[0] = cL * cos(beta) * sin(alpha);
	cpos[1] = cL * sin(beta);
	cpos[2] = cL * cos(beta) * cos(alpha);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cpos[0], cpos[1], cpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darkGray);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
	glNormal3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-boxW, 0.0, -boxL);
	glVertex3f(-boxW, 0.0, boxL);
	glVertex3f(boxW, 0.0, boxL);
	glVertex3f(boxW, 0.0, -boxL);
	glEnd();
	drawBG();
	glPushMatrix();
	drawSpheres();
	glPopMatrix();
	drawWalls();
	glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	static int polygonmode[2];
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'z':
		if (boxL > 0.5500000)
			boxL -= 0.1;
		printf("box length = %f\n", boxL);
		glutPostRedisplay();
		break;
	case 'Z':
		if (boxL < 5.5500000)
			boxL += 0.1;
		printf("box length = %f\n", boxL);
		glutPostRedisplay();
		break;
	case 'x':
		if (boxW > 0.5500000)
			boxW -= 0.1;
		printf("box width = %f\n", boxW);
		glutPostRedisplay();
		break;
	case 'X':
		if (boxW < 5.5500000)
			boxW += 0.1;
		printf("box width = %f\n", boxW);
		glutPostRedisplay();
		break;
	case 'y':
		if (boxH > 0.5500000)
			boxH -= 0.1;
		printf("box height = %f\n", boxH);
		glutPostRedisplay();
		break;
	case 'Y':
		if (boxH < 5.5500000)
			boxH += 0.1;
		printf("box height = %f\n", boxH);
		glutPostRedisplay();
		break;
	case '-':
		if (angleView < 80) {
			angleView++;
		}
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glutPostRedisplay();
		break;
	case '+':
		if (angleView > 10) {
			angleView--;
		}
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glutPostRedisplay();
		break;
	case 'i':
	//	writemessage();
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
		if (alpha > 2 * PI)
			alpha = alpha - 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		alpha = alpha - PI / 180;
		if (alpha < 0)
			alpha = alpha + 2 * PI;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (beta < 0.45*PI)
			beta = beta + PI / 180;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (beta > -0.45*PI)
			beta = beta - PI / 180;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}



/*
void processHits(GLuint hits, GLuint buffer[])
{
	unsigned int k, m;
	GLuint names, *ptr;
	printf("hits = %d\n", hits);
	ptr = (GLuint *)buffer;
	for (k = 0; k < hits; k++) {
		names = *ptr;
		printf(" number of names on stack for this hit = %d\n", names); ptr++;
		printf(" z1 is %f;", (float)*ptr / 0xffffffff); ptr++;
		printf(" z2 is %f\n", (float)*ptr / 0xffffffff); ptr++;
		printf(" names on stack are ");
		for (m = 0; m < names; m++) {


			//TO DO: Use hits to update array of colors
			if (groundPolarity[*ptr] == 2) {
				groundPolarity[*ptr] = 0;
			}
			else {
				groundPolarity[*ptr] = groundPolarity[*ptr] + 1;
			}
			printf("%d ", *ptr); ptr++;
		}
		printf("\n");
	}
	printf("\n");
}*/





int main(int argc, char** argv)
{
	srand(time(NULL));
	writemessage();
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkey);

	glutMainLoop();
	return 0;
}
