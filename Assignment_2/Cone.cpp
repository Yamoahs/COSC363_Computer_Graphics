/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir). 
*/
float Cone::intersect(Vector pos, Vector dir)
{
	float k = pow((radius/height), 2);
	float a = (dir.x * dir.x) + (dir.z * dir.z) - (k * dir.y * dir.y);
	float b = 2 * ((pos.x * dir.x - center.x * dir.x) + (pos.z * dir.z - center.z * dir.z) + ((k * height * dir.y) - (k * pos.y * dir.y) + (k * center.y * dir.y)));
	//float c = pow((pos.x - center.x), 2) + pow((pos.z - center.z), 2) - k * ((pos.y - center.y) * (pos.y - center.y - 2 * height));
	float c = pow((pos.x - center.x), 2) + pow((pos.z - center.z), 2) - k * ((center.y - pos.y + height) * (center.y - pos.y + height));
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
* Assumption: The input point p lies on the cone.
*/
Vector Cone::normal(Vector p)
{
	float alpha = atan((p.x - center.x) / (p.z - center.z));
	float theta = atan(radius/height);
    Vector n = Vector(sin(alpha) *cos(theta), sin(theta), cos(alpha) * cos(theta));
    n.normalise();
    return n;
}
