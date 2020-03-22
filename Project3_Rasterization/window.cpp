#ifdef _WIN32
#include <windows.h>
#endif

#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "Window.h"

#define BRIGHTNESS 300
#define SPIN_NUM 30

static int window_width = 512, window_height = 512;
static float* pixels = new float[window_width * window_height * 3];
static float* zBuffer = new float[window_width * window_height];

using namespace std;

vector<float> positionB;	//position of bunny	
vector<float> normalB;		//normal of bunny
vector<float> positionD;	//position of dragon
vector<float> normalD;		//normal of dragon

int firstTime = 0;

Camera camera;

Matrix4 d;	//viewport matrix
Matrix4 mP;	//projection matrix
Matrix4 ci;	//camera inverse matrix
Matrix4 m;	//model matrix
int FOV = 60;
double mNear = 1.0;
double mFar = 1000.0;

Vector4 pP;		//point vecror
Vector4 pPrime;	//

Vector3 vec3;
Vector3 minB;
Vector3 maxB;
Vector3 minD;
Vector3 maxD;

char * bunny = "Bunny.xyz";
char * dragon = "Dragon.xyz";
int pointCloud = 2;
int keyNum = 1;

// These are the x,y,z coordinates of the vertices of the triangles
float vertices[] = {
	-4, -4, 4, 4, -4, 4, 4, 4, 4, -4, 4, 4,     // front face
	-4, -4, -4, -4, -4, 4, -4, 4, 4, -4, 4, -4, // left face
	4, -4, -4, -4, -4, -4, -4, 4, -4, 4, 4, -4,  // back face
	4, -4, 4, 4, -4, -4, 4, 4, -4, 4, 4, 4,     // right face
	4, 4, 4, 4, 4, -4, -4, 4, -4, -4, 4, 4,     // top face
	-4, -4, 4, -4, -4, -4, 4, -4, -4, 4, -4, 4, // bottom face

	-20, -4, 20, 20, -4, 20, 20, -4, -20, -20, -4, -20, // grass
	-4, 4, 4, 4, 4, 4, 0, 8, 4,                       // front attic wall
	4, 4, 4, 4, 4, -4, 0, 8, -4, 0, 8, 4,               // left slope
	-4, 4, 4, 0, 8, 4, 0, 8, -4, -4, 4, -4,             // right slope
	4, 4, -4, -4, 4, -4, 0, 8, -4 };                   // rear attic wall


struct Color    // generic color class
{
	float r, g, b;  // red, green, blue
};

Vector3 getMax()

{

	Vector3 max;

	if (pointCloud == 0)
	{
		max.v3[0] = positionB[0];

		max.v3[1] = positionB[1];

		max.v3[2] = positionB[2];


		for (int i = 3; i < positionB.size() - 3; i += 3)

		{

			max.v3[0] = ((max.v3[0] > positionB[i]) ? max.v3[0] : positionB[i]);

			max.v3[1] = ((max.v3[1] > positionB[i + 1]) ? max.v3[1] : positionB[i + 1]);

			max.v3[2] = ((max.v3[2] > positionB[i + 2]) ? max.v3[2] : positionB[i + 2]);

		}
	}

	else if (pointCloud == 1)
	{
		max.v3[0] = positionD[0];

		max.v3[1] = positionD[1];

		max.v3[2] = positionD[2];


		for (int i = 3; i < positionD.size() - 3; i += 3)

		{

			max.v3[0] = ((max.v3[0] > positionD[i]) ? max.v3[0] : positionD[i]);

			max.v3[1] = ((max.v3[1] > positionD[i + 1]) ? max.v3[1] : positionD[i + 1]);

			max.v3[2] = ((max.v3[2] > positionD[i + 2]) ? max.v3[2] : positionD[i + 2]);

		}
	}

	return max;
}

Vector3 getMin()

{

	Vector3 min;

	if (pointCloud == 0)
	{
		min.v3[0] = positionB[0];

		min.v3[1] = positionB[1];

		min.v3[2] = positionB[2];

		for (int i = 3; i < positionB.size() - 3; i += 3)

		{

			min.v3[0] = ((min.v3[0] < positionB[i]) ? min.v3[0] : positionB[i]);

			min.v3[1] = ((min.v3[1] < positionB[i + 1]) ? min.v3[1] : positionB[i + 1]);

			min.v3[2] = ((min.v3[2] < positionB[i + 2]) ? min.v3[2] : positionB[i + 2]);

		}
	}

	else if (pointCloud == 1)
	{
		min.v3[0] = positionD[0];

		min.v3[1] = positionD[1];

		min.v3[2] = positionD[2];

		for (int i = 3; i < positionD.size() - 3; i += 3)

		{

			min.v3[0] = ((min.v3[0] < positionD[i]) ? min.v3[0] : positionD[i]);

			min.v3[1] = ((min.v3[1] < positionD[i + 1]) ? min.v3[1] : positionD[i + 1]);

			min.v3[2] = ((min.v3[2] < positionD[i + 2]) ? min.v3[2] : positionD[i + 2]);

		}
	}

	return min;
}

void parser(char * fileName)
{
	if (strcmp(fileName, bunny) == 0)
	{
		if (firstTime == 1)
		{
			positionB.clear();
			normalB.clear();
		}
		ifstream file;
		file.open(fileName);
		int numOfPoints = 0;
		double x1, y1, z1, x2, y2, z2;
		if (file.is_open())
		{
			while (!file.eof())
			{
				file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;

				positionB.push_back(x1);
				positionB.push_back(y1);
				positionB.push_back(z1);
				normalB.push_back(x2);
				normalB.push_back(y2);
				normalB.push_back(z2);

				numOfPoints += 3;
			}
			numOfPoints *= 2;
		}
		file.close();
		Vector3 v;
		for (int x = 0; x < normalB.size(); x += 3)
		{
			v.v3[0] = normalB[x];
			v.v3[1] = normalB[x + 1];
			v.v3[2] = normalB[x + 2];

			v.normalize();

			normalB[x] = v.v3[0];
			normalB[x + 1] = v.v3[1];
			normalB[x + 2] = v.v3[2];
		}
		minB = getMin();
		maxB = getMax();
	}

	else if (strcmp(fileName, dragon) == 0)
	{
		if (firstTime == 1)
		{
			positionD.clear();
			normalD.clear();
		}
		ifstream file;
		file.open(fileName);
		int numOfPoints = 0;
		double x1, y1, z1, x2, y2, z2;
		if (file.is_open())
		{
			while (!file.eof())
			{
				file >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;

				positionD.push_back(x1);
				positionD.push_back(y1);
				positionD.push_back(z1);
				normalD.push_back(x2);
				normalD.push_back(y2);
				normalD.push_back(z2);

				numOfPoints += 3;
			}
			numOfPoints *= 2;
		}
		file.close();
		Vector3 v;
		for (int x = 0; x < normalD.size(); x += 3)
		{
			v.v3[0] = normalD[x];
			v.v3[1] = normalD[x + 1];
			v.v3[2] = normalD[x + 2];

			v.normalize();

			normalD[x] = v.v3[0];
			normalD[x + 1] = v.v3[1];
			normalD[x + 2] = v.v3[2];
		}
		minD = getMin();
		maxD = getMax();
	}
}

void loadData()
{
	// point cloud parser goes here
	pointCloud = 0;
	parser(bunny);
	pointCloud = 1;
	parser(dragon);
	pointCloud = 2;
}

// Clear frame buffer
void clearBuffer()
{
	Color clearColor = { 0.0, 0.0, 0.0 };   // clear color: black
	for (int i = 0; i<window_width*window_height; ++i)
	{
		pixels[i * 3] = clearColor.r;
		pixels[i * 3 + 1] = clearColor.g;
		pixels[i * 3 + 2] = clearColor.b;
		zBuffer[i] = 1;
	}
}

int f(double distance)
{
	double range;
	double interval;
	double min;
	double max;
	if (pointCloud == 0)
	{
		min = minB.v3[2];
		max = maxB.v3[2];
		range = max - min;
		interval = (distance - min) / range;
		if ((interval > (4.0 / 5.0)) && (interval <= 1.0))
			return 1;
		else if ((interval > (3.0 / 5.0)) && (interval <= (4.0 / 5.0)))
			return 2;
		else if ((interval > (2.0 / 5.0)) && (interval <= (3.0 / 5.0)))
			return 3;
		else if ((interval > (1.0 / 5.0)) && (interval <= (2.0 / 5.0)))
			return 4;
		else if ((interval > 0) && (interval <= (1.0 / 5.0)))
			return 5;
		/*if ((interval > (2.0 / 3.0)) && (interval <= 1.0))
			return 1;
		else if ((interval > (1.0 / 3.0)) && (interval <= (2.0 / 3.0)))
			return 2;
		else if ((interval > 0) && (interval <= (1.0 / 3.0)))
			return 3;*/
	}
	else if (pointCloud == 1)
	{
		min = minD.v3[2];
		max = maxD.v3[2];
		range = max - min;
		interval = (distance - min) / range;
		if ((interval > (4.0 / 5.0)) && (interval <= 1.0))
			return 1;
		else if ((interval > (3.0 / 5.0)) && (interval <= (4.0 / 5.0)))
			return 2;
		else if ((interval > (2.0 / 5.0)) && (interval <= (3.0 / 5.0)))
			return 3;
		else if ((interval > (1.0 / 5.0)) && (interval <= (2.0 / 5.0)))
			return 4;
		else if ((interval > 0) && (interval <= (1.0 / 5.0)))
			return 5;
		/*if ((interval > (2.0 / 3.0)) && (interval <= 1.0))
			return 1;
		else if ((interval > (1.0 / 3.0)) && (interval <= (2.0 / 3.0)))
			return 2;
		else if ((interval > 0) && (interval <= (1.0 / 3.0)))
			return 3;*/
	}
}

// Draw a point into the frame buffer
void drawPoint(int x, int y, float r, float g, float b)
{
	int offset = y*window_width * 3 + x * 3;
	pixels[offset] = r;
	pixels[offset + 1] = g;
	pixels[offset + 2] = b;
}

void rasterize()
{
	// Put your main rasterization loop here
	// It should go over the point model and call drawPoint for every point in it
	Vector3 vec1;
	vec1.v3[0] = 0;
	vec1.v3[1] = 0;
	vec1.v3[2] = 1;

	Vector3 vec2;
	vec2.v3[0] = 0;
	vec2.v3[1] = 1;
	vec2.v3[2] = 0;

	Vector3 vec3;
	vec3.v3[0] = 0;
	vec3.v3[1] = 0;
	vec3.v3[2] = 0;

	Matrix4 trans;
	trans.identity();
	trans.makeTranslate(0, 0, -30);

	Matrix4 scale;
	scale.identity();
	//scale.makeScale(100, 100, 100);

	Matrix4 transScale;
	transScale.identity();

	Camera camera(vec1, vec2, vec3);
	camera.inverse();
	ci = camera.cam;
	ci = trans * ci;

	Vector4 pL4;
	Vector4 nL4;

	Vector3 light;
	Vector3 pL3;
	Vector3 nL3;
	Vector3 posL;
	Vector3 color;
	Vector3 lightColor;
	Vector3 comp;
	Vector3 shadeColor;


	posL.v3[0] = -5;
	posL.v3[1] = 5;
	posL.v3[2] = 5;

	double dist;

	if (firstTime == 0)
		m.identity();


	int size;
	if (pointCloud == 2)
		size = 126;
	else if (pointCloud == 0)
		size = positionB.size();
	else if (pointCloud == 1)
		size = positionD.size();
	for (int i = 0; i < size - 3; i+=3)
	{
		if (pointCloud == 2)
		{
			pP.v4[0] = vertices[i];
			pP.v4[1] = vertices[i + 1];
			pP.v4[2] = vertices[i + 2];
		}
		else if (pointCloud == 0)
		{
			pP.v4[0] = positionB[i];
			pP.v4[1] = positionB[i + 1];
			pP.v4[2] = positionB[i + 2];

			nL4.v4[0] = normalB[i];
			nL4.v4[1] = normalB[i + 1];
			nL4.v4[2] = normalB[i + 2];
			nL4.v4[3] = 0;

			if ((maxB.v3[0] - minB.v3[0]) > (maxB.v3[1] - minB.v3[1]))
			{
				double sF;
				sF = 40 * tan((FOV / 2)*(M_PI / 180));
				sF = sF / (maxB.v3[0] - minB.v3[0]);
				scale.makeScale(sF, sF, sF);
			}
			else
			{
				double sF;
				sF = 40 * tan((FOV / 2)*(M_PI / 180));
				sF = sF / (maxB.v3[1] - minB.v3[1]);
				scale.makeScale(sF, sF, sF);
			}
			transScale.makeTranslate(-(maxB.v3[0] + minB.v3[0]) / 2, -(maxB.v3[1] + minB.v3[1]) / 2, -(maxB.v3[2] + minB.v3[2]) / 2);
		}
		else if (pointCloud == 1)
		{
			pP.v4[0] = positionD[i];
			pP.v4[1] = positionD[i + 1];
			pP.v4[2] = positionD[i + 2];

			nL4.v4[0] = normalD[i];
			nL4.v4[1] = normalD[i + 1];
			nL4.v4[2] = normalD[i + 2];
			nL4.v4[3] = 0;

			if ((maxD.v3[0] - minD.v3[0]) > (maxD.v3[1] - minD.v3[1]))
			{
				double sF;
				sF = 40 * tan((FOV / 2)*(M_PI / 180));
				sF = sF / (maxD.v3[0] - minD.v3[0]);
				scale.makeScale(sF, sF, sF);
			}
			else
			{
				double sF;
				sF = 40 * tan((FOV / 2)*(M_PI / 180));
				sF = sF / (maxD.v3[1] - minD.v3[1]);
				scale.makeScale(sF, sF, sF);
			}

			transScale.makeTranslate(-(maxD.v3[0] + minD.v3[0]) / 2, -(maxD.v3[1] + minD.v3[1]) / 2, -(maxD.v3[2] + minD.v3[2]) / 2);
		}
		pP.v4[3] = 1;
		pPrime = m * pP;

		pL3.v3[0] = pPrime.v4[0];
		pL3.v3[1] = pPrime.v4[1];
		pL3.v3[2] = pPrime.v4[2];

		nL4 = m * nL4;

		nL3.v3[0] = nL4.v4[0];
		nL3.v3[1] = nL4.v4[1];
		nL3.v3[2] = nL4.v4[2];

		light = posL - pL3;
		dist = light.length();
		light.normalize();

		double tmpNumerator;
		tmpNumerator = nL3.dot(nL3, light);
		tmpNumerator = tmpNumerator * BRIGHTNESS;

		double tmpScale;
		tmpScale = tmpNumerator / (pow(dist, 2)*M_PI);

		color.v3[0] = 1;
		color.v3[1] = 1;
		color.v3[2] = 1;

		lightColor.v3[0] = 1;
		lightColor.v3[1] = 1;
		lightColor.v3[2] = 1;

		comp = lightColor * color;
		comp.scale(tmpScale);

		if (keyNum == 1)
		{
			shadeColor.v3[0] = 1;
			shadeColor.v3[1] = 1;
			shadeColor.v3[2] = 1;
		}
		else if (keyNum == 2)
		{
			shadeColor = comp;
		}

		pPrime = transScale * pPrime;
		pPrime = scale * pPrime;
		pPrime = ci * pPrime;
		pPrime = mP * pPrime;
		pPrime.dehomogenize();
		if ((pPrime.v4[0] >= -1 && pPrime.v4[0] <= 1) && (pPrime.v4[1] >= -1 && pPrime.v4[1] <= 1) && (pPrime.v4[2] >= 0 && pPrime.v4[2] <= 1))
		{
			pPrime = d * pPrime;
			if (keyNum == 3)
			{
				double xP = pPrime.v4[0];
				double yP = pPrime.v4[1];
				shadeColor = comp;
				if (zBuffer[(int)(yP*window_width) + (int)xP] > pPrime.v4[2])
				{
					zBuffer[i / 3] = pPrime.v4[2];
					drawPoint(pPrime.v4[0], pPrime.v4[1], shadeColor.v3[0], shadeColor.v3[1], shadeColor.v3[2]);
				}
			}
			else if (keyNum == 4)
			{
				double xP = pPrime.v4[0];
				double yP = pPrime.v4[1];
				shadeColor = comp;
				if (zBuffer[(int)(yP*window_width) + (int)xP] > pPrime.v4[2])
				{
					zBuffer[i / 3] = pPrime.v4[2];
					int pSize = f(pPrime.v4[2]);
					for (int i = -pSize; i < pSize; i++)
					{
						for (int j = -pSize; j < pSize; j++)
						{
							double x = (double)i;
							double y = (double)j;

							x /= (window_width/45);
							y /= (window_height/45);

							if ((pPrime.v4[0] + x >= 0 && pPrime.v4[0] + x <= window_width) && (pPrime.v4[1] + y >= 0 && pPrime.v4[1] + y <= window_height) && (pPrime.v4[2] >= 0 && pPrime.v4[2] <= 1))
								drawPoint(pPrime.v4[0] + x, pPrime.v4[1] + y, shadeColor.v3[0], shadeColor.v3[1], shadeColor.v3[2]);
						}
					}
				}
			}
			else
			{
				drawPoint(pPrime.v4[0], pPrime.v4[1], shadeColor.v3[0], shadeColor.v3[1], shadeColor.v3[2]);
			}

		}
	}
}

void displayCallback()
{
	clearBuffer();
	rasterize();

	// glDrawPixels writes a block of pixels to the framebuffer
	glDrawPixels(window_width, window_height, GL_RGB, GL_FLOAT, pixels);

	glutSwapBuffers();
}

// Called whenever the window size changes
void reshapeCallback(int new_width, int new_height)
{
	window_width = new_width;
	window_height = new_height;
	cout << "width:" << window_width << endl;
	cout << "height:" << window_height << endl;
	delete[] pixels;
	delete[] zBuffer;
	pixels = new float[window_width * window_height * 3];
	zBuffer = new float[window_width * window_height];
	d.viewportMatrix(0, window_width, 0, window_height);
	mP.perProjectionMatrix(window_width / window_height, FOV, mNear, mFar);
	displayCallback();
}

void keyboardCallback(unsigned char key, int, int)
{
	cerr << "Key pressed: " << key << endl;

	Matrix4 m4;
	m4.identity();
	switch (key)
	{
	case 's':
		m4.makeScale(.8, .8, .8);
		m = m * m4;
		displayCallback();
		break;
	case 'S':
		m4.makeScale(1.2, 1.2, 1.2);
		m = m * m4;
		displayCallback();
		break;
	case 'r':
		m4.makeRotateY(SPIN_NUM);
		m = m * m4;
		displayCallback();
		break;
	case 'R':
		m4.makeRotateY(-SPIN_NUM);
		m = m * m4;
		displayCallback();
		break;
	case '1':
		keyNum = 1;
		displayCallback();
		break;
	case '2':
		keyNum = 2;
		displayCallback();
		break;
	case '3':
		keyNum = 3;
		displayCallback();
		break;
	case '4':
		keyNum = 4;
		displayCallback();
		break;
	}

	vec3.v3[0] = m.m[0][3];
	vec3.v3[1] = m.m[1][3];
	vec3.v3[2] = m.m[2][3];
	vec3.print("New Position is:");
}

void specialKeysProcess(int key, int x, int y)
{

	switch (key)
	{
	case GLUT_KEY_F1:
		cout << "Bunny" << endl;
		firstTime = 1;
		pointCloud = 0;
		displayCallback();
		break;
	case GLUT_KEY_F2:
		cout << "Dragon" << endl;
		firstTime = 1;
		pointCloud = 1;
		displayCallback();
		break;
	}
}

void Window::idleCallback()
{
	displayCallback();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Rasterizer");

	loadData();

	glutReshapeFunc(reshapeCallback);
	glutDisplayFunc(displayCallback);
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialKeysProcess);
	glutMainLoop();
}