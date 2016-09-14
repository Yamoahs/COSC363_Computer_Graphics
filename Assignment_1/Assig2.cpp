//  ========================================================================
//  COSC363: Computer Graphics (2015);  University of Canterbury.
//
//  FILE NAME: Vase.cpp
//  See Lab04 (II) for details
//  ========================================================================
 
#include <iostream>
#include <cmath> 
#include <GL/freeglut.h>
#include "loadBMP.h"
#include "loadTGA.h"
#include <iostream>

using namespace std;

const int N = 4;					// Total number of vertices on the Enterprise and miniSpaceships
GLuint txId[5];						//Texture ID's
const float CDR = 3.14159265/180.0;
float alien_x = 0;
float alien_y = 0;
float inital_vel = 80;
float alien_time = 0;
float alienTheta = 30.0;
int moveAlien = 0;
float colorTime = 0;

int enterpriseHgt = 0;
bool takeOff = false;
bool landing = false;
bool enterpriseOrbit_set = false;
bool enterpriseOrbit_set2 = false;
int enterpriseTheta = 0;
int enterpriseOrbit = 0;
int max_enterpriseOrbit = 400;
int max_enterpriseHgt = 300;
 
float vx[N] = {0, 50, 50, 0};
float vy[N] = {0, 5, 10, 15};
float vz[N] = {0};
float miniShipTheta = 0;
int miniRotation = 0;

float xz_angle = 290;				//Rotation xz_angle
float yz_angle = -0.5;				//Rotation yz_angle
float cam_x = 500, cam_hgt = 500.0,  cam_z = 400;		//Initial Camera Position
float lookpoint_x = 500., lookpoint_y = 500., lookpoint_z = 350.;
float dir_x = -sin(xz_angle), dir_y = 0,  dir_z = -cos(xz_angle); 
int pan_x = 0, pan_y = 0, pan_z = 0;				//Moving the Camera in x, y or z direction

//----------------------------------------------------------------------
void loadTexture()				
{
	glGenTextures(5, txId); 	// Create 2 texture ids

	glBindTexture(GL_TEXTURE_2D, txId[0]);  //Use this texture
    loadTGA((char*)"resources/Skybox/Skybox.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Skybox Walls Texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[1]);  //Use this texture
    loadTGA((char*)"resources/Skybox/ground.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Skybox Ground
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[2]);  //Use this texture
    loadTGA((char*)"resources/miniSpaceship.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//MiniSpacehip1 Texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[3]);  //Use this texture
    loadTGA((char*)"resources/Enterprise.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Enterprise Texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, txId[4]);  //Use this texture
    loadTGA((char*)"resources/miniSpaceship2.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//MiniSpacehip2 Texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

//-------------------------------------------------------------------
void initialise(void) 
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};
    float green[4]  = {0., 0.41, 0., 1.0};
    
    loadTexture();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glEnable(GL_SMOOTH);
	
	//Light1
    glLightfv(GL_LIGHT1, GL_AMBIENT, green);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, green);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,0.001);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor (0, 0, 0, 0.0);
    
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[5]);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(66.0, 1.0, 20.0, 2700.0);
    
}
//----------------------------------------------------------------------

void normal(float x1, float y1, float z1, 
            float x2, float y2, float z2,
		      float x3, float y3, float z3 )
{
	  float nx, ny, nz;
	  nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
	  ny = -(z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2));
	  nz = -(x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2));

      glNormal3f(nx, ny, nz);
}
//----------------------------------------------------------------------

void skybox()
{
	int size = 1000;
	//float white[4] = {1., 1., 1., 1.};
	//float grey[4] = {134.0, 134.0, 134.0, 1.0};
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glBegin(GL_QUADS);					//Front Wall
		glTexCoord2f(0,1); glVertex3f(-size, size, size);
		glTexCoord2f(0,0); glVertex3f(-size, 0, size);
		glTexCoord2f(1,0); glVertex3f(size, 0, size);
		glTexCoord2f(1,1); glVertex3f(size, size, size);
	glEnd();
	
	
	glBegin(GL_QUADS);					//Back Wall
		glTexCoord2f(0,1); glVertex3f(-size, size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, 0, -size);
		glTexCoord2f(1,0); glVertex3f(size, 0, -size);
		glTexCoord2f(1,1); glVertex3f(size, size, -size);
	glEnd();
	
	glBegin(GL_QUADS);			 		//Left Wall
		glTexCoord2f(0,1); glVertex3f(-size, size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, 0, -size);
		glTexCoord2f(1,0); glVertex3f(-size, 0, size);
		glTexCoord2f(1,1); glVertex3f(-size, size, size);
	glEnd();
	
	glBegin(GL_QUADS);			 		//right Wall
		glTexCoord2f(0,1); glVertex3f(size, size, -size);
		glTexCoord2f(0,0); glVertex3f(size, 0, -size);
		glTexCoord2f(1,0); glVertex3f(size, 0, size);
		glTexCoord2f(1,1); glVertex3f(size, size, size);
	glEnd();
	
	glBegin(GL_QUADS);			 		//Top
		glTexCoord2f(0,1); glVertex3f(-size, size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, size, size);
		glTexCoord2f(1,0); glVertex3f(size, size, size);
		glTexCoord2f(1,1); glVertex3f(size, size, -size);
	glEnd();
	
}

//void floor()
//{
//	
//}

void floor()
{
	float size = 1000;
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glNormal3f(0, 1, 0);
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	
	for(float i = -size; i < size; i += 10)
	{
		for(float j = -size;  j < size; j += 10)
		{
			glBegin(GL_QUADS);
				glTexCoord2f((j/(size * 2)), i/(size * 2)); glVertex3f(i, 0, j);
				glTexCoord2f(((j+10)/(size * 2)), i/(size * 2)); glVertex3f(i, 0, j+10);
				glTexCoord2f((j+10/(size * 2)), (i+10)/(size * 2)); glVertex3f(i+10, 0, j+10);
				glTexCoord2f(j/(size * 2),(i+10)/(size * 2)); glVertex3f(i+10, 0, j);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
	
	
	/*
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glBegin(GL_QUADS);			 		//Ground
		glTexCoord2f(0,1); glVertex3f(-size, 0, -size);
		glTexCoord2f(0,0); glVertex3f(-size, 0, size);
		glTexCoord2f(1,0); glVertex3f(size, 0, size);
		glTexCoord2f(1,1); glVertex3f(size, 0, -size);
	glEnd();*/
}
//----------------------------------------------------------------------

void alien()
{
	GLUquadric *q;
	
	glPushMatrix();							//Body
		glColor3f (0.7, 0.2, 1.0);
		glRotatef(-90, 1, 0, 0);
		glutSolidCone(6, 18, 50, 20);
	glPopMatrix();
	
	glPushMatrix();							//Head
		glPushMatrix();						//Face
			glTranslatef(0,14,0);
			glutSolidSphere(5, 20, 20);
		glPopMatrix();
		
		glPushMatrix();						//Antenna connector 1
			glTranslatef(-6,21,0);
			glRotatef(-270, 0, 1, 0);
			glRotatef(-45, 0, 0, 1);
			glRotatef(45, 1, 1, 0);
			q = gluNewQuadric();
			gluCylinder(q, 0.8, 0.8, 10, 20, 20);
			gluQuadricDrawStyle(q, GLU_FILL);
		glPopMatrix();
		
		glPushMatrix();						//Antenna connector 2
			glTranslatef(6,21,0);
			glRotatef(270, 0, 1, 0);
			glRotatef(-45, 0, 0, 1);
			glRotatef(45, 1, 1, 0);
			q = gluNewQuadric();
			gluCylinder(q, 0.8, 0.8, 10, 20, 20);
			gluQuadricDrawStyle(q, GLU_FILL);
		glPopMatrix();
		
		glPushMatrix();						//Antenna 1
			glColor3f (0.93, 0.64, 1.0);
			glTranslatef(-7,22,0);
			glRotatef(-90, 1, 0, 0);
			glutSolidSphere(2, 20, 20);
		glPopMatrix();
		
		glPushMatrix();						//Antenna 1
			glTranslatef(7,22,0);
			glRotatef(-90, 1, 0, 0);
			glutSolidSphere(2, 20, 20);
		glPopMatrix();
		
		glPushMatrix();						//eye 1
			glTranslatef(-1,15,4);
			glRotatef(-90, 1, 0, 0);
			glutSolidSphere(2, 20, 20);
		glPopMatrix();
		
		
	glPopMatrix();

}
//----------------------------------------------------------------------

void spaceHub()
{
	glColor3ub(224, 14, 14);
	glPushMatrix();						//Ring
		glRotatef(90, 1, 0, 0);
		glutSolidTorus(5, 32,15, 40);
	glPopMatrix();
	
	glPushMatrix();						//base
		glColor3ub(129, 170, 198);
		GLUquadric *q;
		q = gluNewQuadric();
		
		glRotatef(90, 1, 0, 0);
		gluCylinder(q, 35, 10, 20, 50, 50);
		gluQuadricDrawStyle(q, GLU_FILL);
	glPopMatrix();

}
//----------------------------------------------------------------------

void lightTower()
{
	GLUquadric *q;
	q = gluNewQuadric();
	
	
	glPushMatrix();					//Beam
		glTranslatef(0, 150, 0);
		glColor3ub(237, 135, 30);
		glRotatef(90, 1, 0, 0);
		gluCylinder(q, 10, 10, 150, 50,50);
		gluQuadricDrawStyle(q, GLU_FILL);
	glPopMatrix();
	
	glPushMatrix();					//Base
		glTranslatef(0, 23, 0);
		glScalef(1, 1, 2);
		glutSolidCube(45);
	glPopMatrix();
	
	glPushMatrix();				//Control tower (Top)
		glColor3ub(237, 135, 30);
		glTranslatef(0, 150, 0);
		glScalef(40, 40, 40);
		glutSolidIcosahedron();
	glPopMatrix();

}
//----------------------------------------------------------------------

void miniSpaceship()
{
	glRotatef((miniRotation), 0, 1, 0);
	glRotatef(7, 0 ,0 ,1);
	glScalef(0.7,0.7,0.7);
	glPushMatrix();
					
		float wx[N], wy[N], wz[N];
		float angStep = 2.0*3.1415926/180.0;  //Rotate in 2 deg (converted to radians)

		for(int j = 0; j < 180; j++){
			for(int i = 0; i < N; i++){
				wx[i] = (vx[i] * cos(angStep)) + (vz[i] * sin(angStep));
				wy[i] = vy[i];
				wz[i] = vz[i] * cos(angStep) - vx[i] * sin(angStep) ;
			}
			
		    glBindTexture(GL_TEXTURE_2D, txId[2]);
		    glEnable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLE_STRIP);
				for(int i = 0; i < N; i++){
					if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
									vx[i-1], vy[i-1], vz[i-1],
									vx[i], vy[i], vz[i] );
					glTexCoord2f(j/180.0, i/(float)(N-1)); glVertex3f(vx[i], vy[i], vz[i]);
					if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
									  vx[i], vy[i], vz[i],
									  wx[i], wy[i], wz[i] );
					glTexCoord2f((j+1)/180.0, i/(float)(N-1)); glVertex3f(wx[i], wy[i], wz[i]);
				}
				glEnd();
			   
				for(int i = 0; i < N; i++){
					vx[i] = wx[i];
					vy[i] = wy[i];
					vz[i] = wz[i];
				}	
			}
			glDisable(GL_TEXTURE_2D);
			
			//Cabin
			glColor4f (1.0, 0.75, 0.5, 1.0);
			glPushMatrix();
				glTranslatef(0, 10, 0);
				glutSolidSphere(10, 10, 10);
			glPopMatrix();

	glPopMatrix();
}
//----------------------------------------------------------------------

void miniSpaceship2()
{
	glRotatef((miniRotation), 0, 1, 0);
	glRotatef(7, 0 ,0 ,1);
	glScalef(0.7,0.7,0.7);
	glPushMatrix();
					
		float wx[N], wy[N], wz[N];
		float angStep = 2.0*3.1415926/180.0;  //Rotate in 2 deg (converted to radians)

		for(int j = 0; j < 180; j++){
			for(int i = 0; i < N; i++){
				wx[i] = (vx[i] * cos(angStep)) + (vz[i] * sin(angStep));
				wy[i] = vy[i];
				wz[i] = vz[i] * cos(angStep) - vx[i] * sin(angStep) ;
			}
			
		    glBindTexture(GL_TEXTURE_2D, txId[4]);
		    glEnable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLE_STRIP);
				for(int i = 0; i < N; i++){
					if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
									vx[i-1], vy[i-1], vz[i-1],
									vx[i], vy[i], vz[i] );
					glTexCoord2f(j/180.0, i/(float)(N-1)); glVertex3f(vx[i], vy[i], vz[i]);
					if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
									  vx[i], vy[i], vz[i],
									  wx[i], wy[i], wz[i] );
					glTexCoord2f((j+1)/180.0, i/(float)(N-1)); glVertex3f(wx[i], wy[i], wz[i]);
				}
				glEnd();
			   
				for(int i = 0; i < N; i++){
					vx[i] = wx[i];
					vy[i] = wy[i];
					vz[i] = wz[i];
				}	
			}
			glDisable(GL_TEXTURE_2D);
			
			//Cabin
			glColor3ub (230, 120, 15);
			glPushMatrix();
				glTranslatef(0, 10, 0);
				glutSolidSphere(10, 10, 10);
			glPopMatrix();

	glPopMatrix();
}
//----------------------------------------------------------------------

void spaceship()
{
    float wx[N], wy[N], wz[N];
    float angStep = 2.0*3.1415926/180.0;  //Rotate in 2 deg (converted to radians)
    int canHgt = -25;
    GLUquadric *q;

	glPushMatrix();
	glRotatef((miniRotation), 0, 1, 0);
		for(int j = 0; j < 180; j++){
			for(int i = 0; i < N; i++){
				wx[i] = (vx[i] * cos(angStep)) + (vz[i] * sin(angStep));
				wy[i] = vy[i];
				wz[i] = vz[i] * cos(angStep) - vx[i] * sin(angStep) ;
			}
		   
			glBindTexture(GL_TEXTURE_2D, txId[3]);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLE_STRIP);
				for(int i = 0; i < N; i++){
					if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
									vx[i-1], vy[i-1], vz[i-1],
									vx[i], vy[i], vz[i] );
					glTexCoord2f(j/180.0, i/(float)(N-1)); glVertex3f(vx[i], vy[i], vz[i]);
					if(i > 0) normal( wx[i-1], wy[i-1], wz[i-1],
									  vx[i], vy[i], vz[i],
									  wx[i], wy[i], wz[i] );
					glTexCoord2f((j+1)/180.0, i/(float)(N-1)); glVertex3f(wx[i], wy[i], wz[i]);
				}
				glEnd();
			   
				for(int i = 0; i < N; i++){
					vx[i] = wx[i];
					vy[i] = wy[i];
					vz[i] = wz[i];
				}
			}
	
        glDisable(GL_TEXTURE_2D);

       
        //Cabin
        glPushMatrix();
            glTranslatef(0, 10, 0);
            glutSolidSphere(10, 10, 10);
        glPopMatrix();
	glPopMatrix();

       
        // Cannon
        {
        glPushMatrix();
            glRotatef(180, 0, 1, 0);
            glTranslatef(0, 0, 10);
           
            glPushMatrix();                     //Outer Ring
                glTranslatef(0, canHgt, 0);
                q = gluNewQuadric();
                gluCylinder(q, 15, 5, 50, 20, 15);
                gluQuadricDrawStyle(q, GLU_FILL);  
            glPopMatrix();
           
            glPushMatrix();                     //Ball end of outer Ring
                glTranslatef(0, canHgt, 49);
                glutSolidSphere(5, 20, 20);
            glPopMatrix();
           
            glPushMatrix();                     //Outer Ring cover
                glColor3f (0.7, 0.9, 1.0);
                glTranslatef(0, canHgt, 0);
                q = gluNewQuadric();
                gluDisk(q, 5, 15, 20, 5);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
            glPushMatrix();
                glColor3f (1.0, 0.0, 1.0);      //2nd Ring
                glTranslatef(0, canHgt, -8);
                q = gluNewQuadric();
                gluCylinder(q, 10, 10, 18, 20, 10);
                gluQuadricDrawStyle(q, GLU_FILL);  
            glPopMatrix();
           
            glPushMatrix();                     //2nd Ring cover
                glColor3f (0.7, 0.2, 1.0);
                glTranslatef(0, canHgt, -8);
                q = gluNewQuadric();
                gluDisk(q, 5, 10, 20, 5);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
            glPushMatrix();
                glColor3f (1.0, 0.0, 1.0);      //3nd Ring
                glTranslatef(0, canHgt, -15);
                q = gluNewQuadric();
                gluCylinder(q, 7, 7, 18, 20, 10);
                gluQuadricDrawStyle(q, GLU_FILL);  
            glPopMatrix();
           
            glPushMatrix();                     //3nd Ring cover
                glColor3f (0.7, 0.2, 1.0);
                glTranslatef(0, canHgt, -15);
                q = gluNewQuadric();
                gluDisk(q, 3, 7, 20, 5);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
            glPushMatrix();                     //Ball end of 3rd Ring
                glColor3f (0.0, 1.0, 0.0);
                glTranslatef(0, canHgt, -14);
                glutSolidSphere(4, 20, 20);
            glPopMatrix();
           
            glPushMatrix();
                glColor3f (1.0, 0.0, 1.0);      //Barrel
                glTranslatef(0, canHgt, -25);
                q = gluNewQuadric();
                gluCylinder(q, 1, 1, 10, 20, 10);
                gluQuadricDrawStyle(q, GLU_FILL);  
            glPopMatrix();
        glPopMatrix();
	}
       
        //cabin Connectors
        glPushMatrix();
            glColor3f (1.0, 0.0, 0.0);
           
            glPushMatrix();                 //Cabin Connector
                q = gluNewQuadric();
                //glScalef(1,1,1.5);
                glRotatef(135,1, 0, 0);
                glTranslatef(0, -5, 0);
                gluCylinder(q, 3, 3, 30, 20, 20);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
            glPushMatrix();                //Thruster 1 Connector
                q = gluNewQuadric();
                glScalef(1,1,0.5);
                glTranslatef(-5, -20, -90);
                glRotatef(42,0,1,0);
                glRotatef(225,1, 0, 0);
                gluCylinder(q, 2, 2, 65, 20, 20);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
            
            glPushMatrix();                //Thruster 2 Connector
                q = gluNewQuadric();
                glScalef(1,1,0.5);
                glTranslatef(5, -20, -90);
                glRotatef(-42,0,1,0);
                glRotatef(225,1, 0, 0);
                gluCylinder(q, 2, 2, 65, 20, 20);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
        glPopMatrix();
       
        //Thrusters
        glPushMatrix();
            glColor3f (0.0, 0.0, 1.0);
            glPushMatrix();                 //Thruster 1
                glTranslatef(-35, 25, -90);
                q = gluNewQuadric();
                gluCylinder(q, 5, 5, 50, 20, 20);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
            glPushMatrix();                 //Thruster 1 Back Cap
                glColor3f (0.0, 0.0, 1.0);
                glTranslatef(-35, 25, -90);
                glutSolidSphere(5, 20, 20);
            glPopMatrix();
            
            
            glPushMatrix();                 //Thruster 1 Front Cap
				if((int)colorTime % 2 == 1){
					glColor3f (0.84, 0.33, 0.13);
				}
				else
				{
					glColor3ub(255, 255, 0);
				}  
   
                glTranslatef(-35, 25, -40);
                glutSolidSphere(5, 20, 20);
            glPopMatrix();
           
            glPushMatrix();                 //Thruster 2
                glColor3f (0.0, 0.0, 1.0);
                glTranslatef(35, 25, -90);
                q = gluNewQuadric();
                gluCylinder(q, 5, 5, 50, 20, 20);
                gluQuadricDrawStyle(q, GLU_FILL);
            glPopMatrix();
           
            glPushMatrix();                 //Thruster 2 Back Cap
                glColor3f (0.0, 0.0, 1.0);
                glTranslatef(35, 25, -90);
                glutSolidSphere(5, 20, 20);
            glPopMatrix();
            glPushMatrix();                 //Thruster 2 Front Cap
				if((int)colorTime % 2 == 1){
					glColor3ub(255, 255, 0);
				}
				else
				{
					glColor3f (0.84, 0.33, 0.13);
				}
                glTranslatef(35, 25, -40);
                glutSolidSphere(5, 20, 20);
            glPopMatrix();
        glPopMatrix();  
        
}
//----------------------------------------------------------------------

void suroundSpaceships()
{
	glPushMatrix();				//MiniSpaceship1
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 42 + cos(miniShipTheta) * 5, 0);
			miniSpaceship();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub1
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship2
		glRotatef(45,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 42 + cos(miniShipTheta) * 10, 0);
			miniSpaceship2();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub2
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship3
		glRotatef(90,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 42 + cos(miniShipTheta) * 15, 0);
			miniSpaceship();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub3
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship4
		glRotatef(135,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 42 + cos(miniShipTheta) * 20, 0);
			miniSpaceship2();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub4
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship5
		glRotatef(180,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 49 + cos(miniShipTheta) * 25, 0);
			miniSpaceship();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub5
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship6
		glRotatef(225,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 42 + cos(miniShipTheta) * 20, 0);
			miniSpaceship2();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub6
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship7
		glRotatef(270,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 49 + cos(miniShipTheta) * 25, 0);
			miniSpaceship();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub7
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();
	
	glPushMatrix();				//MiniSpaceship8
		glRotatef(315,0,1,0);
		glTranslatef(0, 1, -350);
		glEnable(GL_TEXTURE_2D);		
		glPushMatrix();				
			glColor3ub(21, 146, 21);
			glTranslatef(0, 42 + cos(miniShipTheta) * 20, 0);
			miniSpaceship2();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		
		glPushMatrix();				//SpaceHub8
			glPushMatrix();
				glTranslatef(0, 20, 0);
				spaceHub();
			glPopMatrix();	
		glPopMatrix();
	glPopMatrix();	
	
}
//----------------------------------------------------------------------

void camera(void)
{
	dir_x = -sin(xz_angle), dir_y = tan(yz_angle),  dir_z = -cos(xz_angle);
	
	
	lookpoint_x = cam_x + 2 * dir_x;
	lookpoint_y = cam_hgt + 2 * dir_y;
	lookpoint_z = cam_z + 2 * dir_z;
	
	gluLookAt(cam_x, cam_hgt, cam_z, lookpoint_x, lookpoint_y, lookpoint_z, 0., 1., 0.);
}
//----------------------------------------------------------------------

void takeOffSeq()
{
		glRotatef(enterpriseTheta, 0, 1, 0);
		glTranslatef(0, enterpriseHgt, 0);
		
		if(enterpriseOrbit_set == true){
			glTranslatef(0, 0, enterpriseOrbit);
		}
		
}

void landingSeq()
{
		glRotatef(enterpriseTheta, 0, 1, 0);
		glTranslatef(0, enterpriseHgt, 0);
		if(enterpriseOrbit_set2 == true){
			glTranslatef(0, 0, enterpriseOrbit);
		}
		enterpriseOrbit_set = false;		
}
//----------------------------------------------------------------------

void display(void)
{
	float lgt_pos[]={-360.0f, 500.0f, -360.0f, 1.0f}; 	 //light0 position above light tower
	float spot_pos[] = {0.0f, 0.0f, 0.0f, 1.0f}; //
    float spot_dir[] = {-5, -5, 10}; //Direction of the spotlight

	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera();
	glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position
	glEnable(GL_LIGHTING);	

	glColor4f (1.0, 0.75, 0.5, 1.0);   			 //Temporary
	
	
	glEnable(GL_TEXTURE_2D);					//Draw Skybox
		skybox();
	glDisable(GL_TEXTURE_2D);
	
	
    glPushMatrix();								//Draw Enterprise
		if (takeOff == true) {
			takeOffSeq();
			}
		else if (takeOff == false && landing == true) {landingSeq();}
		glTranslatef(0, 40, 0);
		spaceship();
		glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);
	glPopMatrix();
	
	 glPushMatrix();							//Draw Surounding Spaceships
		suroundSpaceships();
	glPopMatrix();					
	
	
	glPushMatrix();								//Draw Alien1
		glRotatef(-moveAlien, 0, 1, 0); 
		glTranslatef(alien_x, alien_y, 0);
		glTranslatef(0, 15, 250);
		glRotatef(-2 * alienTheta, 0, 1, 0);
		alien();
	glPopMatrix();
	
	glPushMatrix();								//Draw Alien2
		glRotatef(-moveAlien, 0, 1, 0); 
		glTranslatef(alien_x, alien_y, 0);
		glTranslatef(0, 15, -250);
		glRotatef(2 * alienTheta, 0, 1, 0);
		alien();
	glPopMatrix();
	
	glPushMatrix();								//Draw Alien3
		glTranslatef(0, 30, 600);
		glRotatef(180, 0, 1, 0);
		alien();
	glPopMatrix();
	
	glPushMatrix();
		glPushMatrix();								//Draw Alien3 (Light Tower)
			glTranslatef(-360, 76 + cos(miniShipTheta) * 15, -335);
			alien();
		glPopMatrix();
		
		glPushMatrix();								//Platform
		glColor3ub(237, 78, 30);
		glTranslatef(-360, 75 + cos(miniShipTheta) * 15, -340);
			glScalef(20, 1, 30);
			glutSolidCube(1);
		glPopMatrix();
		
	glPopMatrix();
	
	
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPushMatrix();								//Light Tower
		glTranslatef(-360, 0, -360);
		lightTower();
	glPopMatrix();
		
	
	
	
    glDisable(GL_LIGHTING);

	glutSwapBuffers();
}
//----------------------------------------------------------------------

void special(int key, int x, int y)
{
	if(cam_hgt <=35){
		if(yz_angle <= 0.3) yz_angle = 0.3;
	}
	
	if(key==GLUT_KEY_LEFT) xz_angle += 0.1;           //Rotate camera left
	else if(key==GLUT_KEY_RIGHT) xz_angle -= 0.1;	 //Rotate camera Right
	else if(key==GLUT_KEY_UP) yz_angle += 0.1;      //Rotate camera up
	else if(key==GLUT_KEY_DOWN) yz_angle -= 0.1;   //Rotate camera Down
	
	if(yz_angle <= -1.5) yz_angle = -1.5;			//Clamping Minimum yz Angle
	if(yz_angle >= 1.5) yz_angle = 1.5;		//Clamping Maximum yz Angle

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) 
{
	pan_z = 0;
	int camSpeed = 5;
	switch(key)
	{
		case 'a': cam_x += dir_z * camSpeed, cam_z -= dir_x * camSpeed; break;
		case 'd': cam_x -= dir_z * camSpeed, cam_z += dir_x * camSpeed; break;
		case 's':
			pan_z = -1;
			cam_x = cam_x + pan_z * dir_x * camSpeed;
			cam_z = cam_z + pan_z * dir_z * camSpeed;
			break;
		case 'w':
			pan_z = 1;
			cam_x = cam_x + pan_z * dir_x * camSpeed;
			cam_z = cam_z + pan_z * dir_z * camSpeed;
			break;
		case 'e': cam_hgt += 5; break;
		case 'c': cam_hgt -= 5; break;
		case 'z': 						//Zoom out
			pan_z = -1;
			cam_x = cam_x + pan_z * dir_x * camSpeed;
			cam_hgt = cam_hgt + pan_z * dir_y;
			cam_z = cam_z + pan_z * dir_z * camSpeed;
			break;
		case 'x':						//Zoom In
			pan_z = 1;
			cam_x = cam_x + pan_z * dir_x * camSpeed;
			cam_hgt = cam_hgt + pan_z * dir_y;
			cam_z = cam_z + pan_z * dir_z * camSpeed;
			break;
		case '1': cam_x = 0, cam_hgt = 50.0, cam_z = 100, xz_angle = 0, yz_angle = 0; break; //Changes camera to origin position
		case '2': 										//Changes camera to Start position
			cam_x = 500, cam_hgt = 500.0,  cam_z = 400,
			lookpoint_x = 500., lookpoint_y = 500., lookpoint_z = 350.,
			xz_angle = 290, yz_angle = -0.5;
			break;
		case '3': cam_x = 0, cam_hgt = 30.0, cam_z = 600, xz_angle = 0, yz_angle = 0; break;
		case ' ': if(takeOff == false && landing == false) { takeOff = true; cam_x = 0, cam_hgt = 30.0, cam_z = 600, xz_angle = 0, yz_angle = 0;}
				else if (takeOff == true && landing == false) {takeOff = false, landing = true; cam_x = 0, cam_hgt = 30.0, cam_z = 600, xz_angle = 0, yz_angle = 0.5;}
				break;
						
	}
	
	if(cam_x <= -800) cam_x = -800;			//Clamping Minimum x position
	else if(cam_x >= 800) cam_x = 800;		//Clamping Maximum x position
	if(cam_hgt <= 35)  cam_hgt = 35;		//Clamping Minimum y position
	else if(cam_hgt >= 800) cam_hgt = 800;	//Clamping Maximum y position
	if(cam_z <= -400) cam_z = -400;			//Clamping Minimum z position
	else if(cam_z >= 600) cam_z = 600;		//Clamping Maximum z position
	
	glutPostRedisplay();
}

//--Timer Function for camera rotation----------------------------------
void myTimer(int value)
{
	alien_time += 0.02;	
	float alienY_vel = (cos(M_PI * (alienTheta) / 180) * inital_vel);
	float alienX_vel = (sin(M_PI * (alienTheta) / 180) * inital_vel);
	alien_x = ((10 + alien_time * alienX_vel));
	alien_y = ((0 + alien_time * alienY_vel) + (-50 * (alien_time * alien_time)));
	
	if (alien_y <= 1) alien_y = 1;				//Stops the Alien from falling below the ground
	if (alien_time > 0.8) alien_time = 0;
	
	if (takeOff == true) enterpriseHgt++;		//Enterprise lift off seq.
	if (enterpriseHgt >= max_enterpriseHgt){
		 enterpriseHgt = max_enterpriseHgt;
		 if (enterpriseTheta >=360){
			  enterpriseTheta = 360;
			  enterpriseOrbit_set = true;
			  if(yz_angle >= 0.8) yz_angle = 0.8;
			  yz_angle +=0.001;
			  enterpriseOrbit++;
		}
	}
	if(enterpriseOrbit >= max_enterpriseOrbit){
		enterpriseOrbit = max_enterpriseOrbit;
		enterpriseOrbit_set2 = true;
	}
	
	if (landing == true && enterpriseOrbit_set2 ==true){
		if(yz_angle <= 0.1) yz_angle = 0.1;
			  yz_angle -=0.001;
		enterpriseOrbit--;
	}
	
	if (landing == true) enterpriseHgt--;
	if (enterpriseHgt <= 20){
		if (enterpriseTheta >=360){
			  enterpriseTheta = 360;
		}
		enterpriseHgt = 20, landing = false;
	}
	
	
	enterpriseTheta++;
	moveAlien++;
	miniRotation++;
	colorTime += 0.016;
	miniShipTheta += 0.01;
	glutPostRedisplay();
	glutTimerFunc(16, myTimer, 0);
}

//-------------------------------------------------------------------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE| GLUT_DEPTH);
   int screenHeight = 800;
   
   //int screenWidth = 800;
   screenHeight = glutGet (GLUT_SCREEN_HEIGHT);
   //screenWidth = glutGet (GLUT_SCREEN_WIDTH);
   glutInitWindowSize (screenHeight, screenHeight); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Spaceport");
   initialise ();
   glutDisplayFunc(display);
   glutTimerFunc(50, myTimer, 0);
   glutSpecialFunc(special);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}
