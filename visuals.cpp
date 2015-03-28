#include <stdio.h>     // - Just for some ASCII messages
#include <stdlib.h>     /* srand, rand */
#include <string.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>       /* time */
#include <ctime>
#include "gl/glut.h"   // - An interface and windows
#include "gl/glm/glm.hpp"
#include "visuals.h"   // Header file for our OpenGL functions
#include "colors.h"

using namespace std;
using namespace glm;


model md;
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)
int LifeRemaining = 3;
int ModeSelect = 0;
float AsteroidSpeed;
static int GameStarted = 0;
static int asteroidFlag = 0;
static int AsteroidSize_1 =5;
static int AsteroidSize_2 =5;
static int AsteroidSize_3 =5;
static int Pause = 0;
static int ActiveasteroidFlag = 0;
static float PowSize = 0.01;
static int ColisionFlag = 0;
static int Originalx = 0;
static int Originaly = 0;
static int Originalz = 0;
static int AsteroidX = 0;
static int AsteroidY = 0;
static int AsteroidZ = 0;
static float TranslateY = 0; //Var for enviroment movement in Yaxis
static float TranslateX = 0; //Var for enviroment movement in Xaxis
static float RotateY = 0; //Var to rotate camera in Yaxis
static float RotateX = 0; //Var to rotate camera in Xaxis
static int Ytilt = 0; //Var for plane tilt in Yaxis
static int Xtilt = 0; //Var for plane tilt in Xaxis
static float transx = 0;
static float transy = 0;
static float transz = 0;
static float rotx = 0; //Var for helics rotation animation
static float fanSpeed = 14.5; //Rotation Speed of the helics
static float SunPos[3] ={ 35, 35, -50 }; //Sun position is global so as light position is the same as Sun Position


void KeyOperations()
{
	if(!GameStarted)
	{
		if (keyStates['W'] || keyStates['w'])
		{
			if(!ModeSelect)
				ModeSelect = 2;
			else
				ModeSelect--;
			keyStates['W'] = false;
			keyStates['w'] = false;
		}
		if (keyStates['S'] || keyStates['s'])
		{
			ModeSelect=(ModeSelect+1)%3;
			keyStates['S'] = false;
			keyStates['s'] = false;
		}
		if (keyStates[13])
		{
			switch(ModeSelect)
			{
				case 0:
					AsteroidSpeed = 0.5;
					break;
				case 1:
					AsteroidSpeed = 0.7;
					break;
				case 2:
					AsteroidSpeed = 1;
					break;
				default:
					AsteroidSpeed = 0.01;
			}
			GameStarted = 1;
		}
	}
	if(keyStates['p'] || keyStates['P'])
		Pause = 1 - Pause;		//Toggle Pause
	if (GameStarted)
	{
		if(keyStates['q'] || keyStates['Q'])
			exit(0);
		if(keyStates['j'] || keyStates['J'])
			RotateY += 2;			//Camera manipulation
		if(keyStates['l'] || keyStates['L'])
			RotateY -= 2;			//Camera manipulation
		if(keyStates['i'] || keyStates['I'])
	 		RotateX += 2;			//Camera manipulation
		if(keyStates['k'] || keyStates['K'])
			RotateX -= 2;			//Camera manipulation
	}
	if(!Pause && !ColisionFlag && GameStarted)
	{
		if(keyStates['w'] || keyStates['W'])
		{
			TranslateY -= 0.9f;	//Move upwards by 0.9
			if (Ytilt <= 35)		//Tilt plane upwards with a maximum 36.8% angle
				Ytilt += 2;
		}
		if(keyStates['s'] || keyStates['S'])
		{
			TranslateY += 0.9f;	//Move downwards by 0.9
			if (Ytilt >= -35)	//Tilt plane downwards with a maximum -36.8% angle
				Ytilt -= 2;
		}
		if(keyStates['a'] || keyStates['A'])
		{
			TranslateX += 0.9f;	//Move SideWays by 0.9
			if (Xtilt <= 35)	//Tilt plane camera-left with a maximum 36.8% angle
				Xtilt += 2;
		}
		if(keyStates['d'] || keyStates['D'])
		{
			TranslateX -= 0.9f;	//Move SideWays by 0.9
			if (Xtilt >= -35)	//Tilt plane camera-righr with a maximum 36.8% angle
				Xtilt -= 2;
		}
	}
}

void resetAsteroid()
{
	transz = 0;
	transy = 0;
	transx = 0;
	asteroidFlag = 0;
}

void drawHalfSphere(int scaley, int scalex, GLfloat r)
{
	int i, j;
	GLfloat v[scalex * scaley][3];

	for (i = 0; i < scalex; ++i)
	{
		for (j = 0; j < scaley; ++j)
		{
			v[i * scaley + j][0] = r * cos(j * 2 * M_PI / scaley)
					* cos(i * M_PI / (2 * scalex));
			v[i * scaley + j][1] = r * sin(i * M_PI / (2 * scalex));
			v[i * scaley + j][2] = r * sin(j * 2 * M_PI / scaley)
					* cos(i * M_PI / (2 * scalex));
		}
	}

	glBegin (GL_QUADS);
	for (i = 0; i < scalex - 1; ++i)
	{
		for (j = 0; j < scaley; ++j)
		{
			glVertex3fv(v[i * scaley + j]);
			glVertex3fv(v[i * scaley + (j + 1) % scaley]);
			glVertex3fv(v[(i + 1) * scaley + (j + 1) % scaley]);
			glVertex3fv(v[(i + 1) * scaley + j]);
		}
	}
	glEnd();
}

void plane(float Xpos, float Ypos)
{
	glTranslatef(-Xpos, -Ypos, 0); //Plane must always remain at the center of the screen
	glRotatef(-90, 0, 1, 0); //So as the back of the plane is originaly visible to the player
	glRotatef(Xtilt, 1, 0, 0); //Tilt on X axis when turn
	glRotatef(-Ytilt, 0, 0, 1); //Tilt on Y axis when turn
	glScalef(0.3, 0.7, 0.5); //Plane was initaly designe larger than needed

	//Body
	glPushMatrix();
	glColor3fv (GreenCopper);
	glRotatef(180, 0, 1, 0);
	glScalef(28, 10, 8);
	glutSolidIcosahedron();
	glutWireIcosahedron();
	glPopMatrix();

	//PlaneHead
	glPushMatrix();
	glScalef(0.9, 1, 0.85);
	glTranslatef(-15, 0.6, 0);
	glRotatef(-90, -0.06, 1, 0);
	glColor3fv (Silver);
	glutWireCone(9, 16.0, 17, 20);
	glPopMatrix();

	//Tail
	glPushMatrix();
	glColor3fv (Brass);
	glScalef(1.7, 1, 0.85);
	glTranslatef(7, -0.4f, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidCone(9, 16.0, 17, 20); //color red 0.81 green 0.71 blue 0.23
	glPopMatrix();

	//Wings
	glPushMatrix();
	glColor3fv (MediumWood);
	glTranslatef(0, -0.4f, -15.9);
	glScalef(1.2, 0.4, 2);
	glutSolidCube(9); //color red 0.81 green 0.71 blue 0.23
	glTranslatef(0, -0.4f, 15.9);
	glutSolidCube(9); //color red 0.81 green 0.71 blue 0.23
	glPopMatrix();
	//SmallHelics1
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glTranslatef(-20.3, -1, -6.65);
	glRotatef(rotx, 0, 0, 1);
	glColor3fv(Silver); //color red 0.196078 green 0.6 blue 0.8
	for (float i = 0; i < 1; i += 0.01) //Many small Wire cones at a small distance between them to give a 3D view
	{
		glPushMatrix();
		glTranslatef(i, i, i);
		glScalef(1, 1.25, 0.01);
		glutWireCone(4, 30, 4, 0);
		glPopMatrix();
	}
	glPopMatrix();
	//SmallHelics2
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glTranslatef(20.3, -1, -6.65);
	glRotatef(rotx, 0, 0, 1);
	for (float i = 0; i < 1; i += 0.01)
	{
		glPushMatrix();
		glTranslatef(i, i, i);
		glScalef(1, 1.25, 0.01);
		glutWireCone(4, 30, 4, 0);

		glPopMatrix();
	}
	glPopMatrix();
	//BigHelics
	glPushMatrix();
	glColor3fv (DarkTan);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, -1, -28.7);
	glRotatef(rotx, 0, 0, 1);
	for (float i = 0; i < 1; i += 0.01)
	{
		glPushMatrix();
		glTranslatef(i, i, i);
		glScalef(1.75, 2, 0.01);
		glutWireCone(5, 30, 4, 0);

		glPopMatrix();
	}
	glPopMatrix();

	//PilotHatch
	glPushMatrix();
	glScalef(0.6, 1, 0.6);
	glTranslatef(-10, 8, 0);
	glColor4fv (SkyBlueTrans); //Semi transparent blue for the hatch
	drawHalfSphere(10, 20, 6.4);
	glPopMatrix();

	//Pilot
	//Head
	glPushMatrix();
	glColor3fv (IndianRed);
	glTranslatef(-6, 12, 0);
	glutSolidSphere(2, 20, 20);
	glTranslatef(-0, 0, 0);
	//Eyes
	glScalef(0.1, 1, 1);
	glColor3fv (White);
	glTranslatef(-20.8, 0.4, 1);
	glutSolidSphere(0.6, 10, 10);	 	//EYE1
	glColor3fv (Black);
	glTranslatef(-0.5, 0, 0);
	glutSolidSphere(0.25, 10, 10);	 	//EYE1
	glTranslatef(0, 0, -1.3);
	glColor3fv(White);
	glutSolidSphere(0.6, 10, 10);	 	//EYE2
	glColor3fv(Black);
	glTranslatef(-0.5, 0, 0);
	glutSolidSphere(0.25, 10, 10);	 	//EYE2
	//Mouth
	glColor3fv (OrangeRed);
	glScalef(0, 0.4f, 1);
	glTranslatef(0, -3, 0.6);
	glutSolidSphere(0.6, 10, 10);
	//teeth (uselessly small)
	glColor3fv(White);
	glScalef(0.3, 1, 1);
	for (float i = 0.1; i <= 1; i += 0.1)
	{
		glPushMatrix();
		if (i < 0.6)
			glTranslatef(0, cos(i) - 0.5, 0 + i);
		else
			glTranslatef(0, cos(i) - 0.2, 0.5 - i);
		glutSolidCube(0.2);
		glPopMatrix();
	}
	glPopMatrix();
}

void asteroid()
{
	glPushMatrix();
	AsteroidX = -Originalx -50 + transx;
	AsteroidY = -Originaly+ 50+ transy;
	AsteroidZ = -50 + transz;
	glTranslatef(AsteroidX,AsteroidY,AsteroidZ); //Move Sun to the top right
	glRotatef(rotx, 0, rotx, 1);
	glColor3fv(OrangeRed);
	DisplayModel(md);
	glPopMatrix();
}


void sun(void)
{
	glPushMatrix();
	glTranslatef(SunPos[0], SunPos[1], SunPos[2]); //Move Sun to the top right
	glColor3fv (MediumGoldenrod);
	//Randomly change Size and color to simulate sun
	float random = rand() % 10;
	glutSolidSphere(5.25, 80, 80);
	glColor4f(0.9f, 0.25f, 0.0f, 0.85 + (random / 40));
	glutSolidSphere(5.25 + random / 5, 80, 80);
	glColor4f(0.858824f, 0.858824f, 0.439216f, random / 45);
	glutSolidSphere(7.25 + random / 10, 80, 80);
	glPopMatrix();
}

void stars(void) //Create mini-Suns around the plane pseudo-randomly in every direction number of starts 800*4
{
	for (int i = 0; i < 800; i++)
	{
		float random = rand() % 20;
		glPushMatrix();
		glRotatef(0.0 + i, 1, 0, 0);
		glPushMatrix();
		glTranslatef(i * cos(i), i * sin(i), i);
		glColor3fv (Gold);
		glutSolidSphere(0.07, 4, 4);
		glColor4f(0.858824f, 0.858824f, 0.439216f, (random / 50));
		glutSolidSphere(0.07 + random / 200, 4, 4);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(i * sin(i), i * cos(i), i * (-1 + rand() % 1));
		glColor3fv(Gold);
		glutSolidSphere(0.07, 4, 4);
		glColor4f(0.858824f, 0.858824f, 0.439216f, (random / 50));
		glutSolidSphere(0.07 + random / 200, 4, 4);
		glPopMatrix();
		glRotatef(0.0 - i, 1, 0, 0);
		glRotatef(0.0 + i, 0, 1, 0);
		glPushMatrix();
		glTranslatef(i * cos(i), i * sin(i), i);
		glColor3fv(Gold);
		glutSolidSphere(0.07, 4, 4);
		glColor4f(0.858824f, 0.858824f, 0.439216f, (random / 50));
		glutSolidSphere(0.07 + random / 200, 4, 4);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(i * sin(i), i * cos(i), i * (-1 + rand() % 1));
		glColor3fv(Gold);
		glutSolidSphere(0.07, 4, 4);
		glColor4f(0.858824f, 0.858824f, 0.439216f, (random / 50));
		glutSolidSphere(0.07 + random / 200, 4, 4);
		glPopMatrix();
		glRotatef(0.0 - i, 0, 1, 0);
		glPopMatrix();
	}
}

void Lives()
{
    char life[10];
	sprintf(life, "%d", LifeRemaining);
	char score[30] = "life:";
	strcat(score,life);
	glPushMatrix();
	glScalef(0.015,0.015,0.001);
	glTranslatef(-3100,3000,0);
	glColor3fv(White);
	for (int i=0;i<strlen(score);i++)
	  glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN ,score[i]);
	glPopMatrix();

}

void PrintText(const char *str,float size,const char *color)
{

	glPushMatrix();
	glTranslatef(-TranslateX,-TranslateY,15);
	glScalef(size,size,size);
	if(!strcmp(color,"Red"))
		glColor3fv(Red);
	else if(!strcmp(color,"White"))
		glColor3fv(White);
	for (int i=0;i<strlen(str);i++)
	  glutStrokeCharacter(GLUT_STROKE_ROMAN,str[i]);
	glPopMatrix();

}

bool loadOBJ(model *md){

	FILE * file = fopen("asteroid.obj", "r");
	if (file == NULL){
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			point vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			//              cout << "I have read: " << vertex.x << '\n';
			md->obj_points.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0){
			face temp;
			unsigned int vertexIndex, uvIndex, normalIndex;
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &(temp.vtx[0]), &uvIndex, &(temp.vtx[1]), &vertexIndex, &(temp.vtx[2]), &normalIndex);
			//              cout << "I have read: " << temp.vtx[0] << '\n';
			if (matches != 6){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}

			md->obj_faces.push_back(temp);

		}

	}

}

void Render()
{
	KeyOperations();
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel (GL_FLAT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	Lives();
	glRotatef(RotateY, 0, 1, 0); //Change camera Y view when pressing  i || k
	glRotatef(RotateX, 1, 0, 0); //Change camera X view when pressing  j || l
	srand (time(NULL));

	glTranslatef(TranslateX, TranslateY, 0); //Pseudo movement in X and Y axis when pressing WASD keys
	sun(); //create sun before rotations and movements
	if(asteroidFlag)
	{
		asteroid();	
		if(sqrt((AsteroidX+TranslateX)*(AsteroidX+TranslateX) + (AsteroidY + TranslateY	)*(AsteroidY + TranslateY) < 350))
		{
			ColisionFlag = 1;
			LifeRemaining--;
			asteroidFlag = 0;
		}
	}
	else if(ColisionFlag)
	{
		if(!LifeRemaining)
		{
			glPushMatrix();
			glTranslatef(-15,30,0);
			PrintText("Game Over",0.04,"Red");
			glPopMatrix();
			asteroid();
		}
		else
		{
			glPushMatrix();
			glTranslatef(-10,20,0);
			PrintText("POW",PowSize,"Red");
			glPopMatrix();
			asteroid();
		}
	}
	stars(); //create starts
	if(GameStarted)
	{
		plane(TranslateX, TranslateY); //Plane must remain in the center so we must negate env movement
		glTranslatef(50, 50, 50);
		glScalef(50,50,50);
	}
	else
	{
		glPushMatrix();
		glTranslatef(-15,30,0);
		PrintText("Mode Select:",0.04,"White");
		glTranslatef(0,-10,0);
		if(ModeSelect == 0)
		{
			glPushMatrix();
			glTranslatef(-2,1,0);
			glColor3fv(Green);
			glutSolidSphere(1, 20, 20);
			glPopMatrix();
			LifeRemaining = 10;
		}
		PrintText("Easy",0.02,"White");
		glTranslatef(0,-10,0);
		if(ModeSelect == 1)
		{
			glPushMatrix();
			glTranslatef(-2,1,0);
			glColor3fv(OrangeRed);
			glutSolidSphere(1, 20, 20);
			glPopMatrix();
			LifeRemaining = 5;
		}
		PrintText("Medium",0.02,"White");
		glTranslatef(0,-10,0);
		if(ModeSelect == 2)
		{
			glPushMatrix();
			glTranslatef(-2,1,0);
			glColor3fv(Red);
			glutSolidSphere(1, 20, 20);
			glPopMatrix();
			LifeRemaining = 3;
		}
		PrintText("Hard",0.02,"White");
		glPopMatrix();
	}
	glutSwapBuffers();
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations 
	glFlush(); // Flush the OpenGL buffers to the window
}

//-----------------------------------------------------------

void Idle()
{
	if(!Pause && GameStarted)
	{
		rotx += fanSpeed; 	//Helics rotation engine
		//PseudoSpeed += 3.0;
		if (Ytilt > 0)	//If Ytilt was altered by keyboard input then decrease it
			Ytilt -= 0.2;//in a smaller rate than it is increased to make sure eventual return to balance
		else if (Ytilt < 0)//If Ytilt was altered by keyboard input then increase it
			Ytilt += 0.2;//in a smaller rate than it is decreased to make sure eventual return to balance
		if (Xtilt > 0)			//...
			Xtilt -= 0.2;		//...
		else if (Xtilt < 0)	//...
			Xtilt += 0.2;		//...
		if(asteroidFlag)
		{
			transz+=AsteroidSpeed;
			transy-=AsteroidSpeed;
			transx+=AsteroidSpeed;
			if(transy < -110)
				resetAsteroid();
		}
		else
		{
			if(ColisionFlag)
			{
				if(PowSize >= 0.18)
				{
					if(!LifeRemaining)
						exit(0);
					resetAsteroid();
					ColisionFlag = 0;
					PowSize = 0.01;
				}
				PowSize+=0.003;
			}
			else if(rand()%5 == 4)
			{
				asteroidFlag = 1;
				AsteroidSize_1 = 5+rand()%3;
				AsteroidSize_1 = 5+rand()%5;
				AsteroidSize_1 = 5+rand()%10;
				Originalx = TranslateX;
				Originaly = TranslateY;
			}
		}
	}
	glutPostRedisplay();//Re display changes
}

void KeyboardPressed(unsigned char key, int x, int y)
{
	keyStates[key] = true;
}

void KeyboardUnPressed (unsigned char key, int x, int y) 
{  
	keyStates[key] = false; // Set the state of the current key to not pressed  
}  


//-----------------------------------------------------------

void Resize(int w, int h)
{
	// define the visible area of the window ( in pixels )
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);

	// Setup viewing volume

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	// L     R       B      T      N      F
	glOrtho(-50.0f, 50.0f, -50.0f, 50.0f, -500.0f, 500.0f);
}

//-----------------------------------------------------------

void Setup()
{
	bool res = loadOBJ(&md);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);
	//Set up light source
	GLfloat ambientLight[] =
	{ 0.8, 0.8, 0.8 };
	GLfloat diffuseLight[] =
	{ 0.8, 0.8, 0.8, 1 };
	GLfloat lightPos[] = { SunPos[0], SunPos[1], SunPos[2], 1.0 };//light position is the same as sun position
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	// polygon rendering mode and material properties
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glFrontFace(GL_CW);
}

void DisplayModel(model md)
{

	glPushMatrix();
	glScalef(0.01, 0.01, 0.01);
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < 18240; i++)
	{
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[0] - 1].x, md.obj_points[md.obj_faces[i].vtx[0] - 1].y, md.obj_points[md.obj_faces[i].vtx[0] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[1] - 1].x, md.obj_points[md.obj_faces[i].vtx[1] - 1].y, md.obj_points[md.obj_faces[i].vtx[1] - 1].z);
		glVertex3f(md.obj_points[md.obj_faces[i].vtx[2] - 1].x, md.obj_points[md.obj_faces[i].vtx[2] - 1].y, md.obj_points[md.obj_faces[i].vtx[2] - 1].z);
	}

	glEnd();
	glPopMatrix();

}

