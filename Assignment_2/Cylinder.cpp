/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(Vector pos, Vector dir)
{
   float a = ((dir.x * dir.x) + (dir.z * dir.z));
   float b = 2 * (dir.x * (pos.x - center.x) + dir.z * (pos.z - center.z));
   float c = pow(pos.x - center.x, 2) + pow(pos.z - center.z, 2) - radius * radius;
   float delta = (b * b - 4.0 * a * c);
   
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta))  / (2 * a);
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) return t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;
    
    float max;
    float min;

	if(t1 > t2){
		max = t1;
		min = t2;
	}
	else {
		max = t2;
		min = t1;
	}
	float y1 = (pos.y + min * dir.y) - center.y;
	float y2 = (pos.y + max * dir.y) - center.y;
	if(! (y1 < 0 || y1 > height) && min != -1.0) return min;
	else if(! (y2 < 0 || y2 > height) && max != -1.0) return max;
	else return -1.0;
	
	
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
Vector Cylinder::normal(Vector p)
{
    Vector n = Vector(p.x - center.x, 0.0, p.z - center.z);
    n.normalise();
    return n;
}
