

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <ctime> 
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

#define PI 3.14159265

static float alpha = 0.0;
static float beta = PI / 6.0;
static GLdouble cpos[3];
static GLint ww, hh;      /* window width and heght */
static GLfloat cL = 10.0;
static GLdouble angleView = 100.0;

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
static GLfloat radius;
static GLfloat boxL = 1.0;
static GLfloat boxW = 1.0;
static GLfloat boxH = 2.0;
static GLint wallOrder[4] = { 0, 1, 2, 3 }; //initial wall render order. -z,-x,+x,+z
static GLfloat zNegative[4][3] = {};		//array for vertices for wall facing -z direction				
static GLfloat zPositive[4][3] = {};		//array for vertices for wall facing +z direction
static GLfloat xNegative[4][3] = {};		//array for vertices for wall facing -x direction							
static GLfloat xPositive[4][3] = {};		//array for vertices for wall facing +x direction	

typedef struct {	//defines the box
	float length;
	float width;
	float height;
}Box;

const float  PI_F = 3.14159265358979f;	//fixed pi
const float packingConstant = (PI_F / (3 * sqrt(2)));	//the most dense pack possible using spheres and boxes

float l = 0.0, w = 0.0, h = 0.0, r = 0.0;
float boxVolume = 0.0;
float sphereVolume = 0.0;

int numDensePack = 0;		//gonna use all of these later sadly
int numNormalPack = 0;

int layer = 0;
int temp = 0;

Box box = { -1.0,-1.0,-1.0 };	//inital box dimensions (not possible)

struct center {					//centeres to all of the spheres
	float x, y, z, r;
};

center** SphereLocations;		//2D array holding all of the boxes, [i] = layer , [j] = spheres in the layer
float** SphereCenters;			//numSpheres x 4
center first;					//The first sphere in the box will be this far away
center** SphereDenseLocations;
float** SphereCentersDense;

void
getDimensions() {		//this method is pretty good i think, just gets the dimensions, it wasnt hard to write

	printf("Enter the length of the Box in Inches: ");
	cin >> l;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid length of the Box in Inches: ");
		cin >> l;

	}

	printf("Enter the width of the Box in Inches: ");
	cin >> w;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid width of the Box in Inches: ");
		cin >> w;

	}

	printf("Enter the height of the Box in Inches: ");
	cin >> h;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid height of the Box in Inches: ");
		cin >> h;

	}

	printf("\n\n");

	box.length = l;
	box.width = w;
	box.height = h;

}
void
getRadiusSpheres() {	//gets the radius, nothing wrong here

	printf("Enter the radius of the Spheres in Inches: ");
	cin >> r;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid radius of the Sphere in Inches: ");
		cin >> r;
	}
}

void
getNumberSpheres() {	//calculates how many spheres you need per layer

	numDensePack = (int)((boxVolume / sphereVolume) * packingConstant);

	layer = (int)((box.length / (r * 2))*(box.width / (r * 2)));
	temp = (int)(box.height / (r * 2));

	numNormalPack = layer * temp;
	printf("\n\n%s %d %s", "The layers will consist of ", layer, " spheres.");

	layer = (layer * (((int)(box.height / (r * 2)))));

	printf("\n\n %s %d %s", "There will be ", temp, " layers of Spheres.");
}

void
getCoodsNormalPack() {	//calcualtes the Centers of spheres for a normal pack

	int numLength = (int)(box.length / (r + r));
	int numWidth = (int)(box.width / (r + r));		//gets total number of spheres that fit per length/width/height
	int numHeight = (int)(box.height / (r + r));

	int numArea = (numLength * numWidth);			//how many fit on each layer

	printf("\n Number of Spheres that fit the length: %d", numLength);
	printf("\n Number of Spheres that fit the width : %d", numWidth);
	printf("\n Number of Spheres that fit the Height: %d", numHeight);

	printf("\n\n Radius is: %f\n\n", r);

	SphereLocations = new center*[numHeight];
	for (int i = 0; i < numHeight; i++) {
		SphereLocations[i] = new center[numWidth * numLength];	//initalize the 2D array to fit box dimensions
	}

	first = { r,r,r };		//use this as base to map on, first sphere is "r" away from left plane of box, right plane of box, and bottom of the box

	for (int i = 0; i < numHeight; i++) {

		float h = r;
		float l = r;
		float w = r;	//used to input of sphere centers

		for (int j = 0, k = 0; j < numArea; j++) {
			SphereLocations[i][j] = { l,w,first.z,r };

			if (k == numLength - 1) {
				l = r;
				w = w + 2 * r;
				k = 0;
			}
			else {
				l = l + r + r;
				k++;
			}

		}
		first = { r , r , first.z + 2 * r }; //going up to the next height layer
	}

	first = { r,r, r };

	SphereCenters = new float*[numArea * numHeight];
	for (int i = 0; i < (numArea * numHeight); i++) {
		SphereCenters[i] = new float[4];
	}

	int temp = 0;

	for (int i = 0; i < numHeight; i++) {
		for (int j = 0; j < numArea; j++, temp++) {

			SphereCenters[temp][0] = SphereLocations[i][j].x;
			SphereCenters[temp][1] = SphereLocations[i][j].y;
			SphereCenters[temp][2] = SphereLocations[i][j].z;
			SphereCenters[temp][3] = SphereLocations[i][j].r;
		}
	}
}

int
getCoordsDensePack() {

	int numSpheres = 0;
	int longRow = (int)(box.length / (r + r)); //number of spheres that fit in the length (long row)
	int shortRow = longRow - 1;				   //number of spheres that fit in the legnth (short row)

	int numHeight = 0; //how many layers high does this fit?
	float hi = r;
	for (int i = 0; ; i++) {

		if (hi > box.height)
			break;
		else {
			hi = hi + ((sqrt(6.0) * r * 2.0) / 3.0);
			numHeight++;
		}

	}

	int numWidth = 0; //how many rows of spheres are there for the width that fit?
	float wi = r;
	for (int i = 0; ; i++) {

		if (wi > box.width)
			break;
		else {
			wi = wi + (sqrt(3.0)*r);
			numWidth++;
		}
	}

	int temp = (numWidth / 2);

	int numSpheresLongFirstLayer = ((longRow * temp) + (shortRow * (numWidth - temp)));	 //how many fit on each layer, if the long row is first
	int numSpheresShortFirstLayer = ((shortRow * temp) + (longRow * (numWidth - temp))); //how many fit on each layer, if the short row is first

	printf("\n Number of Spheres that fit the long row: %d", longRow);
	printf("\n Number of Spheres that fit the short row: %d", shortRow);
	printf("\n Number of Rows that fit the width : %d", numWidth);
	printf("\n Number of Layers that fit the Height: %d", numHeight);

	printf("\n Number of Spheres that fit on a layer if Long row is put first: %d", numSpheresLongFirstLayer);
	printf("\n Number of Spheres that fit on a layer if Short Row is put first: %d", numSpheresShortFirstLayer);

	printf("\n\n Radius is: %f\n\n", r);

	SphereDenseLocations = new center*[numHeight];
	for (int i = 0; i < numHeight; i++) {
		if (i % 2 == 0)
			SphereDenseLocations[i] = new center[numSpheresLongFirstLayer];	//initalize the 2D array to fit box dimensions
		else
			SphereDenseLocations[i] = new center[numSpheresShortFirstLayer];	//initalize the 2D array to fit box dimensions
	}
	first = { r,r,r };		//use this as base to map on, first sphere is "r" away from left plane of box, right plane of box, and bottom of the box

	int k = 0; //used to know when the row is done
	int m = 0; //to know which type of line to make

	for (int i = 0; i < numHeight; i++) {	//for all the layers possible

		float h = first.z;	//used for height holder
		float l = first.x;	//used for length holder
		float w = first.y;	//used for width holder
		k = 0;

		if (i % 2 == 0) { //if its the first row, or every odd row that starts with the longSphereFirstLayer
			for (int v = 0; v < numWidth; v++) {
				if (v % 2 == 0) {
					for (int j = 0; j < longRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h };
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = r + r;
				}
				else {
					for (int j = 0; j < shortRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h };
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = first.x;
				}
				//printf("\n%d", k);
				w = w + (sqrt(3.0)*r);
			}

			first = { r , r , (float)(first.z + ((sqrt(6.0) * r * 2.0) / 3.0)) };
		}
		else { //else its the even rows so the shortSphereFirstLayer goes

			l = r + r;

			for (int v = 0; v < numWidth; v++) {
				if (v % 2 == 0) {
					for (int j = 0; j < shortRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h };
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = first.x;
				}
				else {
					for (int j = 0; j < longRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h };
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = r + r;
				}
				w = w + (sqrt(3.0)*r);
			}

			first = { r , r , (float)(first.z + ((sqrt(6.0) * r * 2.0) / 3.0)) };

		}
	}

	SphereCentersDense = new float*[numSpheres];
	for (int i = 0; i < numSpheres; i++) {
		SphereCentersDense[i] = new float[4];
	}

	int temp2 = 0;

	for (int i = 0; i < numHeight; i++) {
		for (int j = 0; j < (longRow * numWidth); j++, temp2++) {	//****************BIGGER THAN IT NEEDS TO BE TO AVOID HARD CONFUSING MATH***************
			if (temp2 == numSpheres)								//****************THE ARRAY IS BIGGER THAN IT NEEDS TO BE SO SHOULD STOP ADDING WHEN YOU FIND {0.0,0.0,0.0,0.0} IN AN INDEX********************
				break;
			SphereCentersDense[temp2][0] = SphereDenseLocations[i][j].x;
			SphereCentersDense[temp2][1] = SphereDenseLocations[i][j].y;
			SphereCentersDense[temp2][2] = SphereDenseLocations[i][j].z;
			SphereCentersDense[temp2][3] = SphereDenseLocations[i][j].r;
		}
	}

	return numSpheres;
}











void writemessage()
{
	printf("\n\
		   z/Z ----------------------- increase/decrease box length\n\
		   x/X ----------------------- increase/decrease box width\n\
		   y/Y ----------------------- increase/decrease box height\n\
		   \n");
}

void init() {

	radius = 0.1;
	boxL = box.length/2;
	boxW = box.width/2;
	boxH = box.height/2;
	cL = 100.0;
	angleView = 30.0;	
	int i;
	for (i = 0; i < numSpheres; i++) {
	//	L[i][0] = -boxW + radius + ((double)i) * (boxW/ (double)numSpheres);
	//	L[i][1] = radius;
	//	L[i][2] = 0;
	}	
}

void reshape(int w, int h)
{
	ww = w;
	hh = h;
	glViewport(0, 0, (GLsizei)ww, (GLsizei)hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleView, (GLfloat)ww / (GLfloat)hh, 1.0, 100000.0);
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
void drawSpheres() {

	int i;
	///glPushMatrix()
	for (i = 0; i < numSpheres; i++) {
		if (i%2 == 0)
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
		else
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
		glPushMatrix();
	//	glTranslatef(L[i][0], L[i][1], L[i][2]);
	//	glutSolidSphere(radius, 25, 25);
		glPopMatrix();
	}
}




void display(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
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
int main(int argc, char** argv)
{
	while (box.height <= 0 || box.length <= 0 || box.width <= 0) {
		getDimensions();

		if (box.height <= 0 || box.length <= 0 || box.width <= 0) {
			printf("%s", "One of your values was less than or equal to Zero, please enter them again. \n\n");
		}
	}

	while (r <= 0.0) {
		getRadiusSpheres();

		if (r <= 0) {
			printf("%s %f %s", "\nYour Radius was", r, ". \nPlease enter the radius again. \n\n");
		}
	}

	boxVolume = (box.height * box.length * box.width);
	sphereVolume = ((4.0 / 3.0) * (PI_F)* pow(r, 3));

	printf("\n\n%s %f", "The box Volume is:", boxVolume);
	printf("\n\n%s %f", "The sphere volume is:", sphereVolume);

	getNumberSpheres();

	printf("\n\n %s %d %s", "Normal packing yields:", numNormalPack, "Spheres.");
	printf("\n\n %s %d %s", "Maximum packing yields:", numDensePack, "Spheres.\n\n");

	getCoodsNormalPack();

	getCoordsDensePack();
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
