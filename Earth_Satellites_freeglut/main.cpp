#include "Libraries\glew\glew.h"
#include "Libraries\freeglut\freeglut.h"
#include "Libraries\SOIL\SOIL.h"
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include "cOrbit.h"

# define WINDOW_TITLE "First Program"

int 
	CurrentWidth = 800,
	CurrentHeight = 600,
	WindowHandle = 0;

GLfloat	xRot;							// X Rotation
GLfloat	yRot;							// Y Rotation
GLfloat zCoord = -5.0f;
GLfloat earthRotation = 0;

GLuint	texture[2];						// storage for textures
char textureNames[2][256] = {
	"Data/earth.jpg",
	"Data/null.jpg"
};

int  earthPoints = 64;
int orbitPoints = 32;

const int renderTime = 25;				// ms
const int timerID = 1;

double currentDay = 345.0;			// the number of days from the beginning of the year
const double daysInOneRender = 4.17e-5; // ~ 1 day in 10 min

cOrbit *satellites = new cOrbit[10];
const int satNum = 2;

const float PI     = 3.14159265358979f;
const float TWOPI  = 6.28318530717958f;
const float PIDIV2 = 1.57079632679489f;

// window functions
void Initialize(int argc, char* argv[]);
void InitWindow(int argc, char **argv);
void ResizeFunc(int width, int height);
void RenderFunc(void);
void timerFunc(int timerID);

// keys functions
void processSpecialKeys(int key, int x, int y);

// render functions
void LoadGLTextures(void);
void SceneInit(void);
void renderSphere(float r, int np );
void renderEarth(void);
void renderOrbit(cOrbit sat);
void renderSattelite(cOrbit sat);

int main(int argc, char **argv)
{
	Initialize(argc, argv);

	//MOLNIYA 2-9             
	//1 07276U 74026A   15343.07762157 -.00000007  00000-0  23907-3 0  9994
	//2 07276  62.5803 276.6272 7086829 282.7770  12.4157  2.45084692190991
	satellites[0].orbitInit(15.0, 343.07762157, 9099.0, 
		62.5803, 276.6272, 0.7,
		282.7770, 12.4157, 2.4508469219);

	//NOAA 1 [-] 
	//1 04793U 70106A   15342.22087536 -.00000028  00000-0  11324-3 0  9990
	//2 04793 101.8864  37.1500 0031864 248.0906 155.3366 12.53974919 59340
	satellites[1].orbitInit(15.0, 342.22087536, 59340.0, 
		101.8864, 37.1500, 0.0031864,
		248.0906, 155.3366, 12.53974919);

	glutMainLoop();

	fprintf(
		stdout,
		"exit"
		);
 
	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	InitWindow(argc, argv);
	
	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
	);

	LoadGLTextures();

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
}

void InitWindow(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(2, 0);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	WindowHandle = glutCreateWindow(WINDOW_TITLE);
	if(WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
		);
		exit(EXIT_FAILURE);
	}
	
	glutReshapeFunc(ResizeFunc);
	glutDisplayFunc(RenderFunc);

	glutSpecialFunc(processSpecialKeys);

	glutTimerFunc(renderTime, timerFunc, timerID);
}

void ResizeFunc(int width, int height)
{
	if(0 == height)
		height = 1;

	CurrentWidth = width;
	CurrentHeight = height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void RenderFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SceneInit();
	renderEarth();

	for(int i=0; i<satNum; i++)
	{
		SceneInit();
		renderOrbit(satellites[i]);

		SceneInit();
		renderSattelite(satellites[i]);
	}

	glutSwapBuffers();
}

void timerFunc(int timerID)
{
	if(1 == timerID)
	{
		currentDay += daysInOneRender;
		earthRotation = -360.0 * ( currentDay - floor(currentDay));
		glutPostRedisplay(); // redisplay scene
		glutTimerFunc(renderTime, timerFunc, timerID);
	}
}

void processSpecialKeys(int key, int x, int y)
{
	switch(key) 
	{
		case GLUT_KEY_LEFT:
			yRot += 3.0f;
			break;
		case GLUT_KEY_RIGHT:
			yRot -= 3.0f;
			break;
		case GLUT_KEY_UP:
			xRot += 3.0f;
			break;
		case GLUT_KEY_DOWN:
			xRot -= 3.0f;
			break;
		case GLUT_KEY_PAGE_UP:
			if(zCoord > -10.0f) zCoord -= 0.1f;
			break;
		case GLUT_KEY_PAGE_DOWN:
			if(zCoord < -3.0f) zCoord += 0.1f;
			break;
	}
}

void LoadGLTextures(void)
{
    for(int i=0; i<2; i++)
	{
		texture[i] = SOIL_load_OGL_texture(
			textureNames[i],
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y
			);
		if(texture[i] == 0) 
			fprintf(
				stdout,
				"Couldn't load texture %i",i
				);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
}

void SceneInit(void)
{
	glLoadIdentity();

	glTranslatef( 0.0f, 0.0f, zCoord );
	glRotatef( xRot, 1.0f, 0.0f, 0.0f );
	glRotatef( yRot, 0.0f, 1.0f, 0.0f );

	glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
}

void renderSphere(float r, int np )
{
    float alpha1 = 0.0;
    float alpha2 = 0.0;
    float alpha3 = 0.0;

    float ex = 0.0f;
    float ey = 0.0f;
    float ez = 0.0f;

    float px = 0.0f;
    float py = 0.0f;
    float pz = 0.0f;

    for(int i = 0; i < np; i++)
    {
        alpha1 = -(i*PI/np) + PI;
        alpha2 = -(i+1)*PI/np + PI;

        glBegin( GL_TRIANGLE_STRIP );
        {
            for(int j = 0; j <= np; j++)
            {
                alpha3 = j*TWOPI/np;

                ex = sin(alpha2)*cos(alpha3);
                ey = sin(alpha2)*sin(alpha3);
                ez = cos(alpha2);
                px = r * ex;
                py = r * ey;
                pz = r * ez;

                glTexCoord2f( (j/(float)np) , (i+1)/(float)np );
                glVertex3f( px, py, pz );

                ex = sin(alpha1)*cos(alpha3);
                ey = sin(alpha1)*sin(alpha3);
                ez = cos(alpha1);
                px = r * ex;
                py = r * ey;
                pz = r * ez;

                glTexCoord2f( (j/(float)np), i/(float)np );
                glVertex3f( px, py, pz );
            }
        }
        glEnd();
    }
}

void renderEarth(void)
{
	glRotatef( -180.0f + earthRotation, 0.0f, 0.0f, 1.0f );

	glColor3f( 1.0f, 1.0f, 1.0f );
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	renderSphere( 0.9f, earthPoints );
}

void renderOrbit(cOrbit sat)
{
	float dt = sat.getTimePeriod() / orbitPoints;
	float startTime = sat.getEpochDay();
	float endTime  = sat.getEpochDay() + sat.getTimePeriod() + dt;
	coord satCoord;

	glColor3f( 0.0f, 0.0f, 1.0f );
	glBegin( GL_LINE_STRIP );
    {
		for(float time = startTime; time <= endTime; time += dt)
		{
			satCoord = sat.calculateCoord(time);
                
			glVertex3f( satCoord.x, satCoord.y, satCoord.z );
        }
	}
	glEnd();
}

void renderSattelite(cOrbit sat)
{
	coord satCoord = sat.calculateCoord(currentDay);

	glTranslatef( satCoord.x, satCoord.y, satCoord.z );
	glColor3f( 1.0f, 0.0f, 0.0f );
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	renderSphere(0.01f, 8);
}