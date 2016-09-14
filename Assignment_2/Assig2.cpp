// ========================================================================
// COSC 363  Computer Graphics  Lab07
// A simple ray tracer
// ========================================================================

#include <iostream>
#include <cmath>
#include <vector>
#include "Vector.h"
#include "Sphere.h"
#include "Color.h"
#include "Object.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "stdio.h"
#include <GL/glut.h>
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int PPU = 50;     //Total 600x600 pixels
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<Object*> sceneObjects;


Vector light;
Color backgroundCol;
Color groundCheck;
Color ballColTwo;
TextureBMP texture1;
TextureBMP texture2;

//A useful struct
struct PointBundle   
{
	Vector point;
	int index;
	float dist;
};

/*
* This function compares the given ray with all objects in the scene
* and computes the closest point  of intersection.
*/
PointBundle closestPt(Vector pos, Vector dir)
{
    Vector  point(0, 0, 0);
	float min = 10000.0;

	PointBundle out = {point, -1, 0.0};

    for(unsigned int i = 0;  i < sceneObjects.size();  i++)
	{
        float t = sceneObjects[i]->intersect(pos, dir);
		if(t > 0)        //Intersects the object
		{
			point = pos + dir*t;
			if(t < min)
			{
				out.point = point;
				out.index = i;
				out.dist = t;
				min = t;
			}
		}
	}

	return out;
}

/*
* Computes the colour value obtained by tracing a ray.
* If reflections and refractions are to be included, then secondary rays will 
* have to be traced from the point, by converting this method to a recursive
* procedure.
*/

Color trace(Vector pos, Vector dir, int step)
{
    PointBundle q = closestPt(pos, dir);
    float lDotn;
    float refleCoeff = 0.9;
    float transparentCoeff = 0.4;
    float airIndex = 1.0;
    float glassIndex = 1.5;
    Vector reflectionVector;
    Vector v(-dir.x, -dir.y, -dir.z); //View vector;
    Color colorSum;
    

    if(q.index == -1) return backgroundCol;        //no intersection
    
    Vector n = sceneObjects[q.index]->normal(q.point); 
    Vector l = light - q.point;
    l.normalise();
    lDotn = l.dot(n);
    Vector r = ((n * 2) * lDotn) - l; // r = 2(L.n)n – L. ‘l’ = el
	r.normalise();

    Color col = sceneObjects[q.index]->getColor(); //Object's colour
    
    
		
	//generate the pattern ball floor
	if (q.index == 2){
		if((int)(q.point.y - 7) % 2){
			col = ballColTwo;
		}
	}
	
	//generate the texture mapped ball
	if (q.index == 0)
	{
					
		float u = asin(n.x)/M_PI + 0.5;
		float v = asin(n.y)/M_PI + 0.5;
	
		col = texture1.getColorAt(u, v);
		
	}
    
    Vector lightVector = light - q.point;
	float lightDist = lightVector.length(); //Distance to light
	lightVector.normalise();
	PointBundle s = closestPt(q.point, lightVector);
	
	//shadow stuff
	if(s.index>-1 && s.dist < lightDist)
	colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
    
    
    else if (lDotn <= 0){
		colorSum = col.phongLight(backgroundCol, 0.0, 0.0);
	}
		
	else
	{
		
		float rDotv = r.dot(v);
		float spec;
		if(rDotv < 0) spec = 0.0;
		else spec = pow(rDotv, 10); //Phong exponent = 10
		
		//return col.phongLight(backgroundCol, lDotn, spec);
		colorSum = col.phongLight(backgroundCol, lDotn, spec);
		
	}
	
	

	//generate the reflection ray
	if (q.index == 1 && step < MAX_STEPS)
	{
		reflectionVector = ((n*2)*(n.dot(v))) - v;
		Color reflectionCol = trace(q.point, reflectionVector, step+1);
		colorSum.combineColor (reflectionCol, refleCoeff);
		//return reflectionCol;
	}
		
	//generate the transparent ray
	if (q.index == 4 && step < MAX_STEPS)
	{
		Color transparentCol = trace(q.point, dir, step+1);
		colorSum.combineColor (transparentCol, transparentCoeff);
		//return reflectionCol;
	}
		
		
	//~ //generate the refracted ray
		//~ if (q.index == 6 && step < MAX_STEPS)
		//~ {
			//~ float ratio = airIndex / glassIndex;
			//~ Color transparentCol = trace(q.point, dir, step+1);
			//~ colorSum.combineColor (transparentCol, transparentCoeff);
			//~ //return reflectionCol;
		//~ }
		
	//generate the checkerboard floor
	if (q.index == 3){
		int modx = (int)((q.point.x - 200) /2) % 2;
		int modz = (int)((q.point.z - 200) /2) % 2;
		
		if((modx && modz) || (!modx && !modz)){
			colorSum = col.phongLight(groundCheck, 0.0, 0.0);
		}
	}
		
	
		
	return colorSum;
	
    

}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each pixel as quads.
//---------------------------------------------------------------------------------------
void display()
{
	int widthInPixels = (int)(WIDTH * PPU);
	int heightInPixels = (int)(HEIGHT * PPU);
	float pixelSize = 1.0/PPU;
	float halfPixelSize = pixelSize/2.0;
	float quarterPixelSize = pixelSize/4.0;
	float x1, y1, xc, yc;
	Vector eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);  //Each pixel is a quad.

	for(int i = 0; i < widthInPixels; i++)	//Scan every "pixel"
	{
		x1 = XMIN + i*pixelSize;
		xc = x1 + halfPixelSize;
		//xc = x1 + quarterPixelSize;
		for(int j = 0; j < heightInPixels; j++)
		{
			y1 = YMIN + j*pixelSize;
			yc = y1 + halfPixelSize;
		

			//Anti-Aliasing below
		    Vector dir(xc - quarterPixelSize, yc - quarterPixelSize, -EDIST);	//direction of the primary 1st quarter ray
		    Vector dir2(xc + quarterPixelSize, yc - quarterPixelSize, -EDIST);	//direction of the primary 2nd quarter ray
		    Vector dir3(xc + quarterPixelSize, yc + quarterPixelSize, -EDIST);	//direction of the primary 3rd quarter ray
		    Vector dir4(xc - quarterPixelSize, yc + quarterPixelSize, -EDIST);	//direction of the primary 4th quarter ray

		    dir.normalise();			//Normalise this direction
		    dir2.normalise();			//Normalise this direction
		    dir3.normalise();			//Normalise this direction
		    dir4.normalise();			//Normalise this direction

		    Color col = trace (eye, dir, 1); //Trace the primary ray and get the colour value
		    Color col2 = trace (eye, dir2, 1); //Trace the primary ray and get the colour value
		    Color col3 = trace (eye, dir3, 1); //Trace the primary ray and get the colour value
		    Color col4 = trace (eye, dir4, 1); //Trace the primary ray and get the colour value
		    float rCol = (col.r + col2.r + col3.r + col4.r) / 4;
		    float gCol = (col.g + col2.g + col3.g + col4.g) / 4;
		    float bCol = (col.b + col2.b + col3.b + col4.b) / 4;
			glColor3f(rCol, gCol, bCol);
			
			glVertex2f(x1, y1);				//Draw each pixel with its color value
			glVertex2f(x1 + pixelSize, y1);
			glVertex2f(x1 + pixelSize, y1 + pixelSize);
			glVertex2f(x1, y1 + pixelSize);
        }
    }

    glEnd();
    glFlush();
}

void cube()
{
	float scale = 0.5;
	float x = 2 * scale;
	float y = 4 * scale;
	float z = 10;
	
	Plane *cube1 = new Plane(Vector(x-x, -y, -(z-x)), Vector(-x, -y, -z), Vector(x-x, -y, -(z+x)), Vector(x, -y, -z), Color(1, 1, 0)); // bottom
	Plane *cube2 = new Plane(Vector(x, -y, -z), Vector(x - x, -y, -(z+x)), Vector(x-x, y-y, -(z+x)), Vector(x, y-y, -z), Color(1, 1, 0));  //Right
	Plane *cube3 = new Plane(Vector(x-x, y-y, -(z-x)), Vector(x, y-y, -z), Vector(x-x, y-y, -(z+x)), Vector(-x, -y, -z), Color(1, 1, 0)); //Top
	Plane *cube4 = new Plane(Vector(-x, -y, -z), Vector(x-x, -y, -(z-x)), Vector(x-x, y-y, -(z-x)), Vector(-x, y-y, -z), Color(1, 1, 0));  //left
	Plane *cube5 = new Plane(Vector(x-x, -y, -(z-x)), Vector(x, -y, -z), Vector(x, y-y, -z), Vector(x-x, y-y, -(z-x)), Color(1, 1, 0)); //Front
	Plane *cube6 = new Plane(Vector(-x, -y, -z), Vector(-x, y-y, -z), Vector(x-x, y-y, -(z-x)), Vector(x-x, -y, -(z-x)), Color(1, 1, 0)); //Back
	
	sceneObjects.push_back(cube1);
	sceneObjects.push_back(cube2);
	sceneObjects.push_back(cube3);
	sceneObjects.push_back(cube4);
	sceneObjects.push_back(cube5);
	sceneObjects.push_back(cube6);
	
}

void tetrahedron()
{
	Plane *tetra1 = new Plane(Vector(13, 5, -55), Vector(10.5, 10, -57.5), Vector(8, 5, -55),Vector(13, 5, -55), Color::RED);
	Plane *tetra2 = new Plane(Vector(13, 5, -55), Vector(8, 5, -55), Vector(10.5, 10, -57.5), Vector(13, 5, -55), Color::RED);
	Plane *tetra3 = new Plane(Vector(10.5, 5, -60), Vector(8, 5, -55), Vector(10.5, 10, -57.5), Vector(10.5, 5, -60), Color::RED);
	Plane *tetra4 = new Plane(Vector(10.5, 5, -60), Vector(13, 5, -55), Vector(8, 5, -55), Vector(10.5, 5, -60), Color::RED);
	
	sceneObjects.push_back(tetra1);
	sceneObjects.push_back(tetra2);
	sceneObjects.push_back(tetra3);
	sceneObjects.push_back(tetra4);
}


void initialize()
{
	//Iniitialize background colour and light's position
	backgroundCol = Color::GRAY;
	groundCheck = Color::PURPLE;
	ballColTwo = Color::PURPLE;
	light = Vector(10.0, 40.0, -5.0);
	texture1 = TextureBMP((char*)"Coke.bmp");

	//Add objects to the list of scene objects here.
	Sphere *sphere1 = new Sphere(Vector(-7, 5, -50), 3.0, Color::RED);
	Sphere *sphere2 = new Sphere(Vector(0, 7, -70), 10.0, Color::BLUE);
	Sphere *sphere3 = new Sphere(Vector(8, 0, -60), 3.0, Color::GREEN);
	Plane *ground = new Plane(Vector(-200, -10, -40), Vector(200, -10, -40), Vector(200., -10, -300), Vector(-200., -10, -300), Color(1, 0, 1));
	Cylinder *cylinder = new Cylinder(Vector(-8, -10, -60) , 4.0, 4.0, Color::WHITE);
	Cone *cone = new Cone(Vector(-8, -6, -60) , 4.0, 6.0, Color::BLUE);
	Cylinder *coke = new Cylinder(Vector(8, -10, -60) , 2.0, 8.0, Color::ORANGE);
	

	
	sceneObjects.push_back(sphere1);
	sceneObjects.push_back(sphere2);
	sceneObjects.push_back(sphere3);
	sceneObjects.push_back(ground);
	sceneObjects.push_back(cylinder);
	sceneObjects.push_back(cone);
	sceneObjects.push_back(coke);

	
	cube();
	tetrahedron();
	

	//The following are OpenGL functions used only for drawing the pixels
	//of the ray-traced scene.
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);
}


int main(int argc, char *argv[]) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(20000, 20);
    glutCreateWindow("Raytracing Assignment");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
