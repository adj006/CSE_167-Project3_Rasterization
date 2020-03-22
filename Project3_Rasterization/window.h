#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Matrix4.h"
#include "Camera.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <sstream>
#include <vector>

class Window	  // OpenGL output window related routines
{
public:
	static int width, height; 	            // window size

	static Vector3 getMax();
	static Vector3 getMin();
	static void parser(char * fileName);
	static void loadData();
	static void clearBuffer();
	static int f(double distance);
	void drawPoint(int x, int y, float r, float g, float b);
	static void reshapeCallback(int, int);
	static void specialKeysProcess(int key, int x, int y);
	static void idleCallback();
};

#endif

