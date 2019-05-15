#include <GL/glut.h>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <ctime> 
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

//Constants
#define PI 3.14159265						//fixed double pi
const float  PI_F = 3.14159265358979f;		//fixed float pi

//Camera and View
static float alpha = 0.0;
static float beta = PI / 6.0;
static GLdouble cpos[3];
static GLint ww, hh;						//window width and height
static GLfloat cL = 10.0;
static GLdouble angleView = 100.0;
static GLfloat lpos[] = { 0.0, 1.0, 0.0, 0.0 };

//Colors
static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat darkGray[] = { 0.2, 0.2, 0.2, 1.0 };
static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat windowColor[] = { 0.0, 191.0 / 255.0, 1.0, 0.1 };
static GLfloat cyan[] = { 0.0, 1.0, 1.0, 0.1 };

//Box and Walls
typedef struct {							//defines the box
	float length;
	float width;
	float height;
}Box;

float boxVolume = 0.0;						//volume of the box
Box box = { -1.0,-1.0,-1.0 };				//inital box dimensions (not possible)

static GLfloat boxL = 1.0;					//half of box.length
static GLfloat boxW = 1.0;					//half of box.width
static GLfloat boxH = 2.0;					//just box.height
static GLint wallOrder[4] = { 0, 1, 2, 3 }; //initial wall render order. -z,-x,+x,+z
static GLfloat zNegative[4][3] = {};		//array for vertices for wall facing -z direction				
static GLfloat zPositive[4][3] = {};		//array for vertices for wall facing +z direction
static GLfloat xNegative[4][3] = {};		//array for vertices for wall facing -x direction							
static GLfloat xPositive[4][3] = {};		//array for vertices for wall facing +x direction	

//Radius and Center Representation of Spheres
float r = 0.0;								//radius of spheres

struct center {								//center+radius representation of all of the spheres
	float x, y, z, r;
};

center first;								//The first sphere in the box will be this far away

//Normal Pack
center** SphereLocations;					//2D array holding all of the boxes, [i] = layer , [j] = spheres in the layer
float** SphereCenters;						//numSpheres x 4
int numNormalPack = 0;						//number of spheres in the normal pack

//Dense Pack
center** SphereDenseLocations;				//2D array holding all of the boxes, [i] = layer , [j] = spheres in the layer
float** SphereCentersDense;					//numSpheres x 4
int numDensePack = 0;						//number of spheres in the dense pack

bool packingMode = true;					//true -> normal, false -> dense

//Gets Dimensions of the Initial Box from the User
void getDimensions() {

	printf("Enter the length of the Box: ");
	cin >> box.length;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid length of the Box: ");
		cin >> box.length;
	}

	printf("Enter the width of the Box: ");
	cin >> box.width;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid width of the Box: ");
		cin >> box.width;
	}

	printf("Enter the height of the Box: ");
	cin >> box.height;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid height of the Box: ");
		cin >> box.height;
	}

	printf("\n");
}

//Gets Initial Radius from the User
void getRadiusSpheres() {

	printf("Enter the radius of the Spheres: ");
	cin >> r;

	while (!cin.good())
	{
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		printf("Enter a valid radius of the Sphere: ");
		cin >> r;
	}
	printf("\n");
}

//Calculates Centers of the Spheres in Normal Pack
void getCoodsNormalPack() {

	//gets total number of spheres that fit per length/width/height
	int numLength = (int)(box.length / (r + r));
	int numWidth = (int)(box.width / (r + r));		
	int numHeight = (int)(box.height / (r + r));

	int numArea = (numLength * numWidth);	//how many fit on each layer

	//initalize the 2D array to fit box dimensions
	SphereLocations = new center*[numHeight];
	for (int i = 0; i < numHeight; i++) {
		SphereLocations[i] = new center[numWidth * numLength];	
	}

	first = { r,r,r };						//use this as base to map on, first sphere is "r" away from left plane of box, right plane of box, and bottom of the box

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

	//Copying data over to SphereCenters
	SphereCenters = new float*[numArea * numHeight];
	for (int i = 0; i < (numArea * numHeight); i++) {
		SphereCenters[i] = new float[4];
	}

	int temp = 0;

	//Filling SphereCenters
	for (int i = 0; i < numHeight; i++) {
		for (int j = 0; j < numArea; j++, temp++) {

			SphereCenters[temp][0] = SphereLocations[i][j].x;
			SphereCenters[temp][1] = SphereLocations[i][j].y;
			SphereCenters[temp][2] = SphereLocations[i][j].z;
			SphereCenters[temp][3] = SphereLocations[i][j].r;
		}
	}
	numNormalPack = temp;
}

//Calculates Centers of the Spheres in Dense Pack
void getCoordsDensePack() {

	int numSpheres = 0;
	int longRow = (int)(box.length / (r + r)); //number of spheres that fit in the length (long row)
	int shortRow = longRow - 1;				   //number of spheres that fit in the legnth (short row)

	int numHeight = 0; //how many layers high does this fit?
	float hi = r;
	for (int i = 0; ; i++) {
		if (hi > box.height - r)
			break;
		else {
			hi = hi + ((sqrt(6.0) * r * 2.0) / 3.0);
			numHeight++;
		}
	}

	int numWidth = 0; //how many rows of spheres are there for the width that fit?
	float wi = r;
	for (int i = 0; ; i++) {
		if (wi > box.width - r)
			break;
		else {
			wi = wi + (sqrt(3.0)*r);
			numWidth++;
		}
	}

	int split = (numWidth / 2);

	int numSpheresLongFirstLayer = ((longRow * split) + (shortRow * (numWidth - split)));  //how many fit on each layer, if the long row is first
	int numSpheresShortFirstLayer = ((shortRow * split) + (longRow * (numWidth - split))); //how many fit on each layer, if the short row is first

	//Declares SphereDenseLocations of the Correct Size
	SphereDenseLocations = new center*[numHeight];
	for (int i = 0; i < numHeight; i++) {
		int size = max(numSpheresLongFirstLayer, numSpheresShortFirstLayer);
		if (size < 0) {
			size = 0;
		}
		SphereDenseLocations[i] = new center[size];	//initalize the 2D array to be large enough (size)
	}

	//Initializes SphereDenseLocations with -1, -1, -1
	for (int i = 0; i < numHeight; i++) {
		for (int j = 0; j < numSpheresLongFirstLayer; j++) {
			SphereDenseLocations[i][j] = { -1.0, -1.0, -1.0 };
		}
	}
	first = { r,r,r }; //use this as base to map on, first sphere is "r" away from left plane of box, right plane of box, and bottom of the box

	int k = 0; //used to know when the row is done
	int m = 0; //to know which type of line to make

	for (int i = 0; i < numHeight; i++) { //for all the layers possible

		float h = first.z;	//used for height holder
		float l = first.x;	//used for length holder
		float w = first.y;	//used for width holder
		k = 0;

		if (i % 2 == 0) { //if its the first row, or every odd row that starts with the longSphereFirstLayer
			for (int v = 0; v < numWidth; v++) {
				if (v % 2 == 0) {
					for (int j = 0; j < longRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h,r };

						SphereCentersDense[numSpheres][0] = l;
						SphereCentersDense[numSpheres][1] = w;
						SphereCentersDense[numSpheres][2] = h;
						SphereCentersDense[numSpheres][3] = r;
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = r + r;
				}
				else {
					for (int j = 0; j < shortRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h,r };
						SphereCentersDense[numSpheres][0] = l;
						SphereCentersDense[numSpheres][1] = w;
						SphereCentersDense[numSpheres][2] = h;
						SphereCentersDense[numSpheres][3] = r;
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = first.x;
				}
				w = w + (sqrt(3.0)*r);
			}
			first = { r , r , (float)(first.z + ((sqrt(6.0) * r * 2.0) / 3.0)) };
		}
		else { //else its the even rows so the shortSphereFirstLayer goes

			l = r + r;

			for (int v = 0; v < numWidth; v++) {
				if (v % 2 == 0) {
					for (int j = 0; j < shortRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h,r };
						SphereCentersDense[numSpheres][0] = l;
						SphereCentersDense[numSpheres][1] = w;
						SphereCentersDense[numSpheres][2] = h;
						SphereCentersDense[numSpheres][3] = r;
						++numSpheres;
						l = l + r + r;
						k++;
					}
					l = first.x;
				}
				else {
					for (int j = 0; j < longRow; j++) {
						SphereDenseLocations[i][k] = { l,w,h,r };

						SphereCentersDense[numSpheres][0] = l;
						SphereCentersDense[numSpheres][1] = w;
						SphereCentersDense[numSpheres][2] = h;
						SphereCentersDense[numSpheres][3] = r;
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
	numDensePack = numSpheres;
}

//Prints the Controls to the Screen
void writemessage()
{
	printf("\n\nControls:\n\
		   x/X ----------------------- decrease/increase box length\n\
		   y/Y ----------------------- decrease/increase box height\n\
		   z/Z ----------------------- decrease/increase box depth\n\
		   -/+ ----------------------- zoom out/in\n\
		   p ------------------------- switch pack normal/dense\n\
		   i-------------------------- reprint instructions\n\
		   ESC ----------------------- close\n\
		   Arrow Keys ---------------- move camera view\n\
		   \n");
}

//Initialize Variables
void init() {

	boxL = box.length / 2;
	boxW = box.width / 2;
	boxH = box.height;
	cL = 100.0;
	angleView = 30.0;
}

//Handles When the Window is Resized
void reshape(int w, int h)
{
	ww = w;
	hh = h;
	glViewport(0, 0, (GLsizei)ww, (GLsizei)hh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleView, (GLfloat)ww / (GLfloat)hh, 1.0, 100000.0);
}

//Creates a Transparent Box
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

//Draws the Transparent Box
void drawWalls() {
	int i;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, windowColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, windowColor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64);
	setWallOrder();

	if (cpos[1] < boxH) {
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
	if (cpos[1] > boxH) {
		glBegin(GL_POLYGON);
		glVertex3f(-boxW, boxH, -boxL);
		glVertex3f(-boxW, boxH, boxL);
		glVertex3f(boxW, boxH, boxL);
		glVertex3f(boxW, boxH, -boxL);
		glEnd();
	}
	glDisable(GL_BLEND);
}

//Draws Spheres
void drawSpheres() {
	glPushMatrix();
	glRotatef(90, 0.0, 1.0, 0.0);
	glTranslatef(-boxL, 0.0, -boxW);

	if (packingMode == true) {
		for (int i = 0; i < numNormalPack; i++) {
			if (i % 2 == 0) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
			}
			else if (i % 2 != 0) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cyan);
			}
			glPushMatrix();
			glTranslatef(SphereCenters[i][0], SphereCenters[i][1], SphereCenters[i][2]);
			glutSolidSphere(SphereCenters[i][3], 25, 25);
			glPopMatrix();
		}
	}
	else {
		for (int i = 0; i < numDensePack; i++) {
			if (SphereCentersDense[i][0] < 0.0 || SphereCentersDense[i][1] < 0.0 || SphereCentersDense[i][2] < 0.0 || SphereCentersDense[i][3] <= 0.0) {
				continue;
			}
			if (i % 2 == 0) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
			}
			else if (i % 2 != 0) {
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cyan);
			}
			glPushMatrix();
			glTranslatef(SphereCentersDense[i][0], SphereCentersDense[i][1], SphereCentersDense[i][2]);
			glutSolidSphere(SphereCentersDense[i][3], 25, 25);
			glPopMatrix();
		}
	}
	glPopMatrix();
}

//Displays Box+Spheres
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

//Recalculates Normal and Dense Pack
void translateSpheres() {
	boxVolume = (box.height * box.length * box.width);
	
	//Normal Pack
	getCoodsNormalPack();
	printf("Normal packing real yields: %i Spheres\n", numNormalPack);

	//Flips Y and Z Coordinates for Drawing
	for (int k = 0; k < numNormalPack; k++)
	{
		float x1 = SphereCenters[k][1];
		SphereCenters[k][1] = SphereCenters[k][2];
		SphereCenters[k][2] = x1;
	}

	//Dense Pack
	SphereCentersDense = new float*[boxVolume];
	for (int i = 0; i < boxVolume; i++) {
		SphereCentersDense[i] = new float[4];
	}

	for (int i = 0; i < boxVolume; i++) {
		for (int j = 0; j < 4; j++) {
			SphereCentersDense[i][j] = -1;
		}
	}
	getCoordsDensePack();
	printf("Dense packing real yields: %i Spheres\n", numDensePack);
	
	//Flips Y and Z Coordinates for Drawing
	for (int k = 0; k < numDensePack; k++)
	{
		float x1 = SphereCentersDense[k][1];
		SphereCentersDense[k][1] = SphereCentersDense[k][2];
		SphereCentersDense[k][2] = x1;
	}

	//Determine Which Method is Better
	int minus = numDensePack - numNormalPack;
	if (minus < 0) {
		printf("Normal Pack has %i more.\n", abs(minus));
		printf("You should use Normal Pack!\n\n");
	}
	else if (minus > 0) {
		printf("Dense Pack has %i more.\n", minus);
		printf("You should use Dense Pack!\n\n");
	}
	else {
		printf("There are the same number of spheres!\n");
		printf("It doesn't matter which one!\n\n");
	}
}

//Handles Keyboard Functionality
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
		box.length = boxL * 2;
		translateSpheres();
		glutPostRedisplay();
		break;
	case 'Z':
		if (boxL < 500)
			boxL += 0.1;
		box.length = boxL * 2;
		translateSpheres();
		glutPostRedisplay();
		break;
	case 'x':
		if (boxW > 0.5500000)
			boxW -= 0.1;
		box.width = boxW * 2;
		translateSpheres();
		glutPostRedisplay();
		break;
	case 'X':
		if (boxW < 500)
			boxW += 0.1;
		box.width = boxW * 2;
		translateSpheres();
		glutPostRedisplay();
		break;
	case 'y':
		if (boxH > 0.5500000)
			boxH -= 0.1;
		box.height = boxH;
		translateSpheres();
		glutPostRedisplay();
		break;
	case 'Y':
		if (boxH < 500)
			boxH += 0.1;
		box.height = boxH;
		translateSpheres();
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
		writemessage();
		glutPostRedisplay();
		break;
	case 'p':
		packingMode = !packingMode; //change between normal/dense pack
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

//Handles Arrow Key Functionality
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

//Main
int main(int argc, char** argv)
{
	//Get Box Size
	while (box.height <= 0 || box.length <= 0 || box.width <= 0) {
		getDimensions();

		if (box.height <= 0 || box.length <= 0 || box.width <= 0) {
			printf("%s", "One of your values was less than or equal to Zero, please enter them again. \n\n");
		}
	}

	//Get Radius
	while (r < 1) {
		getRadiusSpheres();

		if (r < 1) {
			printf("%s%f%s", "Your Radius was ", r, ". It must be greater than 1. Please enter a new radius.\n");
		}
	}

	//Calls Normal and Dense Pack
	translateSpheres();

	//Viewing Functionality
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
